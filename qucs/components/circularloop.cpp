/*
* circularloop.cpp - Circular loop inductor class implementation
*
* copyright (C) 2016 Andres Martinez-Mera <andresmartinezmera@gmail.com>
*
* This is free software; you can redistribute it and/or modify
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
*
*/

#include "circularloop.h"


circularloop::circularloop()
{
  Description = QObject::tr("Printed loop inductor");

  //Loop
  Arcs.push_back(qucs::Arc(-18, -25, 35, 35, -370, 16*230,QPen(Qt::darkBlue,3)));

  Lines.push_back(qucs::Line(-30,  0, -16,  0,QPen(Qt::darkBlue,4)));
  Lines.push_back(qucs::Line(  16,  0, 30,  0,QPen(Qt::darkBlue,4)));

  Ports.push_back(qucs::Port(-30, 0));
  Ports.push_back(qucs::Port( 30, 0));

  set_x1(-30);
  set_y1(-30);
  set_x2(30);
  set_y2(5);

  set_qucs_text_position(x1(), y1()+40);
  Model = "CIRCULARLOOP";
  Name  = "CIRCULARLOOP";

  Props.push_back(qucs::Property("Subst", "Subst1", true,
		QObject::tr("Substrate")));
  Props.push_back(qucs::Property("W", "25 um", false,
		QObject::tr("Width of line")));
  Props.push_back(qucs::Property("a", "500 um", false,
		QObject::tr("Radius")));
  Props.push_back(qucs::Property("Temp", "26.85", false,
		QObject::tr("simulation temperature in degree Celsius")));

}

circularloop::~circularloop()
{
}

Component* circularloop::newOne()
{
  return new circularloop();
}

Element* circularloop::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Circular loop");
  BitmapFile = (char *) "circularloop";

  if(getNewOne)  return new circularloop();
  return 0;
}
