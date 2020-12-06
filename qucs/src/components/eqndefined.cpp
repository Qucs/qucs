/***************************************************************************
                               eqndefined.cpp
                              ----------------
    begin                : Thu Apr 19 2007
    copyright            : (C) 2007 by Stefan Jahn
    email                : stefan@lkcc.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include "eqndefined.h"
#include "qucs.h"
#include "schematic_doc.h"
#include "some_font_stuff.h"

#include <QFileInfo>

EqnDefined::EqnDefined()
{
  Description = QObject::tr("equation defined device");

  Model = "EDD";
  Name  = "D";

  // first properties !!!
  Props.append(new Property("Type", "explicit", false,
		QObject::tr("type of equations")+" [explicit, implicit]"));
  Props.append(new Property("Branches", "1", false,
		QObject::tr("number of branches")));

  // last properties
  Props.append(new Property("I1", "0", true,
		QObject::tr("current equation") + " 1"));
  Props.append(new Property("Q1", "0", false,
		QObject::tr("charge equation") + " 1"));

  createSymbol();
}

// -------------------------------------------------------
Component* EqnDefined::newOne()
{
  EqnDefined* p = new EqnDefined();
  p->Props.at(0)->Value = Props.at(0)->Value;
  p->Props.at(1)->Value = Props.at(1)->Value;
  p->recreate(0);
  return p;
}

// -------------------------------------------------------
Element* EqnDefined::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Equation Defined Device");
  BitmapFile = (char *) "edd";

  if(getNewOne) {
    EqnDefined* p = new EqnDefined();
    p->Props.at(0)->Value = "explicit";
    p->Props.at(1)->Value = "1";
    p->recreate(0);
    return p;
  }
  return 0;
}

// -------------------------------------------------------
QString EqnDefined::netlist() const
{
#if 0
  QString s = Model+":"+name();
  QString e = "\n";

  // output all node names
  foreach(Port *p1, Ports)
    s += " "+p1->Connection->name();   // node names

  // output all properties
  Property *p2 = Props.at(2);
  while(p2) {
    s += " "+p2->name()+"=\""+name()+"."+p2->name()+"\"";
    e += "  Eqn:Eqn"+name()+p2->name()+" "+
      name()+"."+p2->name()+"=\""+p2->Value+"\" Export=\"no\"\n";
    p2 = Props.next();
  }

  return s+e;
#endif
  return "";
}

// -------------------------------------------------------
void EqnDefined::createSymbol()
{  
  QFont Font(QucsSettings.font); // default application font
  // symbol text is smaller (10 pt default)
  //Font.setPointSizeF(Font.pointSizeF()/1.2);  // symbol text size proportional to default font size
  Font.setPointSize(10); // symbol text size fixed at 10 pt
  // get the small font size; use the screen-compatible metric
  FontMetrics  smallmetrics;
  smallmetrics.setSmall();
  int fHeight = smallmetrics.lineSpacing();
  QString tmp;
  int i, PortDistance = 60;

  // adjust branch number
  int Num = Props.at(1)->Value.toInt();
  if(Num < 1) Num = 1;
  else if(Num > 4) {
    PortDistance = 40;
    if(Num > 20) Num = 20;
  }
  Props.at(1)->Value = QString::number(Num);

  // adjust actual number of properties
  int NumProps = (Props.count() - 2) / 2; // current number of properties
  if (NumProps < Num) {
    for(i = NumProps; i < Num; i++) {
      Props.append(new Property("I"+QString::number(i+1), "0", false,
		QObject::tr("current equation") + " " +QString::number(i+1)));
      Props.append(new Property("Q"+QString::number(i+1), "0", false,
		QObject::tr("charge equation") + " " +QString::number(i+1)));
    }
  } else {
    for(i = Num; i < NumProps; i++) {
      Props.removeLast();
      Props.removeLast();
    }
  }

  // adjust property names
  Property * p1 = Props.at(2);
  for(i = 1; i <= Num; i++) {
    p1->Name = "I"+QString::number(i);
    p1 = Props.next();
    p1->Name = "Q"+QString::number(i);
    p1 = Props.next();
  }

  // draw symbol
  int h = (PortDistance/2)*((Num-1)) + PortDistance/2; // total component half-height
  Lines.append(new Line(-15, -h, 15, -h,QPen(Qt::darkBlue,2))); // top side
  Lines.append(new Line( 15, -h, 15,  h,QPen(Qt::darkBlue,2))); // right side
  Lines.append(new Line(-15,  h, 15,  h,QPen(Qt::darkBlue,2))); // bottom side
  Lines.append(new Line(-15, -h,-15,  h,QPen(Qt::darkBlue,2))); // left side

  i=0;
  int y = PortDistance/2-h, yh; // y is the actual vertical center
  while(i<Num) { // for every branch
    i++;
    // left connection with port
    Lines.append(new Line(-30, y,-15, y,QPen(Qt::darkBlue,2)));
    Ports.append(new ComponentPort(-30, y));
    // small black arrow inside the box
    Lines.append(new Line( 7,y-3, 10, y,QPen(Qt::black,1)));
    Lines.append(new Line( 7,y+3, 10, y,QPen(Qt::black,1)));
    Lines.append(new Line(-10, y, 10, y,QPen(Qt::black,1)));

    if (i > 1) {
      yh = y-PortDistance/2; // bottom of the branch box
      // draw horizontal separation between boxes
      Lines.append(new Line(-15, yh, 15, yh, QPen(Qt::darkBlue,2)));
    }
    // right connection with port
    Lines.append(new Line( 15, y, 30, y,QPen(Qt::darkBlue,2)));
    Ports.append(new ComponentPort( 30, y));
    // add branch number near the right connection port
    Texts.append(new Text(25,y-fHeight-2,QString::number(i))); // left-aligned
    // move the vertical center down for the next branch
    y += PortDistance;
  }

  x1 = -30; y1 = -h-2;
  x2 =  30; y2 =  h+2;
  // compute component name text position - normal size font
  FontMetrics  metrics;
  tx = x1+4;
  ty = y1 - 2*metrics.lineSpacing() - 4;
}
