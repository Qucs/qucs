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

#include "globals.h"
#include "sckt_base.h"

namespace{
class sda : public SubcktBase  {
public:
  explicit sda() : SubcktBase () {
  }
  explicit sda(sda const&x) : SubcktBase (x) {
    new_subckt(); // really?

    auto a = symbol_dispatcher.clone("subckt_proto");
    assert(a);
    a->setLabel("main");
    a->setOwner(this);
    subckt()->pushBack(a);

    // legacy Lib components in this circuit.
    _sub = symbol_dispatcher.clone("LegacySub");
    assert(a);
    _sub->setLabel("Sub");
    _sub->setOwner(this);
    subckt()->pushBack(_sub);
  }
  ~sda(){
  }
private: // Symbol
  Symbol* clone() const{return new sda(*this);}
  void setParameter(std::string const& n, std::string const& v){
    if(n == "$filename"){
		 // BUG. only pass prefix
      _sub->setParameter("$SUB_PATH", v);
    }else{ untested();
      SubcktBase::setParameter(n, v);
    }
  }

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
      qDebug() << "unknown parameter" << QString::fromStdString(x);
      incomplete();
      return "unknown";
      throw ExceptionCantFind(x, "main");
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

public:
  SchematicModel* subckt(){return _subckt;}
  // Commandlist* commands(){ ... }

private:
  Symbol* _sub;
}d0;
static Dispatcher<Symbol>::INSTALL p(&symbol_dispatcher, "schematic_root", &d0);
}
