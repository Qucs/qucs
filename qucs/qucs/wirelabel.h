/***************************************************************************
                                wirelabel.h
                               -------------
    begin                : Sun Feb 29 2004
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

#ifndef WIRELABEL_H
#define WIRELABEL_H

#include "element.h"

#include <QString>

class QPainter;
class Wire;
class Node;


class WireLabel : public Element {
public:
  WireLabel(const QString& _Name=0, int _cx=0, int _cy=0,
            int _x1=0, int _y1=0, int _Type=isNodeLabel);
  virtual ~WireLabel() {}

  virtual QRectF boundingRect() const;

  void paintScheme(QPainter *p);
  void setCenter(int x, int y, bool relative=false);
  bool getSelected(int, int);
  void setName(const QString& Name_);
  void setHighlighted (bool newval) { isHighlighted = newval; }


  Conductor *pOwner;  // Wire or Node where label belongs to
  QString Name, initValue;

  void paint(QPainter* painter, const QStyleOptionGraphicsItem* item, QWidget* widget);
  void    rotate();
  QString save();
  bool    load(const QString& s);
  bool    isHorizontal();
  void    getLabelBounding(int& _xmin, int& _ymin, int& _xmax, int& _ymax);

private:
  bool isHighlighted;

};

#endif
