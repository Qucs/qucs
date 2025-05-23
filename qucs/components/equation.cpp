/***************************************************************************
                          equation.cpp  -  description
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
#include "componentdialog.h"
#include "equation.h"
#include "qucs.h"

#include <QFontMetrics>

Equation::Equation()
{
  Type = isComponent; // Analogue and digital component.
  Description = QObject::tr("equation");

  QFont f = QucsSettings.font;
  f.setWeight(QFont::Light);
  f.setPointSizeF(12.0);
  QFontMetrics  metrics(f, 0);  // use the the screen-compatible metric
  QSize r = metrics.size(0, QObject::tr("Equation"));
  int xb = r.width()  >> 1;
  int yb = r.height() >> 1;

  Lines.push_back(qucs::Line(-xb, -yb, -xb,  yb,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line(-xb,  yb,  xb+3,yb,QPen(Qt::darkBlue,2)));
  Texts.push_back(qucs::Text(-xb+4,  -yb-3, QObject::tr("Equation"),
			QColor(0,0,0), 12.0));

  set_x1(-xb-3);
  set_y1(-yb-5);
  set_x2(xb+9);
  set_y2(yb+3);

  set_qucs_text_position(x1()+4, y2()+4);
  Model = "Eqn"; // BUG: don't use
  Name  = "Eqn"; // BUG: don't use

  Props.push_back(qucs::Property("y", "1", true));
  Props.push_back(qucs::Property("Export", "yes", false,
  		QObject::tr("put result into dataset")+" [yes, no]"));
}

Equation::~Equation()
{
}

// -------------------------------------------------------
QString Equation::verilogCode(int)
{
  QString s;
  // output all equations
  for(auto pr = Props.begin(); pr != Props.end(); ++pr)
    if(pr->Name != "Export")
      s += "  real "+pr->Name+"; initial "+pr->Name+" = "+pr->Value+";\n";
  return s;
}

// -------------------------------------------------------
QString Equation::vhdlCode(int)
{
  QString s;
  // output all equations
  for(auto pr = Props.begin(); pr != Props.end(); ++pr)
    if(pr->Name != "Export")
      s += "  constant "+pr->Name+" : time := "+pr->Value+";\n";
  return s;
}

Component* Equation::newOne()
{
  return new Equation();
}

Element* Equation::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Equation");
  BitmapFile = (char *) "";

  if(getNewOne)  return new Equation();
  return 0;
}

void Equation::dialgButtStuff(ComponentDialog& d)const
{
  d.enableButtons();
}

// vim:ts=8:sw=2:noet
