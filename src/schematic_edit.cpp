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
	assert(!_ins.size());
//	assert(!_del.size());
	std::swap(_del, _ins);
	trace2("save", del.size(), ins.size());
	trace2("save", _del.size(), _ins.size());
	// return;
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

}
/*--------------------------------------------------------------------------*/
inline Symbol const* symbol(ElementGraphics const* g)
{
	auto p = prechecked_cast<Symbol const*>(element(g));
	//assert(p);
	return p;
}
/*--------------------------------------------------------------------------*/
inline Place const* place(ElementGraphics const* g)
{
	auto p = prechecked_cast<Place const*>(element(g));
	//assert(p);
	return p;
}
/*--------------------------------------------------------------------------*/
class Footprint {
public:
	explicit Footprint(SchematicScene* s) : _scene(s){}
	~Footprint(){ }

public:
	void prepare(Symbol const* s) {
		assert(s);
		for(unsigned i=0; i<s->numPorts(); ++i){
			do_place(s->nodePosition(i));
		}
	}
private:
	void do_place(pos_t p){
		trace2("do_place", getX(p), getY(p));
		assert(_scene);
		if(auto fp = _scene->find_place(p)){
			trace2("already there", getX(p), getY(p));
			_old_plg.insert(fp);

		}else{
			ElementGraphics* plg = _scene->new_place(p/*, hint?*/);
			Place const* pl = place(plg);
			std::string name = pl->port_value(0);
			trace5("not already there", getX(p), getY(p), name, pl->label(), plg);
			_new_plg.insert(plg);
		}
	}
public:
	void collectPlaces(ElementGraphics const* e);
	std::set<ElementGraphics*> const& new_plg(){
		return _new_plg;
	}
	std::set<ElementGraphics*> const& old_plg(){
		return _old_plg;
	}

private:
	SchematicModel* _scope;
	SchematicScene* _scene;
	std::set<ElementGraphics*> _new_plg; // new places.
	std::set<ElementGraphics*> _old_plg; // places that were already there
};
/*--------------------------------------------------------------------------*/
void Footprint::collectPlaces(ElementGraphics const* e)
{
	auto& p = _old_plg;
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
// Perform an edit action for the first time. keep track of induced changes.
// This is a generic version of legacy implementation, and it still requires a
// scene implementing the geometry.
void SchematicEdit::do_it_first()
{itested();

	std::vector<ElementGraphics*> done_ins;
	std::vector<ElementGraphics*> done_del;

	// footprint. keep track of places and nodes
//	SchematicModel* m = scene()->scope();
	Footprint f(scene());

	trace1("============ edit delete...", _del.size());
	// remove symbols staged for deletion. keep track of port places.
	for( auto r : _del){itested();
		trace1("remove", element(r)->label());
		f.collectPlaces(r);
		r->hide(); // detaches from model and all sorts of stuff.
	}

	// remove symbols adjacent to the collected places.
	// queue for re-insertion. keep a list of removed items.
	for(auto portremove : f.old_plg()){itested();
		trace1("postremove", element(portremove)->label());
		postRmPort(place(portremove), done_del);
	}

	// prepare places for insertion of i.
	trace1("============ prepare", _ins.size());
	for( auto i : _ins){ itested();
		if(auto sym = dynamic_cast<Symbol const*>(symbol(i))){
			f.prepare(sym);
		}else{
		}
	}

	trace1("============ edit insert...", _ins.size());
	while(_ins.size()){
		ElementGraphics* gfx = _ins.front();
		trace2("try insert...", element(gfx)->label(), gfx->has_port_values());
		Element* e = element(gfx);
		assert(!dynamic_cast<Place*>(e));


#ifndef NDEBUG
		if(auto sym=dynamic_cast<Symbol const*>(element(gfx))){
			for(unsigned i=0; i<sym->numPorts(); ++i){
				trace3("edit insert", sym->label(), i, sym->port_value(i));
			}
		}
#endif

		_ins.pop_front();

		assert(e);
		SchematicScene* scn = gfx->scene();
		assert(scn);
		scn->possiblyRename(e);
		trace1("collision during merge attempt?", done_del.size());
		if(addmerge(gfx, done_del)){
			trace1("collision during merge attempt", done_del.size());
			// done with this one, others have been queued.
		}else{
			trace3("done insert, show", gfx, e->label(), e->mutable_owner());
			gfx->show_();
			done_ins.push_back(gfx);
		}
	}
	trace2("save", done_ins.size(), done_del.size());

	save(done_ins, done_del);

	trace2("saved", _del.size(), _ins.size());

	trace2("saved", f.new_plg().size(), f.old_plg().size());

	for(auto i : f.new_plg()){
		trace3("FP", place(i)->node_degree(),
				          getX(place(i)->position()),
				          getY(place(i)->position()));
		if(place(i)->node_degree()){
			_del.push_back(i);
		}else{
			i->hide();
			delete i;
		}
	}
	for(auto i : f.old_plg()){
		trace3("FPOLD", place(i)->node_degree(),
				          getX(place(i)->position()),
				          getY(place(i)->position()));
		if(place(i)->node_degree()){
			// still in use.
		}else{
			// has become redundant.
			i->hide();
			_ins.push_back(i);
		}
	}
	trace2("saved with places", _del.size(), _ins.size());
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
void stash_more_places(Symbol* s)
{
	// ...
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

		// gfxi could be a place hit by the new element.
			// port hit by bb.
			// if they are disconnected now, we may not need them later on.
//		stash_more_places(element(gfxi));

		// gfxi is already on scene.
		if(dynamic_cast<Place const*>( element(gfxi))){
			// a place.. revisit_later
			_check_places.insert(gfxi);
		}else if(auto n = gfxi->newUnion(new_elt)){
			trace4("addmerge coll?", element(gfxi)->label(), gfxi, gfxi->pos(), n);
			// if gfxi is a place...

			collision = true;
			trace2("hiding", element(gfxi)->label(), gfxi);

			assert(element(gfxi)->mutable_owner());
			assert(gfxi->isVisible());
			gfxi->hide();
			assert(!element(gfxi)->mutable_owner());

			del_done.push_back(gfxi);
			auto nc = n->childItems();
			trace1("got union", nc.size());

			size_t kk=0;

			// unpack q insert.
			for(auto c : nc){itested();
				auto g = dynamic_cast<ElementGraphics*>(c);

				if(!g){
					trace0("not useful");
					// text, maybe
					// unreachable();
				}else if(dynamic_cast<Place const*>(element(g))){
					// it's a place. why?
				}else{
					auto cc = g->clone();
					assert(cc);
					assert(!cc->isVisible());
					_scn.addItem(cc);
					assert(!cc->isVisible());
					assert(!element(cc)->scope());
					c->setParentItem(nullptr);
					trace2("queueing", element(g)->label(), g);
					_ins.push_front(cc); // BUG // need a different q for derived objects
					++kk;
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
			trace4("addmerge no coll", element(gfxi)->label(), gfxi, gfxi->pos(), n);
		}
	}

	trace1("done addmerge", collision);
	return collision;
}
/*--------------------------------------------------------------------------*/
// remove stuff adjacent to nodes that have become degree-2 after removal.
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
}
/*--------------------------------------------------------------------------*/
void SchematicEdit::do_it()
{
	trace2("do_it", _del.size(), _ins.size());
	for(auto& d : _del){itested();
		trace1("hide", d);
		assert(d->isVisible());
		d->hide();
		assert(!d->isVisible());
	}
	for(auto& d : _ins){itested();
		trace1("show", d);
		assert(!d->isVisible());
		//d->update(); // really?
		d->show_(); // restore? show?
		assert(d->isVisible());
	}
	std::swap(_ins, _del);
}
/*--------------------------------------------------------------------------*/
void SchematicEdit::qDelete(ElementGraphics* gfx)
{
	// Elements must be properly intact (and visible),
	// as we need to (re)store them
	assert(gfx->isVisible());
	_del.push_back(gfx);
}
/*--------------------------------------------------------------------------*/
void SchematicEdit::qInsert(ElementGraphics* gfx)
{
	assert(!dynamic_cast<Place const*>(element(gfx)));

	assert(!gfx->isVisible());
	assert(gfx->scene() == &_scn); // wrong?
	_ins.push_back(gfx);

#ifndef NDEBUG
	if(auto sym=dynamic_cast<Symbol const*>(element(gfx))){
		for(unsigned i=0; i<sym->numPorts(); ++i){
			trace3("qInsert", sym->label(), i, sym->port_value(i));
		}
	}
#endif
}
/*--------------------------------------------------------------------------*/
// turn swap into add/delete
void SchematicEdit::qSwap(ElementGraphics* gfx, Element* e)
{
	assert(!e->mutable_owner());

	auto ng = new ElementGraphics(e);
	ng->setSelected(gfx->isSelected());
	assert(!ng->has_port_values());
	{// ?
		assert(!element(ng)->scope());
		_scn.addItem(ng);
		assert(!ng->isVisible());
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
