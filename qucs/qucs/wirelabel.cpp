/***************************************************************************
                          wirelabel.cpp  -  description
                             -------------------
    begin                : Sun February 29 2004
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
#include "wirelabel.h"
#include "wire.h"
#include "qucs.h"

#include <QString>
#include <QPainter>

WireLabel::WireLabel(const QString& _Name, int _cx, int _cy,
                     int _x1, int _y1, int _Type)
{
  cx = _cx;
  cy = _cy;
  x1 = _x1;
  y1 = _y1;
  setName(_Name);
  initValue = "";

  ElemType = _Type;
  ElemSelected = false;
  isHighlighted = false;

  setFlags(ItemIsSelectable|ItemIsMovable);
#if QT_VERSION < 0x050000
  setAcceptsHoverEvents(true);
#else
  setAcceptHoverEvents(true);
#endif
}

QRectF WireLabel::boundingRect() const
{
  return *(new QRectF(x1,y1,x2,y2));
}

// ----------------------------------------------------------------
void WireLabel::paintScheme(QPainter *p)
{
  p->drawRect(x1, y1, x2, y2);

  // which corner of rectangle should be connected to line ?
  if(cx < x1+(x2>>1)) {
    if(cy < y1+(y2>>1))
      p->drawLine(cx, cy, x1, y1);
    else
      p->drawLine(cx, cy, x1, y1+y2);
  }
  else {
    if(cy < y1+(y2>>1))
      p->drawLine(cx, cy, x1+x2, y1);
    else
      p->drawLine(cx, cy, x1+x2, y1+y2);
  }
}

// ----------------------------------------------------------------
void WireLabel::setCenter(int x_, int y_, bool relative)
{
  switch(ElemType) {
    case isMovingLabel:
      if(relative) {
        x1 += x_;  cx += x_;
        y1 += y_;  cy += y_;
      }
      else {
        x1 = x_;  cx = x_;
        y1 = y_;  cy = y_;
      }
      break;
    case isHMovingLabel:
      if(relative) { x1 += x_;  cx += x_; }
      else { x1 = x_;  cx = x_; }
      break;
    case isVMovingLabel:
      if(relative) { y1 += y_;  cy += y_; }
      else { y1 = y_;  cy = y_; }
      break;
    default:
      if(relative) {
        x1 += x_;  y1 += y_; // moving cx/cy is done by owner (wire, node)
      }
      else { x1 = x_; y1 = y_; }
  }
}

// ----------------------------------------------------------------
bool WireLabel::getSelected(int x, int y)
{
  if(x1 <= x)
    if(y1 <= y)
      if((x1+x2) >= x)
        if((y1+y2) >= y)
          return true;

  return false;
}

// ----------------------------------------------------------------
void WireLabel::paint(QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget)
{
  Q_UNUSED(item);
  Q_UNUSED(widget);

  //qDebug() << cx << cy << Name;

  /*
  /// \bug crash on focusOutEvent if drawText
  /// issue with pointers??
  */
   //painter->drawText( boundingRect(), Name);
   //painter->setPen(QPen(Qt::darkGray,3));
   //painter->drawText(cx+x1, cy+y1, Name);

  // draw component bounding box
  if(isSelected()) {
    //ElemSelected = true;
    painter->setPen(QPen(Qt::darkGray,3));
    painter->drawRoundedRect(boundingRect(), 5.0, 5.0);
  }

  QFont font = painter->font();
  if (isHighlighted)
  {
//    QColor highlightfill (Qt::blue);
//    highlightfill.setAlpha(50);
//    p->fillRect(x1-1, y1-1, x2, y2, highlightfill);
    painter->setPen(QPen(Qt::darkBlue,3));
    font.setWeight (QFont::DemiBold);
  }
  else
  {
    font.setWeight (QFont::Normal);
    painter->setPen(QPen(Qt::black,1));
  }
  painter->setFont (font);
  QRectF rf = painter->boundingRect(QRectF(x1, y1, 0, 0), Qt::TextDontClip, Name);
  painter->drawText(QRect(x1, y1, 0, 0), Qt::TextDontClip, Name);
  //text width and height
  x2 = rf.width();
  y2 = rf.height();

  // cx, cy : coord where the label attaches, be it a wire or a node.
  // x1, y1 : text label bounding box lower-left corner
  // x2, y2 : text label width and height

  // coordinate of line from arc to label frame
  int xpaint=0, ypaint=0;
  // orientation of arc if attached to a wire
  int phi=0;
  switch(ElemType) {
    case isVWireLabel:  xpaint=4, ypaint=0; phi=16*140; break;
    case isHWireLabel:  xpaint=0, ypaint=4; phi=16*50; break;
    case isNodeLabel:   ;
    default:            ;
  }

  /*
   Draw label arc, line and text frame.
   Label Placement:
   x                 x
     LABEL     LABEL

     LABEL     LABEL
   x                 x

   Arc orientation:
    - acording to wire direction
    - opening oposite to label location
  */
  int c, d;
  int a = int(double(x2) / 2);
  int b = int(double(y2) / 2);
  if(cx < x1+a) {    // where should frame be painted ?
    if(cy < y1+b) {
      if(phi == 16*50)  phi += 16*180;
      // label low right from arc
      a = x1-3;
      b = y1-2;
      c = a + (x2>>1);
      d = b + y2;
      xpaint = cx+xpaint;
      ypaint = cy+ypaint;
    }
    else {
      if(phi != 0)  phi += 16*180;
      // label up right from arc
      a = x1-3;
      b = y1+2;
      b += y2;
      c  = a + (x2>>1);
      d  = b - y2;
      xpaint = cx+xpaint;
      ypaint = cy-ypaint;
    }
  }
  else {
    if(cy < y1+b) {
      // label low left from arc
      a = x1+3;
      b = y1-2;
      a += x2;
      c  = a - (x2>>1);
      d  = b + y2;
      xpaint = cx-xpaint;
      ypaint = cy+ypaint;
    }
    else {
      if(phi > 16*90)  phi += 16*180;
      // label up left from arc
      a = x1+3;
      b = y1+2;
      a += x2;
      b += y2;
      c  = a - (x2>>1);
      d  = b - y2;
      xpaint = cx-xpaint;
      ypaint = cy-ypaint;
    }
  }

  // color label red if has initial value
  if(initValue.isEmpty())
    painter->setPen(QPen(Qt::darkMagenta,0));
  else
    painter->setPen(QPen(Qt::red,0));

  // draw open arc around wire
  if(phi)
    painter->drawArc(cx-4, cy-4, 8, 8, phi, 16*255);

  // label frame
  painter->drawLine(a, b, c, b); //horizontal
  painter->drawLine(a, b, a, d); //vertical

  // line from Wire or Node to WireLabel label
  painter->drawLine(xpaint, ypaint, a, b);

  if(ElemSelected)
  {
    painter->setPen(QPen(Qt::darkGray,3));
    painter->drawRoundRect(x1-2, y1-2, x2+6, y2+5);
  }
}

