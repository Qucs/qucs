/***************************************************************************
                                arrow.cpp
                               -----------
    begin                : Sun Nov 23 2003
    copyright            : (C) 2003 by Michael Margraf
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

/*!
  \class Arrow
  \brief The Arrow class implements the arrow painting
*/

#include "arrow.h"
#include "arrowdialog.h"
#include "schematic.h"
#include "misc.h"
#include <cmath>

#include <QPolygon>
#include <QPainter>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>

Arrow::Arrow()
{
  Name = "Arrow ";
  ElemSelected = false;
  Pen = QPen(QColor());
  cx = cy = 0;
  x1 = x2 = 0;
  y1 = y2 = 0;

  Height = 20.0;
  Width  =  8.0;
  Style  = 0;   // arrow head not filled
  beta   = atan2(double(Width), double(Height));
  Length = sqrt(Width*Width + Height*Height);
}

QRectF Arrow::boundingRect() const
{
  // from Bounding
  int _x1, _y1, _x2, _y2;
  if(x2 < 0) { _x1 = cx+x2; _x2 = cx; }
  else { _x1 = cx; _x2 = cx+x2; }

  if(y2 < 0) { _y1 = cy+y2; _y2 = cy; }
  else { _y1 = cy; _y2 = cy+y2; }
  return *(new QRectF(_x1, _y1, _x2-_x1, _y2-_y1));
}


// --------------------------------------------------------------------------
void Arrow::paint(QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget)
{
  Q_UNUSED(item);
  Q_UNUSED(widget);

  // paint mouse decoration, relative to mouse cursor
  // track mouse move event
  if(drawScheme){
    painter->drawLine(ex+x1+25, ey+y1,   ex+x1+13, ey+y1+12);
    painter->drawLine(ex+x1+18, ey+y1+2, ex+x1+25, ey+y1);
    painter->drawLine(ex+x1+23, ey+y1+7, ex+x1+25, ey+y1);
  }

  // preview arrow after first mouse press
  if(State > 0) {
    painter->drawLine(cx, cy, cx+x2, cy+y2);
    painter->drawLine(cx+x2, cy+y2, cx+xp1, cy+yp1);
    painter->drawLine(cx+x2, cy+y2, cx+xp2, cy+yp2);
    return;
  }

  /// \todo cleanup Arrow::paint
  //QPolygon Points;
  //int x1_, y1_, x2_, y2_, x3_, y3_;

  /// \todo precise selection over the lines, not the boundingRect
  /// http://www.qtcentre.org/threads/25206-QGraphicsItem-QLineF-Precise-Selection

  if(isSelected()) {
    painter->setPen(QPen(Qt::darkGray, Pen.width()+5));
    painter->drawLine(cx, cy, cx+x2, cy+y2);
    painter->drawLine(cx+x2, cy+y2, cx+xp1, cy+yp1);
    painter->drawLine(cx+x2, cy+y2, cx+xp2, cy+yp2);
    if(Style == 0) {   // arrow head with two lines ?
      painter->setPen(QPen(Qt::white, Pen.width(), Pen.style()));
      painter->drawLine(cx, cy, cx+x2, cy+y2);
      painter->setPen(QPen(Qt::white, Pen.width(), Qt::SolidLine));
      painter->drawLine(cx+x2, cy+y2, cx+xp1, cy+yp1);
      painter->drawLine(cx+x2, cy+y2, cx+xp2, cy+yp2);
    }
    else {   // filled arrow head
      painter->drawLine(cx+xp1, cy+yp1, cx+xp2, cy+yp2);
      painter->setPen(QPen(Qt::white, Pen.width(), Pen.style()));
      painter->drawLine(cx, cy, cx+x2, cy+y2);

      painter->setPen(QPen(Qt::white, Pen.width(), Qt::SolidLine));
      painter->setBrush(Qt::white);
      /// \todo ViewPainter::map does scaling, displace and cast to int
      //p->map(cx+xp1, cy+yp1, x1_, y1_);
      //p->map(cx+x2, cy+y2, x2_, y2_);
      //p->map(cx+xp2, cy+yp2, x3_, y3_);
      //Points.setPoints(3, x1_, y1_, x2_, y2_, x3_, y3_);
      //painter->drawConvexPolygon(Points,3);
      painter->setBrush(Qt::NoBrush); // no filling for next paintings
    }

    //dummy box
    painter->setPen(QPen(Qt::green,2));
    painter->drawRect(boundingRect());

    painter->setPen(QPen(Qt::darkRed,2));
    painter->drawRect(cx-5,    cy-5,    10, 10);  // markers for changing the size
    painter->drawRect(cx+x2-5, cy+y2-5, 10, 10);
    return;
  }

  painter->setPen(Pen);
  painter->drawLine(cx, cy, cx+x2, cy+y2);
  painter->setPen(QPen(Pen.color(), Pen.width(), Qt::SolidLine));
  if(Style == 0) {   // arrow head with two lines ?
    painter->drawLine(cx+x2, cy+y2, cx+xp1, cy+yp1);
    painter->drawLine(cx+x2, cy+y2, cx+xp2, cy+yp2);
  }
  else {   // filled arrow head
    painter->setBrush(Pen.color());
    //p->map(cx+xp1, cy+yp1, x1_, y1_);
    //p->map(cx+x2, cy+y2, x2_, y2_);
    //p->map(cx+xp2, cy+yp2, x3_, y3_);
    //Points.setPoints(3, x1_, y1_, x2_, y2_, x3_, y3_);
    //Points.setPoints(3, cx+xp1, cx+yp1, cx+x2, cy+y2, cx+xp2, cy+yp2);
    //painter->drawConvexPolygon(Points);
    painter->setBrush(Qt::NoBrush); // no filling for next paintings
    }
}

