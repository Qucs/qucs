/***************************************************************************
                                waveac.cpp
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

#ifndef WAVEAC_H
#define WAVEAC_H

#include "diagram.h"


class Waveac : public Diagram  {
public: 
  Waveac(int _cx=0, int _cy=0);
 ~Waveac();


  Diagram* newOne();
  static Element* info(QString&, char* &, bool getNewOne=false);
  int  calcDiagram();
  void calcLimits();
  void calcCoordinate(const double*, const double*, const double*, float*, float*, Axis const*) const;
  void finishMarkerCoordinates(float&, float&) const;
  bool insideDiagram(float, float) const;

protected:
  void clip(Graph::iterator &) const;
  void calcData(Graph*);
};

#endif
