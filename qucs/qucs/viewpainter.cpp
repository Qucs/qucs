/***************************************************************************
                        viewpainter.cpp  -  description
                             -------------------
    begin                : Tue Oct 05 2004
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

#include "viewpainter.h"

#include <math.h>

ViewPainter::ViewPainter(QPainter *p)
{
  Painter = p;
  DX = DY = Scale = 0.0;
}

ViewPainter::~ViewPainter()
{
}

// -------------------------------------------------------------
void ViewPainter::init(QPainter *p, float Scale_, int DX_, int DY_, int dx_, int dy_)
{
  Painter = p;
  Scale = Scale_;
  DX = floor(float(DX_) * Scale) - float(dx_);
  DY = floor(float(DY_) * Scale) - float(dy_);

  QFont f = p->font();
  f.setPointSizeFloat( float(Scale) * float(f.pointSize()) );
  p->setFont(f);
  LineSpacing = p->fontMetrics().lineSpacing();
  p->setWorldXForm(false);   // we use our own coordinate transformation
}

// -------------------------------------------------------------
void ViewPainter::map(int x1, int y1, int *x, int *y)
{
  float z;
  z  = float(x1)*Scale + DX;
  *x = z > 0.0 ? int(z + 0.5) : int(z - 0.5);
  z  = float(y1)*Scale + DY;
  *y = z > 0.0 ? int(z + 0.5) : int(z - 0.5);
}

// -------------------------------------------------------------
void ViewPainter::drawPoint(int x1, int y1)
{
  float z;
  z = float(x1)*Scale + DX;
  x1 = z > 0.0 ? int(z + 0.5) : int(z - 0.5);
  z = float(y1)*Scale + DY;
  y1 = z > 0.0 ? int(z + 0.5) : int(z - 0.5);

  Painter->drawPoint(x1, y1);
}

// -------------------------------------------------------------
void ViewPainter::drawLine(int x1, int y1, int x2, int y2)
{
  float z;
  z = float(x1)*Scale + DX;
  x1 = z > 0.0 ? int(z + 0.5) : int(z - 0.5);
  z = float(y1)*Scale + DY;
  y1 = z > 0.0 ? int(z + 0.5) : int(z - 0.5);
  z = float(x2)*Scale + DX;
  x2 = z > 0.0 ? int(z + 0.5) : int(z - 0.5);
  z = float(y2)*Scale + DY;
  y2 = z > 0.0 ? int(z + 0.5) : int(z - 0.5);

  Painter->drawLine(x1, y1, x2, y2);
}

// -------------------------------------------------------------
void ViewPainter::drawRect(int x1, int y1, int dx, int dy)
{
  float z;
  z = float(x1)*Scale + DX;
  x1 = z > 0.0 ? int(z + 0.5) : int(z - 0.5);
  z = float(y1)*Scale + DY;
  y1 = z > 0.0 ? int(z + 0.5) : int(z - 0.5);
  z = float(dx)*Scale;
  dx = z > 0.0 ? int(z + 0.5) : int(z - 0.5);
  z = float(dy)*Scale;
  dy = z > 0.0 ? int(z + 0.5) : int(z - 0.5);

  Painter->drawRect(x1, y1, dx, dy);
}

// -------------------------------------------------------------
void ViewPainter::drawRectD(int x1, int y1, int dx, int dy)
{
  float z;
  z = float(x1)*Scale + DX;
  x1 = z > 0.0 ? int(z + 0.5) : int(z - 0.5);
  z = float(y1)*Scale + DY;
  y1 = z > 0.0 ? int(z + 0.5) : int(z - 0.5);

  Painter->drawRect(x1, y1, dx, dy);
}

// -------------------------------------------------------------
void ViewPainter::drawRoundRect(int x1, int y1, int dx, int dy)
{
  float z;
  z = float(x1)*Scale + DX;
  x1 = z > 0.0 ? int(z + 0.5) : int(z - 0.5);
  z = float(y1)*Scale + DY;
  y1 = z > 0.0 ? int(z + 0.5) : int(z - 0.5);
  z = float(dx)*Scale;
  dx = z > 0.0 ? int(z + 0.5) : int(z - 0.5);
  z = float(dy)*Scale;
  dy = z > 0.0 ? int(z + 0.5) : int(z - 0.5);

  Painter->drawRoundRect(x1, y1, dx, dy);
}

// -------------------------------------------------------------
void ViewPainter::drawEllipse(int x1, int y1, int dx, int dy)
{
  float z;
  z = float(x1)*Scale + DX;
  x1 = z > 0.0 ? int(z + 0.5) : int(z - 0.5);
  z = float(y1)*Scale + DY;
  y1 = z > 0.0 ? int(z + 0.5) : int(z - 0.5);
  z = float(dx)*Scale;
  dx = z > 0.0 ? int(z + 0.5) : int(z - 0.5);
  z = float(dy)*Scale;
  dy = z > 0.0 ? int(z + 0.5) : int(z - 0.5);

  Painter->drawEllipse(x1, y1, dx, dy);
}

// -------------------------------------------------------------
// Returns width of text (and height if pointer is not null).
int ViewPainter::drawText(const QString& Text, int x1, int y1, int *Height)
{
  float z;
  z = float(x1)*Scale + DX;
  x1 = z > 0.0 ? int(z + 0.5) : int(z - 0.5);
  z = float(y1)*Scale + DY;
  y1 = z > 0.0 ? int(z + 0.5) : int(z - 0.5);

  QRect r;
  Painter->drawText(x1, y1, 0, 0, Qt::DontClip, Text, -1, &r);

  if(Height)  *Height = r.height();
  return r.width();
}

// -------------------------------------------------------------
// Returns width of text (and height if pointer is not null).
int ViewPainter::drawArc(int x1, int y1, int w, int h, int Angle, int ArcLen)
{
  float z;
  z = float(x1)*Scale + DX;
  x1 = z > 0.0 ? int(z + 0.5) : int(z - 0.5);
  z = float(y1)*Scale + DY;
  y1 = z > 0.0 ? int(z + 0.5) : int(z - 0.5);

  z = float(w)*Scale;
  w  = z > 0.0 ? int(z + 0.5) : int(z - 0.5);
  z = float(h)*Scale;
  h  = z > 0.0 ? int(z + 0.5) : int(z - 0.5);

  Painter->drawArc(x1, y1, w, h, Angle, ArcLen);
}

// -------------------------------------------------------------
void ViewPainter::fillRect(int x1, int y1, int dx, int dy, const QColor& Color)
{
  float z;
  z = float(x1)*Scale + DX;
  x1 = z > 0.0 ? int(z + 0.5) : int(z - 0.5);
  z = float(y1)*Scale + DY;
  y1 = z > 0.0 ? int(z + 0.5) : int(z - 0.5);

  z = float(dx)*Scale;
  dx  = z > 0.0 ? int(z + 0.5) : int(z - 0.5);
  z = float(dy)*Scale;
  dy  = z > 0.0 ? int(z + 0.5) : int(z - 0.5);

  Painter->fillRect(x1, y1, dx, dy, QBrush(Color));
}

// -------------------------------------------------------------
void ViewPainter::eraseRect(int x1, int y1, int dx, int dy)
{
  float z;
  z = float(x1)*Scale + DX;
  x1 = z > 0.0 ? int(z + 0.5) : int(z - 0.5);
  z = float(y1)*Scale + DY;
  y1 = z > 0.0 ? int(z + 0.5) : int(z - 0.5);

  Painter->eraseRect(x1, y1, dx, dy);
}

// -------------------------------------------------------------
// Draw little resize rectangles with center x1/y1 and size independent
// of zoom factor.
void ViewPainter::drawResizeRect(int x1, int y1)
{
  float z;
  z = float(x1)*Scale + DX;
  x1 = z > 0.0 ? int(z + 0.5) : int(z - 0.5);
  z = float(y1)*Scale + DY;
  y1 = z > 0.0 ? int(z + 0.5) : int(z - 0.5);

  Painter->drawRect(x1-5, y1-5, 10, 10);
}
