/***************************************************************************
                          symtrafo.cpp  -  description
                             -------------------
    begin                : Sat Aug 23 2003
    copyright            : (C) 2003 by Michael Margraf
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
#include "symtrafo.h"

#include<QFontMetrics>


symTrafo::symTrafo()
{
  Description = QObject::tr("ideal symmetrical transformer");

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

  // core lines
  Lines.push_back(qucs::Line( -1,-57, -1, 57,QPen(Qt::darkBlue,1)));
  Lines.push_back(qucs::Line(  1,-57,  1, 57,QPen(Qt::darkBlue,1)));

  stmp = "T1"; w = smallmetrics.horizontalAdvance(stmp); // compute width to right-align
  Texts.push_back(qucs::Text(-13-w,-57,stmp));
  stmp = "T2"; w = smallmetrics.horizontalAdvance(stmp); // compute width to right-align
  Texts.push_back(qucs::Text(-13-w, 22,stmp));

  // mark the turn direction
  Arcs.push_back(qucs::Arc(-21,-64,  5,  5,  0, 16*360,QPen(Qt::darkBlue,2)));
  Arcs.push_back(qucs::Arc(-21, 15,  5,  5,  0, 16*360,QPen(Qt::darkBlue,2)));
  Arcs.push_back(qucs::Arc( 15,-24,  5,  5,  0, 16*360,QPen(Qt::darkBlue,2)));

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
  Model = "sTr";
  Name  = "Tr";

  Props.push_back(qucs::Property("T1", "1", true,
		QObject::tr("voltage transformation ratio of coil 1")));
  Props.push_back(qucs::Property("T2", "1", true,
		QObject::tr("voltage transformation ratio of coil 2")));
}

symTrafo::~symTrafo()
{
}

Component* symTrafo::newOne()
{
  return new symTrafo();
}

Element* symTrafo::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("symmetric Transformer");
  BitmapFile = (char *) "symtrans";

  if(getNewOne)  return new symTrafo();
  return 0;
}
