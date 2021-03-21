/***************************************************************************
    copyright            : (C) 2020, 2021 Felix Salfelder
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "data.h"
#include "factory.h"
#include "qucs_globals.h"
#include "element_list.h"
#include "sckt_base.h"
#include "widget.h"
/*--------------------------------------------------------------------------*/
namespace{
/*--------------------------------------------------------------------------*/
using qucs::element_dispatcher;
using qucs::ElementList;
using qucs::Node;
using qucs::SubcktBase;
using qucs::ViewPainter;
using qucs::Widget;
/*--------------------------------------------------------------------------*/
class sda : public SubcktBase  {
public:
  explicit sda() : SubcktBase () {
  }
  explicit sda(sda const&x) : SubcktBase (x) {
    new_subckt();

    auto a = qucs::device_dispatcher.clone("subckt_proto");
    assert(a);
    a->set_label("main");
    a->set_owner(this);
    subckt()->push_back(a);

		// Sub components in this circuit.
		// instance used when parsing a netlist.
		_sub = element_dispatcher.clone("LegacySub");
		if(!_sub){ // legacy
			Component* sub = qucs::symbol_dispatcher.clone("LegacySub");

			assert(sub);
			sub->set_label("Sub");
			sub->set_dev_type("Sub"); // why?
			_sub = sub;
		}else{
			_sub->set_label("Sub"); // why?
		}

		assert(a);
		_sub->set_owner(this);
		auto f = dynamic_cast<SymbolFactory*>(_sub);
		assert(f);
		f->_scope = scope();
		subckt()->push_back(_sub);

//		assert(_sub->common());
//		assert(_sub->common()->modelname()=="Sub");

#if 1
		// dat file access...
		// TODO: default according to properties in .sch file?
		_dat = qucs::data_dispatcher.clone("datfiles");
		assert(a);
		_dat->set_label("datfiles");
		subckt()->push_back(_dat);
#endif
  }
  ~sda(){}

private: // Component
  Component* clone() const{return new sda(*this);}
  std::string param_value_by_name(std::string const& n) const override{
	  if(n == "$filename"){ untested();
		  return _full_path;
	  }else{ untested();
		  throw qucs::ExceptionCantFind(n, label());
	  }
  }
  void set_param_by_name(std::string const& n, std::string const& v) override{
    if(n == "$filename") {
		 _full_path = v;
		 auto pos = v.find_last_of("/");
		 if(pos==std::string::npos){
			 // something wrong.
			 assert(false);
		 }else if(dynamic_cast<Component const*>(_sub)){
			 // legacy hack
			 _sub->set_param_by_name("$SUB_PATH", v);
		 }else{
			 _sub->set_param_by_name("$SUB_PATH", v.substr(0, pos)); // TODO: handle include paths...
		 }
      _dat->set_param_by_name("$schematic_filename", v);
    }else{ untested();
      SubcktBase::set_param_by_name(n, v);
    }
  }

private: // SchematicSymbol
  ElementList* scope(){
    return subckt();
  }
  // bug, forward to schematic object (it doesn't exist yet).
  std::string paramValue(std::string const&x) const{
    if(x=="DocName"){
      unreachable();
		return "???";
    }else if(x=="ViewX1"){
      incomplete(); // there is no view.
      return "0";
    }else if(x=="ViewX2"){
      incomplete(); // there is no view.
      return "0";
    }else if(x=="ViewY1"){
      incomplete(); // there is no view.
      return "0";
    }else if(x=="ViewY2"){
      incomplete(); // there is no view.
      return "0";
    }else if(x=="Scale"){
      incomplete(); // there is no view.
      return "0";
    }else if(x=="GridOn"){
      incomplete(); // there is no view.
      return "0";
    }else if(x=="GridX"){
      incomplete(); // there is no view.
      return "0";
    }else if(x=="GridY"){
      incomplete(); // there is no view.
      return "0";
    }else if(x=="tmpViewX1"){
      incomplete(); // there is no view.
      return "0";
    }else{
      incomplete();
      return "unknown";
      throw qucs::ExceptionCantFind(x, "main");
    }
  }

private:
  unsigned numPorts() const override{ incomplete(); return 0; }
  virtual Port& port(unsigned) override{ return *new Port();}
  pos_t portPosition(unsigned) const override{
    unreachable();
    return pos_t(0,0);
  }
  Node const* portValue(unsigned)const {unreachable(); return nullptr;}
  void setPort(unsigned, Node*){incomplete();}

  Widget* schematicWidget(qucs::Doc*) const { untested();
	  trace0("root::schematicWidget");

	  Widget const* w = qucs::widget_dispatcher["SchematicDoc"];
	  auto ww = dynamic_cast<Widget const*>(w);

	  assert(ww);
	  Widget* clone = ww->clone();
	  assert(clone);
	  return clone;
  }


private:
 	Element* _sub{nullptr};
	Data* _dat;
	std::string _full_path;
}d0;
static Dispatcher<qucs::Component>::INSTALL p(&qucs::device_dispatcher, "schematic_root", &d0);
/*--------------------------------------------------------------------------*/
} // namespace
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
