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


class Rect3DDiagram : public Diagram  {
public:
  Rect3DDiagram(int _cx=0, int _cy=0);
 ~Rect3DDiagram();


  Diagram* newOne();
  static Element* info(QString&, char* &, bool getNewOne=false);
  int  calcDiagram();
  void calcLimits();
  int  calcAxis(Axis*, int, int, int, int, bool);
  void calcData(Graph *g);
  void calcCoordinate(double* &, double* &, double* &, int*, int*, Axis*);

  void createAxisLabels() {};   // labels created during calcDiagram
  bool insideDiagram(int, int);
  void clip(int* &);

  void removeLines();

private:
  void   calcCoefficients();
  double calcX_2D(double, double, double);
  double calcY_2D(double, double, double);
  double calcZ_2D(double, double, double);

  int xorig, yorig;    // where is the 3D origin with respect to cx/cy
  double cxx, cxy, cxz, cyx, cyy, cyz, czx, czy, czz; // coefficients 3D -> 2D
  double scaleX, scaleY;
};

#endif