// ----------------------------------------------------------------
void WireLabel::setName(const QString& Name_)
{
  Name = Name_;
  
  // get size of text using the screen-compatible metric
  QFontMetrics metrics(QucsSettings.font, 0);
  QSize r = metrics.size(0, Name);
  x2 = r.width();
  y2 = r.height()-2;    // remember size of text
}

// ----------------------------------------------------------------
// Converts all necessary data of the wire into a string. This can be used to
// save it to an ASCII file or to transport it via the clipboard.
// Wire labels use the same format like wires, but with length zero.
QString WireLabel::save()
{
  QString s("<");
	s += QString::number(cx)+" "+QString::number(cy)+" "
	  +  QString::number(cx)+" "+QString::number(cy)
	  +  " \""+Name +"\" "
	  +  QString::number(x1)+" "+QString::number(y1)+" 0 \""
	  +  initValue+"\">";
  return s;
}


void WireLabel::getLabelBounding(int& _xmin, int& _ymin, int& _xmax, int& _ymax)
{
    _xmin = std::min(x1,x1+(x2+6));
    _xmax = std::max(x1,x1+(x2+6));
    _ymin = std::min(y1,y1+(y2+6));
    _ymax = std::max(y1,y1+(y2+5));
    _ymax = std::max(cy,_ymax);
}
