/***************************************************************************
                        id_text.cpp  -  description
                             -------------------
    begin                : Thu Oct 14 2004
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

#include "id_text.h"

#include "id_dialog.h"


ID_Text::ID_Text(int cx_, int cy_)
{
  Name = ".ID ";
  isSelected = false;
  cx = cx_;
  cy = cy_;
  x2 = y2 = 20;

  Prefix = "SUB";
}

ID_Text::~ID_Text()
{
}

// --------------------------------------------------------------------------
void ID_Text::paint(ViewPainter *p)
{
  int Width1, Width2, Height;
  p->Painter->setPen(QPen(QPen::black,1));
  Width1 = p->drawText(Prefix, cx, cy, &Height);
  y2 = Height;
  Width2 = p->drawText("File=name", cx, cy+Height, &Height);
  y2 += Height;
  if(Width1 > Width2)  x2 = Width1;
  else  x2 = Width2;

  if(isSelected) {
    p->Painter->setPen(QPen(QPen::darkGray,3));
    p->drawRoundRect(cx-4, cy-4, x2+8, y2+8);
  }
}

// --------------------------------------------------------------------------
void ID_Text::paintScheme(QPainter *p)
{
  p->drawRect(cx, cy, x2, y2);
}

// --------------------------------------------------------------------------
void ID_Text::getCenter(int& x, int &y)
{
  x = cx+(x2>>1);
  y = cy+(y2>>1);
}

// --------------------------------------------------------------------------
// Sets the center of the painting to x/y.
void ID_Text::setCenter(int x, int y, bool relative)
{
  if(relative) { cx += x;  cy += y; }
  else { cx = x-(x2>>1);  cy = y-(y2>>1); }
}

// --------------------------------------------------------------------------
bool ID_Text::load(const QString& s)
{
  bool ok;

  QString n;
  n  = s.section(' ',1,1);    // cx
  cx = n.toInt(&ok);
  if(!ok) return false;

  n  = s.section(' ',2,2);    // cy
  cy = n.toInt(&ok);
  if(!ok) return false;

  Prefix = s.section(' ',3,3);    // Prefix
  if(Prefix.isEmpty()) return false;

  return true;
}

// --------------------------------------------------------------------------
QString ID_Text::save()
{
  QString s = Name+QString::number(cx)+" "+QString::number(cy)+" ";
  s += Prefix;
  return s;
}

// --------------------------------------------------------------------------
// Checks if the coordinates x/y point to the painting.
bool ID_Text::getSelected(int x, int y)
{
  if(x < cx)  return false;
  if(y < cy)  return false;
  if(x > cx+x2)  return false;
  if(y > cy+y2)  return false;

  return true;
}

// --------------------------------------------------------------------------
void ID_Text::Bounding(int& _x1, int& _y1, int& _x2, int& _y2)
{
  _x1 = cx;     _y1 = cy;
  _x2 = cx+x2;  _y2 = cy+y2;
}

// --------------------------------------------------------------------------
// Rotates around the center.
void ID_Text::rotate()
{
}

// --------------------------------------------------------------------------
// Mirrors about center line.
void ID_Text::mirrorX()
{
}

// --------------------------------------------------------------------------
// Mirrors about center line.
void ID_Text::mirrorY()
{
}

// --------------------------------------------------------------------------
// Calls the property dialog for the painting and changes them accordingly.
// If there were changes, it returns 'true'.
bool ID_Text::Dialog()
{
  bool changed = false;

  ID_Dialog *d = new ID_Dialog();
  d->Prefix->setText(Prefix);

  if(d->exec() == QDialog::Rejected) {
    delete d;
    return false;
  }

  if(Prefix  != d->Prefix->text()) {
    Prefix = d->Prefix->text();
    changed = true;
  }

  delete d;
  return changed;
}
