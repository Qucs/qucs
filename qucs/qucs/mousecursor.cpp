/*
 * mousecursor.cpp - implement graphic items for mouse cursor
 *
 * Copyright (C) 2015, Guilherme Brondani Torri, guitorri@gmail.com
 *
 * Based on code from mouseactions.cpp, by:
 * Copyright (C) 2003, Michael Margraf, michael.margraf@alumni.tu-berlin.de
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

#include <QPainter>

#include "mousecursor.h"

MouseCursor::MouseCursor()
{
  // default
  cursorType = NoCursor;

  setFlags(ItemIgnoresTransformations);
}

QRectF MouseCursor::boundingRect() const
{
  /// \todo return size based on scene boundaries
  return QRectF(0, 0, 0, 0);
}

void MouseCursor::paint(QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget)
{
  Q_UNUSED(item);
  Q_UNUSED(widget);

  switch (cursorType) {

    /// Do nothing, normal system cursor
    case NoCursor:
      break;

    /// Paints a crossed box mouse cursor to "(de)activate" components.
    case CursorActivate:
      painter->drawRect( 0, -9, 14, 10);
      painter->drawLine( 0, -9, +13, 0);
      painter->drawLine( 0,  0, +13, -9);
      break;

    /// Paints a cross under the mouse cursor to show the delete mode.
    case CursorDelete:
      painter->drawLine(-15,   -15,  +15,  +15);
      painter->drawLine(-15,   +15,  +15,  -15);
      break;

    case CursorInsert:
      break;

    ///Paints a label above the mouse cursor for "set wire label".
    case CursorLabel:
      // paint marker
      painter->drawLine(  0,    0, +10, -10);
      painter->drawLine( +10, -10, +20, -10);
      painter->drawLine( +10, -10, +10, -17);
      // paint A
      painter->drawLine( +12, -12, +15, -23);
      painter->drawLine( +14, -17, +17, -17);
      painter->drawLine( +19, -12, +16, -23);
      break;

    ///Paints a triangle above the mouse for "set marker on graph"
    case CursorMarker:
      painter->drawLine( 0,  -2, -8, -10);
      painter->drawLine(+1,  -3, +8, -10);
      painter->drawLine(-7, -10, +7, -10);
      break;

   /// Paints rounded "mirror about x axis" mouse cursor
    case CursorMirrorX:
      painter->drawLine( -4, -11, -9, -9);
      painter->drawLine( -3, -11, -3, -6);
      painter->drawLine( -4, +11, -9, +9);
      painter->drawLine( -3, +11, -3, +6);
      painter->drawArc( -8, -10, 10, 21, 16*110, 16*140);
      break;

    /// Paints rounded "mirror about y axis" mouse cursor
    case CursorMirrorY:
      painter->drawLine( -11, -4, -9, -9);
      painter->drawLine( -11, -3, -6, -3);
      painter->drawLine( +11, -4, +9, -9);
      painter->drawLine( +11, -3, +6, -3);
      painter->drawArc( -10, -8, 21, 10, 16*20, 16*140);
      break;

    /// Paints "rotate" mouse cursor
    case CursorRotate:
      painter->drawLine( -6, +8, -6, +1);
      painter->drawLine( -7, +8, -12, +8);
      painter->drawArc( -10, -10, 21, 21, -16*20, 16*240);
      break;

    /// Paints mouse symbol for "move component text" mode.
    case CursorMoveTextB:
      painter->drawLine( +14,   0, +16,   0);
      painter->drawLine( +23,   0, +25,   0);
      painter->drawLine( +13,   0, +13, + 3);
      painter->drawLine( +13, + 7, +13, +10);
      painter->drawLine( +14, +10, +16, +10);
      painter->drawLine( +23, +10, +25, +10);
      painter->drawLine( +26,   0, +26, + 3);
      painter->drawLine( +26, + 7, +26, +10);
      break;

    /// Paints rectangle around component text being mouse moved
    case CursorMoveText:
      /// \todo painter->drawRect(MAx1, MAy1, MAx2, MAy2);
      break;

    /// Paints a grid beside the mouse cursor, put "on grid" mode.
    case CursorOnGrid:
      painter->drawLine( +10, + 3, +25, +3);
      painter->drawLine( +10, + 7, +25, +7);
      painter->drawLine( +10, +11, +25, +11);
      painter->drawLine( +13,   0, +13, +15);
      painter->drawLine( +17,   0, +17, +15);
      painter->drawLine( +21,   0, +21, +15);
      break;

    case CursorPaste:
      break;

    /// Paints symbol beside the mouse to show the "Zoom in" modus.
    case CursorZoomIn:
      painter->drawLine( +14,  0, +22,  0);
      painter->drawLine( +18, -4, +18, +4);
      painter->drawEllipse( +12, -6, 13, 13);
      break;
    }
}
