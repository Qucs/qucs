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

#include "graphictext.h"
#include "graphictextdialog.h"

#include <qwidget.h>
#include <qpainter.h>


GraphicText::GraphicText()
{
  isSelected = false;
  Color = QColor(0,0,0);
  Size = 12;
  cx = cy = 0;
  x1 = x2 = 0;
  y1 = y2 = 0;
}

GraphicText::~GraphicText()
{
}

// --------------------------------------------------------------------------
void GraphicText::paint(QPainter *p)
{
  if(isSelected) {
    p->setPen(QPen(QPen::darkGray,3));
    p->drawRect(cx-2, cy-2, x2+2, y2+2);
  }
  p->setPen(Color);
  p->setFont(QFont("Helvetica",Size, QFont::Light));
  p->drawText(cx, cy, 0, 0, Qt::AlignAuto | Qt::DontClip, Text);
}

// --------------------------------------------------------------------------
void GraphicText::paintScheme(QPainter *p)
{
  p->drawRect(cx, cy, x2, y2);
}

// --------------------------------------------------------------------------
void GraphicText::getCenter(int& x, int &y)
{
  x = cx+(x2>>1);
  y = cy+(y2>>1);
}

// --------------------------------------------------------------------------
// Sets the center of the painting to x/y.
void GraphicText::setCenter(int x, int y, bool relative)
{
  if(relative) { cx += x;  cy += y; }
  else { cx = x-(x2>>1);  cy = y-(y2>>1); }
}

// --------------------------------------------------------------------------
GraphicText* GraphicText::newOne()
{
  return new GraphicText();
}

// --------------------------------------------------------------------------
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

  n  = s.section(' ',4,4);    // color
  Color.setNamedColor(n);
  if(!Color.isValid()) return false;

  Text  = s.mid(s.find('"')+1);    // Text
  Text = Text.left(Text.length()-1);
  if(Text.isEmpty()) return false;

  Text.replace("\\n", "\n");
  Text.replace("\\\\", "\\");
  QWidget w;
  QPainter p(&w);
  p.setFont(QFont("Helvetica",Size, QFont::Light));
  QRect r = p.boundingRect(0,0,0,0,Qt::AlignAuto,Text);      // get width of text
  x2 = r.width();
  y2 = r.height();

  return true;
}

// --------------------------------------------------------------------------
QString GraphicText::save()
{
  QString t = Text;
  t.replace('\\', "\\\\");
  t.replace('\n', "\\n");
  
  // the 'Text' property has to be the last within the line !
  QString s = "   <Text "+QString::number(cx)+" "+QString::number(cy)+" ";
  s += QString::number(Size)+" "+Color.name()+" \""+t+"\">";
  return s;
}

// --------------------------------------------------------------------------
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

// --------------------------------------------------------------------------
bool GraphicText::MousePressing()
{
  GraphicTextDialog *d = new GraphicTextDialog();
  if(d->exec() != QDialog::Accepted) {
    delete d;
    return false;
  }

  Text = "";
  for(int z=0; z<=d->text->paragraphs(); z++)
    Text += d->text->text(z) + "\n";
  if(!Text.isEmpty()) Text = Text.left(Text.length()-2);  // remove last line break
  Color = d->ColorButt->paletteBackgroundColor();
  Size  = d->TextSize->text().toInt();

  if(Text.isEmpty()) {
    delete d;
    return false;
  }

  QWidget w;
  QPainter p(&w);
  p.setFont(QFont("Helvetica",Size, QFont::Light));
  QRect r = p.boundingRect(0,0,0,0,Qt::AlignAuto,Text);      // get width of text
  x2 = r.width();
  y2 = r.height();

  x1 = 0;
  y1 = 0;
  delete d;
  return true;    // painting is ready
}

// --------------------------------------------------------------------------
// Checks if the coordinates x/y point to the painting.
// 5 is the precision the user must point onto the painting.
bool GraphicText::getSelected(int x, int y)
{
  if(x >= cx) if(y >= cy) if(x <= (cx+x2)) if(y <= (cy+y2)) return true;
  return false;
}

// --------------------------------------------------------------------------
void GraphicText::Bounding(int& _x1, int& _y1, int& _x2, int& _y2)
{
  _x1 = cx; _x2 = cx+x2;
  _y1 = cy; _y2 = cy+y2;
}

// --------------------------------------------------------------------------
// Rotates around the center.
void GraphicText::rotate()
{
/*  cx += (x2>>1) - (y2>>1);
  cy += (x2>>1) + (y2>>1);

  int tmp = x2;
  x2  =  y2;
  y2  = -tmp;*/
}

// --------------------------------------------------------------------------
// Mirrors about center line.
void GraphicText::mirrorX()
{
/*  cy +=  y2;
  y2  = -y2;*/
}

// --------------------------------------------------------------------------
// Mirrors about center line.
void GraphicText::mirrorY()
{
/*  cx +=  x2;
  x2  = -x2;*/
}

// --------------------------------------------------------------------------
// Calls the property dialog for the painting and changes them accordingly.
// If there were changes, it returns 'true'.
bool GraphicText::Dialog()
{
  bool changed = false;

  GraphicTextDialog *d = new GraphicTextDialog();
  d->ColorButt->setPaletteBackgroundColor(Color);
  d->TextSize->setText(QString::number(Size));
  d->text->setText(Text);

  if(d->exec() == QDialog::Rejected) return false;

  if(Color != d->ColorButt->paletteBackgroundColor()) {
    Color = d->ColorButt->paletteBackgroundColor();
    changed = true;
  }
  if(Size  != d->TextSize->text().toInt()) {
    Size  = d->TextSize->text().toInt();
    changed = true;
  }

  QString _Text = "";
  for(int z=0; z<=d->text->paragraphs(); z++)
    _Text += d->text->text(z) + "\n";
  if(!_Text.isEmpty()) _Text = _Text.left(_Text.length()-2);  // remove last line break
  if(!_Text.isEmpty())
    if(_Text != Text) {
      Text = _Text;
      changed = true;
    }
  
  return changed;
}
