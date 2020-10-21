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
#include <assert.h>
#include "net.h"
#include "geometry.h"
/*--------------------------------------------------------------------------*/
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
		_angle(0)
{
}
/*--------------------------------------------------------------------------*/
Symbol::Symbol(Symbol const& s)
    : Element(s),
		_subckt(nullptr),
		_vflip(s._vflip),
		_hflip(s._hflip),
		_angle(s._angle)
{
  setTypeName(s.typeName());
}
/*--------------------------------------------------------------------------*/
// dup
SchematicModel* Symbol::scope()
{
	if(auto o=dynamic_cast<Symbol*>(owner())){
		return o->subckt();
	}else{ untested();
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
// connect to a node. (connectPort?)
Node* Symbol::connectNode(unsigned i, NodeMap&nm)
{
	trace2("connectNode", label(), i);
	Port const& pp = port(i);
	Port& mp = port(i);
	pos_t p(pp.x_()+cx(), pp.y_()+cy());
	Node* n = &nm.at(p);
	assert(n->hasNet());

	mp.connect(n /*,this*/);
	return n;
}
/*--------------------------------------------------------------------------*/
// disconnect a node. (disconnectPort?)
// (does not use the map, but could)
Node* Symbol::disconnectNode(unsigned i, NodeMap&)
{
	trace2("disconnectNode", label(), i);
	Port& mp = port(i);
	Node* n = mp.value();
	mp.disconnect(n /*, this*/);

	return n;
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
Net const* Symbol::portValue(unsigned i) const
{
  assert(i<unsigned(numPorts()));
  if(port(i).isConnected()){
	  assert( port(i).value()->net() );
	  return port(i).value()->net();
  }else{
	  return nullptr;
  }
}
/*--------------------------------------------------------------------------*/
// same as Element::center?
pos_t Symbol::center()const
{itested();
	return pos_t(_cx, _cy);
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
		return std::to_string(cy());
	}else if(n=="$mfactor"){
		return "1"; // isActive in Component.
	}else if(n=="$param_display"){
		return "0";
	}else if(n=="$hflip"){
		incomplete();
		return std::to_string(_hflip);
	}else if(n=="$vflip"){
		incomplete();
		return std::to_string(_vflip);
	}else if(n=="$angle"){itested();
		return std::to_string(_angle);
	}else{ untested();
		throw ExceptionCantFind(n, label().toStdString());
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
			p->setPen(QPen(Qt::red,2));
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
	if(port(i).isConnected()){
		return port(i)->position();	
	}else{
		pos_t c(center().first, center().second); // ...
		return port(i).position() + c;
	}
}
/*--------------------------------------------------------------------------*/
// local/relative position
pos_t Symbol::portPosition(unsigned i) const
{
	auto p = port(i).position();	
	return p;
}
/*--------------------------------------------------------------------------*/
// BUG: not here. legacy stuff...
void Symbol::new_subckt()
{
	assert(!_subckt);
	_subckt = new SchematicModel(nullptr);
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
		throw ExceptionCantFind(std::to_string(i), label().toStdString());
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
		throw ExceptionCantFind(std::to_string(i), label().toStdString());
	}
}
/*--------------------------------------------------------------------------*/
void Symbol::setParameter(unsigned n, std::string const& v)
{
	if(n<Symbol::paramCount()){
		incomplete();
	}else{
		throw ExceptionCantFind(std::to_string(n), label().toStdString());
	}
}
/*--------------------------------------------------------------------------*/
void Symbol::setParameter(std::string const& name, std::string const& v)
{
	if(name == "$xposition"){
		_cx = atoi(v.c_str());
	}else if(name == "$yposition"){
		_cy = atoi(v.c_str());
	}else if(name == "$angle"){
		trace1("angle", v);
		_angle = atoi(v.c_str());
	}else if(name == "$hflip"){
		_hflip = atoi(v.c_str());
		assert(_hflip==1 || _hflip==-1);
	}else if(name == "$vflip"){
		_vflip = atoi(v.c_str());
		assert(_hflip==1 || _hflip==-1);
	}else{
		throw ExceptionCantFind(name, label().toStdString());
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
QDialog* Symbol::schematicWidget(QucsDoc* Doc) const
{ untested();
  return nullptr; // new ComponentDialog(Doc); // memory leak?
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