// --------------------------------------------------------------------------
void Arrow::getCenter(int& x, int &y)
{
  x = cx+(x2>>1);
  y = cy+(y2>>1);
}

// --------------------------------------------------------------------------
// Sets the center of the painting to x/y.
void Arrow::setCenter(int x, int y, bool relative)
{
  if(relative) { cx += x;  cy += y; }
  else { cx = x-(x2>>1);  cy = y-(y2>>1); }
}

// --------------------------------------------------------------------------
Painting* Arrow::newOne()
{
  return new Arrow();
}

// --------------------------------------------------------------------------
Element* Arrow::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Arrow");
  BitmapFile = (char *) "arrow";

  if(getNewOne)  return new Arrow();
  return 0;
}

// --------------------------------------------------------------------------
bool Arrow::load(const QString& s)
{
  bool ok;

  QString n;
  n  = s.section(' ',1,1);    // cx
  cx = n.toInt(&ok);
  if(!ok) return false;

  n  = s.section(' ',2,2);    // cy
  cy = n.toInt(&ok);
  if(!ok) return false;

  n  = s.section(' ',3,3);    // x2
  x2 = n.toInt(&ok);
  if(!ok) return false;

  n  = s.section(' ',4,4);    // y2
  y2 = n.toInt(&ok);
  if(!ok) return false;

  n  = s.section(' ',5,5);    // height
  Height = n.toDouble(&ok);
  if(!ok) return false;

  n  = s.section(' ',6,6);    // width
  Width = n.toDouble(&ok);
  if(!ok) return false;

  n  = s.section(' ',7,7);    // color
  QColor co;
  co.setNamedColor(n);
  Pen.setColor(co);
  if(!Pen.color().isValid()) return false;

  n  = s.section(' ',8,8);    // thickness
  Pen.setWidth(n.toInt(&ok));
  if(!ok) return false;

  n  = s.section(' ',9,9);    // line style
  Pen.setStyle((Qt::PenStyle)n.toInt(&ok));
  if(!ok) return false;

  n  = s.section(' ',10,10);    // arrow style
  if(!n.isEmpty()) {            // backward compatible
    Style = n.toInt(&ok);
    if(!ok) return false;
  }

  beta   = atan2(double(Width), double(Height));
  Length = sqrt(Width*Width + Height*Height);
  calcArrowHead();
  return true;
}

// --------------------------------------------------------------------------
QString Arrow::save()
{
  QString s = Name+QString::number(cx)+" "+QString::number(cy)+" ";
  s += QString::number(x2)+" "+QString::number(y2)+" ";
  s += QString::number(int(Height))+" "+QString::number(int(Width))+" ";
  s += Pen.color().name()+" "+QString::number(Pen.width())+" ";
  s += QString::number(Pen.style()) + " " + QString::number(Style);
  return s;
}

