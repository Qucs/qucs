/***************************************************************************
                          polardiagram.h  -  description
                             -------------------
    begin                : Fri Oct 17 2003
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

#ifndef POLARDIAGRAM_H
#define POLARDIAGRAM_H

#include "diagram.h"


/**
  *@author Michael Margraf
  */

class PolarDiagram : public Diagram  {
public: 
  PolarDiagram(int _cx=0, int _cy=0);
  ~PolarDiagram();


  PolarDiagram* newOne();
  int calcDiagram();
  int calcCoordinate(double* &, double* &, int*, int*, Axis*);
};

#endif
