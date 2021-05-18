/***************************************************************************
    copyright            : (C) 2020 Felix Salfelder
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
// Base class for components, i.e. devices with ports and parameters
/*--------------------------------------------------------------------------*/
#include "component.h"
#include "element_list.h"
#include "node.h"
#include "library.h"
/*--------------------------------------------------------------------------*/
namespace qucs{
/*--------------------------------------------------------------------------*/
	// l_pmatch.cc
inline bool Umatch(const std::string& str1, const std::string& str2)
{
  CS cmd(CS::_STRING, str1); //call to CS member on string
  if (cmd.umatch(str2)) {
    return true;
  }else{
    return 0;
  }
}
/*--------------------------------------------------------------------------*/
void CommonComponent::set_param_by_index(index_t i, std::string const& Value) // , int Offset)
{
  switch (i) {
  case 0:untested();  _tnom_c = Value; break;
  case 1:untested();  _dtemp = Value; break;
  case 2:untested();  _temp_c = Value; break;
  case 3:  _mfactor = Value; break;
  default:untested(); throw Exception_Too_Many(i, 3, 0); break;
  }
}
/*--------------------------------------------------------------------------*/
std::string CommonComponent::param_name(index_t i)const
{
  switch (i) {
  case 0:  return "tnom";
  case 1:  return "dtemp";
  case 2:  return "temp";
  case 3:  return "m";
  default: return "";
  }
}
/*--------------------------------------------------------------------------*/
void CommonComponent::set_param_by_name(std::string const& Name, std::string const& Value)
{
//  if (has_parse_params_obsolete_callback()) {untested();
//    std::string args(Name + "=" + Value);
//    CS cmd(CS::_STRING, args); //obsolete_callback
//    bool ok = parse_params_obsolete_callback(cmd); //BUG//callback
//    if (!ok) {untested();
//      throw Exception_No_Match(Name);
//    }else{untested();
//    }
//  }else
	{
		//BUG// ugly linear search
		for (int i = param_count() - 1;  i >= 0;  --i) {
//			for (int j = 0;  param_name(i,j) != "";  ++j) {
				if (Umatch(Name, param_name(i) + ' ')) {
					set_param_by_index(i, Value); // (, 0/*offset*/);
					return; //success
				}else{
					//keep looking
				}
//			}
		}
		untested();
		throw qucs::ExceptionNoMatch(Name);
	}
}
/*--------------------------------------------------------------------------*/
std::string CommonComponent::param_value(index_t i)const
{
  switch (i) {
  case 0:untested();  return _tnom_c.string();
  case 1:untested();  return _dtemp.string();
  case 2:untested();  return _temp_c.string();
  case 3:  return _mfactor.string();
  default:untested(); return "";
  }
}
/*--------------------------------------------------------------------------*/
std::string CommonComponent::param_value_by_name(std::string const& n)const
{
	incomplete();
	return "ccincomplete_"+n;
	throw qucs::ExceptionCantFind();
}
/*--------------------------------------------------------------------------*/
bool CommonComponent::param_is_printable(index_t i)const
{
  switch (i) {
  case 0:  return _tnom_c.has_hard_value();
  case 1:  return _dtemp.has_hard_value();
  case 2:  return _temp_c.has_hard_value();
  case 3:  return _mfactor.has_hard_value();
  default:untested(); return false;
  }
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
Component::Component()
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
Component::Component(Component const& s)
    : Element(s),
		_subckt(nullptr),
		_vflip(s._vflip),
		_hflip(s._hflip),
		_angle(s._angle),
		_param_display(s._param_display),
		_common(nullptr),
		_net_nodes(s._net_nodes)
{
	// uninit?
  attach_common(s._common);
//  set_dev_type(s.typeName());
}
/*--------------------------------------------------------------------------*/
Component::~Component()
{
	detach_common();
	delete _subckt;
	_subckt = nullptr;
}
/*--------------------------------------------------------------------------*/
std::string Component::param_value(index_t i) const
{
  if (has_common()) {
    return common()->param_value(i);
  }else{
    switch (Component::param_count() - 1 - i) {
//    case 0:  return value().string();
//    case 1:  return _mfactor.string();
    default:untested(); return Element::param_value(i);
    }
  }
//	throw qucs::ExceptionCantFind(label(), std::to_string(i), "param values");
}
/*--------------------------------------------------------------------------*/
std::string Component::param_value_by_name(std::string const& n) const
{
  if (has_common()) {
    return common()->param_value_by_name(n);
  }else{itested();
	throw qucs::ExceptionCantFind(label(), n, "params");
  }
}
/*--------------------------------------------------------------------------*/
void Component::set_port_by_name(std::string const&, std::string const&)
{
	incomplete();
	assert(false);
}
/*--------------------------------------------------------------------------*/
std::string Component::param_name(index_t i) const
{
  if (has_common()) {
    return common()->param_name(i);
  }else{
    switch (Component::param_count() - 1 - i) {
//    case 0:  return value_name();
//    case 1:  return "m";
    default:untested(); return Element::param_name(i);
    }
  }
	incomplete(); // ask common
	return "incomplete";
	throw qucs::ExceptionCantFind(label(), std::to_string(i), "param names");
}
/*--------------------------------------------------------------------------*/
void Component::set_param_by_index(index_t i, std::string const& Value)
{
  if (has_common()) {untested();
    CommonComponent* c = common()->clone();
    assert(c);
    c->set_param_by_index(i, Value); // (, offset);
    attach_common(c);
  }else{
    switch (Component::param_count() - 1 - i) {
//    case 0: _value = Value; break;
//    case 1:untested(); _mfactor = Value; break;
    default:untested(); Element::set_param_by_index(i, Value); // (, offset);
    }
  }
//	throw qucs::ExceptionCantFind(label(), std::to_string(n), "params");
}
/*--------------------------------------------------------------------------*/
void Component::new_subckt()
{
	assert(!_subckt);
	_subckt = new ElementList();
}
/*--------------------------------------------------------------------------*/
void Component::set_param_by_name(std::string const& name, std::string const& v)
{
  if (has_common()) {
    CommonComponent* c = common()->clone();
    assert(c);
    c->set_param_by_name(name, v);
    attach_common(c);
  }else{
	  throw qucs::ExceptionCantFind(label(), name, "params");
  }
}
/*--------------------------------------------------------------------------*/
bool Component::param_is_printable(index_t i) const
{
  if (has_common()) {
    return common()->param_is_printable(i);
  }else{
    return true;
  }
  //  switch (COMPONENT::param_count() - 1 - i) {
  //  case 0:  return value().has_hard_value();
  //  case 1:  return _mfactor.has_hard_value();
  //  default:untested(); return CARD::param_is_printable(i);
  //  }
  //}
}
/*--------------------------------------------------------------------------*/
Port const& Component::port(index_t i) const
{
	Component* s=const_cast<Component*>(this);
	return s->port(i);
}
/*--------------------------------------------------------------------------*/
void Component::set_port_by_index(index_t num, std::string const& ext_name)
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
		throw qucs::Exception_Too_Many(num+1, max_nodes(), 0/*offset*/);
	}
}
/*--------------------------------------------------------------------------*/
Node const* Component::portNode(index_t i) const
{ untested();
  assert(i<index_t(numPorts()));
  if(port(i).isConnected()){ untested();
	  assert( port(i).value() );
	  return port(i).value();
  }else{ untested();
	  return nullptr;
  }
}
/*--------------------------------------------------------------------------*/
Node const* Component::portValue(index_t i) const
{
  assert(i<index_t(numPorts()));
  if(port(i).isConnected()){
	  assert( port(i).value() );
	  return port(i).value();
  }else{
	  return nullptr;
  }
}
/*--------------------------------------------------------------------------*/
std::string Component::port_value(index_t i) const
{
	trace2("port_value", label(), i);
	Port const& p = port(i);
	return p.nodeLabel();
}
/*--------------------------------------------------------------------------*/
void Component::set_dev_type(const std::string& new_type)
{
	if (common()) {
		trace2("Component::set_dev_type common", dev_type(), new_type);
		if (new_type != dev_type()) {
			trace2("Component::set_dev_type", short_label(), new_type);
			CommonComponent* c = common()->clone();
			assert(c);
			c->set_modelname(new_type);
			attach_common(c);
		}else{
		}
	}else{
		trace1("incomplete? set_dev_type", short_label());
		Element::set_dev_type(new_type);
	}
	trace2("Component::set_dev_type done", short_label(), dev_type());
}
/*--------------------------------------------------------------------------*/
bool CommonComponent::operator==(const CommonComponent& x)const
{
	trace2("cc op==", _modelname, x._modelname);
  return true
	  && _modelname == x._modelname;
//	  && _model == x._model
//	  && _tnom_c == x._tnom_c
//	  && _dtemp == x._dtemp
//	  && _temp_c == x._temp_c
//	  && _mfactor == x._mfactor
//	  && _value == x._value);
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
} // qucs
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
