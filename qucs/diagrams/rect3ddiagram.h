/***************************************************************************
                              rect3ddiagram.h
                             -----------------
    begin                : Sat Mar 5 2005
    copyright            : (C) 2005 by Michael Margraf
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

#ifndef RECT3DDIAGRAM_H
#define RECT3DDIAGRAM_H

#include "diagram.h"

/**
  *@author Michael Margraf
  */

class Rect3DDiagram : public Diagram  {
public:
  Rect3DDiagram(int _cx=0, int _cy=0);
  ~Rect3DDiagram();


  Diagram* newOne();
  int  calcDiagram();
  void calcLimits();
  bool calcYAxis(Axis*, int);
  void calcData(Graph *g);
  void calcCoordinate(double* &, double* &, double* &, int*, int*, Axis*);

private:
  void calcCoefficients();
  double calcX_2D(double, double, double);
  double calcY_2D(double, double, double);

  int xorig, yorig;    // where is the 3D origin with respect to cx/cy
  double cxx, cxy, cxz, cyx, cyy, cyz; // coefficients 3D -> 2D transformation
  double scaleX, scaleY;
};

#endif