// --------------------------------------------------------------------------
QString Arrow::saveCpp()
{
  // arrow not allowed in symbols, thus we use line here
  QString s =
    QString ("new Line (%1, %2, %3, %4, QPen (QColor (\"%5\"), %6, %7))").
    arg(cx+x1).arg(cy+y1).arg(cx+x2).arg(cy+y2).
    arg(Pen.color().name()).arg(Pen.width()).arg(toPenString(Pen.style()));
  s = "Lines.append (" + s + ");";
  return s;
}

QString Arrow::saveJSON()
{
  // arrow not allowed in symbols, thus we use line here
  QString s =
    QString("{\"type\" : \"arrow\", "
       "\"x1\" : %1, \"y1\" : %2, \"x2\" : %3, \"y2\" : %4, "
       "\"color\" : \"%5\", \"thick\" : %6, \"style\" : \"%7\"},").
       arg(cx+x1).arg(cy+y1).arg(cx+x2).arg(cy+y2).
       arg(Pen.color().name()).arg(Pen.width()).arg(toPenString(Pen.style()));
  return s;
}

// --------------------------------------------------------------------------
// Checks if the resize area was clicked.
bool Arrow::resizeTouched(float fX, float fY, float len)
{
  float fCX = float(cx),fCY = float(cy);
  if(fX < fCX+len) if(fX > fCX-len) if(fY < fCY+len) if(fY > fCY-len) {
    State = 1;
    return true;
  }

  fCX += float(x2);
  fCY += float(y2);
  if(fX < fCX+len) if(fX > fCX-len) if(fY < fCY+len) if(fY > fCY-len) {
    State = 2;
    return true;
  }

  State = 0;
  return false;
}

// --------------------------------------------------------------------------
// Mouse move action during resize.
void Arrow::MouseResizeMoving(int x, int y, Schematic *p)
{
  if(State == 1) { x2 += cx-x; y2 += cy-y; cx = x; cy = y; } // moving shaft
  else { x2 = x-cx;  y2 = y-cy; }  // moving head

  calcArrowHead();
}

// --------------------------------------------------------------------------
void Arrow::calcArrowHead()
{
  double phi  = atan2(double(y2), double(x2));

  double w = beta+phi;
  xp1 = x2-int(Length*cos(w));
  yp1 = y2-int(Length*sin(w));

  w = phi-beta;
  xp2 = x2-int(Length*cos(w));
  yp2 = y2-int(Length*sin(w));
}

// --------------------------------------------------------------------------
// fx/fy are the precise coordinates, gx/gy are the coordinates set on grid.
// x/y are coordinates without scaling.
void Arrow::MouseMoving(
	Schematic *paintScale, int, int, int gx, int gy,
	Schematic *p, int x, int y, bool drawn)
{
  if(State > 0) { // after first press
    // update end point
    x2 = gx-cx;
    y2 = gy-cy;
  }
  else { cx = gx; cy = gy; } // update start point

  calcArrowHead();

  // track mouse move event to show scheme
  ex = x;
  ey = y;
}

// --------------------------------------------------------------------------
bool Arrow::MousePressing()
{
  State++;
  if(State == 2) {
    x1 = y1 = 0;
    State = 0;
    calcArrowHead();
    return true;    // painting is ready
  }
  return false;
}

// --------------------------------------------------------------------------
// Checks if the coordinates x/y point to the painting.
// 5 is the precision the user must point onto the painting.
bool Arrow::getSelected(float fX, float fY, float w)
{
  float A, xn, yn;
  // first check if coordinates match the arrow body
  fX -= float(cx);
  fY -= float(cy);

  if(fX < -w) {
    if(fX < float(x2)-w)  // is point between x coordinates ?
      goto Head1;
  }
  else {
    if(fX > w)
      if(fX > float(x2)+w)
        goto Head1;
  }

  if(fY < -w) {
    if(fY < float(y2)-w)   // is point between y coordinates ?
      goto Head1;
  }
  else {
    if(fY > w)
      if(fY > float(y2)+w)
        goto Head1;
  }

  A  = float(x2)*fY - fX*float(y2); // calculate the rectangle area spanned
  A *= A;               // avoid the need for square root

  if(A <= w*w*float(x2*x2 + y2*y2))
    return true;     // x/y lies on the graph line

Head1:    // check if coordinates match the first arrow head line
  xn = float(xp1-x2);  fX -= float(x2);
  yn = float(yp1-y2);  fY -= float(y2);

  if(fX < -w) {
    if(fX < xn-w)    // is point between x coordinates ?
      goto Head2;
  }
  else {
    if(fX > w)
      if(fX > xn+w)
        goto Head2;
  }

  if(fY < -w) {
    if(fY < yn-w)    // is point between y coordinates ?
      goto Head2;
  }
  else
    if(fY > w)
      if(fY > yn+w)
        goto Head2;

  A  = xn*fY - fX*yn;   // calculate the rectangle area spanned
  A *= A;               // avoid the need for square root

  if(A <= w*w*(xn*xn + yn*yn))
    return true;     // x/y lies on the arrow head

Head2:    // check if coordinates match the second arrow head line
  xn = float(xp2-x2);
  yn = float(yp2-y2);

  if(fX < -w) {
    if(fX < xn-w)   // is point between x coordinates ?
      return false;
  }
  else
    if(fX > w)
      if(fX > xn+w)
        return false;

  if(fY < -w) {
    if(fY < yn-w)   // is point between y coordinates ?
      return false;
  }
  else
    if(fY > w)
      if(fY > yn+w)
        return false;

  A  = xn*fY - fX*yn;   // calculate the rectangle area spanned
  A *= A;               // avoid the need for square root

  if(A <= w*w*(xn*xn + yn*yn))
    return true;     // x/y lies on the arrow head

  return false;
}

