/***************************************************************************
                                 ground.h
                                ----------
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

#ifndef GROUND_H
#define GROUND_H

#include "schematic.h"
#include "component.h"


class Ground : public Component  {
public:
  Ground();
 ~Ground();
  Component* newOne() {return new Ground(*this);}
  static Element* info(QString&, char* &, bool getNewOne=false);

//private: // overrides
//  virtual void insertComponentNodes(Schematic *sch, bool noOptimize){
//	  Component* c=this;
//	  Component::insertComponentNodes(sch, noOptimize);
//	  if(c->obsolete_model_hack() == "GND") { // BUG
//		  c->gnd_obsolete_model_override_hack("x");
//		  Element *pe = sch->getWireLabel(c->Ports.first()->Connection);
//		  if(pe) if((pe->Type & isComponent) == 0)
//		  {
//			  delete ((Conductor*)pe)->Label;
//			  ((Conductor*)pe)->Label = 0;
//		  }
//		  c->gnd_obsolete_model_override_hack("GND");
//	  }
//  }

protected:
  QString netlist() const;
};

#endif
