/***************************************************************************
    copyright            : (C) 2003 by Michael Margraf
                               2020 Felix Salfelder
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include "node.h"
#include "nodemap.h"
#include "element_list.h"
#include "component.h"
#include "qucs_globals.h"
#include "module.h"
#include "sckt_base.h"

namespace{
using qucs::SubcktBase;

class SubCirPort : public Component /*Symbol*/  {
private:
  SubCirPort(SubCirPort const& p)
	  : Component(p), _pos(p._pos){

		 // BUG: do in Component??
		 // (this does not work either way)
		  for(auto i : p.Lines){
			 Lines.append(new Line(*i));
		  }
	  }
public:
  SubCirPort();
 ~SubCirPort() {};
  Component* newOne() {untested(); return new SubCirPort(*this);}
  Element* clone() const { return new SubCirPort(*this);}
  static Element* info(QString&, char* &, bool getNewOne=false);

private: // Symbol
  void set_port_by_index(index_t, std::string const&) override;
//  unsigned numPorts() const override{
//	  return 1;
//  }

private:
//  Node* connectNode(unsigned n, NodeMap& l) override;
  bool useObsoleteProps() const override{ return true; }

private:
  void setParameter(unsigned i, std::string const&) override;
  std::string paramName(unsigned i) const override;
  std::string paramValue(unsigned i) const override;
  unsigned paramCount() const override{
	  return Symbol::paramCount() + num_component_params + 2;
  }

protected:
  QString netlist() const;
  QString vhdlCode(int);
  QString verilogCode(int);
  void createSymbol();
private:
  int _pos;
  std::string _some_type; // domain? discipline?
}D;
Dispatcher<Symbol>::INSTALL p(&symbol_dispatcher, "Port", &D);
Module::INSTALL pp("lumped", &D);

SubCirPort::SubCirPort() : Component(), _pos(1)
{
  info(Name, bitmap_file);
  Type = isComponent;   // both analog and digital
  Description = QObject::tr("port of a subcircuit");

  // This property must be the first one !
  Props.append(new Property("Num", "1", true,
		QObject::tr("number of the port within the subcircuit")));
  // This property must be the second one !
  Props.append(new Property("Type", "analog", false,
		QObject::tr("type of the port (for digital simulation only)")
		+" [analog, in, out, inout]"));

  createSymbol();
  tx = x1+4;
  ty = y2+4;
  Model = "Port";
  Name  = "P";
}

