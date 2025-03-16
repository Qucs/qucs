/***************************************************************************
                               rfedd.cpp
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
#include "rfedd.h"
#include "qucs.h"
#include "schematic.h"

#include <QFileInfo>


RFedd::RFedd()
{
  Description = QObject::tr("equation defined RF device");

  Model = "RFEDD";
  Name  = "RF";

  // first properties !!!
  Props.push_back(qucs::Property("Type", "Y", false,
		QObject::tr("type of parameters")+" [Y, Z, S]"));
  Props.push_back(qucs::Property("Ports", "2", false,
		QObject::tr("number of ports")));
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
Component* RFedd::newOne()
{
  RFedd* p = new RFedd();
  p->prop(0).Value = prop(0).Value;
  p->prop(1).Value = prop(1).Value;
  p->recreate(0);
  return p;
}

// -------------------------------------------------------
Element* RFedd::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Equation Defined RF Device");
  BitmapFile = (char *) "rfedd";

  if(getNewOne) {
    RFedd* p = new RFedd();
    p->prop(0).Value = "Y";
    p->prop(1).Value = "2";
    p->recreate(0);
    return p;
  }
  return 0;
}

// -------------------------------------------------------
QString RFedd::netlist()
{
  QString s = Model+":"+Name;
  QString e = "\n";
  QString n, p;

  // output all node names
  for(auto p1 = Ports.begin(); p1 != Ports.end(); ++p1)
    s += " "+p1->getConnection()->Name;   // node names

  // output all properties
  s += " "+prop(0).Name+"=\""+prop(0).Value+"\"";
  p = prop(0).Value;
  s += " "+prop(2).Name+"=\""+prop(2).Value+"\"";
  for (int i = 3; i < int(Props.size()); ++i) {
    n = prop(i).Name.mid(1);
    s += " "+prop(i).Name+"=\""+Name+"."+p+n+"\"";
    e += "  Eqn:Eqn"+Name+prop(i).Name+" "+
      Name+"."+p+n+"=\""+prop(i).Value+"\" Export=\"no\"\n";
  }

  return s+e;
}

// -------------------------------------------------------
void RFedd::createSymbol()
{
  QFont Font(QucsSettings.font); // default application font
  // symbol text is smaller (10 pt default)
  Font.setPointSize(10); 
  // get the small font size; use the screen-compatible metric
  QFontMetrics  smallmetrics(Font, 0); 
  int fHeight = smallmetrics.lineSpacing();
  QString tmp;
  int w, i;

  // adjust port number
  int No = prop(1).Value.toInt();
  if(No < 1) No = 1;
  if(No > 8) No = 8;
  prop(1).Value = QString::number(No);

  // adjust property number and names
  int NumProps = Props.size() - 3; // Type, Ports, duringDC
  if (NumProps < No * No) { // number of ports was increased, add properties
    for(i = 0; i < NumProps; i++) {
      tmp=QString::number((i)/No+1)+QString::number((i)%No+1);
      prop(i+3).Name="P"+tmp;
      prop(i+3).Description=QObject::tr("parameter equation") + " " +tmp;
    }
    for(i = NumProps; i < No * No; i++) {
      tmp=QString::number((i)/No+1)+QString::number((i)%No+1);
      Props.push_back(qucs::Property("P"+tmp, "0", false,
		QObject::tr("parameter equation") + " " +tmp));
    }
  } else { // number of ports was decreased, remove properties
    for(i = No * No; i < NumProps; i++) {
      Props.pop_back();
    }
    for(i = 0; i < No * No; i++) {
      tmp=QString::number((i)/No+1)+QString::number((i)%No+1);
      prop(i+3).Name="P"+tmp;
      prop(i+3).Description=QObject::tr("parameter equation") + " " +tmp;
    }
  }

  // draw symbol
  #define HALFWIDTH  17
  int h = 30*((No-1)/2) + 15;
  Lines.push_back(qucs::Line(-HALFWIDTH, -h, HALFWIDTH, -h,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line( HALFWIDTH, -h, HALFWIDTH,  h,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line(-HALFWIDTH,  h, HALFWIDTH,  h,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line(-HALFWIDTH, -h,-HALFWIDTH,  h,QPen(Qt::darkBlue,2)));

  // component text name, centered
  tmp = QObject::tr("RF");
  w = smallmetrics.horizontalAdvance(tmp);
  Texts.push_back(qucs::Text(-w/2, -fHeight/2, tmp)); // text centered in box

  i = 0;
  int y = 15-h;
  while(i<No) { // add ports lines and numbers
    // left side
    Lines.push_back(qucs::Line(-30,  y,-HALFWIDTH,  y,QPen(Qt::darkBlue,2)));
    Ports.push_back(qucs::Port(-30,  y));
    tmp = QString::number(i+1);
    w = smallmetrics.horizontalAdvance(tmp);
    Texts.push_back(qucs::Text(-25-w, y-fHeight-2, tmp)); // text right-aligned
    i++;

    if(i == No) break; // if odd number of ports there will be one port less on the right side
    // right side
    Lines.push_back(qucs::Line(HALFWIDTH,  y, 30,  y,QPen(Qt::darkBlue,2)));
    Ports.push_back(qucs::Port( 30,  y));
    tmp = QString::number(i+1);
    Texts.push_back(qucs::Text(25, y-fHeight-2, tmp)); // text left-aligned
    y += 60;
    i++;
  }

  x1 = -30; y1 = -h-2;
  x2 =  30; y2 =  h+2;
  // compute component name text position - normal size font
  QFontMetrics  metrics(QucsSettings.font, 0);   // use the screen-compatible metric
  set_qucs_text_position(x1+4,
  y1 - metrics.lineSpacing() - 4); // yikes.
}