// --------------------------------------------------------------------------
void Arrow::Bounding(int& _x1, int& _y1, int& _x2, int& _y2)
{
  if(x2 < 0) { _x1 = cx+x2; _x2 = cx; }
  else { _x1 = cx; _x2 = cx+x2; }

  if(y2 < 0) { _y1 = cy+y2; _y2 = cy; }
  else { _y1 = cy; _y2 = cy+y2; }
}

// --------------------------------------------------------------------------
// Rotates around the center.
void Arrow::rotate()
{
  cx += (x2>>1) - (y2>>1);
  cy += (x2>>1) + (y2>>1);

  int tmp = x2;
  x2  =  y2;
  y2  = -tmp;

  tmp =  xp1;
  xp1 =  yp1;
  yp1 = -tmp;

  tmp =  xp2;
  xp2 =  yp2;
  yp2 = -tmp;
}

// --------------------------------------------------------------------------
// Mirrors about center line.
void Arrow::mirrorX()
{
  yp1 = -yp1;
  yp2 = -yp2;
  cy +=  y2;   // change cy after the other changes !
  y2  = -y2;
}

// --------------------------------------------------------------------------
// Mirrors about center line.
void Arrow::mirrorY()
{
  xp1 = -xp1;
  xp2 = -xp2;
  cx +=  x2;   // change cx after the other changes !
  x2  = -x2;
}

// --------------------------------------------------------------------------
// Calls the property dialog for the painting and changes them accordingly.
// If there were changes, it returns 'true'.
bool Arrow::Dialog()
{
  bool changed = false;

  ArrowDialog *d = new ArrowDialog();
  d->HeadWidth->setText(QString::number(Width));
  d->HeadLength->setText(QString::number(Height));
  misc::setPickerColor(d->ColorButt, Pen.color());
  d->LineWidth->setText(QString::number(Pen.width()));
  d->SetComboBox(Pen.style());
  d->ArrowStyleBox->setCurrentIndex(Style);

  if(d->exec() == QDialog::Rejected) {
    delete d;
    return false;
  }

  if(Width != d->HeadWidth->text().toDouble()) {
    Width = d->HeadWidth->text().toDouble();
    changed = true;
  }
  if(Height != d->HeadLength->text().toDouble()) {
    Height = d->HeadLength->text().toDouble();
    changed = true;
  }
  /// \todo deduplicate
  QColor penColor = misc::getWidgetBackgroundColor(d->ColorButt);
  if(Pen.color() != penColor) {
    Pen.setColor(penColor);
    changed = true;
  }
  if(Pen.width() != d->LineWidth->text().toInt()) {
    Pen.setWidth(d->LineWidth->text().toInt());
    changed = true;
  }
  if(Pen.style() != d->LineStyle) {
    Pen.setStyle(d->LineStyle);
    changed = true;
  }
  if(Style != d->ArrowStyleBox->currentIndex()) {
    Style = d->ArrowStyleBox->currentIndex();
    changed = true;
  }

  beta   = atan2(double(Width), double(Height));
  Length = sqrt(Width*Width + Height*Height);
  calcArrowHead();

  delete d;
  return changed;
}
