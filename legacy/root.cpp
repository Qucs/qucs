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
using qucs::SymbolFactory;
using qucs::ViewPainter;
using qucs::Widget;
/*--------------------------------------------------------------------------*/
class SchematicRoot : public SubcktBase  {
public:
	explicit SchematicRoot() : SubcktBase () {
	}
	explicit SchematicRoot(SchematicRoot const&x) : SubcktBase (x) {
		new_subckt();

		auto mm = qucs::device_dispatcher.clone("subckt_proto");
		assert(mm);
		mm->set_label("main");
		mm->set_owner(this);
		subckt()->push_back(mm);

		// Sub components in this circuit.
		// instance used when parsing a netlist.
		// get all from model_dispatcher?
		{
			_sub = element_dispatcher.clone("ModelFactory");
			if(_sub){
				_sub->set_owner(this);
				_sub->set_dev_type("Sub");
				subckt()->push_back(_sub);
			}else{
			}

			_verilog = element_dispatcher.clone("ModelFactory");
			if(_verilog){
				_verilog->set_owner(this);
				try{
					_verilog->set_dev_type("Verilog");
					subckt()->push_back(_verilog);
				}catch(qucs::ExceptionCantFind const&){ itested();
					message(qucs::MsgLog, "no Verilog");
				}
			}else{
			}
		}

#if 1
		// dat file access...
		// TODO: default according to properties in .sch file?
		_dat = qucs::data_dispatcher.clone("datfiles");
		assert(mm);
		_dat->set_label("datfiles");
		subckt()->push_back(_dat);
#endif
  }
  ~SchematicRoot(){}

private: // Component
  Component* clone() const{return new SchematicRoot(*this);}
  std::string param_value_by_name(std::string const& n) const override{ untested();
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
		 std::string p = v;
		 if(pos==std::string::npos){ untested();
			 p = "."; // something wrong?
		 }else{
			 p = v.substr(0, pos);
		 }

       if(_sub){
			 _sub->set_param_by_name("$SUB_PATH", p); // TODO: handle include paths...
		 }else{
		 }

		 if(_verilog){
			 _verilog->set_param_by_name("$SUB_PATH", p); // TODO: INCLUDE_PATH
		 }else{
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
  std::string paramValue(std::string const&x) const{ untested();
    if(x=="DocName"){ untested();
      unreachable();
		return "???";
    }else if(x=="ViewX1"){ untested();
      incomplete(); // there is no view.
      return "0";
    }else if(x=="ViewX2"){ untested();
      incomplete(); // there is no view.
      return "0";
    }else if(x=="ViewY1"){ untested();
      incomplete(); // there is no view.
      return "0";
    }else if(x=="ViewY2"){ untested();
      incomplete(); // there is no view.
      return "0";
    }else if(x=="Scale"){ untested();
      incomplete(); // there is no view.
      return "0";
    }else if(x=="GridOn"){ untested();
      incomplete(); // there is no view.
      return "0";
    }else if(x=="GridX"){ untested();
      incomplete(); // there is no view.
      return "0";
    }else if(x=="GridY"){ untested();
      incomplete(); // there is no view.
      return "0";
    }else if(x=="tmpViewX1"){ untested();
      incomplete(); // there is no view.
      return "0";
    }else{ untested();
      incomplete();
      return "unknown";
      throw qucs::ExceptionCantFind(x, "main");
    }
  }

private:
  unsigned numPorts() const override{ incomplete(); return 0; }
  virtual Port& port(unsigned) override{ return *new Port();}
#if 0
  pos_t portPosition(unsigned) const override{ untested();
    unreachable();
    return pos_t(0,0);
  }
#endif
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


private: // BUG
  Element* _sub{nullptr};
  Element* _verilog{nullptr};

private:
	Data* _dat;
	std::string _full_path;
}d0;
static Dispatcher<qucs::Component>::INSTALL p(&qucs::device_dispatcher, "schematic_root", &d0);
/*--------------------------------------------------------------------------*/
} // namespace
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
