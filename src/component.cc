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
/*--------------------------------------------------------------------------*/
namespace qucs{
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
  attach_common(s._common);
  setTypeName(s.typeName());
}
/*--------------------------------------------------------------------------*/
Component::~Component()
{
	detach_common();
	delete _subckt;
	_subckt = nullptr;
}
/*--------------------------------------------------------------------------*/
index_t Component::param_count() const
{
	return 0; // for now.
}
/*--------------------------------------------------------------------------*/
std::string Component::param_value(index_t i) const
{
	throw qucs::ExceptionCantFind(label(), std::to_string(i), "param values");
}
/*--------------------------------------------------------------------------*/
std::string Component::param_value_by_name(std::string const& n) const
{
	throw qucs::ExceptionCantFind(label(), n, "params");
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
	incomplete(); // ask common
	return "incomplete";
}
/*--------------------------------------------------------------------------*/
void Component::set_param_by_index(index_t n, std::string const& v)
{
	throw qucs::ExceptionCantFind(label(), std::to_string(n), "params");
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
	throw qucs::ExceptionCantFind(label(), name, "params");
}
/*--------------------------------------------------------------------------*/
bool Component::paramIsPrintable() const
{
	return true;
  //if (has_common()) {
  //  return common()->param_is_printable(i);
  //}else{
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
		if (new_type != typeName()) {
			CommonComponent* c = common()->clone();
			assert(c);
			c->set_modelname(new_type);
			attach_common(c);
		}else{
		}
	}else{
		assert(false); // for now
		// CARD::set_dev_type(new_type);
	}
}
/*--------------------------------------------------------------------------*/
} // qucs
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
