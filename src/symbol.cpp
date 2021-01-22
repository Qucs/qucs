/***************************************************************************
    copyright            : (C) 2016-2020 Felix Salfelder / QUCS team
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include "symbol.h"
#include "schematic_model.h"
#include "schematic_dialog.h"
#include <assert.h>
#include "net.h"
#include "geometry.h"
/*--------------------------------------------------------------------------*/
#include <QVBoxLayout>
#include <QTableWidget>
#include <QComboBox>
#include <QCheckBox>
#include <QLabel>
/*--------------------------------------------------------------------------*/
// recreate schematic symbol. not sure why, maybe after parameter changes
// (why not just call "Symbol::create??!")
void Symbol::recreate(){ // }SchematicModel const&){ untested();
}
/*--------------------------------------------------------------------------*/
Symbol::Symbol()
    : Element(),
		_subckt(nullptr),
		_vflip(1),
		_hflip(1),
		_angle(0),
		_param_display(0),
		_common(nullptr),
		_net_nodes(0)
{
}
/*--------------------------------------------------------------------------*/
Symbol::Symbol(Symbol const& s)
    : Element(s),
		_subckt(nullptr),
		_vflip(s._vflip),
		_hflip(s._hflip),
		_angle(s._angle),
		_param_display(s._param_display),
		_common(nullptr),
		_net_nodes(s._net_nodes)
{
  attach_common(s._common);
  setTypeName(s.typeName());
}
/*--------------------------------------------------------------------------*/
// dup
SchematicModel* Symbol::scope()
{
	return Element::scope();

	if(auto o=dynamic_cast<Symbol*>(owner())){ untested();
		return o->subckt();
	}else if(dynamic_cast<QucsDoc*>(owner())){ untested();
		assert(subckt());
		return subckt(); // yikes
	}else{untested();
		trace1("owner not a symbol?", label());
		return nullptr;
	}
}
/*--------------------------------------------------------------------------*/
// reuse overrides to give both const and non-const access.
// (move to header)
SchematicModel const* Symbol::scope() const
{
	auto s=const_cast<Symbol*>(this);
	return s->scope();
}
/*--------------------------------------------------------------------------*/
#if 0
// obsolete? portValue?
QString const& Symbol::netLabel(unsigned i) const
{ untested();
//	assert(hasPort(i));
	return portValue(i);
}
#endif
/*--------------------------------------------------------------------------*/
void Symbol::set_port_by_name(std::string const& name, std::string const& value)
{
	incomplete();
}
/*--------------------------------------------------------------------------*/
void Symbol::set_port_by_index(index_t num, std::string const& ext_name)
{
	trace5("spbi", num, max_nodes(), _net_nodes, ext_name, label());

	if (num < max_nodes()) {
		port(num).new_node(ext_name, this);
		if (num+1 > _net_nodes) {
			// make the list bigger
			_net_nodes = num+1;
		}else{
			// it's already big enough, probably assigning out of order
		}
		assert(ext_name=="" || port(num)->hasNet());
	}else{
		throw Exception_Too_Many(num+1, max_nodes(), 0/*offset*/);
	}
}
/*--------------------------------------------------------------------------*/
// connect to a node. (connectPort?)
Node* Symbol::connectNode(unsigned i, NodeMap&nm)
{
	incomplete(); // obsolete;
	return nullptr;

#if 0
	trace2("connectNode", label(), i);
	pos_t p = nodePosition(i);
	Node* n = &nm.at(p);
	assert(n->hasNet());

	trace0("connectNode set port");

	Port& mp = port(i); // hmm setPort(...) instead?
	mp.connect(n /*,this*/);

	return n;
#endif
}
/*--------------------------------------------------------------------------*/
void set_port_by_name(std::string const& name, std::string const& value)
{
	incomplete();
}
/*--------------------------------------------------------------------------*/
void set_port_by_index(index_t i, std::string const& value)
{
	incomplete();
}
/*--------------------------------------------------------------------------*/
Node* Symbol::disconnectNode(unsigned i, NodeMap&)
{
#if 1
	//set_port_by_index(i, "");
#else
	trace2("disconnectNode", label(), i);
	Port& mp = port(i);
	Node* n = mp.value();
	mp.disconnect(n /*, this*/);

	return n;
#endif
	return nullptr;
}
/*--------------------------------------------------------------------------*/
Node const* Symbol::portNode(unsigned i) const
{ untested();
  assert(i<unsigned(numPorts()));
  if(port(i).isConnected()){ untested();
	  assert( port(i).value() );
	  return port(i).value();
  }else{ untested();
	  return nullptr;
  }
}
/*--------------------------------------------------------------------------*/
Node const* Symbol::portValue(unsigned i) const
{
  assert(i<unsigned(numPorts()));
  if(port(i).isConnected()){
	  assert( port(i).value() );
	  return port(i).value();
  }else{
	  return nullptr;
  }
}
/*--------------------------------------------------------------------------*/
// same as Element::center?
pos_t Symbol::center()const
{
	return Element::center(); // pos_t(_cx, _cy);
}
/*--------------------------------------------------------------------------*/
Port const& Symbol::port(unsigned i) const
{
	Symbol* s=const_cast<Symbol*>(this);
	return s->port(i);
}
/*--------------------------------------------------------------------------*/
std::string Symbol::paramValue(std::string const& n) const
{
	if(n=="$xposition"){
		return std::to_string(cx());
	}else if(n=="$yposition"){
		trace1("..", cy());
		return std::to_string(cy());
	}else if(n=="$mfactor"){
		return "1"; // isActive in Component.
	}else if(n=="$param_display"){
		return std::to_string(_param_display);
	}else if(n=="$hflip"){
		return std::to_string(_hflip);
	}else if(n=="$vflip"){
		return std::to_string(_vflip);
	}else if(n=="$angle"){
		return std::to_string(_angle);
	}else{ untested();
		throw ExceptionCantFind(label(), n, "params");
	}
}
/*--------------------------------------------------------------------------*/
void Symbol::paint(ViewPainter* p) const
{itested();

	 // does not make a lot of sense right now
	for(unsigned i=0; i<numPorts(); ++i){ itested();
//		auto pp = common()->portPosition(i);
		auto pp = portPosition(i);
		int x = getX(pp);
		int y = getY(pp);

		if(!port(i).isConnected()){ untested();
			unreachable();
			p->setPen(QPen(Qt::green,2));
			p->drawEllipse(x-1, y-1, 2, 2);
		}else if(port(i)->numPorts()==0){ untested();
			unreachable();
		}else if(port(i)->numPorts()==1){itested();
			// p->setPen(QPen(Qt::red,2));
			p->setPen(QPen(Qt::red,1));
			p->drawEllipse(x-2, y-2, 4, 4);
		}else if(port(i)->numPorts()>2){itested();
			p->setPen(QPen(Qt::black,2));
			p->drawEllipse(x-1, y-1, 2, 2);
		}else{itested();
		}
	}
}
/*--------------------------------------------------------------------------*/
// global/external position
pos_t Symbol::nodePosition(unsigned i) const
{
	pos_t pp = portPosition(i);

	if(legacyTransformHack()){
		// ports use external coordinates...
	}else{
		rotate_after_mirror a(_angle, (1-_hflip)/2, (1-_vflip)/2);
		trace4("nodePosition pre", pp, _angle, _hflip, _vflip);
		pp = a.apply(pp);
		trace2("nodePosition post", pp, center());
	}
	return pp + center();
}
/*--------------------------------------------------------------------------*/
// BUG: not here. legacy stuff...
void Symbol::new_subckt()
{
	assert(!_subckt);
	_subckt = new SchematicModel();
}
/*--------------------------------------------------------------------------*/
Symbol::~Symbol()
{
	delete _subckt;
	_subckt = nullptr;
}
/*--------------------------------------------------------------------------*/
bool Symbol::paramIsPrintable() const
{
	return true;
}
/*--------------------------------------------------------------------------*/
unsigned Symbol::paramCount() const
{
	return 2; // TODO:  vflip hflip angle mfactor ...?
	return 6;
}
/*--------------------------------------------------------------------------*/
std::string Symbol::paramValue(unsigned i) const
{
	switch(i){
	case 0:
		return std::to_string(cx());
	case 1:
		return std::to_string(cy());
	default:
		throw ExceptionCantFind(label(), std::to_string(i), "param values");
	}
}
/*--------------------------------------------------------------------------*/
std::string Symbol::paramName(unsigned i) const
{
	trace2("Symbol::paramName", label(), i);
	switch(i){
	case 0:
		return "$xposition";
	case 1:
		return "$yposition";
	default:
		throw ExceptionCantFind(label(), std::to_string(i), "param names");
	}
}
/*--------------------------------------------------------------------------*/
void Symbol::setParameter(unsigned n, std::string const& v)
{
	if(n==0){ // xpos
		setPosition(pos_t(atoi(v.c_str()), cy()));
	}else if(n==1){ // ypos
		setPosition(pos_t(cx(), atoi(v.c_str())));
	}else if(n<Symbol::paramCount()){
		unreachable();
	}else{
		throw ExceptionCantFind( label(), std::to_string(n), "params");
	}
}
/*--------------------------------------------------------------------------*/
void Symbol::setParameter(std::string const& name, std::string const& v)
{
	if(name == "$xposition"){
		setPosition(pos_t(atoi(v.c_str()), cy()));
	}else if(name == "$yposition"){
		setPosition(pos_t(cx(), atoi(v.c_str())));
	}else if(name == "$angle"){
		trace1("angle", v);
		_angle = atoi(v.c_str());
	}else if(name == "$param_display"){
		_param_display = atoi(v.c_str());
	}else if(name == "$hflip"){
		_hflip = atoi(v.c_str());
		assert(_hflip==1 || _hflip==-1);
	}else if(name == "$vflip"){
		_vflip = atoi(v.c_str());
		assert(_hflip==1 || _hflip==-1);
	}else{
		throw ExceptionCantFind( label(), name, "params");
	}
}
/*--------------------------------------------------------------------------*/
void Symbol::setParameter(QString const& name, QString const& b)
{
	std::string n = name.toStdString();
	std::string v = b.toStdString();
	setParameter(n, v);
}
/*--------------------------------------------------------------------------*/
void Symbol::setParameter(unsigned pos, QString const& b)
{
	auto v = b.toStdString();
	setParameter(pos, v);
}
/*--------------------------------------------------------------------------*/
COMMON_COMPONENT::COMMON_COMPONENT(int c)
  :Object(),
