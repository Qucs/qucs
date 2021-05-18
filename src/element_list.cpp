/***************************************************************************
    copyright            : 2018, 2020 Felix Salfelder
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "schematic_lang.h"
#include "qucs_globals.h"
#include "nodemap.h"
#include "net.h"
#include "netlist.h" // BUG
#include "io_trace.h"
#include "parameter.h"
#include "painting.h" // BUG
#include "place.h" // BUG?
#include "diagram.h" // BUG?
#include "task_element.h" // BUG?
/*--------------------------------------------------------------------------*/
namespace qucs {
/*--------------------------------------------------------------------------*/
ElementList::ElementList()
  : Nets(nullptr),
    _nm(nullptr),
    _parent(nullptr),
    _params(nullptr)
{
	Nets = new NetList(); // BUG
	_nm = new NodeMap(*Nets);
}
/*--------------------------------------------------------------------------*/
ElementList::~ElementList()
{
	for(auto i : *this){
	  	if(auto c=dynamic_cast<Symbol*>(i)){
			if(c->is_device()){
				// disconnect(c);
			}else{
				assert(!dynamic_cast<Conductor*>(i));
			}
		}else{
		}
	}
}
/*--------------------------------------------------------------------------*/
void ElementList::clear()
{
	for(auto& pc : _cl){
		if(auto s=prechecked_cast<Symbol*>(pc)){
			disconnect(s);
		}else{
		}
		delete pc;
		pc = nullptr;
	}
	_cl.clear();
	nodes()->clear();
	// paintings().clear();
	_map.clear();
	//SymbolPaints.clear(); ??
}
/*--------------------------------------------------------------------------*/
// called from schematic::erase only
// // possibly not needed. all actions must be undoable anyway
// -> use detach, store reference in UndoAction.
void ElementList::erase(Element* what)
{
	Element* e = detach(what);
	delete(e);
}
/*--------------------------------------------------------------------------*/
// supposedly equivalent to clone and erase.
Element* ElementList::detach(Element* what)
{
	assert(what);
	assert(what->owner());
	std::string l = what->label();

	if(_map.find(l) == _map.end()){ untested();
		std::cerr << "BUG, gone: " << l << "\n";
		unreachable(); // nets??
	}else{
		auto pos = _map.lower_bound(l);
		assert(pos!=_map.end());
		while(pos->second != what){
			++pos;
		}
		/// TODO: map list iterators
		_map.erase(pos);
		trace1("erased", l);
	}

	if(auto d=dynamic_cast<Diagram*>(what)){itested();
		_cl.erase(std::find(begin(), end(), d));
	}else if(auto c=dynamic_cast<Symbol*>(what)){
		disconnect(c);
		_cl.erase(std::find(begin(), end(), c));
	}else{
		unreachable(); // model?
		_cl.erase(std::find(begin(), end(), what));
	}

	what->set_owner(nullptr);
	return what;
}
/*--------------------------------------------------------------------------*/
// stash object and keep track of label.
void ElementList::push_back(Element* what)
{
	_map.insert(std::make_pair(what->label(), what));
	trace2("ElementList::push_back", what->label(), this);
	_cl.push_back(what);
}
/*--------------------------------------------------------------------------*/
ElementList& ElementList::erase(const_iterator what)
{
	assert(what!=end());
	erase(*what);
	return *this;
}
/*--------------------------------------------------------------------------*/
// BUG: connect and push_back in one go. don't use.
#if 0
void ElementList::pushBack(Element* what)
{
	incomplete();
	push_back(what);

	trace2("ElementList::push_back", what->label(), this);
	if(dynamic_cast<TaskElement*>(what)){ untested();
	}else if(auto c=dynamic_cast<Symbol*>(what)){
		if(c->is_device()){
			trace1("connect?", what->label());
			connect(c); // BUG. wrong place.
		}else{
			assert(!dynamic_cast<Conductor*>(what));
		}
	}else if(dynamic_cast<Element*>(what)){
	}else{ untested();
//		unreachable?
		incomplete();
	}
}
#endif
/*--------------------------------------------------------------------------*/
// QFileInfo const& ElementList::getFileInfo ()const
// {
// 	return FileInfo;
// }
/*--------------------------------------------------------------------------*/
NodeMap* ElementList::nodes() const
{
	return _nm;
}
/*--------------------------------------------------------------------------*/
//PaintingList const& ElementList::symbolPaints() const
//{ untested();
//	return SymbolPaints;
//}

//PaintingList& ElementList::paintings()
//{
//	return Paintings;
//}
//
//PaintingList& ElementList::symbolPaintings()
//{ untested();
//	assert(_symbol);
//	// temporary. move stuff here....
//	return _symbol->symbolPaintings();
//}
//
// same, but const.
//ElementList const& ElementList::components() const
//{
//	return _cl;
//}

// NodeMap const& ElementList::nodes() const
// {
// 	return Nodes;
// }

// PaintingList const& ElementList::paintings() const
// {
// 	return Paintings;
// }

//PaintingList const& ElementList::symbolPaints() const
//{ untested();
//	return SymbolPaintings;
//}
/*--------------------------------------------------------------------------*/
#if 0 // TODO: what is this? (perhaps DocumentFormat?)
static void createNodeSet(QStringList& Collect, int& countInit,
		Conductor *pw, Node *p1)
{ untested();
	if(pw->Label)
		if(!pw->Label->initValue.isEmpty())
			Collect.append("NodeSet:NS" + QString::number(countInit++) + " " +
					p1->name() + " U=\"" + pw->Label->initValue + "\"");
}
#endif
/*--------------------------------------------------------------------------*/
void ElementList::disconnect(Symbol* c)
{
	trace1("disconnect", c->label());
	incomplete();
	// drop port connections
	for(index_t i=0; i<c->numPorts(); ++i) {
		// trace3("sm:ds", i, c->label(), c->portPosition(i));
		c->set_port_by_index(i, "");

		// find and cleanup associated places here?
		// (probably not.)
	}
}
/*--------------------------------------------------------------------------*/
static Place const* place_at(pos_t p, Symbol* m)
{
	std::string ps = ":" + std::to_string(getX(p)) + ":" + std::to_string(getY(p));
	auto scope = m->scope();
	assert(scope);
	auto i = scope->find_(ps);
	Place const* ret = nullptr;

	assert(m->owner());

	if(i == scope->end()){
	}else if(auto p=dynamic_cast<Place const*>(*i)){
		ret = p;
	}else{
		incomplete();// find_again...
		assert(false); //for now.
	}

	if(!ret){
		Symbol* c = symbol_dispatcher.clone("place");
		auto s = prechecked_cast<Place*>(c);
		assert(s);
		s->setPosition(p);
		s->set_dev_type("place");
		s->set_label(ps);
		s->set_owner(m->owner());
		s->set_port_by_index(0, ps);
		scope->push_back(s);

		ret = s;
	}else{
	}

	return ret;

}
#if 1 // obsolete. free?
void ElementList::connect(Symbol* sym)
{

#if 0
	incomplete();
	assert(c->is_device());
	for(unsigned i=0; i<c->numPorts(); ++i){
		c->connectNode(i, *nodes()); // use scope.
//		assert(dynamic_cast<Symbol const*>(c)->port(i).connected());
	}
#endif

	for(index_t i=0; i<sym->numPorts(); ++i){
		incomplete(); // free?

		try{
			pos_t p = sym->nodePosition(i);
			auto q = place_at(p, sym);
			
			std::string const& l = q->label();
			sym->set_port_by_index(i, l);
		}catch(ExceptionCantFind const&){ untested();
			/// non-schematic node?
		}
	}
}
#endif
/*--------------------------------------------------------------------------*/
// BUG?
//unsigned ElementList::numPorts() const
//{
//	assert(this);
//	trace1("ElementList::numPorts", this);
//	// incomplete
//	return _ports.size();
//}
/*--------------------------------------------------------------------------*/
//void ElementList::setPort(unsigned i, Node* n)
//{
//	trace2("setPort", i, _ports.size());
//	_ports.resize(std::max(_ports.size(), size_t(i)+1));
//	_ports[i] = n;
//}
///*--------------------------------------------------------------------------*/
//Node const* ElementList::portValue(unsigned i) const
//{
//	assert(i<numPorts());
//	if(_ports[i]){
//		return _ports[i];
//	}else{ untested();
//		return nullptr;
//	}
//}
/*--------------------------------------------------------------------------*/
void ElementList::prepare()
{
	for(auto pc : _cl){
		assert(pc);
		trace2("prepare", pc->label(), pc);
		pc->prepare();
	}
}
/*--------------------------------------------------------------------------*/
void ElementList::set_owner(Element* o)
{
	for(auto pc : _cl){
		assert(pc);
		trace3("set_owner", pc->label(), pc, o);
		pc->set_owner(o);
		const Element* sym = pc;
		assert(sym->owner() == o);
	}
}
/*--------------------------------------------------------------------------*/
ElementList const* ElementList::parent() const
{
	return nullptr;
}
/*--------------------------------------------------------------------------*/
bool operator==(Object const*p, std::string const&s)
{
	if(p){
		return p->short_label() == s;
	}else{
		return false;
	}
}
/*--------------------------------------------------------------------------*/
ElementList::iterator ElementList::find_again(std::string const& short_name,
						ElementList::iterator /*Begin*/)
{
	// incomplete, does not find again.
	trace1("find_again", short_name);
	auto it = std::find(_cl.begin(), _cl.end(), short_name);
	trace2("found?", _cl.size(), it==_cl.end());
	return it;
}
/*--------------------------------------------------------------------------*/
ElementList::const_iterator ElementList::find_again(const std::string& short_name,
						ElementList::const_iterator /*Begin*/)const
{
	// incomplete, does not find again.
	trace1("find_again", short_name);
	auto it = std::find(_cl.begin(), _cl.end(), short_name);
	trace2("found?", _cl.size(), it==_cl.end());
	return it;
}
/*--------------------------------------------------------------------------*/
// HACK
unsigned ElementList::nextIdx(std::string const& s) const
{
	unsigned r=0;
	if(s==""){
		return 0;
	}else{
		auto j = _map.lower_bound(s);
		auto n = s.size();

		while (j!=_map.end()){
			if(j->second->label().substr(0, n) == s){
				auto str = j->second->label().substr(n);
				trace3("nextIdx", j->second->label().substr(0, n), n, str);
				unsigned z;
				if(sscanf(str.c_str(), "%d", &z) == 1){ itested();
					r = std::max(z, r);
				}else{ itested();
				}

				++j;
			}else{
				break;
			}
		}
	}

	trace2("nextId", s, r);
	return r+1;
}
/*--------------------------------------------------------------------------*/
PARAM_LIST* ElementList::params()
{
  if (!_params) {
    assert(!_parent);
    _params = new PARAM_LIST;
  }else{
  }
  return _params;
}
/*--------------------------------------------------------------------------*/
PARAM_LIST const* ElementList::params() const
{
  if (!_params) {
    assert(!_parent);
    _params = new PARAM_LIST;
  }else{
  }
  return _params;
}
/*--------------------------------------------------------------------------*/
// debug (any other use?!)
size_t numWires(ElementList const& m)
{
	size_t r = 0;
	for(auto i : m){
		if(dynamic_cast<Conductor const*>(i)){
			++r;
		}else{
		}
	}
	return r;
}
/*--------------------------------------------------------------------------*/
size_t ElementList::numNets() const
{
	assert(Nets);
	return Nets->size();
}
/*--------------------------------------------------------------------------*/
size_t ElementList::numNodes() const
{
	assert(nodes());
	return nodes()->size();
}
/*--------------------------------------------------------------------------*/
} // qucs
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
