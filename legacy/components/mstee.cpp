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
#include "qucs_app.h" // BUG
#include "some_font_stuff.h" // BUG
#include "component.h"
#include "module.h"
#include "qucs_globals.h"

namespace{
class MStee : public MultiViewComponent  {
private:
	MStee(MStee const& m) : MultiViewComponent(m) {}
public:
  MStee();
 ~MStee();
  Component* newOne();
  static Element* info(QString&, char* &, bool getNewOne=false);

protected:
  void createSymbol();
}d0;
static Dispatcher<Symbol>::INSTALL p(&symbol_dispatcher, "MTEE", &d0);
static Module::INSTALL pp("RF", &d0);

MStee::MStee() : MultiViewComponent()
{
  setLabel("microstrip tee");
  setTypeName("MTEE");

  x1 = -30; y1 = -11;
  x2 =  30; y2 =  30;

  // use the screen-compatible metric
  FontMetrics metrics;
  tx = x1+4;
  ty = y1 - 5*metrics.lineSpacing() - 4; // 5 lines of text
  Model = "MTEE";
  Name  = "MS";

  Props.append(new Property("Subst", "Subst1", true,
		QObject::tr("substrate")));
  Props.append(new Property("W1", "1 mm", true,
		QObject::tr("width of line 1")));
  Props.append(new Property("W2", "1 mm", true,
		QObject::tr("width of line 2")));
  Props.append(new Property("W3", "2 mm", true,
		QObject::tr("width of line 3")));
  Props.append(new Property("MSModel", "Hammerstad", false,
	QObject::tr("quasi-static microstrip model")+
	" [Hammerstad, Wheeler, Schneider]"));
  Props.append(new Property("MSDispModel", "Kirschning", false,
	QObject::tr("microstrip dispersion model")+" [Kirschning, Kobayashi, "
	"Yamashita, Hammerstad, Getsinger, Schneider, Pramanick]"));
  Props.append(new Property("Temp", "26.85", false,
		QObject::tr("temperature in degree Celsius")));
  Props.append(new Property("Symbol", "showNumbers", false,
	QObject::tr("show port numbers in symbol or not")+
	" [showNumbers, noNumbers]"));

  createSymbol();
}

MStee::~MStee()
{
}

Component* MStee::newOne()
{
  return new MStee();
}

#if 0
Element* MStee::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Microstrip Tee");
  BitmapFile = (char *) "mstee";

  if(getNewOne)  return new MStee();
  return 0;
}
#endif

void MStee::createSymbol()
{
  QFont Font(QucsSettings.font); // default application font
  // symbol text is smaller (10 pt default)
  Font.setPointSize(10); 
  // get the small font size; use the screen-compatible metric
  FontMetrics smallmetrics;
  smallmetrics.setSmall();
  
  Lines.append(new Line(-30,  0,-18,  0,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( 18,  0, 30,  0,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(  0, 18,  0, 30,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(-18, -8, 18, -8,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(-18,  8, -8,  8,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(  8,  8, 18,  8,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(-18, -8,-18,  8,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( 18, -8, 18,  8,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( -8,  8, -8, 18,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(  8,  8,  8, 18,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( -8, 18,  8, 18,QPen(Qt::darkBlue,2)));

  if(Props.getLast()->Value.at(0) != 'n') {
    QString stmp = "1"; 
    int w = smallmetrics.width(stmp);
    int d = smallmetrics.descent();
    int a = smallmetrics.ascent();
    
    Texts.append(new Text(-25-w, -d+6, stmp)); // right-aligned, top-aligned
    Texts.append(new Text( 25, -d+6, "2")); // left-aligned, top-aligned
    Texts.append(new Text(  5, 30-a-1, "3")); // bottom-aligned
  }

  Ports.append(new ComponentPort(-30, 0));
  Ports.append(new ComponentPort( 30, 0));
  Ports.append(new ComponentPort(  0,30));
}
}