//   _tnom_c(NOT_INPUT),
//   _dtemp(0),
//   _temp_c(NOT_INPUT),
//   _mfactor(1),
//   _value(0),
//   _modelname(),
//   _model(0),
   _attach_count(c)
{
}
/*--------------------------------------------------------------------------*/
COMMON_COMPONENT::~COMMON_COMPONENT()
{
  trace1("common,destruct", _attach_count);
  assert(_attach_count == 0 || _attach_count == CC_STATIC);
}
/*--------------------------------------------------------------------------*/
void COMMON_COMPONENT::attach_common(COMMON_COMPONENT*c, COMMON_COMPONENT**to)
{
  assert(to);
  if (c == *to) {
    // The new and old are the same object.  Do nothing.
  }else if (!c) {untested();
    // There is no new common.  probably a simple element
    detach_common(to);
  }else if (!*to) {
    // No old one, but have a new one.
    ++(c->_attach_count);
    trace1("++1", c->_attach_count);
    *to = c;
  }else if (*c != **to) {
    // They are different, usually by edit.
    detach_common(to);
    ++(c->_attach_count);
    trace1("++2", c->_attach_count);
    *to = c;
  }else if (c->_attach_count == 0) {
    // The new and old are identical.
    // Use the old one.
    // The new one is not used anywhere, so throw it away.
    trace1("delete", c->_attach_count);    
    delete c;
  }else{untested();
    // The new and old are identical.
    // Use the old one.
    // The new one is also used somewhere else, so keep it.
  }
}
/*--------------------------------------------------------------------------*/
void COMMON_COMPONENT::detach_common(COMMON_COMPONENT** from)
{
  assert(from);
  if (*from) {
    assert((**from)._attach_count > 0);
    --((**from)._attach_count);
    trace1("--", (**from)._attach_count);
    if ((**from)._attach_count == 0) {
      trace1("delete", (**from)._attach_count);
      delete *from;
    }else{
      trace1("nodelete", (**from)._attach_count);
    }
    *from = NULL;
  }else{
  }
}
/*--------------------------------------------------------------------------*/
bool COMMON_COMPONENT::operator==(const COMMON_COMPONENT& x)const
{
  return true;
//  (_modelname == x._modelname
//	  && _model == x._model
//	  && _tnom_c == x._tnom_c
//	  && _dtemp == x._dtemp
//	  && _temp_c == x._temp_c
//	  && _mfactor == x._mfactor
//	  && _value == x._value);
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// symbolwidget.cpp?
// #include "qucs.h"
#include "schematic_doc.h"
#include "misc.h"

#include <cmath>
/*--------------------------------------------------------------------------*/
ElementList const* Symbol::symbolPaintings() const
{
	unreachable(); // obsolete
	return nullptr;
}
/*--------------------------------------------------------------------------*/
std::string Symbol::port_value(unsigned i) const
{
	trace2("port_value", label(), i);
	Port const& p = port(i);
	return p.nodeLabel();
}
/*--------------------------------------------------------------------------*/
