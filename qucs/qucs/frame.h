/*
 * frame.h - Implement the schematic frame
 *
 * Copyright (C) 2006, Michael Margraf
 * Copyright (C) 2008, Stefan Jahn
 * Copyright (C) 2017, Guilherme Brondani Torri, guitorri@gmail.com
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

#ifndef FRAME_H
#define FRAME_H

#include <QGraphicsItem>

/*!
 * \brief The Frame class
 *
 * QGraphicsItem to represent the frame around the schematic.
 */
class Frame : public QGraphicsItem
{
public:
  Frame();
  virtual ~Frame() {}

  QRectF boundingRect() const;
  void paint(QPainter* painter, const QStyleOptionGraphicsItem* item, QWidget* widget);

  bool sizeOfFrame(int& xall, int& yall);

  QString Title;    //Frame_0
  QString Author;   //Frame_1
  QString Date;     //Frame_2
  QString Revision; //Frame_3

  int PageType; // 0 is no-frame, 1 to 8 are available types of frames

  int Height;
  int Width;

};

#endif // FRAME_H
