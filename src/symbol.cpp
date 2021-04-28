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
#include "element_list.h"
#include "schematic_dialog.h"
#include <assert.h>
#include "net.h"
#include "node.h"
#include "geometry.h"
#include "viewpainter.h"
#include "qucs_globals.h"

#include <QVBoxLayout>
#include <QTableWidget>
#include <QComboBox>
#include <QCheckBox>
#include <QLabel>

#include "misc.h"
#include <cmath>
#include "qucsdoc.h" //BUG?
/*--------------------------------------------------------------------------*/
namespace qucs {
/*--------------------------------------------------------------------------*/
// recreate schematic symbol. not sure why, maybe after parameter changes
// (why not just call "Symbol::create??!")
void Symbol::recreate(){ // }ElementList const&){ untested();
}
/*--------------------------------------------------------------------------*/
Symbol::Symbol()
    : Component(),
		_vflip(1),
		_hflip(1),
		_angle(0),
		_param_display(0)
{
}
/*--------------------------------------------------------------------------*/
Symbol::Symbol(Symbol const& s)
    : Component(s),
		_vflip(s._vflip),
		_hflip(s._hflip),
		_angle(s._angle),
		_param_display(s._param_display)
{
	assert(s.common()==common());
}
/*--------------------------------------------------------------------------*/
// same as Element::center?
pos_t Symbol::center()const
{
	return Element::center(); // pos_t(_cx, _cy);
}
/*--------------------------------------------------------------------------*/
std::string Symbol::param_value_by_name(std::string const& n) const
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
	}else if(n=="$param_hidename"){ untested();
		return "0";
	}else if(n=="$hflip"){
		return std::to_string(_hflip);
	}else if(n=="$vflip"){
		return std::to_string(_vflip);
	}else if(n=="$angle"){
		return std::to_string(_angle);
	}else{
		return Component::param_value_by_name(n);
	}
}
/*--------------------------------------------------------------------------*/
rect_t Symbol::bounding_rect() const
{ untested();
//	if(auto p=dynamic_cast<CommonSymbol const*>(common())){
//	   return p->bouning_rect();
//	}else{
		return rect_t();
//	}
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

		if(!port(i).isConnected()){itested();
			trace4("BUG port not connected", short_label(), i, getX(nodePosition(i)), getY(nodePosition(i)));
			unreachable();
			p->setPen(QPen(Qt::green,2));
			p->drawEllipse(x-1, y-1, 2, 2);
		}else if(port(i)->numPorts()==0){ untested();
			unreachable();
			p->setPen(QPen(Qt::red,3));
			p->drawEllipse(x-2, y-2, 4, 4);
		}else if(port(i)->numPorts()==1){itested();
			// getting here in ScktPort...
#ifdef DO_TRACE
			// p->setPen(QPen(Qt::purple,3));
			// p->drawEllipse(x-2, y-2, 6, 6);
#endif
		}else if(port(i)->numPorts()==2){itested();
			// dangling port (just this and place)
			p->setPen(QPen(Qt::red,1));
			p->drawEllipse(x-2, y-2, 4, 4);
		}else if(port(i)->numPorts()>2){itested();
			p->setPen(QPen(Qt::black,2));
			p->drawEllipse(x-1, y-1, 2, 2);
		}else{itested();
#ifdef DO_TRACE
			p->setPen(QPen(Qt::blue,3));
			p->drawEllipse(x-1, y-1, 3, 3);
#endif
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
//		trace4("nodePosition pre", pp, _angle, _hflip, _vflip);
		pp = a.apply(pp);
//		trace2("nodePosition post", pp, center());
	}
	return pp + center();
}
/*--------------------------------------------------------------------------*/
Symbol::~Symbol()
{
}
/*--------------------------------------------------------------------------*/
index_t Symbol::param_count() const
{
	return 4 + Component::param_count(); // xy position hv flip  TODO?  angle mfactor ...
}
/*--------------------------------------------------------------------------*/
bool Symbol::param_is_printable(index_t i) const
{
	index_t n = Symbol::param_count() - 1 - i;
	trace5("Symbol::param_is_printable", label(), Symbol::param_count(), i, Symbol::param_name(i), n);
	switch(n){
	case 0: // x
	case 1: // y
	case 2: // hflip
	case 3: // vflip
		return true;
	default:
		return Component::param_is_printable(i);
	}
}
/*--------------------------------------------------------------------------*/
std::string Symbol::param_value(index_t i) const
{
	switch(int(Symbol::param_count()) - 1 - i){
	case 0:
		return std::to_string(cx());
	case 1:
		return std::to_string(cy());
	case 2:
		return std::to_string(_vflip);
	case 3:
		return std::to_string(_hflip);
	default: untested();
		trace2("fwd", Component::param_count(), i);
		return Component::param_value(i); // really??
	}
}
/*--------------------------------------------------------------------------*/
std::string Symbol::param_name(index_t i) const
{
	index_t n = Symbol::param_count() - 1 - i;
	switch(n){
	case 0:
		return "$xposition";
	case 1:
		return "$yposition";
	case 2:
		return "$vflip";
	case 3:
		return "$hflip";
	default:
		return Component::param_name(i);
	}
}
/*--------------------------------------------------------------------------*/
void Symbol::set_param_by_index(index_t n, std::string const& v)
{
	if(n==0){ untested();
		// xpos
		setPosition(pos_t(atoi(v.c_str()), cy()));
	}else if(n==1){ untested();
		// ypos
		setPosition(pos_t(cx(), atoi(v.c_str())));
	}else if(n==2){ untested();
		_vflip = atoi(v.c_str()); // TODO: parameters.
	}else if(n==3){ untested();
		_hflip = atoi(v.c_str()); // TODO: parameters.
	}else if(n<Symbol::param_count()){
		trace3("fail", n, v, Symbol::param_count());
		assert(false);
		unreachable();
	}else{
		return Component::set_param_by_index(n, v);
	}
}
/*--------------------------------------------------------------------------*/
void Symbol::set_param_by_name(std::string const& name, std::string const& v)
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
		return Component::set_param_by_name(name, v);
	}
}
/*--------------------------------------------------------------------------*/
// void Symbol::setParameter(QString const& name, QString const& b)
// {
// 	std::string n = name.toStdString();
// 	std::string v = b.toStdString();
// 	setParameter(n, v);
// }
// /*--------------------------------------------------------------------------*/
// void Symbol::setParameter(unsigned pos, QString const& b)
// {
// 	auto v = b.toStdString();
// 	setParameter(pos, v);
// }
/*--------------------------------------------------------------------------*/
CommonComponent::CommonComponent(const CommonComponent& p)
  :Object(p),
