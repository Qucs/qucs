/*
 * mousecursor.h - implement graphic items for mouse cursor
 *
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

#ifndef MOUSECURSOR_H
#define MOUSECURSOR_H

#include <QGraphicsItem>

/*!
 * \brief The MouseCursor class
 * Implement mouse cursors for different interacion modes.
 */
class MouseCursor : public QGraphicsItem
{
public:
  MouseCursor();
  virtual ~MouseCursor() {}

  QRectF boundingRect() const;
  void paint(QPainter* painter, const QStyleOptionGraphicsItem* item, QWidget* widget);

  enum CursorTypes {
    NoCursor,
    CursorActivate,
    CursorDelete,
    CursorInsert,
    CursorLabel,
    CursorMarker,
    CursorMirrorX,
    CursorMirrorY,
    CursorRotate,
    CursorMoveText,
    CursorMoveTextB,
    CursorOnGrid,
    CursorPaste,
    CursorZoomIn
    };

  CursorTypes cursorType;
  void setCursorType(CursorTypes type) { cursorType = type; }
};

#endif // MOUSECURSOR_H
