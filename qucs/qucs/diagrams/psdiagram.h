/***************************************************************************
                        psdiagram.h  -  description
                             -------------------
    begin                : Sat Nov 6 2004
    copyright            : (C) 2004 by Michael Margraf
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

#ifndef PSDIAGRAM_H
#define PSDIAGRAM_H

#include "diagram.h"


/**
  *@author Michael Margraf
  */

// mixed polar and smith diagram
class PSDiagram : public Diagram  {
public:
  PSDiagram(int _cx=0, int _cy=0, bool _polarUp=true);
  ~PSDiagram();


  PSDiagram* newOne();
  int calcDiagram();
  int calcCoordinate(double* &, double* &, int*, int*, Axis*);
};

#endif
