/*
 * spdeembed.cpp - N-port S-parameters de-embedding component
 *
 * Copyright (C) 2017 Qucs Team
 * based on sparamfile.cpp, (C) 2003 by Michael Margraf
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this package; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street - Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 */

#include "spdeembed.h"
#include "qucs.h" // for QucsSettings
#include "schematic.h"
#include "misc.h"

#include <QFileInfo>


SPDeEmbed::SPDeEmbed()
{
  Description = QObject::tr("S parameter file de-embedding");
  Model = "SPDfile";
  Name  = "XD";

  // must be the first property !!!
  Props.push_back(qucs::Property("File", "test.s2p", true,
		QObject::tr("name of the s parameter file")));
  Props.push_back(qucs::Property("Data", "rectangular", false,
		QObject::tr("data type")+" [rectangular, polar]"));
  Props.push_back(qucs::Property("Interpolator", "linear", false,
		QObject::tr("interpolation type")+" [linear, cubic]"));
  Props.push_back(qucs::Property("duringDC", "open", false,
		QObject::tr("representation during DC analysis")+
			    " [open, short, shortall, unspecified]"));

  // must be the last property !!!
  Props.push_back(qucs::Property("Ports", "2", false,
		QObject::tr("number of ports")));

  createSymbol();
}

// -------------------------------------------------------
Component* SPDeEmbed::newOne()
{
  SPDeEmbed* p = new SPDeEmbed();
  p->Props.back().Value = Props.back().Value;
  p->recreate(0);
  return p;
}

Element* SPDeEmbed::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  // "de-embedding" at the beginning, so it's always visible in the component dock
  // to help distinguish it from the regular embedding component
  Name = QObject::tr("de-embedding n-port S parameter file");
  BitmapFile = (char *) "spdfile6";

  if(getNewOne) {
    SPDeEmbed* p = new SPDeEmbed();
    p->Props.front().Value = "test.s6p";
    p->Props.back().Value = "6";
    p->recreate(0);
    return p;
  }
  return 0;
}

// -------------------------------------------------------
Element* SPDeEmbed::info2(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("de-embedding 2-port S parameter file");
  BitmapFile = (char *) "spdfile2";

  if(getNewOne)  return new SPDeEmbed();
  return 0;
}

// -------------------------------------------------------
Element* SPDeEmbed::info4(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("de-embedding 4-port S parameter file");
  BitmapFile = (char *) "spdfile4";

  if(getNewOne) {
    SPDeEmbed* p = new SPDeEmbed();
    p->Props.front().Value = "test.s4p";
    p->Props.back().Value = "4";
    p->recreate(0);
    return p;
  }
  return 0;
}

// -------------------------------------------------------
QString SPDeEmbed::getSubcircuitFile()
{
  // construct full filename
  QString FileName = Props.front().Value;
  return misc::properAbsFileName(FileName);
}

// -------------------------------------------------------
QString SPDeEmbed::netlist()
{
  QString s = Model+":"+Name;

  // output all node names
  for(auto p1 = Ports.begin(); p1 != Ports.end(); ++p1)
    s += " "+p1->getConnection()->Name;   // node names

  // output all properties
  auto p2 = Props.begin();
  s += " "+p2->Name+"=\"{"+getSubcircuitFile()+"}\"";

  // data type
  ++p2;
  s += " "+p2->Name+"=\""+p2->Value+"\"";

  // interpolator type
  ++p2;
  s += " "+p2->Name+"=\""+p2->Value+"\"";

  // DC property
  ++p2;
  s += " "+p2->Name+"=\""+p2->Value+"\"\n";

  return s;
}

// -------------------------------------------------------
void SPDeEmbed::createSymbol()
{
  QFont Font(QucsSettings.font); // default application font
  // symbol text is smaller (10 pt default)
  Font.setPointSize(10 );
  // get the small font size; use the screen-compatible metric
  QFontMetrics  smallmetrics(Font, 0);
  int fHeight = smallmetrics.lineSpacing();
  QString stmp;

  int w, PortDistance = 60;
  int Num = Props.back().Value.toInt();

  // adjust number of ports
  // force even port number
  Num = 2 * (Num / 2);
  if(Num < 2) Num = 2;
  else if(Num > 8) {
    PortDistance = 20;
    if(Num > 40) Num = 40;
  }
  Props.back().Value = QString::number(Num);

  // draw symbol outline
  int h = (PortDistance/2)*((Num-1)/2) + 15;
  QPen pen(Qt::darkBlue, 2, Qt::DashLine);
  Lines.push_back(qucs::Line(-15, -h, 15, -h, pen));
  Lines.push_back(qucs::Line( 15, -h, 15,  h, pen));
  Lines.push_back(qucs::Line(-15,  h, 15,  h, pen));
  Lines.push_back(qucs::Line(-15, -h,-15,  h, pen));
  stmp = QObject::tr("file");
  w = smallmetrics.horizontalAdvance(stmp); // compute text size to center it
  Texts.push_back(qucs::Text(-w/2, -fHeight/2, stmp));

  int i=0, y = 15-h;
  while(i<Num) { // add ports lines and numbers
    i++;
    Lines.push_back(qucs::Line(-30, y,-15, y,QPen(Qt::darkBlue,2)));
    Ports.push_back(qucs::Port(-30, y));
    stmp = QString::number(i);
    w = smallmetrics.horizontalAdvance(stmp);
    Texts.push_back(qucs::Text(-25-w, y-fHeight-2, stmp)); // text right-aligned

    if(i == Num) break; // if odd number of ports there will be one port less on the right side
    i++;
    Lines.push_back(qucs::Line( 15, y, 30, y,QPen(Qt::darkBlue,2)));
    Ports.push_back(qucs::Port( 30, y));
    stmp = QString::number(i);
    Texts.push_back(qucs::Text(25, y-fHeight-2, stmp)); // text left-aligned
    y += PortDistance;
  }

  Lines.push_back(qucs::Line( 0, h, 0,h+15,QPen(Qt::darkBlue,2)));
  Texts.push_back(qucs::Text( 4, h,"Ref"));
  Ports.push_back(qucs::Port( 0,h+15));    // 'Ref' port

  set_x1(-30);
  set_y1(-h-2);
  set_x2(30);
  set_y2(h+15);
  // compute component name text position - normal size font
  QFontMetrics  metrics(QucsSettings.font, 0);   // use the screen-compatible metric
  set_qucs_text_position(x1()+4, 
  y1() - 2*metrics.lineSpacing() - 4); // yikes
}
