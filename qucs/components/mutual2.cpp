/***************************************************************************
                               mutual2.cpp
                              --------------
    begin                : Sat Aug 20 2005
    copyright            : (C) 2005 by Michael Margraf
    email                : michael.margraf@alumni.tu-berlin.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "qucs.h"
#include "mutual2.h"

#include<QFontMetrics>


Mutual2::Mutual2()
{
  Description = QObject::tr("three mutual inductors");

  QFont Font(QucsSettings.font); // default application font
  // symbol text is smaller (10 pt default)
  Font.setPointSize(10); 
  // get the small font size; use the screen-compatible metric
  QFontMetrics  smallmetrics(Font, 0);

  int w;
  QString stmp;

  Arcs.push_back(qucs::Arc(-16,-58,12,12, 16*270,16*180, QPen(Qt::darkBlue,2)));
  Arcs.push_back(qucs::Arc(-16,-46,12,12, 16*270,16*180, QPen(Qt::darkBlue,2)));
  Arcs.push_back(qucs::Arc(-16,-34,12,12, 16*270,16*180, QPen(Qt::darkBlue,2)));
  Arcs.push_back(qucs::Arc(-16, 46,12,12, 16*270,16*180, QPen(Qt::darkBlue,2)));
  Arcs.push_back(qucs::Arc(-16, 34,12,12, 16*270,16*180, QPen(Qt::darkBlue,2)));
  Arcs.push_back(qucs::Arc(-16, 22,12,12, 16*270,16*180, QPen(Qt::darkBlue,2)));
  Arcs.push_back(qucs::Arc(  4,-18,12,12,  16*90,16*180, QPen(Qt::darkBlue,2)));
  Arcs.push_back(qucs::Arc(  4, -6,12,12,  16*90,16*180, QPen(Qt::darkBlue,2)));
  Arcs.push_back(qucs::Arc(  4,  6,12,12,  16*90,16*180, QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line(-10,-58,-10,-70,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line(-10,-70,-30,-70,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line( 10,-18, 10,-30,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line( 10,-30, 30,-30,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line(-10, 58,-10, 70,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line(-10, 70,-30, 70,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line( 10, 18, 10, 30,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line( 10, 30, 30, 30,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line(-10,-10,-30,-10,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line(-10,-22,-10,-10,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line(-10, 10,-30, 10,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line(-10, 10,-10, 22,QPen(Qt::darkBlue,2)));

  stmp = "1"; w = smallmetrics.horizontalAdvance(stmp); // compute width to right-align
  Texts.push_back(qucs::Text(-13-w,-61,stmp));
  stmp = "2"; w = smallmetrics.horizontalAdvance(stmp); // compute width to right-align
  Texts.push_back(qucs::Text(-13-w, 18,stmp));
  Texts.push_back(qucs::Text( 13,-22,"3")); // left-aligned, no need to compute width

  Lines.push_back(qucs::Line(  0,-57,  0, 57,QPen(Qt::darkBlue,1,Qt::DashLine)));

  Ports.push_back(qucs::Port(-30,-70));
  Ports.push_back(qucs::Port( 30,-30));
  Ports.push_back(qucs::Port( 30, 30));
  Ports.push_back(qucs::Port(-30, 70));
  Ports.push_back(qucs::Port(-30, 10));
  Ports.push_back(qucs::Port(-30,-10));

  set_x1(-33);
  set_y1(-74);
  set_x2(33);
  set_y2(74);

  set_qucs_text_position(x1()+4, y2()+4);
  Model = "MUT2";
  Name  = "Tr";

  Props.push_back(qucs::Property("L1", "1 mH", false,
		QObject::tr("inductance of coil 1")));
  Props.push_back(qucs::Property("L2", "1 mH", false,
		QObject::tr("inductance of coil 2")));
  Props.push_back(qucs::Property("L3", "1 mH", false,
		QObject::tr("inductance of coil 3")));
  Props.push_back(qucs::Property("k12", "0.9", false,
		QObject::tr("coupling factor between coil 1 and 2")));
  Props.push_back(qucs::Property("k13", "0.9", false,
		QObject::tr("coupling factor between coil 1 and 3")));
  Props.push_back(qucs::Property("k23", "0.9", false,
		QObject::tr("coupling factor between coil 2 and 3")));
}

Mutual2::~Mutual2()
{
}

Component* Mutual2::newOne()
{
  return new Mutual2();
}

Element* Mutual2::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("3 Mutual Inductors");
  BitmapFile = (char *) "mutual2";

  if(getNewOne)  return new Mutual2();
  return 0;
}
