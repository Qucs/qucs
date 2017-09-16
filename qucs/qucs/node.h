/*
 * node.h - Implementation of an electrical node
 *
 * Copyright (C) 2003, Michael Margraf, michael.margraf@alumni.tu-berlin.de
 * Copyright (C) 2015, Guilherme Brondani Torri, guitorri@gmail.com
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

#ifndef NODE_H
#define NODE_H

#include "element.h"

#include <Q3PtrList>

class Node : public Conductor {
public:
  Node(int, int);
 ~Node();

  QRectF boundingRect() const;
  void paint(QPainter* painter, const QStyleOptionGraphicsItem* item, QWidget* widget);

  bool  getSelected(int, int);
  void  setName(const QString&, const QString&, int x_=0, int y_=0);

  Q3PtrList<Element> Connections;
  QString Name;  // node name used by creation of netlist
  QString DType; // type of node (used by digital files)
  int State;	 // remember some things during some operations
};

#endif
