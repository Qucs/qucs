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
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include "eqndefined.h"
#include "qucs.h"
#include "schematic.h"

#include <QFileInfo>

EqnDefined::EqnDefined()
{
  Description = QObject::tr("equation defined device");

  Model = "EDD";
  Name  = "D";

  // first properties !!!
  Props.push_back(qucs::Property("Type", "explicit", false,
		QObject::tr("type of equations")+" [explicit, implicit]"));
  Props.push_back(qucs::Property("Branches", "1", false,
		QObject::tr("number of branches")));

  // last properties
  Props.push_back(qucs::Property("I1", "0", true,
		QObject::tr("current equation") + " 1"));
  Props.push_back(qucs::Property("Q1", "0", false,
		QObject::tr("charge equation") + " 1"));

  createSymbol();
}

// -------------------------------------------------------
Component* EqnDefined::newOne()
{
  EqnDefined* p = new EqnDefined();
  p->prop(0).Value = prop(0).Value;
  p->prop(1).Value = prop(1).Value;
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
    p->prop(0).Value = "explicit";
    p->prop(1).Value = "1";
    p->recreate(0);
    return p;
  }
  return 0;
}

// -------------------------------------------------------
QString EqnDefined::netlist()
{
  QString s = Model+":"+Name;
  QString e = "\n";

  // output all node names
  for(auto p1 = Ports.begin(); p1 != Ports.end(); ++p1)
    s += " "+p1->getConnection()->Name;   // node names

  // output all properties
  int i = 2;
  while(i < int(Props.size())) {
    s += " "+prop(i).Name+"=\""+Name+"."+prop(i).Name+"\"";
    e += "  Eqn:Eqn"+Name+prop(i).Name+" "+
      Name+"."+prop(i).Name+"=\""+prop(i).Value+"\" Export=\"no\"\n";
    ++i;
  }

  return s+e;
}

// -------------------------------------------------------
void EqnDefined::createSymbol()
{  
  QFont Font(QucsSettings.font); // default application font
  // symbol text is smaller (10 pt default)
  //Font.setPointSizeF(Font.pointSizeF()/1.2);  // symbol text size proportional to default font size
  Font.setPointSize(10); // symbol text size fixed at 10 pt
  // get the small font size; use the screen-compatible metric
  QFontMetrics  smallmetrics(Font, 0); 
  int fHeight = smallmetrics.lineSpacing();
  QString tmp;
  int i, PortDistance = 60;

  // adjust branch number
  int Num = prop(1).Value.toInt();
  if(Num < 1) Num = 1;
  else if(Num > 4) {
    PortDistance = 40;
    if(Num > 20) Num = 20;
  }
  prop(1).Value = QString::number(Num);

  // adjust actual number of properties
  int NumProps = (Props.size() - 2) / 2; // current number of properties
  if (NumProps < Num) {
    for(i = NumProps; i < Num; i++) {
      Props.push_back(qucs::Property("I"+QString::number(i+1), "0", false,
		QObject::tr("current equation") + " " +QString::number(i+1)));
      Props.push_back(qucs::Property("Q"+QString::number(i+1), "0", false,
		QObject::tr("charge equation") + " " +QString::number(i+1)));
    }
  } else {
    for(i = Num; i < NumProps; i++) {
      Props.pop_back();
      Props.pop_back();
    }
  }

  // adjust property names
  int ii = 2;
  for(i = 1; i <= Num; i++) {
    prop(ii++).Name = "I"+QString::number(i);
    prop(ii++).Name = "Q"+QString::number(i);
  }

  // draw symbol
  int h = (PortDistance/2)*((Num-1)) + PortDistance/2; // total component half-height
  Lines.push_back(qucs::Line(-15, -h, 15, -h,QPen(Qt::darkBlue,2))); // top side
  Lines.push_back(qucs::Line( 15, -h, 15,  h,QPen(Qt::darkBlue,2))); // right side
  Lines.push_back(qucs::Line(-15,  h, 15,  h,QPen(Qt::darkBlue,2))); // bottom side
  Lines.push_back(qucs::Line(-15, -h,-15,  h,QPen(Qt::darkBlue,2))); // left side

  i=0;
  int y = PortDistance/2-h, yh; // y is the actual vertical center
  while(i<Num) { // for every branch
    i++;
    // left connection with port
    Lines.push_back(qucs::Line(-30, y,-15, y,QPen(Qt::darkBlue,2)));
    Ports.push_back(qucs::Port(-30, y));
    // small black arrow inside the box
    Lines.push_back(qucs::Line( 7,y-3, 10, y,QPen(Qt::black,1)));
    Lines.push_back(qucs::Line( 7,y+3, 10, y,QPen(Qt::black,1)));
    Lines.push_back(qucs::Line(-10, y, 10, y,QPen(Qt::black,1)));

    if (i > 1) {
      yh = y-PortDistance/2; // bottom of the branch box
      // draw horizontal separation between boxes
      Lines.push_back(qucs::Line(-15, yh, 15, yh, QPen(Qt::darkBlue,2)));
    }
    // right connection with port
    Lines.push_back(qucs::Line( 15, y, 30, y,QPen(Qt::darkBlue,2)));
    Ports.push_back(qucs::Port( 30, y));
    // add branch number near the right connection port
    Texts.push_back(qucs::Text(25,y-fHeight-2,QString::number(i))); // left-aligned
    // move the vertical center down for the next branch
    y += PortDistance;
  }

  x1 = -30; y1 = -h-2;
  x2 =  30; y2 =  h+2;
  // compute component name text position - normal size font
  QFontMetrics  metrics(QucsSettings.font, 0);  // use the screen-compatible metric
  set_qucs_text_position(x1+4,
   y1 - 2*metrics.lineSpacing() - 4); // yikes.
}
