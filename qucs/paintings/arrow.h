/***************************************************************************
                                arrow.h
                               ---------
    begin                : Sun Nov 23 2003
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

#ifndef ARROW_H
#define ARROW_H

#include "painting.h"

#include <QPen>


class Arrow : public Painting  {
public:
  Arrow();
 ~Arrow();

  void paint(ViewPainter*);
  void paintScheme(Schematic*);
  void getCenter(int&, int&);
  void setCenter(int, int, bool relative=false);

  std::string dev_type()const override {return "Arrow";}
  std::string attr_get()const override;
  void set_attribute(std::string name, std::string value)override;

  Painting* newOne();
  static Element* info(QString&, char* &, bool getNewOne=false);
  bool load(const QString&);
  QString save();
  QString saveCpp();
  QString saveJSON();
  void MouseMoving(Schematic*, int, int, int, int, Schematic*, int, int, bool);
  bool MousePressing();
  bool getSelected(float, float, float);
  void Bounding(int&, int&, int&, int&);
  bool resizeTouched(float, float, float);
  void MouseResizeMoving(int, int, Schematic*);

  void rotate();
  void mirrorX();
  void mirrorY();
  bool Dialog();

  void calcArrowHead();

  QPen   Pen;
  int    Style{0};
  double Height{0}, Width{0};  // size of the arrow head
  double Length{0}, beta{0};
  int    xp1{0}, yp1{0}, xp2{0}, yp2{0};   // coordinates to paint the arrow head
};

#endif
