/***************************************************************************
                              verilog.cc
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
#include "node.h"
#include <QString>
#include "globals.h"
#include "command.h"

namespace {
class Verilog : public NetLang
{
  virtual void printCommand(Command const*, QTextStream&) const;
  virtual void printSymbol(Symbol const*, QTextStream&) const;
} V;

static Dispatcher<DocumentLanguage>::INSTALL p(&doclang_dispatcher, "verilog", &V);

/*!
 * verilog does not know about commands
 */
void Verilog::printCommand(Command const* c, QTextStream& s) const
{
  s << "//" << c->label() << "\n";
}

/*!
 * print Components in Verilog
 */
void Verilog::printSymbol(Symbol const* sym, QTextStream& s) const
{ untested();
  Component const* c=dynamic_cast<Component const*>(sym);
  if(!c){
    incomplete();
    return;
  }else if(c->isOpen()) { untested();
    unreachable(); // wrong place.
  }else if(c->isShort()){ untested();
    // replace by some resistors (hack?)
    int z=0;
    QListIterator<Port *> iport(c->ports());
    Port *pp = iport.next();
    QString Node1 = pp->Connection->name();
    while (iport.hasNext()){
      s << "R:" << c->label() << "." << QString::number(z++) << " "
	<< Node1 << " " << iport.next()->Connection->name() << " R=\"0\"\n";
    }
  }else{ untested();
    s << c->type() << " ";

    QString comma="";
    s << "#(";
    for(auto p2 : c->params()) {
      s << comma << "." << p2->name() << "(" << p2->Value << ")";
      comma = ", ";
    }
    s << ") ";
    s << c->label() << "(";

    comma = "";
    for(Port *p1 : c->ports()){
      s << comma << p1->Connection->label();
      if(p1->Name!="") s << " /*" << p1->Name << "*/"; // later.
      comma = ", ";
    }

    s << ");\n";
  }
}
}

// vim:ts=8:sw=2:noet
