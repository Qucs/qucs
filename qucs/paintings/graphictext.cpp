/***************************************************************************
                          graphictext.cpp  -  description
                             -------------------
    begin                : Mon Nov 24 2003
    copyright            : (C) 2003 by Michael Margraf
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

#if HAVE_CONFIG_H
# include <config.h>
#endif

#include "graphictext.h"
#include "graphictextdialog.h"

#include <qwidget.h>
#include <qpainter.h>

#include <math.h>


GraphicText::GraphicText()
{
  isSelected = false;
  Color = QColor(0,0,0);
  Size = 12;
  cx = cy = 0;
  cx_d = cy_d = 0.0;
  x1 = x2 = 0;
  y1 = y2 = 0;
  Angle = 0;
}

GraphicText::~GraphicText()
{
}

// -----------------------------------------------------------------------
void GraphicText::paint(QPainter *p)
{
  p->save();
  p->rotate(-Angle);

  int x = int(cx_d),  y = int(cy_d);
  if(isSelected) {
    p->setPen(QPen(QPen::darkGray,3));
    p->drawRect(x-2, y-2, x2+4, y2+4);
  }
  p->setPen(Color);
  p->setFont(QFont("Helvetica",Size, QFont::Light));
  p->drawText(x, y, 0, 0, Qt::AlignAuto | Qt::DontClip, Text);

  p->restore();
}

// -----------------------------------------------------------------------
void GraphicText::paintScheme(QPainter *p)
{
  p->save();
  p->rotate(-Angle);

  p->drawRect(int(cx_d), int(cy_d), x2, y2);

  p->restore();
}

// ------------------------------------------------------------------------
void GraphicText::getCenter(int& x, int &y)
{
  x = cx+(x2>>1);
  y = cy+(y2>>1);
}

// -----------------------------------------------------------------------
// Sets the center of the painting to x/y.
void GraphicText::setCenter(int x, int y, bool relative)
{
  if(relative) {  cx += x;  cy += y;  }
  else {  cx = x;  cy = y;  }
    double phi = M_PIl/180.0*double(Angle);
    double sine = sin(phi), cosine = cos(phi);
    cx_d = double(cx)*cosine - double(cy)*sine;
    cy_d = double(cy)*cosine + double(cx)*sine;
}

// -----------------------------------------------------------------------
GraphicText* GraphicText::newOne()
{
  return new GraphicText();
}

// -----------------------------------------------------------------------
bool GraphicText::load(const QString& _s)
{
  bool ok;
  QString s = _s;

  if(s.at(0) != '<') return false;
  if(s.at(s.length()-1) != '>') return false;
  s = s.mid(1, s.length()-2);   // cut off start and end character

  QString n;
  n  = s.section(' ',1,1);    // cx
  cx = n.toInt(&ok);
  if(!ok) return false;

  n  = s.section(' ',2,2);    // cy
  cy = n.toInt(&ok);
  if(!ok) return false;

  n  = s.section(' ',3,3);    // Size
  Size = n.toInt(&ok);
  if(!ok) return false;

  n  = s.section(' ',4,4);    // Color
  Color.setNamedColor(n);
  if(!Color.isValid()) return false;

  n  = s.section(' ',5,5);    // Angle
  if(n.at(0) != '"') {        // backward compatible
    Angle = n.toInt(&ok);
    if(!ok) return false;
  }
  else Angle = 0;

  setCenter(cx, cy);

  Text  = s.mid(s.find('"')+1);    // Text
  Text = Text.left(Text.length()-1);
  if(Text.isEmpty()) return false;

  Text.replace("\\n", "\n");
  Text.replace("\\\\", "\\");
  QWidget w;
  QPainter p(&w);
  p.setFont(QFont("Helvetica",Size, QFont::Light));
  QRect r = p.boundingRect(0,0,0,0,Qt::AlignAuto,Text);  // get width of text
  x2 = r.width();
  y2 = r.height();

  return true;
}

// -----------------------------------------------------------------------
QString GraphicText::save()
{
  QString t = Text;
  t.replace('\\', "\\\\");
  t.replace('\n', "\\n");

  // the 'Text' property has to be the last within the line !
  QString s = "   <Text "+QString::number(cx)+" "+QString::number(cy)+" ";
  s += QString::number(Size)+" "+Color.name()+" "+QString::number(Angle);
  s += " \""+t+"\">";
  return s;
}

// -----------------------------------------------------------------------
// x/y are the precise coordinates.
void GraphicText::MouseMoving(int x, int y, int gx, int gy, QPainter *p, bool drawn)
{
  p->setPen(Qt::SolidLine);
  if(drawn) {
    p->drawLine(x1+15, y1+15, x1+20, y1);  // erase old cursor symbol
    p->drawLine(x1+26, y1+15, x1+21, y1);
    p->drawLine(x1+17, y1+8,  x1+23, y1+8);
  }
  x1 = x;
  y1 = y;
  p->drawLine(x1+15, y1+15, x1+20, y1);  // paint new cursor symbol
  p->drawLine(x1+26, y1+15, x1+21, y1);
  p->drawLine(x1+17, y1+8,  x1+23, y1+8);

  cx = gx;
  cy = gy;
}

// ------------------------------------------------------------------------
bool GraphicText::MousePressing()
{
  return Dialog();
}

// ------------------------------------------------------------------------
// Checks if the coordinates x/y point to the painting.
// 5 is the precision the user must point onto the painting.
bool GraphicText::getSelected(int x, int y)
{
  double phi = M_PIl/180.0*double(Angle);
  double sine = sin(phi), cosine = cos(phi);
  int _x = int( double(x-cx)*cosine - double(y-cy)*sine );
  int _y = int( double(y-cy)*cosine + double(x-cx)*sine );

  if(_x >= 0) if(_y >= 0) if(_x <= x2) if(_y <= y2) return true;
  return false;
}

// ------------------------------------------------------------------------
void GraphicText::Bounding(int& xmin, int& ymin, int& xmax, int& ymax)
{
  double phi = M_PIl/180.0*double(Angle);
  double sine = sin(phi), cosine = cos(phi);
  int dx = int( double(y2) * sine );
  int dy = int( double(y2) * cosine );
  xmin = dx;  xmax = cx;
  ymin = dy;  ymax = cy;
  if(xmin < 0)  xmin += cx;
  else { xmax += xmin;  xmin = cx; }
  if(ymin < 0)  ymin += cy;
  else { ymax += ymin;  ymin = cy; }

  int x = cx + int( double(x2) * cosine );
  if(xmax < x)  xmax = x;
  else if(xmin > x)  xmin = x;
  x += dx;
  if(xmax < x)  xmax = x;
  else if(xmin > x)  xmin = x;

  int y = cy - int( double(x2) * sine );
  if(ymax < y)  ymax = y;
  else if(ymin > y)  ymin = y;
  y += dy;
  if(ymax < y)  ymax = y;
  else if(ymin > y)  ymin = y;
}

// -----------------------------------------------------------------------
// Rotates around the center.
void GraphicText::rotate()
{
  Angle += 90;
  Angle %= 360;
  setCenter(cx, cy);
}

// -----------------------------------------------------------------------
// Mirrors about center line.
void GraphicText::mirrorX()
{   // do not mirror, because unreadable
}

// -----------------------------------------------------------------------
// Mirrors about center line.
void GraphicText::mirrorY()
{   // do not mirror, because unreadable
}

// -----------------------------------------------------------------------
// Calls the property dialog for the painting and changes them accordingly.
// If there were changes, it returns 'true'.
bool GraphicText::Dialog()
{
  bool changed = false;

  GraphicTextDialog *d = new GraphicTextDialog();
  d->ColorButt->setPaletteBackgroundColor(Color);
  d->TextSize->setText(QString::number(Size));
  d->Angle->setText(QString::number(Angle));
  d->text->setText(Text);

  if(d->exec() == QDialog::Rejected) {
    delete d;
    return false;
  }

  if(Color != d->ColorButt->paletteBackgroundColor()) {
    Color = d->ColorButt->paletteBackgroundColor();
    changed = true;
  }
  if(Size  != d->TextSize->text().toInt()) {
    Size  = d->TextSize->text().toInt();
    changed = true;
  }
  int tmp = d->Angle->text().toInt();
  if(Angle  != tmp) {
    Angle = tmp % 360;
    changed = true;
  }
  setCenter(cx, cy);

  QString _Text = "";
  _Text = d->text->text();
  if(!_Text.isEmpty())
    if(_Text != Text) {
      Text = _Text;
      changed = true;
    }

  QWidget w;
  QPainter p(&w);
  p.setFont(QFont("Helvetica",Size, QFont::Light));
  QRect r = p.boundingRect(0,0,0,0,Qt::AlignAuto,Text); // get width of text
  x2 = r.width();
  y2 = r.height();

  delete d;
  return changed;
}
