/***************************************************************************
                             qucsator.cc
                              ----------
    begin                : yes
    copyright            : (C) 2015 by Felix Salfelder
    email                : felix@salfelder.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "sim.h"
#include "../components/component.h"
#include "node.h"
#include <QString>

namespace {
// qucslang language implementation
class QucsLang : public NetLang
{
  virtual void printInstance(Component const*, QTextStream&) const;
};

/*!
 * print Components in qucs language
 */
void QucsLang::printInstance(Component const* c, QTextStream& s) const
{
  if(c->isOpen()) {
    // nothing.
  }else if(c->isShort()){
    // replace by some resistors (hack?)
    int z=0;
    QListIterator<Port *> iport(c->ports());
    Port *pp = iport.next();
    QString Node1 = pp->Connection->Name;
    while (iport.hasNext()){
      s << "R:" << c->label() << "." << QString::number(z++) << " "
	<< Node1 << " " << iport.next()->Connection->Name << " R=\"0\"\n";
    }
  }else{
    QString netlist(c->getNetlist());
    if(netlist!="obsolete") {
      // still using obsolete netlister here.
      // other languages must throw here!
      qDebug() << "incomplete, using netlist()";
      s << netlist;
    }else{ // normal netlisting
      s << c->type() << ":" << c->label();

      // output all node names
      for(Port *p1 : c->ports()){
	s << " " << p1->Connection->Name;
      }

      for(auto p2 : c->params()) {
	if(p2->Name != "Symbol") { // hack.
	  s << " " << p2->Name << "=\"" << p2->Value << "\"";
	}
      }
      s << '\n';
    }
  }
}

static QucsLang qucslang;

// qucsator simulator backend
class Qucsator : public Simulator
{
  NetLang const* netLang() const{return &qucslang;}
};

static Qucsator Q;
static Dispatcher<Simulator> p("qucsator", &Q);
}

// vim:ts=8:sw=2:noet
