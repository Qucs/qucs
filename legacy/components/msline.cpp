/***************************************************************************
    copyright            : (C) 2003 by Michael Margraf
                               2020 Felix Salfelder
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "component.h"
#include "qucs_globals.h"
#include "module.h"

namespace{
class MSline : public Component  {
private:
  MSline(MSline const& s) : Component(s) {}
public:
  MSline();
  ~MSline();
  Component* newOne();
  static Element* info(QString&, char* &, bool getNewOne=false);
private: // Symbol
  Symbol* clone() const{return new MSline(*this); }
}d0;
static Dispatcher<Symbol>::INSTALL p(&symbol_dispatcher, "MLIN", &d0);
static Module::INSTALL pp("RF", &d0);

MSline::MSline() : Component()
{
  set_label("microstrip line");
  set_dev_type("MLIN");

  Lines.append(new Line(-30,  0,-18,  0,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( 18,  0, 30,  0,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(-13, -8, 23, -8,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(-23,  8, 13,  8,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(-13, -8,-23,  8,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( 23, -8, 13,  8,QPen(Qt::darkBlue,2)));

  Ports.append(new ComponentPort(-30, 0));
  Ports.append(new ComponentPort( 30, 0));

  x1 = -30; y1 =-11;
  x2 =  30; y2 = 11;

  tx = x1+4;
  ty = y2+4;
  Model = "MLIN";
  Name  = "MS";

  Props.append(new Property("Subst", "Subst1", true,
	QObject::tr("name of substrate definition")));
  Props.append(new Property("W", "1 mm", true,
	QObject::tr("width of the line")));
  Props.append(new Property("L", "10 mm", true,
	QObject::tr("length of the line")));
  Props.append(new Property("Model", "Hammerstad", false,
	QObject::tr("quasi-static microstrip model")+
		    " [Hammerstad, Wheeler, Schneider]"));
  Props.append(new Property("DispModel", "Kirschning", false,
	QObject::tr("microstrip dispersion model")+" [Kirschning, Kobayashi, "
	"Yamashita, Hammerstad, Getsinger, Schneider, Pramanick]"));
  Props.append(new Property("Temp", "26.85", false,
	QObject::tr("simulation temperature in degree Celsius")));
}

MSline::~MSline()
{
}

Component* MSline::newOne()
{
  return new MSline();
}

#if 0
Element* MSline::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Microstrip Line");
  BitmapFile = (char *) "msline";

  if(getNewOne)  return new MSline();
  return 0;
}
#endif

}
