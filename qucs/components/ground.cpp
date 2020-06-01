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

#include "ground.h"
#include "node.h"
#include "module.h"
#include "globals.h"

namespace{
class Ground : public Component  {
private:
	Ground(Ground const& g);
public:
  Ground();
 ~Ground();
  Component* newOne() {return new Ground(*this);}
  static Element* info(QString&, char* &, bool getNewOne=false);

private:
	Node* connectNode(unsigned i, NodeMap&l);
	Node* disconnectNode(unsigned i, NodeMap&l);

protected:
  QString netlist() const;
};

Ground::Ground(Ground const& g) : Component(g)
{
	incomplete();
	if(!Ports.count()){
		Ports.append(new Port(  0,  0));
	}
}

Ground::Ground()
{
  info(Name, bitmap_file);
  Type = isComponent;   // both analog and digital
  Description = QObject::tr("ground (reference potential)");

  Lines.append(new Line(  0,  0,  0, 10,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(-11, 10, 11, 10,QPen(Qt::darkBlue,3)));
  Lines.append(new Line( -7, 16,  7, 16,QPen(Qt::darkBlue,3)));
  Lines.append(new Line( -3, 22,  3, 22,QPen(Qt::darkBlue,3)));

  Ports.append(new Port(  0,  0));

  x1 = -12; y1 =  0;
  x2 =  12; y2 = 25;

  tx = 0;
  ty = 0;
  Model = "GND";
  Name  = "";
  showName = false;
}

Ground::~Ground()
{
}

Node* Ground::connectNode(unsigned i, NodeMap&l)
{
  Node* n = Component::connectNode(i, l);
  assert(n);
  n->setNetLabel("gnd");
  return n;
}

Node* Ground::disconnectNode(unsigned i, NodeMap&l)
{
  Node* n = Component::disconnectNode(i, l);
  assert(n);
  incomplete();
  return n;
}

// -------------------------------------------------------
Element* Ground::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Ground");
  BitmapFile = (char *) "gnd";

  if(getNewOne)  return new Ground();
  return 0;
}

// -------------------------------------------------------
QString Ground::netlist() const
{
  return QString("");
}

Ground D;
Dispatcher<Symbol>::INSTALL p(&symbol_dispatcher, "GND", &D);
Module::INSTALL pp("lumped", &D);
}
