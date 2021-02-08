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

#include "qucs_globals.h"
#include "data.h"
#include "sckt_base.h"
#include "factory.h"
/*--------------------------------------------------------------------------*/
namespace{
/*--------------------------------------------------------------------------*/
class sda : public SubcktBase  {
public:
  explicit sda() : SubcktBase () {
  }
  explicit sda(sda const&x) : SubcktBase (x) {
    new_subckt();

    auto a = symbol_dispatcher.clone("subckt_proto");
    assert(a);
    a->setLabel("main");
    a->set_owner(this);
    subckt()->push_back(a);

		// Sub components in this circuit.
		// instance used when parsing a netlist.
		_sub = symbol_dispatcher.clone("LegacySub");
		assert(a);
		_sub->setLabel("Sub");
		_sub->set_owner(this);
		auto f = dynamic_cast<SymbolFactory*>(_sub);
		assert(f);
		f->_scope = scope();
		subckt()->push_back(_sub);

#if 1
		// dat file access...
		// TODO: default according to properties in .sch file?
		_dat = dataDispatcher.clone("datfiles");
		assert(a);
		_dat->setLabel("datfiles");
		subckt()->push_back(_dat);
#endif
  }
  ~sda(){}

private: // Symbol
  Symbol* clone() const{return new sda(*this);}
  void setParameter(std::string const& n, std::string const& v){
    if(n == "$filename"){
      _sub->setParameter("$SUB_PATH", v); // BUG. only pass search path
      _dat->set_param_by_name("$schematic_filename", v);
    }else{ untested();
      SubcktBase::setParameter(n, v);
    }
  }

private: // BUG? a SubcktBase is a Painting...
  virtual rect_t bounding_rect() const override{ unreachable(); }
  virtual void paint(ViewPainter*) const override{ unreachable(); }

private: // SchematicSymbol
  SchematicModel* scope(){
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

private:
  Symbol* _sub;
  Data* _dat;
}d0;
static Dispatcher<Symbol>::INSTALL p(&symbol_dispatcher, "schematic_root", &d0);
/*--------------------------------------------------------------------------*/
} // namespace
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
