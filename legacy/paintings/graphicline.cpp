/***************************************************************************
    copyright            : (C) 2003 by Michael Margraf
                               2018, 2020 Felix Salfelder
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
#include "qucs_globals.h"
#include "module.h"
#include "viewpainter.h"

#include <QPainter>
#include <QPushButton>
#include <QLineEdit>
#include <QComboBox>
#include "../legacy_painting.h"

#include <QPen>

namespace{


class GraphicLine : public LegacyPainting  {
private:
  GraphicLine(GraphicLine const& g) : LegacyPainting(g), Pen(g.Pen){
  }
public:
  GraphicLine();
  ~GraphicLine();
  
//  void setSomeStuff( int cx_=0, int cy_=0, int x2_=0, int y2_=0,
//              QPen Pen_=QPen(QColor()));

  void getCenter(int&, int&);
//  void setCenter(int, int, bool relative=false);

  Element* clone() const {return new GraphicLine(*this);}
  static Element* info(QString&, char* &, bool getNewOne=false);
  bool load(const QString&);
  QString save();
  QString saveCpp();
  QString saveJSON();
  void paint(ViewPainter*) const override;
  bool MousePressing();
  bool getSelected(float, float, float);
  void Bounding(int&, int&, int&, int&);
  bool resizeTouched(float, float, float);

  void rotate();
  void mirrorX();
  void mirrorY();
  bool Dialog();

  QPen   Pen;
}D;
Dispatcher<Painting>::INSTALL p(&element_dispatcher, "Line", &D);
Module::INSTALL pp("paintings", &D);

GraphicLine::GraphicLine()
{
	// setTypeName("Line");
	Name = "Line"; //??
}

#if 0
void GraphicLine::setSomeStuff(int cx_, int cy_, int x2_, int y2_, QPen Pen_)
{
  Name = "Line ";
  Pen = Pen_;

  incomplete();
//  _cx = cx_;
//  _cy = cy_;
  x1 = y1 = 0;
  x2 = x2_;
  y2 = y2_;
}
#endif

GraphicLine::~GraphicLine()
{
}

// --------------------------------------------------------------------------
void GraphicLine::paint(ViewPainter *p) const
{itested();

	// oops;
	int cx = Element::cx();
	int cy = Element::cy();

	//??
	cx=0;
	cy=0;
#if 0
  if(isSelected()) {
    p->drawLine(cx, cy, cx+x2, cy+y2);
    p->setPen(QPen(Qt::white, Pen.width(), Pen.style()));
    p->drawLine(cx, cy, cx+x2, cy+y2);

    p->setPen(QPen(Qt::darkRed,2));
    p->drawResizeRect(cx, cy);  // markers for changing the size
    p->drawResizeRect(cx+x2, cy+y2);
    return;
  }
#endif
  p->setPen(Pen);
  p->drawLine(cx, cy, cx+x2, cy+y2);
}

// --------------------------------------------------------------------------
void GraphicLine::getCenter(int& x, int &y)
{
	assert(false);
	auto cx=Element::cx();
	auto cy=Element::cy();

	x = cx+(x2>>1);
	y = cy+(y2>>1);
}

// --------------------------------------------------------------------------
// Sets the center of the painting to x/y.
//void GraphicLine::setCenter(int x, int y, bool relative)
//{
//	assert(false);
//	 auto cx=Element::cx();
//     auto cy=Element::cy();
//
//  if(relative) { cx += x;  cy += y; }
//  else { cx = x-(x2>>1);  cy = y-(y2>>1); }
//}

// --------------------------------------------------------------------------
#if 0
Element* GraphicLine::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Line");
  BitmapFile = (char *) "line";

  if(getNewOne)  return new GraphicLine();
  return 0;
}
#endif

// --------------------------------------------------------------------------
bool GraphicLine::load(const QString& s)
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

  trace1("Line::load", s);

  return true;
}

// --------------------------------------------------------------------------
QString GraphicLine::save()
{
	 auto cx=Element::cx();
     auto cy=Element::cy();

  QString s = Name+" "+QString::number(cx)+" "+QString::number(cy)+" ";
  s += QString::number(x2)+" "+QString::number(y2)+" ";
  s += Pen.color().name()+" "+QString::number(Pen.width())+" ";
  s += QString::number(Pen.style());
  return s;
}

// --------------------------------------------------------------------------
#if 0
QString GraphicLine::saveCpp()
{
	 auto cx=Element::cx();
     auto cy=Element::cy();

  QString s =
    QString ("new Line (%1, %2, %3, %4, QPen (QColor (\"%5\"), %6, %7))").
    arg(cx+x1).arg(cy+y1).arg(cx+x2).arg(cy+y2).
    arg(Pen.color().name()).arg(Pen.width()).arg(toPenString(Pen.style()));
  s = "Lines.append (" + s + ");";
  return s;
}

QString GraphicLine::saveJSON()
{
	 auto cx=Element::cx();
     auto cy=Element::cy();

  QString s =
    QString ("{\"type\" : \"line\", "
      "\"x1\" : %1, \"y1\" : %2, \"x2\" : %3, \"y2\" : %4, "
      "\"color\" : \"%5\", \"thick\" : %6, \"style\" : \"%7\"},").
      arg(cx+x1).arg(cy+y1).arg(cx+x2).arg(cy+y2).
      arg(Pen.color().name()).arg(Pen.width()).arg(toPenString(Pen.style()));
  return s;
}
#endif

// --------------------------------------------------------------------------
// Checks if the resize area was clicked.
bool GraphicLine::resizeTouched(float fX, float fY, float len)
{
	   auto cx=Element::cx();
     auto cy=Element::cy();

  float fCX = float(cx), fCY = float(cy);

  if(fX <= fCX+len) if(fX >= fCX-len) if(fY <= fCY+len) if(fY >= fCY-len) {
    State = 1;
    return true;
  }

  fCX += float(x2);
  fCY += float(y2);
  if(fX <= fCX+len) if(fX >= fCX-len) if(fY <= fCY+len) if(fY >= fCY-len) {
    State = 2;
    return true;
  }

  State = 0;
  return false;
}

// --------------------------------------------------------------------------
// Mouse move action during resize.
#if 0
void GraphicLine::MouseResizeMoving(int, int, SchematicDoc *)
{

  paintScheme(p);  // erase old painting
  if(State == 1) { x2 += cx-x; y2 += cy-y; cx = x; cy = y; } // move beginning
  else { x2 = x-cx;  y2 = y-cy; }  // move ending

  paintScheme(p);  // paint new painting
}
#endif

// --------------------------------------------------------------------------
// fx/fy are the precise coordinates, gx/gy are the coordinates set on grid.
// x/y are coordinates without scaling.
#if 0
void GraphicLine::MouseMoving(
	SchematicDoc *pAppaintScale, int, int, int gx, int gy,
	SchematicDoc *p, int x, int y, bool drawn)
{
  if(State > 0) {
    if(State > 1)
      paintScale->PostPaintEvent(_Line, cx, cy, cx+x2, cy+y2);  // erase old painting
    State++;
    x2 = gx-cx;
    y2 = gy-cy;
    paintScale->PostPaintEvent(_Line, cx, cy, cx+x2, cy+y2);  // paint new painting
  }
  else { cx = gx; cy = gy; }


  // FIXME #warning p->setPen(Qt::SolidLine);
  if(drawn) {
    p->PostPaintEvent(_Line, x1+27, y1, x1+15, y1+12,0,0,true);  // erase old cursor symbol
    p->PostPaintEvent(_Line, x1+25, y1-2, x1+29, y1+2,0,0,true);
    p->PostPaintEvent(_Line, x1+13, y1+10, x1+17, y1+14,0,0,true);
  }
  x1 = x;
  y1 = y;
  p->PostPaintEvent(_Line, x1+27, y1, x1+15, y1+12,0,0,true);  // paint new cursor symbol
  p->PostPaintEvent(_Line, x1+25, y1-2, x1+29, y1+2,0,0,true);
  p->PostPaintEvent(_Line, x1+13, y1+10, x1+17, y1+14,0,0,true);
}
#endif

// --------------------------------------------------------------------------
bool GraphicLine::MousePressing()
{
  State++;
  if(State > 2) {
    x1 = y1 = 0;
    State = 0;
    return true;    // painting is ready
  }
  return false;
}

// --------------------------------------------------------------------------
// Checks if the coordinates x/y point to the painting.
// 5 is the precision the user must point onto the painting.
bool GraphicLine::getSelected(float fX, float fY, float w)
{
	   auto cx=Element::cx();
     auto cy=Element::cy();

  fX -= float(cx);
  fY -= float(cy);

  if(fX < -w) {
    if(fX < float(x2)-w)  // is point between x coordinates ?
      return false;
  }
  else {
    if(fX > w)
      if(fX > float(x2)+w)
        return false;
  }

  if(fY < -w) {
    if(fY < float(y2)-w)   // is point between y coordinates ?
      return false;
  }
  else {
    if(fY > w)
      if(fY > float(y2)+w)
        return false;
  }

  float A = float(x2)*fY - fX*float(y2); // calculate the rectangle area spanned
  A *= A;               // avoid the need for square root

  if(A <= w*w*float(x2*x2 + y2*y2))
    return true;     // x/y lies on the graph line

  return false;
}

// --------------------------------------------------------------------------
void GraphicLine::Bounding(int& _x1, int& _y1, int& _x2, int& _y2)
{
	   auto cx=0;
     auto cy=0;

  if(x2 < 0) { _x1 = cx+x2; _x2 = cx; }
  else { _x1 = cx; _x2 = cx+x2; }

  if(y2 < 0) { _y1 = cy+y2; _y2 = cy; }
  else { _y1 = cy; _y2 = cy+y2; }
}

// --------------------------------------------------------------------------
// Rotates around the center.
void GraphicLine::rotate()
{
  //_cx += (x2>>1) - (y2>>1);
  //_cy += (x2>>1) + (y2>>1);

  int tmp = x2;
  x2  =  y2;
  y2  = -tmp;
}

// --------------------------------------------------------------------------
// Mirrors about center line.
void GraphicLine::mirrorX()
{
	// obsolete.
//  cy +=  y2;
//  y2  = -y2;
}

// --------------------------------------------------------------------------
// Mirrors about center line.
void GraphicLine::mirrorY()
{
	// obsolete
  //cx +=  x2;
  //x2  = -x2;
}

// --------------------------------------------------------------------------
// Calls the property dialog for the painting and changes them accordingly.
// If there were changes, it returns 'true'.
bool GraphicLine::Dialog()
{
  bool changed = false;

  FillDialog *d = new FillDialog(QObject::tr("Edit Line Properties"), false);
  misc::setPickerColor(d->ColorButt, Pen.color());
  d->LineWidth->setText(QString::number(Pen.width()));
  d->StyleBox->setCurrentIndex(Pen.style()-1);

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
  if(Pen.style()  != (d->StyleBox->currentIndex()+1)) {
    Pen.setStyle((Qt::PenStyle)(d->StyleBox->currentIndex()+1));
    changed = true;
  }

  delete d;
  return changed;
}

} // namespace
