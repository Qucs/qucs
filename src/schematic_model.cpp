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

#include "schematic_doc.h"
#include "schematic_lang.h"
#include "globals.h"
#include "nodemap.h"
#include "net.h"
#include "io_trace.h"
#include "u_parameter.h"
#include "painting.h" // BUG
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// getting here in CLI mode
SchematicModel::SchematicModel()
  : Nodes(Nets),
    _parent(nullptr),
    _params(nullptr)
{
}
/*--------------------------------------------------------------------------*/
SchematicModel::~SchematicModel()
{
	for(auto i : *this){
	  	if(auto c=dynamic_cast<Symbol*>(i)){
			if(c->is_device()){
				disconnect(c);
			}else{
				assert(!dynamic_cast<Conductor*>(i));
			}
		}else{
		}
	}
}
/*--------------------------------------------------------------------------*/
void SchematicModel::clear()
{
	incomplete(); // disconnect components?
	components().clear();
	diagrams().clear();
	nodes().clear();
	paintings().clear();
	_map.clear();
	//SymbolPaints.clear(); ??
}
/*--------------------------------------------------------------------------*/
QString const& SchematicModel::devType() const
{ untested();
	return DevType;
}
/*--------------------------------------------------------------------------*/
void SchematicModel::setDevType(QString const& s)
{
	DevType = s;
}
/*--------------------------------------------------------------------------*/
// BUG: not here.
/// ACCESS FUNCTIONS.
// these are required to move model methods over to SchematicModel
// note that _doc->...() functions still involve pointer hacks
ElementList& SchematicModel::components()
{
	return Components;
}
/*--------------------------------------------------------------------------*/
// TODO: check relation to attach. maybe not here.
void SchematicModel::pushBack(Element* what)
{
	attach(what);

#if 0
	// here?!
	if(doc()){itested();
		trace1("SchematicModel::pushBack doc", what->label());
		doc()->addToScene(what);
	}else{
		trace1("SchematicModel::pushBack no doc", what->label());
	}
#endif
} // pushBack
/*--------------------------------------------------------------------------*/
// called from schematic::erase only
// // possibly not needed. all actions must be undoable anyway
// -> use detach, store reference in UndoAction.
void SchematicModel::erase(Element* what)
{
	Element* e = detach(what);
	delete(e);
}
/*--------------------------------------------------------------------------*/
Element* SchematicModel::detach(Element* what)
{
	assert(what);
	std::string l = what->label();

	if(_map.find(l) == _map.end()){
		unreachable();
		trace1("gone", l);
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

	if(auto d=diagram(what)){ untested();
		diagrams().removeRef(d);
// 	}else if(dynamic_cast<Conductor*>(element(what))){
// 		auto s = dynamic_cast<Symbol*>(what);
// 		disconnect(s);
// 		wires().removeRef(s);
	}else if(auto c=dynamic_cast<Symbol*>(what)){
		disconnect(c);
		components().removeRef(c);
	}else{ untested();
		unreachable();
	}

	what->setOwner(nullptr);
	return what;
}
/*--------------------------------------------------------------------------*/
Element* SchematicModel::attach(Element* what)
{
	_map.insert(std::make_pair(what->label(), what));

	trace2("SchematicModel::attach", what->label(), this);
	if(auto c=dynamic_cast<TaskElement*>(what)){
#if 0
		if(doc()){
			trace1("SchematicModel::pushBack command", c->label());
			doc()->commands().push_back(c);
		}else{
			trace1("SchematicModel::pushBack no command", c->label());
			// possibly a subcircuit model? ignore commands.
		}
#endif
//	}else if(auto d=dynamic_cast<Diagram*>(what)){
//		components().append(d);
	}else if(auto c=dynamic_cast<Symbol*>(what)){
//		c->recreate(); // BUG: re? create symbol gfx and random other things. needs owner
//		c->build(); // what's this?!
		if(c->is_device()){
			trace1("connect", what->label());
			connect(c);
		}else{
			assert(!dynamic_cast<Conductor*>(element(what)));
		}

		components().append(c);
	}else if(dynamic_cast<Element*>(what)){
		components().append(what);
	}else{ untested();
//		unreachable?
		incomplete();
	}
	return what;
}
/*--------------------------------------------------------------------------*/
QFileInfo const& SchematicModel::getFileInfo ()const
{
	return FileInfo;
}
/*--------------------------------------------------------------------------*/
NodeMap& SchematicModel::nodes()
{
	return Nodes;
}
/*--------------------------------------------------------------------------*/
//PaintingList const& SchematicModel::symbolPaints() const
//{ untested();
//	return SymbolPaints;
//}

PaintingList& SchematicModel::paintings()
{
	return Paintings;
}
//
//PaintingList& SchematicModel::symbolPaintings()
//{ untested();
//	assert(_symbol);
//	// temporary. move stuff here....
//	return _symbol->symbolPaintings();
//}
//
DiagramList& SchematicModel::diagrams()
{
	return Diagrams;
}

// same, but const.
ElementList const& SchematicModel::components() const
{
	return Components;
}

NodeMap const& SchematicModel::nodes() const
{
	return Nodes;
}

PaintingList const& SchematicModel::paintings() const
{
	return Paintings;
}

DiagramList const& SchematicModel::diagrams() const
{
	return Diagrams;
}

//PaintingList const& SchematicModel::symbolPaints() const
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
void SchematicModel::disconnect(Symbol* c)
{
	// drop port connections
	for(unsigned i=0; i<c->numPorts(); ++i) {
		trace3("sm:ds", i, c->label(), c->portPosition(i));
		Node* nn = c->disconnectNode(i, nodes());
		assert(nn);

		if(!nn){ untested();
			unreachable();
		}else if(!nn->hasPorts()){
			nodes().erase(nn); // possibly garbage collect only.
		}else{
		}
	}
}
/*--------------------------------------------------------------------------*/
void SchematicModel::connect(Symbol* c)
{
	assert(c->is_device());
	for(unsigned i=0; i<c->numPorts(); ++i){
		c->connectNode(i, nodes()); // use scope.
//		assert(dynamic_cast<Symbol const*>(c)->port(i).connected());
	}
}
/*--------------------------------------------------------------------------*/
unsigned SchematicModel::numPorts() const
{
	assert(this);
	trace1("SchematicModel::numPorts", this);
	// incomplete
	return _ports.size();
}
/*--------------------------------------------------------------------------*/
void SchematicModel::setPort(unsigned i, Node* n)
{
	trace2("setPort", i, _ports.size());
	_ports.resize(std::max(_ports.size(), size_t(i)+1));
	_ports[i] = n;
}
/*--------------------------------------------------------------------------*/
Node const* SchematicModel::portValue(unsigned i) const
{
	assert(i<numPorts());
	if(_ports[i]){
		return _ports[i];
	}else{ untested();
		return nullptr;
	}
}
/*--------------------------------------------------------------------------*/
void SchematicModel::setOwner(Element* o)
{
	for(auto pc : components()){
		assert(pc);
		trace3("set_owner", pc->label(), pc, o);
		pc->setOwner(o);
		const Element* sym = pc;
		assert(sym->owner() == o);
	}
}
/*--------------------------------------------------------------------------*/
SchematicModel const* SchematicModel::parent() const
{
	return nullptr;
}
/*--------------------------------------------------------------------------*/
bool operator==(Object const*p, std::string const&s)
{
	if(p){
		return p->label() == s;
	}else{
		return false;
	}
}
/*--------------------------------------------------------------------------*/
SchematicModel::const_iterator SchematicModel::find_again(const std::string& short_name,
						SchematicModel::const_iterator /*Begin*/)const
{
	// incomplete, does not find again.
	trace1("find_again", short_name);
	auto it = std::find(components().begin(), components().end(), short_name);
	trace2("found?", components().size(), it==components().end());
	return it;
}
/*--------------------------------------------------------------------------*/
// HACK
unsigned SchematicModel::nextIdx(std::string const& s) const
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
				}else{ untested();
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
PARAM_LIST* SchematicModel::params()
{
  if (!_params) {
    assert(!_parent);
    _params = new PARAM_LIST;
  }else{
  }
  return _params;
}
/*--------------------------------------------------------------------------*/
PARAM_LIST const* SchematicModel::params() const
{
  if (!_params) {
    assert(!_parent);
    _params = new PARAM_LIST;
  }else{
  }
  return _params;
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