//   _tnom_c(p._tnom_c),
//   _dtemp(p._dtemp),
//   _temp_c(p._temp_c),
//   _mfactor(p._mfactor),
//   _value(p._value),
   _modelname(p._modelname),
//   _model(p._model),
   _attach_count(0)
{
}
/*--------------------------------------------------------------------------*/
CommonComponent::CommonComponent(int c)
  :Object(),
//   _tnom_c(NOT_INPUT),
//   _dtemp(0),
//   _temp_c(NOT_INPUT),
//   _mfactor(1),
//   _value(0),
   _modelname("(empty)"),
//   _model(0),
   _attach_count(c)
{
}
/*--------------------------------------------------------------------------*/
CommonComponent::~CommonComponent()
{
  trace1("common,destruct", _attach_count);
  if(_attach_count == 0 || _attach_count == CC_STATIC_){
  }else{
	  unreachable(); // BUG
  }
}
/*--------------------------------------------------------------------------*/
void CommonComponent::attach_common(CommonComponent*c, CommonComponent**to)
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
  }else{
    // The new and old are identical.
    // Use the old one.
    // The new one is also used somewhere else, so keep it.
  }
}
/*--------------------------------------------------------------------------*/
void CommonComponent::detach_common(CommonComponent** from)
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
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
ElementList const* Symbol::symbolPaintings() const
{
	unreachable(); // obsolete
	return nullptr;
}
/*--------------------------------------------------------------------------*/
Widget* Symbol::schematicWidget(Doc* Doc) const
{ untested();
	trace0("Symbol::schematicWidget");

	Object const* w = widget_dispatcher["SymbolDialog"];
	auto ww = dynamic_cast<Widget const*>(w);

	assert(ww);
	Widget* clone = ww->clone(); // BUG. not here.
	assert(clone);
	return clone;
}
/*--------------------------------------------------------------------------*/
} // qucs
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
