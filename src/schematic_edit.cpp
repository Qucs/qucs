/***************************************************************************
    copyright            : (C) 2020 Felix Salfelder

 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
/*--------------------------------------------------------------------------*/
#include "schematic_edit.h"
#include "platform.h"
#include "symbol.h"
#include "node.h"
#include "place.h"
#include "schematic_scene.h"
#include <set>
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
static void collectPlaces(ElementGraphics const* e,
                          std::set<ElementGraphics*>& p)
{
	auto scn = e->scene();
	assert(scn);

	if(auto s=dynamic_cast<Symbol const*>(element(e))){

		for(unsigned i=0; i<s->numPorts(); ++i){
			// if s->isConnected(i) ...
			auto pp = s->nodePosition(i);
			ElementGraphics* pg = scn->find_place(pp);
			Place const* place = prechecked_cast<Place const*>(element(pg));
			if(!place){
				assert(!pg);
			}else if(s->port_value(i) == place->port_value(0)){
				p.insert(pg);
			}else{
				assert(false);
				// not here.
			}
		}
	}else{
	}
}
/*--------------------------------------------------------------------------*/
struct{
	bool operator ()(ElementGraphics* a, ElementGraphics* b) const{
		return intptr_t(a) < intptr_t(b);
	}
}lt;
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
template<class T>
void SchematicEdit::save(T& del, T& ins)
{
	trace2("save", del.size(), ins.size());
	// return;
	assert(!_ins.size());
	assert(!_del.size());
	std::sort(ins.begin(), ins.end(), lt);
	std::sort(del.begin(), del.end(), lt);
	std::unique(ins.begin(), ins.end(), lt);
	std::unique(del.begin(), del.end(), lt);

	auto ii = ins.begin();
	auto di = del.begin();

	while(ii != ins.end() && di != del.end()){
		ElementGraphics* i = *ii;
		ElementGraphics* d = *di;
		// trace2("save", i, d);

		if(i==d){itested();
			if(i->isVisible()){
				// has been added eventually
			}else{
				// has been deleted eventually, but added before.
				// -> temporary? no it could have been in the initial insert q.
//				delete i; // crash.
			};
			++ii;
			++di;
		}else if(i<d){itested();
			if(i->isVisible()){
				delete i;
			}else{
				_ins.push_back(i);
			}
			++ii;
		}else{itested();
			if(d->isVisible()){
				_del.push_back(d);
			}else{
				delete d;
			}
			++di;
		}
	}

	_ins.insert(_ins.end(), ii, ins.end());
	_del.insert(_del.end(), di, del.end());

	trace2("saved", _del.size(), _ins.size());
}
/*--------------------------------------------------------------------------*/
inline Place const* place(ElementGraphics const* g)
{
	auto p = prechecked_cast<Place const*>(element(g));
	assert(p);
	return p;
}
/*--------------------------------------------------------------------------*/
// Perform an edit action for the first time. keep track of induced changes.
// This is a generic version of legacy implementation, and it still requires a
// scene implementing the geometry.
void SchematicEdit::do_it_first()
{itested();

	std::vector<ElementGraphics*> done_ins;
	std::vector<ElementGraphics*> done_del;
	std::set<ElementGraphics*> pl;

	// remove ports and join adjacent wires. keep track.
	trace1("============ edit delete...", _del.size());
	while(_del.size()){itested();
		auto r = _del.front();
		trace1("remove", element(r)->label());
		_del.pop_front();
		collectPlaces(r, pl);
		r->hide(); // detaches from model and all sorts of stuff.

		// queued delete.
		done_del.push_back(r); // TODO: different queue? just keep _del?
	}

	std::vector<Place const*> pl2;
	for( auto pp : pl){
		if (place(pp)->node_degree()==0){
			//    // pp will be gone after this operation
			trace1("hideplace0", place(pp)->label());
			pp->hide();
			done_del.push_back(pp);
		}else if (place(pp)->node_degree()==2){
			// why degree 2?
			// these may need "wire adjustments"
			// encapsulation bug?
			pl2.push_back(place(pp));
		}else{
		}
	}

	trace1("============ postrm...", pl.size());
	// now remove symbols adjacent to the collected places of degree 2..
	for(auto portremove : pl2){itested();
		trace1("postremove", portremove->label());
		postRmPort(portremove, done_del);
	}

	trace1("============ edit insert...", _ins.size());
	while(_ins.size()){
		ElementGraphics* gfx = _ins.front();
		trace1("try insert...", element(gfx)->label());
		_ins.pop_front();

		Element* e = element(gfx);
		assert(e);
		SchematicScene* scn = gfx->scene();
		assert(scn);
		scn->possiblyRename(e);
		if(addmerge(gfx, done_del)){
			trace0("collision during merge attempt");
		}else{
			trace2("done insert, show", e->label(), e->mutable_owner());
			// scn->possiblyRename(e); here?
			scn->attachToModel(e); // was part of gfx->show()
			QGraphicsItem* gg=gfx;
			gg->show();
			gfx->setSelected(true); // BUG: keep track somewhere else.
			done_ins.push_back(gfx);
		}
	}

	save(done_ins, done_del);
} // do_it_first
/*--------------------------------------------------------------------------*/
QList<ElementGraphics*> SchematicEdit::items(
           const QPointF &pos, Qt::ItemSelectionMode mode,
           Qt::SortOrder order) const
{
	return _scn.items(pos, mode, order);
}
/*--------------------------------------------------------------------------*/
QList<ElementGraphics*> SchematicEdit::items(QRectF const& r) const
{ untested();
	return _scn.items(r);
}
/*--------------------------------------------------------------------------*/
// merge new item into existing.
// return false if new item does not interfere with existing.
template<class T>
bool SchematicEdit::addmerge(ElementGraphics* new_elt, T& del_done)
{
	bool collision = false;
	assert(!new_elt->isVisible());
	assert(new_elt->scene());
	QRectF bb = new_elt->absoluteBoundingRect();
	trace2("addmerge candidates??", bb.topLeft(), bb.bottomRight());
	auto it = items(bb);
	trace3("addmerge candidates", new_elt, element(new_elt)->label(), it.size());
	for(auto gfxi : it){itested();
		assert(gfxi!=new_elt); // new_elt is invisible.
		trace1("addmerge coll?", element(gfxi)->label());

		// gfxi is already on scene.
		auto n = gfxi->newUnion(new_elt);
		if(n){ untested();
			collision = true;
			trace1("hiding", element(gfxi)->label());

			assert(element(gfxi)->mutable_owner());
			gfxi->hide();
			assert(!element(gfxi)->mutable_owner());

			// collision delete.
			del_done.push_back(gfxi);
			auto nc = n->childItems();
			trace1("got union", nc.size());

			size_t kk=0;

			// unpack q insert.
			for(auto c : nc){itested();
				if(auto g = dynamic_cast<ElementGraphics*>(c)){
					auto cc = g->clone();
					assert(cc);
					assert(!cc->isVisible());
					_scn.addItem(cc);
					assert(!cc->isVisible());
					assert(!element(cc)->scope());
					c->setParentItem(nullptr);
					_ins.push_front(cc); // BUG // need a different q for derived objects
					++kk;
				}else{
					trace0("not useful");
					// text, maybe
					// unreachable();
				}
			}
			if(kk){
				// found something useful
				trace2("unpacked", kk, nc.size());
				delete n; // does it delete them all?
			}else{
				_ins.push_front(n);
			}
			break;
		}else{
			trace0("no union");
		}
	}

	trace1("done addmerge", collision);
	return collision;
}
/*--------------------------------------------------------------------------*/
// remove stuff adjacent nodes that have become degree-2 after removal.
// (add back stuff later).
template<class T>
void SchematicEdit::postRmPort(Place const* remove_at, T& del_done)
{itested();

	assert(remove_at->port_value(0)!="");

	ElementGraphics* placegfx = nullptr;
	unsigned deg = remove_at->node_degree();
	trace2("postremove", deg, remove_at->label());
	if( deg == 2 || deg == 0 ) {
		auto it = items(makeQPointF(remove_at->position()));

		unsigned np=0;
		for(auto gfx : it){
			if(dynamic_cast<Place*>(element(gfx))){
				assert(element(gfx) == remove_at);
				incomplete(); // disconnect?
				placegfx = gfx;
				++np;
				// delete gfx; below.
			}else{
				trace1("hideadj", element(gfx)->label());
				gfx->hide();
				// additional delete
				del_done.push_back(gfx);
				_ins.push_front(gfx); // hmm.
				trace1("queued", gfx);
			}
		}
		trace2("postremove", it.size(), np);
		assert(np<2);
	}else{
		trace1("postrm", remove_at->node_degree());
	}

	if(placegfx){
		trace1("hidegfx", remove_at->label());
		placegfx->hide();
		placegfx->scene()->removeItem(placegfx);
//		delete placegfx;
	}else{
	}
}
/*--------------------------------------------------------------------------*/
void SchematicEdit::do_it()
{
	trace2("do_it", _del.size(), _ins.size());
	for(auto& d : _del){itested();
		trace1("hide", d);
		assert(d->isVisible());
		d->hide();
	}
	for(auto& d : _ins){itested();
		trace1("show", d);
		assert(!d->isVisible());
		//d->update(); // really?
		d->show();
		d->setSelected(true); // BUG. this will select too many.
	}
	std::swap(_ins, _del);
}
/*--------------------------------------------------------------------------*/
void SchematicEdit::qDelete(ElementGraphics* gfx)
{
	assert(gfx->isVisible());
	_del.push_back(gfx);
}
/*--------------------------------------------------------------------------*/
void SchematicEdit::qInsert(ElementGraphics* gfx)
{
	assert(!gfx->isVisible());
	assert(gfx->scene() == &_scn); // wrong?
	_ins.push_back(gfx);
}
/*--------------------------------------------------------------------------*/
// turn swap into add/delete
void SchematicEdit::qSwap(ElementGraphics* gfx, Element* e)
{
	assert(!e->mutable_owner());

	auto ng = new ElementGraphics(e);
	{// ?
		assert(!element(ng)->scope());
		_scn.addItem(ng);
		ng->hide(); // what??
		// e->setOwner(element(gfx)->mutable_owner()); // not here.
		// assert(e->mutable_owner());
		//	assert(element(ng)->scope());
	}

	qDelete(gfx);
	qInsert(ng);
}
/*--------------------------------------------------------------------------*/
void SchematicEdit::redo()
{itested();
	if(_first){
		do_it_first();
		_first = false;
	}else{
		do_it();
	}
}
/*--------------------------------------------------------------------------*/
