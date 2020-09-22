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
#include "schematic_model.h"
#include "component.h"
#include "globals.h"
#include "module.h"

namespace{
class SubCirPort : public Component  {
private:
  SubCirPort(SubCirPort const& p)
	  : Component(p){

		 // BUG: do in Component??
		 // (this does not work either way)
		  for(auto i : p.Lines){ untested();
			 Lines.append(new Line(*i));
		  }


	  }
public:
  SubCirPort();
 ~SubCirPort() {};
  Component* newOne() {untested(); return new SubCirPort(*this);}
  Element* clone() const {untested(); return new SubCirPort(*this);}
  static Element* info(QString&, char* &, bool getNewOne=false);

private:
  Node* connectNode(unsigned n, NodeMap& l) override;

private:
  void setParameter(unsigned i, std::string const&) override;

protected:
  QString netlist() const;
  QString vhdlCode(int);
  QString verilogCode(int);
  void createSymbol();
}D;
Dispatcher<Symbol>::INSTALL p(&symbol_dispatcher, "Port", &D);
Module::INSTALL pp("lumped", &D);

SubCirPort::SubCirPort() : Component()
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
Node* SubCirPort::connectNode(unsigned i, NodeMap& l)
{
	Node* N = Symbol::connectNode(i, l);
	trace2("cn", i, N);

	bool ok=true;
	QString pp;
	try{
		pp = QString::fromStdString(paramValue(0));
	}catch(ExceptionCantFind const&){ untested();
		ok = false;
	}

	int pos;
	if(ok){
		pos = pp.toInt(&ok);
	}else{ untested();
	}

	--pos; // QUCS numbers start at 1.

	if(ok){
		assert(scope());
		trace2("setting port", pos, N->netLabel());
		scope()->setPort(pos, N);
	}else{ untested();
		incomplete();
	}
	return N;
}
// -------------------------------------------------------
void SubCirPort::createSymbol()
{
  x1 = -27; y1 = -8;
  x2 =   0; y2 =  8;

  if(Props.at(1)->Value.at(0) == 'a') { untested();
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

  Ports.append(new Port(  0,  0));
}
// -------------------------------------------------------
void SubCirPort::setParameter(unsigned n, std::string const& vv)
{
	trace3("SubCirPort::setParameter", label(), n, vv);
	Component::setParameter(n, vv);
	QString v = QString::fromStdString(vv);

	bool ok;
	int pos = v.toInt(&ok);

	--pos; // QUCS numbers start at 1.
	if(!ok){
		incomplete();
		// throw approriate error
	}else if(portExists(pos)){ untested();
		// possibly missing more error handling
		assert(scope());
		assert(owner());
		trace2("setting scope port", owner()->label(), pos);
		scope()->setPort(pos, port(pos).value());
	}else{
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