// -------------------------------------------------------
void SubCirPort::set_port_by_index(index_t i, std::string const& value)
{
   if(value==""){
		incomplete();
	}else{
	}
	
	if(i){
		incomplete(); // ?
	}else{
	}

	bool ok=true;
	QString pp;
	try{
		// don't know why this is a parameter and not a local variable
		// (does it make any sense?)
		pp = QString::fromStdString(paramValue(num_component_params + Symbol::paramCount()));
	}catch(qucs::ExceptionCantFind const&){ untested();
		ok = false;
	}

	int pos;
	if(ok){
		pos = pp.toInt(&ok);
	}else{ untested();
	}

	--pos; // QUCS numbers start at 1.

	auto so = dynamic_cast<SubcktBase*>(owner());

	assert(scope());
	assert(scope()->nodes());
	auto& nm = *scope()->nodes();

   if(value == ""){
		Node* n = port(0).value();
		Node* m = port(1).value();
		if( n && m){
			nm.removeEdge(m, n);
			Symbol::set_port_by_index(0, "");
			Symbol::set_port_by_index(1, "");
		}else{
			unreachable();
		}
	}else if(so){
		assert(scope());
		assert(scope()==so->scope());
		trace2("port::spbi", pos, label());
		so->set_port_by_index(pos, label());

		Symbol::set_port_by_index(0, label());
		Symbol::set_port_by_index(1, value);


		Node* n = port(0).value();
		Node* m = port(1).value();
		nm.addEdge(m, n);
		n->setNetLabel(label());

//		addedge?
	}else{ untested();
		incomplete();
	}
}
// -------------------------------------------------------
void SubCirPort::createSymbol()
{
  x1 = -27; y1 = -8;
  x2 =   0; y2 =  8;

  if(Props.at(1)->Value.at(0) == 'a') {
    Arcs.append(new Arc(-25, -6, 12, 12,  0, 16*360,QPen(Qt::darkBlue,2)));
    Lines.append(new Line(-13,  0,  0,  0,QPen(Qt::darkBlue,2)));
  }else{ untested();
    Lines.append(new Line( -9,  0,  0,  0,QPen(Qt::darkBlue,2)));
    if(Props.at(1)->Value == "out") { untested();
      Lines.append(new Line(-20, -5,-25,  0,QPen(Qt::red,2)));
      Lines.append(new Line(-20,  5,-25,  0,QPen(Qt::red,2)));
      Lines.append(new Line(-20, -5, -9, -5,QPen(Qt::red,2)));
      Lines.append(new Line(-20,  5, -9,  5,QPen(Qt::red,2)));
      Lines.append(new Line( -9, -5, -9,  5,QPen(Qt::red,2)));
    }else{ untested();
      Lines.append(new Line(-14, -5, -9,  0,QPen(Qt::darkGreen,2)));
      Lines.append(new Line(-14,  5, -9,  0,QPen(Qt::darkGreen,2)));
      if(Props.at(1)->Value == "in") { untested();
        Lines.append(new Line(-25, -5,-14, -5,QPen(Qt::darkGreen,2)));
        Lines.append(new Line(-25,  5,-14,  5,QPen(Qt::darkGreen,2)));
        Lines.append(new Line(-25, -5,-25,  5,QPen(Qt::darkGreen,2)));
      }else{ untested();
        x1 = -30;
        Lines.append(new Line(-18, -5,-14, -5,QPen(Qt::darkGreen,2)));
        Lines.append(new Line(-18,  5,-14,  5,QPen(Qt::darkGreen,2)));
        Lines.append(new Line(-23, -5,-28,  0,QPen(Qt::red,2)));
        Lines.append(new Line(-23,  5,-28,  0,QPen(Qt::red,2)));
        Lines.append(new Line(-23, -5,-18, -5,QPen(Qt::red,2)));
        Lines.append(new Line(-23,  5,-18,  5,QPen(Qt::red,2)));
      }
    }
  }

  Ports.append(new ComponentPort(  0,  0));
  Ports.append(new ComponentPort(  0,  0));
}
// -------------------------------------------------------
std::string SubCirPort::paramName(unsigned n) const
{
	if(n == num_component_params + Symbol::paramCount()){
		return "Num";
	}else if(n== num_component_params + Symbol::paramCount() + 1){
		return "Type";
	}else{
		return Component::paramName(n);
	}
}
/*--------------------------------------------------------------------------*/
std::string SubCirPort::paramValue(unsigned n) const
{
//	this is not correct.
	if(n == num_component_params + Symbol::paramCount()){
		trace1("SubCirPort::paramValue", _pos);
		return std::to_string(_pos);
	}else if(n==num_component_params + Symbol::paramCount() + 1){
		return _some_type;
	}else{
		return Component::paramValue(n);
	}
}
// -------------------------------------------------------
void SubCirPort::setParameter(unsigned n, std::string const& vv)
{
	trace3("SubCirPort::setParameter", label(), n, vv);
	Component::setParameter(n, vv); // noop?
	QString v = QString::fromStdString(vv);

	if(n==num_component_params + Symbol::paramCount()){
		bool ok;
		int pos = v.toInt(&ok);
		trace1("SubCirPort::setParameter portno", pos);

		--pos; // QUCS numbers start at 1.
		if(!ok){
			incomplete();
			throw qucs::Exception("can't parse " + vv);
			// throw approriate error
		}else if(portExists(pos)){
			// possibly missing more error handling
			assert(scope());
			assert(owner());
			trace3("setting scope port", owner()->label(), pos, port(pos).value());
			auto sb = prechecked_cast<SubcktBase*>(owner());
			assert(sb);
			sb->set_port_by_index(pos, port(pos).nodeLabel());
			_pos = pos+1;
		}else{
			_pos = pos+1;
		}
	}else if(n==1 + num_component_params + Symbol::paramCount()){
		_some_type = vv;
	}else{
		incomplete();
	}
}
// -------------------------------------------------------
Element* SubCirPort::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
	unreachable();
  Name = QObject::tr("Subcircuit Port");
  BitmapFile = (char *) "subport";

  if(getNewOne)  return new SubCirPort();
  return 0;
}

// -------------------------------------------------------
QString SubCirPort::netlist() const
{ untested();
  return QString("");
}

// -------------------------------------------------------
QString SubCirPort::vhdlCode(int)
{ untested();
    return QString("");
#if 0
  if(Props.at(1)->Value != "out")
    return QString("");

  // Insert dummy buffer to avoid reading from an output port.
  QString s = "  net_out";
  Node *pn = Ports.first()->Connection;
  s += pn->name() + " <= ";
  s += pn->name() + ";\n";
  return s;
#endif
}

// -------------------------------------------------------
QString SubCirPort::verilogCode(int)
{ untested();
  return QString("");
}
} // namespace
