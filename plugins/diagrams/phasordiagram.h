/*
 * phasordiagram.h - Header file for the phasor diagram
 *
 * Copyright (C) 2016, Nelson Martins, macedo.pp@gmail.com
 *		 2017, Alberto Silva, 1120196@isep.ipp.pt	
 *		
 * This file is part of Qucs
 *
 * Qucs is free software; you can redistribute it and/or modify
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
 * along with Qucs.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

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
  void calcRestAxis(Axis* , Axis* , Axis*);

protected:
  void calcData(Graph*);

private:
  Axis  xAxisV, yAxisV, zAxisV, xAxisI, yAxisI, zAxisI, xAxisP, yAxisP, zAxisP, xAxisZ, yAxisZ,
 zAxisZ, *xAxisA, *yAxisA, *zAxisA;
};

#endif
