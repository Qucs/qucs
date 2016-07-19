/***************************************************************************
                                phasordiagram.cpp
                               -----------
    begin                : Fri Sep 25 2016
    copyright            : (C) 2016 by Nelson Martins
    email                : macedo.pp@gmail.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef PHASORDIAGRAM_H
#define PHASORDIAGRAM_H

#include "diagram.h"

class PhasorDiagram : public Diagram  {
public: 
  PhasorDiagram(int _cx=0, int _cy=0);
 ~PhasorDiagram();


  Diagram* newOne();
  static Element* info(QString&, char* &, bool getNewOne=false);
  int  calcDiagram();
  void calcLimits();
  void calcCoordinatePh(const double*, float*, float*, Axis const*, Axis const*) const;
  void finishMarkerCoordinates(float&, float&) const;
  void calcRestAxis(Axis* , Axis* , Axis*); //aqui

protected:
  void calcData(Graph*);

};

#endif
