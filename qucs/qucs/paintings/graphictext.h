/***************************************************************************
                          graphictext.h  -  description
                             -------------------
    begin                : Mon Nov 24 2003
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

#ifndef GRAPHICTEXT_H
#define GRAPHICTEXT_H

#include "painting.h"

/**
  *@author Michael Margraf
  */

class GraphicText : public Painting  {
public: 
  GraphicText();
  ~GraphicText();

  void paintScheme(QPainter*);
  void getCenter(int& x, int &y);
  void setCenter(int x, int y, bool relative=false);

  GraphicText* newOne();
  bool load(const QString& s);
  QString save();
  void paint(QPainter *p);
  void MouseMoving(int x, int y, int, int, QPainter *p, bool drawn);
  bool MousePressing();
  bool getSelected(int x, int y);
  void Bounding(int&, int&, int&, int&);

  void rotate();
  void mirrorX();
  void mirrorY();
  bool Dialog();

  QColor   Color;
  QFont    Font;
  QString  Text;
  int      Angle;
  double   cx_d, cy_d;    // cx/cy coordinates already rotated
};

#endif
