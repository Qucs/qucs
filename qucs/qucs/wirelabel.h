/***************************************************************************
                          wirelabel.h  -  description
                             -------------------
    begin                : Sun Feb 29 2004
    copyright            : (C) 2004 by Michael Margraf
    email                : margraf@mwt.ee.tu-berlin.de
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

#include <qpainter.h>
#include <qstring.h>
#include <qptrlist.h>

class Wire;
class Node;

/**
  *@author Michael Margraf
  */


class WireLabel : public Element {
public:
  WireLabel(const QString& _Name=0, int _cx=0, int _cy=0, int _x1=0, int _y1=0,
            ElementType _Type=isNodeLabel);
  ~WireLabel();

  void paintScheme(QPainter *p);
  void setCenter(int x, int y, bool relative=false);
  bool getSelected(int, int);
  void setName(const QString& Name_);

  Wire    *pWire;
  Node    *pNode;
  QString Name;

  void    paint(QPainter *p);
  void    rotate();
  QString save();
  bool    load(const QString& s);
  bool    isHorizontal();
};

#endif
