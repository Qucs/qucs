/***************************************************************************
                          component.cpp  -  description
                             -------------------
    begin                : Sat Aug 23 2003
    copyright            : (C) 2003 by Michael Margraf
    email                : margraf@mwt.ee.tu-berlin.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "component.h"

#include <qpoint.h>
#include <qpainter.h>
#include <qstring.h>
#include <qpen.h>



// ***********************************************************************************
// **********                                                               **********
// **********                      class "Component"                        **********
// **********                                                               **********
// ***********************************************************************************
Component::Component()
{
  isWire = false;
  cx = 0;
  cy = 0;

  mirroredX = false;
  rotated = 0;
}

Component::~Component()
{
}

Component* Component::newOne()
{
  return new Component();
}

// -------------------------------------------------------
void Component::Bounding(int& _x1, int& _y1, int& _x2, int& _y2)
{
  _x1 = x1+cx;
  _y1 = y1+cy;
  _x2 = x2+cx;
  _y2 = y2+cy;
}

// -------------------------------------------------------
void Component::setCenter(int x, int y)
{
  cx = x;  // new component center
  cy = y;
}

// -------------------------------------------------------
void Component::paint(QPainter *p)
{
  
  for(Line *p1 = Lines.first(); p1 != 0; p1 = Lines.next()) {     // paint all lines
    p->setPen(p1->style);
    p->drawLine(cx+p1->x1, cy+p1->y1, cx+p1->x2, cy+p1->y2);
  }

  for(Arc *p3 = Arcs.first(); p3 != 0; p3 = Arcs.next()) {     // paint all arcs
    p->setPen(p3->style);
    p->drawArc(cx+p3->x, cy+p3->y, p3->w, p3->h, p3->angle, p3->arclen);
  }

  if(Sign.at(0) == '.') {   // is simulation component (dc, ac, s parameter, ...)
    QFont tmp = p->font();
    p->setFont(QFont("Helvetica",16, QFont::DemiBold));
    p->drawText(cx+x1+5, cy+y1+5, x2-x1, y2-y1, Qt::WordBreak, Description);
    p->setFont(tmp);
  }

  QString s = Name+"\n";
  p->setPen(QPen(QPen::black,1));
  for(Property *p4 = Props.first(); p4 != 0; p4 = Props.next())    // write all properties
    if(p4->display) s += p4->Name+"="+p4->Value+"\n";
  p->drawText(cx+tx, cy+ty, 120, 120, 0, s);

  if(!isActive) {
    p->setPen(QPen(QPen::red,1));
    p->drawRect(cx+x1, cy+y1, x2-x1, y2-y1);
    p->drawLine(cx+x1, cy+y1, cx+x2, cy+y2);
    p->drawLine(cx+x1, cy+y2, cx+x2, cy+y1);
  }

  if(isSelected) {
    p->setPen(QPen(QPen::darkGray,3));
    p->drawRoundRect(cx+x1, cy+y1, x2-x1, y2-y1);
  }
}

// -------------------------------------------------------
// Paints the component when moved with the mouse.
void Component::paintScheme(QPainter *p) //, int x, int y) //const QPoint& pos)
{
  for(Line *p1 = Lines.first(); p1 != 0; p1 = Lines.next())     // paint all lines
    p->drawLine(cx+p1->x1, cy+p1->y1, cx+p1->x2, cy+p1->y2);

  for(Port *p2 = Ports.first(); p2 != 0; p2 = Ports.next())     // paint all ports
    p->drawEllipse(cx+p2->x-4, cy+p2->y-4, 8, 8);

  
  for(Arc *p3 = Arcs.first(); p3 != 0; p3 = Arcs.next())     // paint all arcs
    p->drawArc(cx+p3->x, cy+p3->y, p3->w, p3->h, p3->angle, p3->arclen);
}

// -------------------------------------------------------
// Rotates the component 90° counter-clockwise around its center
void Component::rotate()
{
  int tmp;
  
  for(Line *p1 = Lines.first(); p1 != 0; p1 = Lines.next()) {     // rotate all lines
    tmp = -p1->x1;
    p1->x1 = p1->y1;
    p1->y1 = tmp;
    tmp = -p1->x2;
    p1->x2 = p1->y2;
    p1->y2 = tmp;
  }

  for(Port *p2 = Ports.first(); p2 != 0; p2 = Ports.next()) {     // rotate all ports
    tmp = -p2->x;
    p2->x = p2->y;
    p2->y = tmp;
  }

  for(Arc *p3 = Arcs.first(); p3 != 0; p3 = Arcs.next()) {     // rotate all arcs
    tmp = -p3->x;
    p3->x = p3->y;
    p3->y = tmp - p3->w +1; // +1 is beauty correction
    tmp = p3->w;
    p3->w = p3->h;
    p3->h = tmp;
    p3->angle += 16*90;
    if(p3->angle >= 16*360) p3->angle -= 16*360;;
  }

  tmp = -x1;   // rotate boundings
  x1  = y1; y1 = -x2;
  x2  = y2; y2 = tmp;
  
  tmp = tx;    // rotate text position
  tx  = ty;
  ty  = tmp;

  rotated++;  // keep track of what's done
  rotated &= 3;
}

// -------------------------------------------------------
// Mirrors the component about the x-axis.
void Component::mirrorX()
{
  for(Line *p1 = Lines.first(); p1 != 0; p1 = Lines.next()) {  // mirror all lines
    p1->y1 = -p1->y1;
    p1->y2 = -p1->y2;
  }

  for(Port *p2 = Ports.first(); p2 != 0; p2 = Ports.next()) {  // mirror all ports
    p2->y = -p2->y;
  }

  for(Arc *p3 = Arcs.first(); p3 != 0; p3 = Arcs.next()) {     // mirror all arcs
    p3->y = -p3->y - p3->h + 1; // +1 is beauty correction
    if(p3->angle > 180) p3->angle -= 16*360;
    p3->angle  = -p3->angle;  // mirror
    p3->angle -= p3->arclen;  // go back to end of arc
    if(p3->angle < 0) p3->angle += 16*360;  // angle has to be > 0
  }

  int tmp = y1;
  y1  = -y2; y2 = -tmp;   // mirror boundings
  ty  = y2+4;   // mirror text position

  mirroredX = !mirroredX;   // keep track of what's done
  rotated += rotated << 1;
  rotated &= 3;
}

// -------------------------------------------------------
QString Component::NetList()
{
  if(Model.isEmpty()) return QString("");   // dummy elements (e.g. ground)
  if(!isActive) return QString("");         // should it be simulated ?
  QString s = Model+":"+Name;
  
  
  for(Port *p1 = Ports.first(); p1 != 0; p1 = Ports.next())     // work on all ports
    s += " "+p1->Connection->Name;    // node names

  for(Property *p2 = Props.first(); p2 != 0; p2 = Props.next())    // work on all properties
    s += " \""+p2->Value+"\"";

  return s;
}

// -------------------------------------------------------
QString Component::save()
{
  QString num;
  QString s = "<"+Sign;
  if(Model.isEmpty()) s += " *";
  else s += " "+Model;
  if(Name.isEmpty()) s += " *";
  else s += " "+Name;

  if(isActive) s += " 1";
  else s += " 0";
  s += " "+num.setNum(cx);  // all numbers must be in different lines !?!
  s += " "+num.setNum(cy);
  s += " "+num.setNum(tx);
  s += " "+num.setNum(ty);
  if(mirroredX) s += " 1";
  else s += " 0";
  s += " "+num.setNum(rotated);
  
  for(Property *p1 = Props.first(); p1 != 0; p1 = Props.next()) {   // write all properties
    s += " \""+p1->Value+"\"";
    if(p1->display) s += " 1";
    else s += " 0";
  }

  return s+">";
}

// -------------------------------------------------------
bool Component::load(const QString& _s)
{
  bool ok;
  int  ttx, tty, tmp;
  QString s = _s;

  if(s.at(0) != '<') return false;
  if(s.at(s.length()-1) != '>') return false;
  s = s.mid(1, s.length()-2);   // cut off start and end character

  QString n;
  Sign = s.section(' ',0,0);    // Sign

  Model = s.section(' ',1,1);   // Model
  if(Model == "*") Model = "";

  Name = s.section(' ',2,2);      // Name
  if(Name == "*") Name = "";

  n  = s.section(' ',3,3);    // isActive
  if(n.toInt(&ok) == 1) isActive = true;
  else isActive = false;
  if(!ok) return false;

  n  = s.section(' ',4,4);    // cx
  cx = n.toInt(&ok);
  if(!ok) return false;

  n  = s.section(' ',5,5);    // cy
  cy = n.toInt(&ok);
  if(!ok) return false;

  n  = s.section(' ',6,6);    // tx
  ttx = n.toInt(&ok);
  if(!ok) return false;

  n  = s.section(' ',7,7);    // ty
  tty = n.toInt(&ok);
  if(!ok) return false;

  n  = s.section(' ',8,8);    // mirroredX
  if(n.toInt(&ok) == 1) mirroredX = true;
  else mirroredX = false;
  if(!ok) return false;
  if(mirroredX) mirrorX();  // mirror component

  n  = s.section(' ',9,9);    // rotated
  tmp = n.toInt(&ok);
  if(!ok) return false;
  for(int z=0; z<tmp; z++) rotate();   // rotate component

  tx = ttx; ty = tty;   // restore text position (was change by rotate/mirror)

  int z=0;
  for(Property *p1 = Props.first(); p1 != 0; p1 = Props.next()) {   // load all properties
    z++;
    p1->Value = s.section('"',z,z);    // property value
  
    z++;
    n  = s.section('"',z,z);    // display
    if(n.toInt(&ok) == 1) p1->display = true;
    else p1->display = false;
    if(!ok) return false;
  }
  
  return true;
}

// ******************************************************************************************
// **********                                                                      **********
// **********                            The Components                            **********
// **********                                                                      **********
// ******************************************************************************************

Resistor::Resistor()
{
  Description = "resistor";

  Lines.append(new Line(-18, -9, 18, -9,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 18, -9, 18,  9,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 18,  9,-18,  9,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-18,  9,-18, -9,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-30,  0,-18,  0,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 18,  0, 30,  0,QPen(QPen::darkBlue,2)));

  Ports.append(new Port( 30,  0));
  Ports.append(new Port(-30,  0));

  x1 = -30; y1 = -11;
  x2 =  30; y2 =  11;

  tx = -20;
  ty = y2+4;
  Sign  = QString("R");
  Model = QString("R");
  Name  = QString("R");

  Props.append(new Property("R", "50 Ohm", true));
  Props.append(new Property("Noise", "yes", false));

  isSelected = false;
  isActive   = true;
}

Resistor::~Resistor()
{
}

Resistor* Resistor::newOne()
{
  return new Resistor();
}

// --------------------------------------------------------------------------
ResistorUS::ResistorUS()
{
  Description = "resistor (US symbol)";

  Lines.append(new Line(-30,  0,-18,  0,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-18,  0,-15, -7,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-15, -7, -9,  7,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( -9,  7, -3, -7,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( -3, -7,  3,  7,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(  3,  7,  9, -7,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(  9, -7, 15,  7,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 15,  7, 18,  0,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 18,  0, 30,  0,QPen(QPen::darkBlue,2)));

  Ports.append(new Port( 30,  0));
  Ports.append(new Port(-30,  0));

  x1 = -30; y1 = -9;
  x2 =  30; y2 =  9;

  tx = -20;
  ty = y2+4;
  Sign  = QString("Rus");
  Model = QString("R");
  Name  = QString("R");

  Props.append(new Property("R", "50 Ohm", true, "ohmic resistance in Ohms"));
  Props.append(new Property("Noise", "yes", false, "thermal noise yes or no"));

  isSelected = false;
  isActive   = true;
}

ResistorUS::~ResistorUS()
{
}

ResistorUS* ResistorUS::newOne()
{
  return new ResistorUS();
}

// --------------------------------------------------------------------------
Capacitor::Capacitor()
{
  Description = "capacitor";

  Lines.append(new Line(- 4,-11, -4, 11,QPen(QPen::darkBlue,4)));
  Lines.append(new Line(  4,-11,  4, 11,QPen(QPen::darkBlue,4)));
  Lines.append(new Line(-30,  0, -4,  0,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(  4,  0, 30,  0,QPen(QPen::darkBlue,2)));

  Ports.append(new Port( 30,  0));
  Ports.append(new Port(-30,  0));

  x1 = -30; y1 = -13;
  x2 =  30; y2 =  13;

  tx = -20;
  ty = y2+4;
  Sign  = QString("C");
  Model = QString("C");
  Name  = QString("C");

  Props.append(new Property("C", "1 pF", true, "capacitance in Farad"));

  isSelected = false;
  isActive   = true;
}

Capacitor::~Capacitor()
{
}

Capacitor* Capacitor::newOne()
{
  return new Capacitor();
}

// --------------------------------------------------------------------------
Inductor::Inductor()
{
  Description = "inductor";

  Arcs.append(new Arc(-17, -6, 13, 13,  0, 16*180,QPen(QPen::darkBlue,2)));
  Arcs.append(new Arc( -6, -6, 13, 13,  0, 16*180,QPen(QPen::darkBlue,2)));
  Arcs.append(new Arc(  5, -6, 13, 13,  0, 16*180,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-30,  0,-17,  0,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 17,  0, 30,  0,QPen(QPen::darkBlue,2)));

  Ports.append(new Port( 30,  0));
  Ports.append(new Port(-30,  0));

  x1 = -30; y1 = -10;
  x2 =  30; y2 =   6;

  tx = -20;
  ty = y2+4;
  Sign  = QString("L");
  Model = QString("L");
  Name  = QString("L");

  Props.append(new Property("L", "1 nH", true, "inductance in Henry"));
  
  isSelected = false;
  isActive   = true;
}

Inductor::~Inductor()
{
}

Inductor* Inductor::newOne()
{
  return new Inductor();
}

// --------------------------------------------------------------------------
Transformer::Transformer()
{
  Description = "ideal transformer";

  Arcs.append(new Arc(-16,-18, 13, 13, 16*270, 16*180,QPen(QPen::darkBlue,2)));
  Arcs.append(new Arc(-16, -6, 13, 13, 16*270, 16*180,QPen(QPen::darkBlue,2)));
  Arcs.append(new Arc(-16,  5, 13, 13, 16*270, 16*180,QPen(QPen::darkBlue,2)));
  Arcs.append(new Arc(  4,-18, 13, 13,  16*90, 16*180,QPen(QPen::darkBlue,2)));
  Arcs.append(new Arc(  4, -6, 13, 13,  16*90, 16*180,QPen(QPen::darkBlue,2)));
  Arcs.append(new Arc(  4,  5, 13, 13,  16*90, 16*180,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-10,-18,-10,-30,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-10,-30,-30,-30,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 10,-18, 10,-30,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 10,-30, 30,-30,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-10, 17,-10, 30,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-10, 30,-30, 30,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 10, 17, 10, 30,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 10, 30, 30, 30,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( -1,-20, -1, 20,QPen(QPen::darkBlue,1)));
  Lines.append(new Line(  1,-20,  1, 20,QPen(QPen::darkBlue,1)));
  Arcs.append(new Arc(-21,-24,  6,  6,  0, 16*360,QPen(QPen::darkBlue,2)));
  Arcs.append(new Arc( 15,-24,  6,  6,  0, 16*360,QPen(QPen::darkBlue,2)));
  

  Ports.append(new Port( 30,-30));
  Ports.append(new Port(-30,-30));
  Ports.append(new Port( 30, 30));
  Ports.append(new Port(-30, 30));

  x1 = -30; y1 = -30;
  x2 =  30; y2 =  30;

  tx = -20;
  ty = y2+4;
  Sign  = QString("Tr");
  Model = QString("Tr");
  Name  = QString("Tr");

  Props.append(new Property("n", "1", true, "voltage transformation ration"));

  isSelected = false;
  isActive   = true;
}

Transformer::~Transformer()
{
}

Transformer* Transformer::newOne()
{
  return new Transformer();
}

// --------------------------------------------------------------------------
symTrafo::symTrafo()
{
  Description = "ideal symmetrical transformer";

  Arcs.append(new Arc(-16,-46, 13, 13, 16*270, 16*180,QPen(QPen::darkBlue,2)));
  Arcs.append(new Arc(-16,-34, 13, 13, 16*270, 16*180,QPen(QPen::darkBlue,2)));
  Arcs.append(new Arc(-16,-22, 13, 13, 16*270, 16*180,QPen(QPen::darkBlue,2)));
  Arcs.append(new Arc(-16, 34, 13, 13, 16*270, 16*180,QPen(QPen::darkBlue,2)));
  Arcs.append(new Arc(-16, 22, 13, 13, 16*270, 16*180,QPen(QPen::darkBlue,2)));
  Arcs.append(new Arc(-16, 10, 13, 13, 16*270, 16*180,QPen(QPen::darkBlue,2)));
  Arcs.append(new Arc(  4,-18, 13, 13,  16*90, 16*180,QPen(QPen::darkBlue,2)));
  Arcs.append(new Arc(  4, -6, 13, 13,  16*90, 16*180,QPen(QPen::darkBlue,2)));
  Arcs.append(new Arc(  4,  5, 13, 13,  16*90, 16*180,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-10,-46,-10,-60,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-10,-60,-30,-60,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 10,-18, 10,-30,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 10,-30, 30,-30,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-10, 46,-10, 60,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-10, 60,-30, 60,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 10, 17, 10, 30,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 10, 30, 30, 30,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-10, 10,-10,-10,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-10,  0,-30,  0,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( -1,-45, -1, 45,QPen(QPen::darkBlue,1)));
  Lines.append(new Line(  1,-45,  1, 45,QPen(QPen::darkBlue,1)));
  Arcs.append(new Arc(-21,-50,  6,  6,  0, 16*360,QPen(QPen::darkBlue,2)));
  Arcs.append(new Arc(-21,  4,  6,  6,  0, 16*360,QPen(QPen::darkBlue,2)));
  Arcs.append(new Arc( 15,-24,  6,  6,  0, 16*360,QPen(QPen::darkBlue,2)));
  Arcs.append(new Arc(-12, -2,  4,  4,  0, 16*360,QPen(QPen::darkBlue,2)));
      
  Ports.append(new Port( 30,-30));
  Ports.append(new Port(-30,-60));
  Ports.append(new Port( 30, 30));
  Ports.append(new Port(-30, 60));
  Ports.append(new Port(-30,  0));

  x1 = -30; y1 = -60;
  x2 =  30; y2 =  60;

  tx = -20;
  ty = y2+4;
  Sign  = QString("sTr");
  Model = QString("sTr");
  Name  = QString("Tr");

  Props.append(new Property("n", "1", true, "voltage transformation ration"));

  isSelected = false;
  isActive   = true;
}

symTrafo::~symTrafo()
{
}

symTrafo* symTrafo::newOne()
{
  return new symTrafo();
}

// --------------------------------------------------------------------------
Ground::Ground()
{
  Description = "ground (reference potential)";

  Lines.append(new Line(  0,  0,  0, 10,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-11, 10, 11, 10,QPen(QPen::darkBlue,3)));
  Lines.append(new Line( -7, 16,  7, 16,QPen(QPen::darkBlue,3)));
  Lines.append(new Line( -3, 22,  3, 22,QPen(QPen::darkBlue,3)));

  Ports.append(new Port(  0,  0));

  x1 = -12; y1 =  0;
  x2 =  12; y2 = 25;

  tx = 0;
  ty = 0;
  Sign  = QString("GND");
  Model = QString("");
  Name  = QString("");

  isSelected = false;
  isActive   = true;
}

Ground::~Ground()
{
}

Ground* Ground::newOne()
{
  return new Ground();
}

// --------------------------------------------------------------------------
Volt_dc::Volt_dc()
{
  Description = "ideal dc voltage source";

  Lines.append(new Line(  4,-13,  4, 13,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( -4, -6, -4,  6,QPen(QPen::darkBlue,4)));
  Lines.append(new Line( 30,  0,  4,  0,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( -4,  0,-30,  0,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 10,  5, 10, 11,QPen(QPen::red,1)));
  Lines.append(new Line( 13,  8,  7,  8,QPen(QPen::red,1)));
  Lines.append(new Line( -9,  5, -9, 11,QPen(QPen::black,1)));

  Ports.append(new Port( 30,  0));
  Ports.append(new Port(-30,  0));

  x1 = -30; y1 = -14;
  x2 =  30; y2 =  14;

  tx = -20;
  ty = y2+4;
  Sign  = QString("V");
  Model = QString("V");
  Name  = QString("V");

  Props.append(new Property("U", "1 V", true, "voltage in Volts"));

  isSelected = false;
  isActive   = true;
}

Volt_dc::~Volt_dc()
{
}

Volt_dc* Volt_dc::newOne()
{
  return new Volt_dc();
}

// --------------------------------------------------------------------------
Ampere_dc::Ampere_dc()
{
  Description = "ideal dc current source";

  Arcs.append(new Arc(-12,-12, 25, 25,  0, 16*360,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-30,  0,-12,  0,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 30,  0, 12,  0,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( -7,  0,  7,  0,QPen(QPen::darkBlue,3)));
  Lines.append(new Line(  6,  0,  0, -4,QPen(QPen::darkBlue,3)));
  Lines.append(new Line(  6,  0,  0,  4,QPen(QPen::darkBlue,3)));

  Ports.append(new Port( 30,  0));
  Ports.append(new Port(-30,  0));

  x1 = -30; y1 = -14;
  x2 =  30; y2 =  14;

  tx = -20;
  ty = y2+4;
  Sign  = QString("I");
  Model = QString("I");
  Name  = QString("I");

  Props.append(new Property("I", "1 mA", true, "current in Ampere"));

  isSelected = false;
  isActive   = true;
}

Ampere_dc::~Ampere_dc()
{
}

Ampere_dc* Ampere_dc::newOne()
{
  return new Ampere_dc();
}

// --------------------------------------------------------------------------
Volt_ac::Volt_ac()
{
  Description = "ideal ac voltage source";

  Arcs.append(new Arc(-12,-12, 25, 25,     0, 16*360,QPen(QPen::darkBlue,2)));
  Arcs.append(new Arc( -4, -8,  9,  9,16*270, 16*180,QPen(QPen::darkBlue,2)));
  Arcs.append(new Arc( -3,  0,  9,  9, 16*90, 16*180,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-30,  0,-12,  0,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 30,  0, 12,  0,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 18,  5, 18, 11,QPen(QPen::red,1)));
  Lines.append(new Line( 21,  8, 15,  8,QPen(QPen::red,1)));
  Lines.append(new Line(-18,  5,-18, 11,QPen(QPen::black,1)));

  Ports.append(new Port( 30,  0));
  Ports.append(new Port(-30,  0));

  x1 = -30; y1 = -14;
  x2 =  30; y2 =  14;

  tx = -20;
  ty = y2+4;
  Sign  = QString("Vac");
  Model = QString("Vac");
  Name  = QString("V");

  Props.append(new Property("U", "1 V", true, "rms voltage in Volts"));
  Props.append(new Property("f", "1 GHz", true, "frequency in Hertz"));

  isSelected = false;
  isActive   = true;
}

Volt_ac::~Volt_ac()
{
}

Volt_ac* Volt_ac::newOne()
{
  return new Volt_ac();
}

// --------------------------------------------------------------------------
Source_ac::Source_ac()
{
  Description = "ac power source";

  Lines.append(new Line(-22,-11, 23,-11,QPen(QPen::darkGray,1)));
  Lines.append(new Line(-22, 10, 23, 10,QPen(QPen::darkGray,1)));
  Lines.append(new Line(-22,-11,-22, 10,QPen(QPen::darkGray,1)));
  Lines.append(new Line( 23,-11, 23, 10,QPen(QPen::darkGray,1)));

  Arcs.append(new Arc(-20, -9, 18, 18,     0, 16*360,QPen(QPen::darkBlue,2)));
  Arcs.append(new Arc(-15, -6,  7,  7,16*270, 16*180,QPen(QPen::darkBlue,2)));
  Arcs.append(new Arc(-14, -1,  7,  7, 16*90, 16*180,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-30,  0,-20,  0,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 30,  0, 20,  0,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( -3,  0,  3,  0,QPen(QPen::darkBlue,2)));

  Lines.append(new Line(  3, -6, 20, -6,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(  3,  6, 20,  6,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(  3, -6,  3,  6,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 20, -6, 20,  6,QPen(QPen::darkBlue,2)));

  Lines.append(new Line( 25,  5, 25, 11,QPen(QPen::red,1)));
  Lines.append(new Line( 28,  8, 22,  8,QPen(QPen::red,1)));
  Lines.append(new Line(-25,  5,-25, 11,QPen(QPen::black,1)));

  Ports.append(new Port( 30,  0));
  Ports.append(new Port(-30,  0));

  x1 = -30; y1 = -14;
  x2 =  30; y2 =  14;

  tx = -20;
  ty = y2+4;
  Sign  = QString("Pac");
  Model = QString("Pac");
  Name  = QString("P");

  Props.append(new Property("P", "0 dBm", true, "ac power in Watts"));
  Props.append(new Property("f", "1 GHz", true, "frequency in Hertz"));

  isSelected = false;
  isActive   = true;
}

Source_ac::~Source_ac()
{
}

Source_ac* Source_ac::newOne()
{
  return new Source_ac();
}

// ******************************************************************************************
// **********                                                                      **********
// **********                         The Simulation Sets                          **********
// **********                                                                      **********
// ******************************************************************************************

DC_Sim::DC_Sim()
{
  Description = "dc simulation";

  Lines.append(new Line(-56,-12, 56,-12,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-56, 12, 56, 12,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-56,-12,-56, 12,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 56,-12, 56, 12,QPen(QPen::darkBlue,2)));

  Lines.append(new Line(-51, 17, 60, 17,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-56, 12,-51, 17,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 56, 12, 60, 17,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 60, -7, 60, 17,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 56,-12, 60, -7,QPen(QPen::darkBlue,2)));

  x1 = -57; y1 = -13;
  x2 =  61; y2 =  18;

  tx = x1+5;
  ty = y2+4;
  Sign  = QString(".DC");
  Model = QString("DC");
  Name  = QString("DC");

  isSelected = false;
  isActive   = true;
}

DC_Sim::~DC_Sim()
{
}

DC_Sim* DC_Sim::newOne()
{
  return new DC_Sim();
}

// --------------------------------------------------------------------------
AC_Sim::AC_Sim()
{
  Description = "ac simulation";

  Lines.append(new Line(-56,-12, 56,-12,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-56, 12, 56, 12,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-56,-12,-56, 12,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 56,-12, 56, 12,QPen(QPen::darkBlue,2)));

  Lines.append(new Line(-51, 17, 60, 17,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-56, 12,-51, 17,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 56, 12, 60, 17,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 60, -7, 60, 17,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 56,-12, 60, -7,QPen(QPen::darkBlue,2)));

  x1 = -57; y1 = -13;
  x2 =  61; y2 =  18;

  tx = x1+5;
  ty = y2+4;
  Sign  = QString(".AC");
  Model = QString("AC");
  Name  = QString("AC");

  Props.append(new Property("Start", "1 GHz", true, "start frequency in Hertz"));
  Props.append(new Property("Stop", "10 GHz", true, "stop frequency in Hertz"));
  Props.append(new Property("Step", "1 GHz", true, "frequency steps in Hertz"));

  isSelected = false;
  isActive   = true;
}

AC_Sim::~AC_Sim()
{
}

AC_Sim* AC_Sim::newOne()
{
  return new AC_Sim();
}

// --------------------------------------------------------------------------
TR_Sim::TR_Sim()
{
  Description = "transient simulation";

  Lines.append(new Line(-45,-21, 45,-21,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-45, 21, 45, 21,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-45,-21,-45, 21,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 45,-21, 45, 21,QPen(QPen::darkBlue,2)));

  Lines.append(new Line(-41, 26, 49, 26,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-45, 21,-41, 26,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 45, 21, 49, 26,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 49,-16, 49, 26,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 45,-21, 49,-16,QPen(QPen::darkBlue,2)));

  x1 = -46; y1 = -22;
  x2 =  50; y2 =  27;

  tx = x1+5;
  ty = y2+4;
  Sign  = QString(".TR");
  Model = QString("TR");
  Name  = QString("TR");

  Props.append(new Property("Start", "0", true, "start time in seconds"));
  Props.append(new Property("Stop", "1 ms", true, "stop time in seconds"));
  Props.append(new Property("Step", "10 us", true, "time step in seconds"));

  isSelected = false;
  isActive   = true;
}

TR_Sim::~TR_Sim()
{
}

TR_Sim* TR_Sim::newOne()
{
  return new TR_Sim();
}

// --------------------------------------------------------------------------
SP_Sim::SP_Sim()
{
  Description = "S parameter simulation";

  Lines.append(new Line(-53,-21, 53,-21,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-53, 21, 53, 21,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-53,-21,-53, 21,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 53,-21, 53, 21,QPen(QPen::darkBlue,2)));

  Lines.append(new Line(-49, 26, 57, 26,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-53, 21,-49, 26,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 53, 21, 57, 26,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 57,-16, 57, 26,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 53,-21, 57,-16,QPen(QPen::darkBlue,2)));

  x1 = -54; y1 = -22;
  x2 =  58; y2 =  27;

  tx = x1+5;
  ty = y2+4;
  Sign  = QString(".SP");
  Model = QString("SP");
  Name  = QString("SP");

  Props.append(new Property("Start", "1 GHz", true, "start frequency in Hertz"));
  Props.append(new Property("Stop", "10 GHz", true, "stop frequency in Hertz"));
  Props.append(new Property("Step", "1 GHz", true, "frequency steps in Hertz"));

  isSelected = false;
  isActive   = true;
}

SP_Sim::~SP_Sim()
{
}

SP_Sim* SP_Sim::newOne()
{
  return new SP_Sim();
}

// --------------------------------------------------------------------------
HB_Sim::HB_Sim()
{
  Description = "Harmonic balance simulation";

  Lines.append(new Line(-75,-21, 75,-21,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-75, 21, 75, 21,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-75,-21,-75, 21,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 75,-21, 75, 21,QPen(QPen::darkBlue,2)));

  Lines.append(new Line(-71, 26, 79, 26,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-75, 21,-71, 26,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 75, 21, 79, 26,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 79,-16, 79, 26,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 75,-21, 79,-16,QPen(QPen::darkBlue,2)));

  x1 = -76; y1 = -22;
  x2 =  80; y2 =  27;

  tx = x1+5;
  ty = y2+4;
  Sign  = QString(".HB");
  Model = QString("HB");
  Name  = QString("HB");

  Props.append(new Property("f", "1 GHz", true, "frequency in Hertz"));
  Props.append(new Property("n", "4", true, "number of harmonics"));

  isSelected = false;
  isActive   = true;
}

HB_Sim::~HB_Sim()
{
}

HB_Sim* HB_Sim::newOne()
{
  return new HB_Sim();
}

// --------------------------------------------------------------------------
Param_Sweep::Param_Sweep()
{
  Description = "Parameter sweep";

  Lines.append(new Line(-46,-22, 46,-22,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-46, 22, 46, 22,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-46,-22,-46, 22,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 46,-22, 46, 22,QPen(QPen::darkBlue,2)));

  Lines.append(new Line(-42, 27, 50, 27,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-46, 22,-42, 27,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 46, 22, 50, 27,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 50,-17, 50, 27,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 46,-22, 50,-17,QPen(QPen::darkBlue,2)));

  x1 = -47; y1 = -23;
  x2 =  51; y2 =  28;

  tx = x1+5;
  ty = y2+4;
  Sign  = QString(".SW");
  Model = QString("SW");
  Name  = QString("SW");

  Props.append(new Property("Param", "R1", true, "parameter to sweep"));
  Props.append(new Property("Start", "5 Ohm", true, "start value for sweep"));
  Props.append(new Property("Stop", "50 Ohm", true, "stop value for sweep"));
  Props.append(new Property("Step", "5 Ohm", true, "step size for sweep"));

  isSelected = false;
  isActive   = true;
}

Param_Sweep::~Param_Sweep()
{
}

Param_Sweep* Param_Sweep::newOne()
{
  return new Param_Sweep();
}
