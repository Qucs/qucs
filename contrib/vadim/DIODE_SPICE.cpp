/***************************************************************************
                          DIODE_SPICE.h  -  description
                      --------------------------------------
    copyright                 : (C) 2007 by Gunther Kraut
    copyright                 : (C) 2015 by Mike Brinson
	 2018 Felix Salfelder (QUCS intrgration)

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include "node.h"
#include "misc.h"
// #include "extsimkernels/spicecompat.h" not yet.
#include "../legacy/components/component.h" // yikes.
#include "qucs_globals.h"
#include "module.h"

namespace {

class DIODE_SPICE : public Component /* BUG: perhaps need a vadim baseclass */ {
public:
  DIODE_SPICE();
  ~DIODE_SPICE();
  Component* newOne();
  static Element* info(QString&, char* &, bool getNewOne=false);
protected:
//  QString netlist();
//  QString spice_netlist(bool isXyce = false);
}D;

// this is the schematic name...
static Dispatcher<qucs::Symbol>::INSTALL p(&qucs::symbol_dispatcher, "DIODE_SPICE", &D);
// the toolbox item
static Module::INSTALL pp("vadim", &D);


DIODE_SPICE::DIODE_SPICE()
{
	setTypeName("DIODE_SPICE");
	set_label("SPICE D:\nMultiple line ngspice or Xyce D model specifications allowed using \"+\" continuation lines.\nLeave continuation lines blank when NOT in use.");


	Lines.append(new Line(-30,  0, -20,  0,QPen(Qt::darkBlue,3)));
	Lines.append(new Line( -20, 0, -6,   0,QPen(Qt::darkRed,3)));
	Lines.append(new Line(  6,  0, 20,   0,QPen(Qt::darkRed,3)));
	Lines.append(new Line( 20,  0, 30,   0,QPen(Qt::darkBlue,3)));


	Lines.append(new Line( -6, -9, -6,  9,QPen(Qt::darkRed,3)));
	Lines.append(new Line(  6, -9,  6,  9,QPen(Qt::darkRed,3)));
	Lines.append(new Line( -6, -9,  6,  0,QPen(Qt::darkRed,3)));
	Lines.append(new Line( -6,  9,  6,  0,QPen(Qt::darkRed,3)));

	Ports.append(new ComponentPort(-30, 0));
	Ports.append(new ComponentPort( 30, 0));

	x1 = -30; y1 = -11;
	x2 =  30; y2 =  11;

	tx = x1+4;
	ty = y2+4;

	Model = "DIODE_SPICE";

	Props.append(new Property("D", "", true,"Param list and\n .model spec."));
	Props.append(new Property("D_Line 2", "", false,"+ continuation line 1"));
	Props.append(new Property("D_Line 3", "", false,"+ continuation line 2"));
	Props.append(new Property("D_Line 4", "", false,"+ continuation line 3"));
	Props.append(new Property("D_Line 5", "", false,"+ continuation line 4"));

}

DIODE_SPICE::~DIODE_SPICE()
{
}

Component* DIODE_SPICE::newOne()
{
  return new DIODE_SPICE(*this);
}

Element* DIODE_SPICE::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("D Diode");
  BitmapFile = (char *) "DIODE_SPICE";

  if(getNewOne)  return new DIODE_SPICE();
  return 0;
}

#if 0
QString DIODE_SPICE::netlist()
{
    return QString("");
}

QString DIODE_SPICE::spice_netlist(bool)
{
    QString s = ""; // TODO spicecompat::check_refdes(Name,SpiceModel);
    foreach(Port *p1, Ports) {
        QString nam = p1->Connection->name();
        if (nam=="gnd") nam = "0";
        s += " "+ nam+" ";   // node names
    }
 
    QString D= Props.at(0)->Value;
    QString D_Line_2= Props.at(1)->Value;
    QString D_Line_3= Props.at(2)->Value;
    QString D_Line_4= Props.at(3)->Value;
    QString D_Line_5= Props.at(4)->Value;

    if(  D.length()  > 0)          s += QString("%1\n").arg(D);
    if(  D_Line_2.length() > 0 )   s += QString("%1\n").arg(D_Line_2);
    if(  D_Line_3.length() > 0 )   s += QString("%1\n").arg(D_Line_3);
    if(  D_Line_4.length() > 0 )   s += QString("%1\n").arg(D_Line_4);
    if(  D_Line_5.length() > 0 )   s += QString("%1\n").arg(D_Line_5);
 
    return s;
}
#endif

} // namespace
