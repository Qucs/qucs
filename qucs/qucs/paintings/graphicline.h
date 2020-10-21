/***************************************************************************
                               graphicline.h
                              ---------------
    begin                : Mon Nov 24 2003
    copyright            : (C) 2003 by Michael Margraf
    email                : michael.margraf@alumni.tu-berlin.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef GRAPHICLINE_H
#define GRAPHICLINE_H

#include "painting.h"

#include <QPen>


class GraphicLine : public Painting  {
public:
  GraphicLine();
  ~GraphicLine();
  
  void setSomeStuff( int cx_=0, int cy_=0, int x2_=0, int y2_=0,
              QPen Pen_=QPen(QColor()));

  void paintScheme(SchematicDoc*);
  void getCenter(int&, int&);
  void setCenter(int, int, bool relative=false);

  Element* clone() const {return new GraphicLine(*this);}
  static Element* info(QString&, char* &, bool getNewOne=false);
  bool load(const QString&);
  QString save();
  QString saveCpp();
  QString saveJSON();
  void paint(ViewPainter*) const override;
  void MouseMoving(SchematicDoc*, int, int, int, int, SchematicDoc*, int, int, bool);
  bool MousePressing();
  bool getSelected(float, float, float);
  void Bounding(int&, int&, int&, int&);
  bool resizeTouched(float, float, float);
  void MouseResizeMoving(int, int, SchematicDoc*);

  void rotate();
  void mirrorX();
  void mirrorY();
  bool Dialog();

  QPen   Pen;
};

#endif
