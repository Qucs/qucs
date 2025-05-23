/***************************************************************************
                               rfedd2p.cpp
                             ----------------
    begin                : Sub Feb 17 2008
    copyright            : (C) 2008 by Stefan Jahn
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
#include "rfedd2p.h"
#include "qucs.h"
#include "schematic.h"

#include <QFileInfo>



RFedd2P::RFedd2P()
{
  Description = QObject::tr("equation defined 2-port RF device");

  Model = "RFEDD2P";
  Name  = "RF";

  // first properties !!!
  Props.push_back(qucs::Property("Type", "Y", false,
		QObject::tr("type of parameters")+" [Y, Z, S, H, G, A, T]"));
  Props.push_back(qucs::Property("duringDC", "open", false,
		QObject::tr("representation during DC analysis")+
			    " [open, short, unspecified, zerofrequency]"));

  // last properties
  Props.push_back(qucs::Property("P11", "0", false,
		QObject::tr("parameter equation") + " 11"));
  Props.push_back(qucs::Property("P12", "0", false,
		QObject::tr("parameter equation") + " 12"));
  Props.push_back(qucs::Property("P21", "0", false,
		QObject::tr("parameter equation") + " 21"));
  Props.push_back(qucs::Property("P22", "0", false,
		QObject::tr("parameter equation") + " 22"));

  createSymbol();
}

// -------------------------------------------------------
Component* RFedd2P::newOne()
{
  RFedd2P* p = new RFedd2P();
  p->prop(0).Value = prop(0).Value;
  p->recreate(0);
  return p;
}

// -------------------------------------------------------
Element* RFedd2P::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Equation Defined 2-port RF Device");
  BitmapFile = (char *) "rfedd";

  if(getNewOne) {
    RFedd2P* p = new RFedd2P();
    p->prop(0).Value = "Y";
    p->recreate(0);
    return p;
  }
  return 0;
}

// -------------------------------------------------------
QString RFedd2P::netlist()
{
  QString s = "RFEDD:"+Name;
  QString e = "\n";
  QString n, p;

  // output all node names
  for(auto p1 = Ports.begin(); p1 != Ports.end(); ++p1)
    s += " "+p1->getConnection()->Name;   // node names

  // output all properties
  s += " "+prop(0).Name+"=\""+prop(0).Value+"\"";
  p = prop(0).Value;
  s += " "+prop(1).Name+"=\""+prop(1).Value+"\"";
  for (int i = 2; i < int(Props.size()); ++i) {
    n = prop(i).Name.mid(1);
    s += " "+prop(i).Name+"=\""+Name+"."+p+n+"\"";
    e += "  Eqn:Eqn"+Name+prop(i).Name+" "+
      Name+"."+p+n+"=\""+prop(i).Value+"\" Export=\"no\"\n";
  }

  return s+e;
}

// -------------------------------------------------------
void RFedd2P::createSymbol()
{
  QFont Font(QucsSettings.font); // default application font
  // symbol text is smaller (10 pt default)
  Font.setPointSize(10); 
  // get the small font size; use the screen-compatible metric
  QFontMetrics  smallmetrics(Font, 0); 
  int fHeight = smallmetrics.lineSpacing();
  QString tmp;
  int w, i;

  // draw symbol
  #define HALFWIDTH  17
  int h = 15;
  Lines.push_back(qucs::Line(-HALFWIDTH, -h, HALFWIDTH, -h,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line( HALFWIDTH, -h, HALFWIDTH,  h,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line(-HALFWIDTH,  h, HALFWIDTH,  h,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line(-HALFWIDTH, -h,-HALFWIDTH,  h,QPen(Qt::darkBlue,2)));

  // component text name
  tmp = prop(0).Value;
  w = smallmetrics.horizontalAdvance(tmp);
  Texts.push_back(qucs::Text(-w/2, -fHeight/2, tmp)); // text centered in the box

  // add port numbers text
  i = 0;
  int y = 15-h;
  Lines.push_back(qucs::Line(-30,  y,-HALFWIDTH,  y,QPen(Qt::darkBlue,2)));
  Ports.push_back(qucs::Port(-30,  y));
  tmp = QString::number(i+1);
  w = smallmetrics.horizontalAdvance(tmp);
  Texts.push_back(qucs::Text(-25-w, y-fHeight-2, tmp)); // text right-aligned
  i++;

  Lines.push_back(qucs::Line(HALFWIDTH,  y, 30,  y,QPen(Qt::darkBlue,2)));
  Ports.push_back(qucs::Port( 30,  y));
  tmp = QString::number(i+1);
  Texts.push_back(qucs::Text(25, y-fHeight-2, tmp)); // text left-aligned
  y += 60;
  i++;

  set_x1(-30);
  set_y1(-h-2);
  set_x2(30);
  set_y2(h+2);
  // compute component name text position - normal size font
  QFontMetrics  metrics(QucsSettings.font, 0);   // use the screen-compatible metric
  set_qucs_text_position(x1()+4,
  y1() - metrics.lineSpacing() - 4); // yikes.
}
