/***************************************************************************
                          rectdiagram.h  -  description
                             -------------------
    begin                : Thu Oct 2 2003
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

#ifndef RECTDIAGRAM_H
#define RECTDIAGRAM_H

#include "diagram.h"

/**
  *@author Michael Margraf
  */

class RectDiagram : public Diagram  {
public: 
  RectDiagram(int _cx=0, int _cy=0);
  ~RectDiagram();


  RectDiagram* newOne();
  void   calcDiagram();
  void   calcData(Graph *g);

  QString xLabel, yLabel;
};

#endif
