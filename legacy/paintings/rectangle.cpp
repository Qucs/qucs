/***************************************************************************
    copyright            : (C) 2003 by Michael Margraf
                               2020 Felix Salfelder
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include "filldialog.h"
#include "misc.h"
#include "schematic_doc.h"
#include "painting.h"
#include "qucs_globals.h"
#include "module.h"

#include <QPainter>
#include <QPushButton>
#include <QLineEdit>
#include <QComboBox>
#include <QCheckBox>

namespace{
class Rectangle : public Painting  {
public:
  Rectangle(bool _filled=false);
 ~Rectangle();

private:
  Rectangle(Rectangle const& p) : Painting(p) {}
  Element* clone() const { return new Rectangle(*this); }

  void paintScheme(SchematicDoc*) const;
  void getCenter(int&, int&);
  void setCenter(int, int, bool relative=false);

  static Element* info(QString&, char* &, bool getNewOne=false);
  static Element* info_filled(QString&, char* &, bool getNewOne=false);
  bool load(const QString&);
  QString save();
  QString saveCpp();
  QString saveJSON();
  void paint(ViewPainter*) const override;
  rect_t bounding_rect() const override;
//  void MouseMoving(SchematicDoc*, int, int, int, int, SchematicDoc*, int, int, bool);
  bool MousePressing();
  bool getSelected(float, float, float);
  bool resizeTouched(float, float, float);
  void MouseResizeMoving(int, int, SchematicDoc*);

  void rotate();
  void mirrorX();
  void mirrorY();
  bool Dialog();

  QPen  Pen;
  QBrush Brush;    // filling style/color
  bool  filled;    // filled or not (for "getSelected" etc.)
}d0;
Dispatcher<Painting>::INSTALL p(&painting_dispatcher, "Rectangle", &d0);
Module::INSTALL pp("paintings", &d0);

#if 1
Rectangle::Rectangle(bool _filled) : Painting()
{
  Name = "Rectangle ";
  Pen = QPen(QColor());
  Brush = QBrush(Qt::lightGray);
  filled = _filled;
  assert(cx() == 0);
  assert(cy() == 0);
  x1 = x2 = 0;
  y1 = y2 = 0;
}
#endif

Rectangle::~Rectangle()
{
}

// --------------------------------------------------------------------------
rect_t Rectangle::bounding_rect() const
{
	trace2("Rectangle::bounding_rect", x2, y2);
	return rect_t(0,0,x2,y2);
}

void Rectangle::paint(ViewPainter *p) const
{
	auto cx=0;
	auto cy=0;

#if 0
  if(isSelected()) {
    p->setPen(QPen(Qt::darkGray,Pen.width()+5));
    if(filled)  p->setBrush(Brush);
    p->drawRect(cx, cy, x2, y2);
    p->setPen(QPen(Qt::white, Pen.width(), Pen.style()));
    p->setBrush(Qt::NoBrush);
    p->drawRect(cx, cy, x2, y2);

    p->setPen(QPen(Qt::darkRed,2));
    p->drawResizeRect(cx, cy+y2);  // markers for changing the size
    p->drawResizeRect(cx, cy);
    p->drawResizeRect(cx+x2, cy+y2);
    p->drawResizeRect(cx+x2, cy);
    return;
  }
#endif

  // assert(false);?
  incomplete(); // probably
  p->setPen(Pen);
  if(filled)  p->setBrush(Brush);
  p->drawRect(cx, cy, x2, y2);
  p->setBrush(Qt::NoBrush); // no filling for the next paintings
}

// --------------------------------------------------------------------------
void Rectangle::getCenter(int& x, int &y)
{
	 auto cx=Element::cx();
     auto cy=Element::cy();

  x = cx+(x2>>1);
  y = cy+(y2>>1);
}

// --------------------------------------------------------------------------
// Sets the center of the painting to x/y.
void Rectangle::setCenter(int x, int y, bool relative)
{
	 auto cx=Element::cx();
     auto cy=Element::cy();

  if(relative) { cx += x;  cy += y; }
  else { cx = x-(x2>>1);  cy = y-(y2>>1); }
}

// --------------------------------------------------------------------------
#if 0
Element* Rectangle::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Rectangle");
  BitmapFile = (char *) "rectangle";

  if(getNewOne)  return new Rectangle();
  return 0;
}

// --------------------------------------------------------------------------
Element* Rectangle::info_filled(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("filled Rectangle");
  BitmapFile = (char *) "filledrect";

  if(getNewOne)  return new Rectangle(true);
  return 0;
}
#endif

// --------------------------------------------------------------------------
bool Rectangle::load(const QString& s)
{
  bool ok;

  QString n;
  n  = s.section(' ',1,1);    // cx
  int cx = n.toInt(&ok);
  if(!ok) return false;

  n  = s.section(' ',2,2);    // cy
  int cy = n.toInt(&ok);
  if(!ok) return false;

  setPosition(pos_t(cx, cy));

  n  = s.section(' ',3,3);    // x2
  x2 = n.toInt(&ok);
  if(!ok) return false;

  n  = s.section(' ',4,4);    // y2
  y2 = n.toInt(&ok);
  if(!ok) return false;

  n  = s.section(' ',5,5);    // color
  QColor co;
  co.setNamedColor(n);
  Pen.setColor(co);
  if(!Pen.color().isValid()) return false;

  n  = s.section(' ',6,6);    // thickness
  Pen.setWidth(n.toInt(&ok));
  if(!ok) return false;

  n  = s.section(' ',7,7);    // line style
  Pen.setStyle((Qt::PenStyle)n.toInt(&ok));
  if(!ok) return false;

  n  = s.section(' ',8,8);    // fill color
  co.setNamedColor(n);
  Brush.setColor(co);
  if(!Brush.color().isValid()) return false;

  n  = s.section(' ',9,9);    // fill style
  Brush.setStyle((Qt::BrushStyle)n.toInt(&ok));
  if(!ok) return false;

  n  = s.section(' ',10,10);    // filled
  if(n.toInt(&ok) == 0) filled = false;
  else filled = true;
  if(!ok) return false;

  return true;
}

// --------------------------------------------------------------------------
QString Rectangle::save()
{
	 auto cx=Element::cx();
     auto cy=Element::cy();

  QString s = Name +
	QString::number(cx) + " " + QString::number(cy) + " " +
	QString::number(x2) + " " + QString::number(y2) + " " +
	Pen.color().name() + " " + QString::number(Pen.width()) + " " +
	QString::number(Pen.style()) + " " +
	Brush.color().name() + " " + QString::number(Brush.style());
  if(filled) s += " 1";
  else s += " 0";
  return s;
}

// --------------------------------------------------------------------------
#if 0
QString Rectangle::saveCpp()
{
	 auto cx=Element::cx();
     auto cy=Element::cy();

  QString b = filled ?
    QString (", QBrush (QColor (\"%1\"), %2)").
    arg(Brush.color().name()).arg(toBrushString(Brush.style())) : "";
  QString s =
    QString ("new Area (%1, %2, %3, %4, "
	     "QPen (QColor (\"%5\"), %6, %7)%8)").
    arg(cx).arg(cy).arg(x2).arg(y2).
    arg(Pen.color().name()).arg(Pen.width()).arg(toPenString(Pen.style())).
    arg(b);
  s = "Rects.append (" + s + ");";
  return s;
}

QString Rectangle::saveJSON()
{
	 auto cx=Element::cx();
     auto cy=Element::cy();

  QString b = filled ?
    QString ("\"colorfill\" : \"%1\", \"stylefill\" : \"%2\"").
    arg(Brush.color().name()).arg(toBrushString(Brush.style())) : "";

  QString s =
    QString("{\"type\" : \"rectangle\", "
      "\"x\" : %1, \"y\" : %2, \"w\" : %3, \"h\" : %4, "
      "\"color\" : \"%5\", \"thick\" : %6, \"style\" : \"%7\", %8},").
      arg(cx).arg(cy).arg(x2).arg(y2).
      arg(Pen.color().name()).arg(Pen.width()).arg(toPenString(Pen.style())).
      arg(b);
  return s;
}
#endif

// --------------------------------------------------------------------------
bool Rectangle::resizeTouched(float fX, float fY, float len)
{
	 auto cx=Element::cx();
     auto cy=Element::cy();

  float fCX = float(cx), fCY = float(cy);
  float fX2 = float(cx+x2), fY2 = float(cy+y2);

  State = -1;
  if(fX < fCX-len) return false;
  if(fY < fCY-len) return false;
  if(fX > fX2+len) return false;
  if(fY > fY2+len) return false;

  State = 0;
  if(fX < fCX+len)  State = 1;
  else if(fX < fX2-len) { State = -1; return false; }
  if(fY < fCY+len)  State |= 2;
  else if(fY < fY2-len) { State = -1; return false; }

  return true;
}

// --------------------------------------------------------------------------
// Mouse move action during resize.
void Rectangle::MouseResizeMoving(int , int , SchematicDoc *)
{
incomplete();
#if 0
  paintScheme(p);  // erase old painting
  switch(State) {
    case 0: x2 = x-cx; y2 = y-cy; // lower right corner
	    break;
    case 1: x2 -= x-cx; cx = x; y2 = y-cy; // lower left corner
	    break;
    case 2: x2 = x-cx; y2 -= y-cy; cy = y; // upper right corner
	    break;
    case 3: x2 -= x-cx; cx = x; y2 -= y-cy; cy = y; // upper left corner
	    break;
  }
  if(x2 < 0) { State ^= 1; x2 *= -1; cx -= x2; }
  if(y2 < 0) { State ^= 2; y2 *= -1; cy -= y2; }

  paintScheme(p);  // paint new painting
#endif
}

// --------------------------------------------------------------------------
// fx/fy are the precise coordinates, gx/gy are the coordinates set on grid.
// x/y are coordinates without scaling.
#if 0
void Rectangle::MouseMoving(
	SchematicDoc *paintScale, int, int, int gx, int gy,
	SchematicDoc *p, int x, int y, bool drawn)
{
	incomplete();
  if(State > 0) {
    if(State > 1)
      paintScale->PostPaintEvent(_Rect,x1, y1, x2-x1, y2-y1);  // erase old painting
    State++;
    x2 = gx;
    y2 = gy;
    paintScale->PostPaintEvent(_Rect,x1, y1, x2-x1, y2-y1);  // paint new rectangle
  }
  else { x2 = gx; y2 = gy; }


  // FIXME #warning p->setPen(Qt::SolidLine);
  if(drawn) {
    p->PostPaintEvent(_Rect, cx+13, cy, 18, 12,0,0,true);  // erase old cursor symbol
    if(filled) {   // hatched ?
      p->PostPaintEvent(_Line, cx+14, cy+6, cx+19, cy+1,0,0,true);
      p->PostPaintEvent(_Line, cx+26, cy+1, cx+17, cy+10,0,0,true);
      p->PostPaintEvent(_Line, cx+29, cy+5, cx+24, cy+10,0,0,true);
    }
  }
  cx = x;
  cy = y;
  p->PostPaintEvent(_Rect,cx+13, cy, 18, 12,0,0,true);  // paint new cursor symbol
  if(filled) {   // hatched ?
    p->PostPaintEvent(_Line, cx+14, cy+6, cx+19, cy+1,0,0,true);
    p->PostPaintEvent(_Line, cx+26, cy+1, cx+17, cy+10,0,0,true);
    p->PostPaintEvent(_Line, cx+29, cy+5, cx+24, cy+10,0,0,true);
  }
}
#endif

// --------------------------------------------------------------------------
bool Rectangle::MousePressing()
{
#if 0
	 auto cx=Element::cx();
     auto cy=Element::cy();

  State++;
  if(State == 1) {
    x1 = x2;
    y1 = y2;    // first corner is determined
  }
  else {
    if(x1 < x2) { cx = x1; x2 = x2-x1; } // cx/cy to upper left corner
    else { cx = x2; x2 = x1-x2; }
    if(y1 < y2) { cy = y1; y2 = y2-y1; }
    else { cy = y2; y2 = y1-y2; }
    x1 = y1 = 0;
    State = 0;
    return true;    // rectangle is ready
  }
#endif
  return false;
}

// --------------------------------------------------------------------------
// Checks if the coordinates x/y point to the painting.
bool Rectangle::getSelected(float fX, float fY, float w)
{
	 auto cx=Element::cx();
     auto cy=Element::cy();

  if(filled) {
    if(int(fX) > cx+x2) return false;   // coordinates outside the rectangle ?
    if(int(fY) > cy+y2) return false;
    if(int(fX) < cx) return false;
    if(int(fY) < cy) return false;
  }
  else {
    fX -= float(cx);
    fY -= float(cy);
    float fX2 = float(x2);
    float fY2 = float(y2);

    if(fX > fX2+w) return false;   // coordinates outside the rectangle ?
    if(fY > fY2+w) return false;
    if(fX < -w) return false;
    if(fY < -w) return false;

    // coordinates inside the rectangle ?
    if(fX < fX2-w) if(fX > w) if(fY < fY2-w) if(fY > w)
      return false;
  }

  return true;
}

// --------------------------------------------------------------------------
// Rotates around the center.
void Rectangle::rotate()
{
//  _cy += (y2-x2) >> 1;
 // _cx += (x2-y2) >> 1;

  std::swap(x2, y2);
}

// --------------------------------------------------------------------------
// Mirrors about center line.
void Rectangle::mirrorX()
{
  // nothing to do
}

// --------------------------------------------------------------------------
// Mirrors about center line.
void Rectangle::mirrorY()
{
  // nothing to do
}

// --------------------------------------------------------------------------
// Calls the property dialog for the painting and changes them accordingly.
// If there were changes, it returns 'true'.
bool Rectangle::Dialog()
{
  bool changed = false;

  FillDialog *d = new FillDialog(QObject::tr("Edit Rectangle Properties"));
  misc::setPickerColor(d->ColorButt, Pen.color());
  d->LineWidth->setText(QString::number(Pen.width()));
  d->StyleBox->setCurrentIndex(Pen.style()-1);
  misc::setPickerColor(d->FillColorButt, Brush.color());
  d->FillStyleBox->setCurrentIndex(Brush.style());
  d->CheckFilled->setChecked(filled);
  d->slotCheckFilled(filled);

  if(d->exec() == QDialog::Rejected) {
    delete d;
    return false;
  }

  /// \todo deduplicate
  QColor penColor = misc::getWidgetBackgroundColor(d->ColorButt);
  if(Pen.color() != penColor) {
    Pen.setColor(penColor);
    changed = true;
  }
  if(Pen.width()  != d->LineWidth->text().toInt()) {
    Pen.setWidth(d->LineWidth->text().toInt());
    changed = true;
  }
  if(Pen.style()  != (Qt::PenStyle)(d->StyleBox->currentIndex()+1)) {
    Pen.setStyle((Qt::PenStyle)(d->StyleBox->currentIndex()+1));
    changed = true;
  }
  if(filled != d->CheckFilled->isChecked()) {
    filled = d->CheckFilled->isChecked();
    changed = true;
  }
  QColor brushColor = misc::getWidgetBackgroundColor(d->FillColorButt);
  if(Brush.color() != brushColor) {
    Brush.setColor(brushColor);
    changed = true;
  }
  if(Brush.style() != d->FillStyleBox->currentIndex()) {
    Brush.setStyle((Qt::BrushStyle)d->FillStyleBox->currentIndex());
    changed = true;
  }

  delete d;
  return changed;
}
}
