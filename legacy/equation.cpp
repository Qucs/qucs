/***************************************************************************
    copyright            : (C) 2003 by Michael Margraf
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include "qucs_globals.h"
#include "module.h"
#include "qucs_app.h"

#include "some_font_stuff.h"

#include "components/component.h" // BUG

namespace{

class Equation : public Component  {

public:
  Equation();
  ~Equation();
  Component* newOne() {return new Equation(*this);}
  static Element* info(QString&, char* &, bool getNewOne=false);

  std::string dev_type() const override{
	  return "Eqn";
  }

protected:
  QString vhdlCode(int);
  QString verilogCode(int);

  void dialgButtStuff(ComponentDialog& d)const;
}D;
static Dispatcher<Symbol>::INSTALL p(&qucs::symbol_dispatcher, "Eqn", &D);
static Module::INSTALL pp("lumped", &D);

Equation::Equation() : Component()
{
  Type = isComponent; // Analogue and digital component.
  Description = QObject::tr("equation");

  QFont f(QString_(QucsSettings.font));
  f.setWeight(QFont::Light);
  f.setPointSizeF(12.0);
  FontMetrics  metrics;
  QSize r = metrics.size(0, QObject::tr("Equation"));
  int xb = r.width()  >> 1;
  int yb = r.height() >> 1;

  Lines.append(new Line(-xb, -yb, -xb,  yb,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(-xb,  yb,  xb+3,yb,QPen(Qt::darkBlue,2)));
  Texts.append(new Text(-xb+4,  -yb-3, QObject::tr("Equation"),
			QColor(0,0,0), 12.0));

  x1 = -xb-3;  y1 = -yb-5;
  x2 =  xb+9; y2 =  yb+3;

  tx = x1+4;
  ty = y2+4;
  Model = "Eqn"; // BUG: don't use
  Name  = "Eqn"; // BUG: don't use

  Props.append(new Property("y", "1", true));
  Props.append(new Property("Export", "yes", false,
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
  for(Property *pr = Props.first(); pr != 0; pr = Props.next())
    if(pr->Name != "Export")
      s += "  real "+pr->Name+"; initial "+pr->Name+" = "+pr->Value+";\n";
  return s;
}

// -------------------------------------------------------
QString Equation::vhdlCode(int)
{
  QString s;
  // output all equations
  for(Property *pr = Props.first(); pr != 0; pr = Props.next())
    if(pr->Name != "Export")
      s += "  constant "+pr->Name+" : time := "+pr->Value+";\n";
  return s;
}

#if 0
Element* Equation::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Equation");
  BitmapFile = (char *) "";

  if(getNewOne)  return new Equation();
  return 0;
}
#endif

void Equation::dialgButtStuff(ComponentDialog&)const
{
  incomplete();
  // d.enableButtons();
}

}

// vim:ts=8:sw=2:noet
