/*
 * spdeembed.cpp - two-port S-parameters de-embedding component
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


SP2DeEmbed::SP2DeEmbed()
{
  Description = QObject::tr("S parameter file de-embedding");
  Model = "SPDfile";
  Name  = "XD";

  // must be the first property !!!
  Props.append(new Property("File", "test.s2p", true,
		QObject::tr("name of the s parameter file")));
  Props.append(new Property("Data", "rectangular", false,
		QObject::tr("data type")+" [rectangular, polar]"));
  Props.append(new Property("Interpolator", "linear", false,
		QObject::tr("interpolation type")+" [linear, cubic]"));
  Props.append(new Property("duringDC", "open", false,
		QObject::tr("representation during DC analysis")+
			    " [open, short, shortall, unspecified]"));

  createSymbol();
}

// -------------------------------------------------------
Component* SP2DeEmbed::newOne()
{
  SP2DeEmbed* p = new SP2DeEmbed();
  return p;
}

Element* SP2DeEmbed::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("2-port S parameter file de-embedding");
  BitmapFile = (char *) "spdfile2";

  if(getNewOne) {
    SP2DeEmbed* p = new SP2DeEmbed();
    p->Props.getFirst()->Value = "test.s2p";
    return p;
  }
  return 0;
}

// -------------------------------------------------------
QString SP2DeEmbed::getSubcircuitFile()
{
  // construct full filename
  QString FileName = Props.getFirst()->Value;
  return misc::properAbsFileName(FileName);
}

// -------------------------------------------------------
QString SP2DeEmbed::netlist()
{
  QString s = Model+":"+Name;

  // output all node names
  foreach(Port *p1, Ports)
    s += " "+p1->Connection->Name;   // node names

  // output all properties
  Property *p2 = Props.first();
  s += " "+p2->Name+"=\"{"+getSubcircuitFile()+"}\"";

  // data type
  p2 = Props.next();
  s += " "+p2->Name+"=\""+p2->Value+"\"";

  // interpolator type
  p2 = Props.next();
  s += " "+p2->Name+"=\""+p2->Value+"\"";

  // DC property
  p2 = Props.next();
  s += " "+p2->Name+"=\""+p2->Value+"\"\n";

  return s;
}

// -------------------------------------------------------
void SP2DeEmbed::createSymbol()
{
  QFont Font(QucsSettings.font); // default application font
  // symbol text is smaller (10 pt default)
  Font.setPointSize(10 );
  // get the small font size; use the screen-compatible metric
  QFontMetrics  smallmetrics(Font, 0);
  int fHeight = smallmetrics.lineSpacing();
  QString stmp;

  int w, PortDistance = 60;
  int Num = 2;

  // draw symbol outline
  int h = (PortDistance/2)*((Num-1)/2) + 15;
  QPen pen(Qt::darkBlue, 2, Qt::DashLine);
  Lines.append(new Line(-15, -h, 15, -h, pen));
  Lines.append(new Line( 15, -h, 15,  h, pen));
  Lines.append(new Line(-15,  h, 15,  h, pen));
  Lines.append(new Line(-15, -h,-15,  h, pen));
  stmp = QObject::tr("file");
  w = smallmetrics.width(stmp); // compute text size to center it
  Texts.append(new Text(-w/2, -fHeight/2, stmp));

  int i=0, y = 15-h;
  while(i<Num) { // add ports lines and numbers
    i++;
    Lines.append(new Line(-30, y,-15, y,QPen(Qt::darkBlue,2)));
    Ports.append(new Port(-30, y));
    stmp = QString::number(i);
    w = smallmetrics.width(stmp);
    Texts.append(new Text(-25-w, y-fHeight-2, stmp)); // text right-aligned

    if(i == Num) break; // if odd number of ports there will be one port less on the right side
    i++;
    Lines.append(new Line( 15, y, 30, y,QPen(Qt::darkBlue,2)));
    Ports.append(new Port( 30, y));
    stmp = QString::number(i);
    Texts.append(new Text(25, y-fHeight-2, stmp)); // text left-aligned
    y += PortDistance;
  }

  Lines.append(new Line( 0, h, 0,h+15,QPen(Qt::darkBlue,2)));
  Texts.append(new Text( 4, h,"Ref"));
  Ports.append(new Port( 0,h+15));    // 'Ref' port

  x1 = -30; y1 = -h-2;
  x2 =  30; y2 =  h+15;
  // compute component name text position - normal size font
  QFontMetrics  metrics(QucsSettings.font, 0);   // use the screen-compatible metric
  tx = x1+4;
  ty = y1 - 2*metrics.lineSpacing() - 4;
}
