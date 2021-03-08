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
//#include "schematic_doc.h" // BUG
#include "viewpainter.h"
#include "misc.h"
#include "qucs_globals.h"
#include "module.h"
#include "../legacy_painting.h"

#include <QPainter>
#include <QPushButton>
#include <QLineEdit>
#include <QComboBox>
#include <QCheckBox>

namespace{

class Ellipse : public LegacyPainting  {
public:
  Ellipse(bool _filled=false);
 ~Ellipse();

  void getCenter(int&, int&);
  void setCenter(int, int, bool relative=false);

  Element* clone() const {return new Ellipse(*this);}
  static Element* info(QString&, char* &, bool getNewOne=false);
  static Element* info_filled(QString&, char* &, bool getNewOne=false);
  bool load(const QString&);
  QString save();
  QString saveCpp();
  QString saveJSON();
  void paint(ViewPainter*);
//  void MouseMoving(SchematicDoc*, int, int, int, int, SchematicDoc*, int, int, bool);
//  bool MousePressing();
  bool getSelected(float, float, float);
  bool resizeTouched(float, float, float);

  void rotate();
  void mirrorX();
  void mirrorY();
  bool Dialog();

  QPen  Pen;
  QBrush Brush;    // filling style/color
  bool  filled;    // filled or not (for "getSelected" etc.)
}D;
Dispatcher<Painting>::INSTALL p(&qucs::element_dispatcher, "Ellipse", &D);
Module::INSTALL pp("paintings", &D);

Ellipse::Ellipse(bool _filled) : LegacyPainting()
{
  Name = "Ellipse ";
  Pen = QPen(QColor());
  Brush = QBrush(Qt::lightGray);
  filled = _filled;
  x1 = x2 = 0;
  y1 = y2 = 0;
}

Ellipse::~Ellipse()
{
}

// --------------------------------------------------------------------------
void Ellipse::paint(ViewPainter *p)
{ untested();
#if 0
  if(isSelected()) { untested();
    p->setPen(QPen(Qt::darkGray,Pen.width()+5));
    if(filled)  p->setBrush(Brush);
    p->drawEllipse(cx, cy, x2, y2);
    p->setPen(QPen(Qt::white, Pen.width(), Pen.style()));
    p->setBrush(Qt::NoBrush);
    p->drawEllipse(cx, cy, x2, y2);

    p->setPen(QPen(Qt::darkRed,2));
    p->drawResizeRect(cx, cy+y2);  // markers for changing the size
    p->drawResizeRect(cx, cy);
    p->drawResizeRect(cx+x2, cy+y2);
    p->drawResizeRect(cx+x2, cy);
    return;
  }
#endif
   auto cx=0;
	auto cy=0;

  p->setPen(Pen);
  if(filled)  p->setBrush(Brush);
  p->drawEllipse(cx, cy, x2, y2);
  p->setBrush(Qt::NoBrush); // no filling for the next paintings
}

// --------------------------------------------------------------------------
void Ellipse::getCenter(int& x, int &y)
{ untested();
	 auto cx=Element::cx();
     auto cy=Element::cy();

  x = cx+(x2>>1);
  y = cy+(y2>>1);
}

// --------------------------------------------------------------------------
// Sets the center of the painting to x/y.
void Ellipse::setCenter(int x, int y, bool relative)
{
	 auto cx=Element::cx();
     auto cy=Element::cy();

  if(relative) { cx += x;  cy += y; }
  else { cx = x-(x2>>1);  cy = y-(y2>>1); }
}

// --------------------------------------------------------------------------
#if 0
Element* Ellipse::info(QString& Name, char* &BitmapFile, bool getNewOne)
{ untested();
  Name = QObject::tr("Ellipse");
  BitmapFile = (char *) "ellipse";

  if(getNewOne)  return new Ellipse();
  return 0;
}

// --------------------------------------------------------------------------
Element* Ellipse::info_filled(QString& Name, char* &BitmapFile, bool getNewOne)
{ untested();
  Name = QObject::tr("filled Ellipse");
  BitmapFile = (char *) "filledellipse";

  if(getNewOne)  return new Ellipse(true);
  return 0;
}
#endif

// --------------------------------------------------------------------------
bool Ellipse::load(const QString& s)
{
  bool ok;

  QString n;
  n  = s.section(' ',1,1);    // cx
  int cx = n.toInt(&ok);
  if(!ok) return false;

  n  = s.section(' ',2,2);    // cy
  int cy = n.toInt(&ok);
  if(!ok) return false;

  setCenter(cx, cy);

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
QString Ellipse::save()
{
	 auto cx=Element::cx();
     auto cy=Element::cy();

  QString s = Name +
	QString::number(cx) + " " + QString::number(cy) + " " +
	QString::number(x2) + " " + QString::number(y2) + " " +
	Pen.color().name()  + " " + QString::number(Pen.width()) + " " +
	QString::number(Pen.style()) + " " +
	Brush.color().name() + " " + QString::number(Brush.style());
  if(filled) s += " 1";
  else s += " 0";
  return s;
}

// --------------------------------------------------------------------------
#if 0
QString Ellipse::saveCpp()
{ untested();
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
  s = "Ellips.append (" + s + ");";
  return s;
}

QString Ellipse::saveJSON()
{ untested();
	 auto cx=Element::cx();
     auto cy=Element::cy();

  QString b = filled ?
    QString ("\"colorfill\" : \"%1\", \"stylefill\" : \"%2\"").
    arg(Brush.color().name()).arg(toBrushString(Brush.style())) : "";
  QString s =
    QString("{\"type\" : \"ellipse\", "
    "\"x\" : %1, \"y\" : %2, \"w\" : %3, \"h\" : %4,"
    "\"color\" : \"%5\", \"thick\" : %6, \"style\" : \"%7\", %8},").
    arg(cx).arg(cy).arg(x2).arg(y2).
    arg(Pen.color().name()).arg(Pen.width()).arg(toPenString(Pen.style())).
    arg(b);
  return s;
}
#endif

// --------------------------------------------------------------------------
// Checks if the resize area was clicked.
bool Ellipse::resizeTouched(float fX, float fY, float len)
{ untested();
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
  if(fX < fCX+len) State = 1;
  else if(fX <= fX2-len) { State = -1; return false; }
  if(fY < fCY+len)  State |= 2;
  else if(fY <= fY2-len) { State = -1; return false; }

  return true;
}

// --------------------------------------------------------------------------
// Mouse move action during resize.
#if 0
void Ellipse::MouseResizeMoving(int, int, SchematicDoc *)
{ untested();
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

}
#endif

// --------------------------------------------------------------------------
// fx/fy are the precise coordinates, gx/gy are the coordinates set on grid.
// x/y are coordinates without scaling.
#if 0
void Ellipse::MouseMoving(
	SchematicDoc *paintScale, int, int, int gx, int gy,
	SchematicDoc *p, int x, int y, bool drawn)
{ untested();
  if(State > 0) { untested();
    if(State > 1)
      // _Ellipse hang/crash application, using _Arc solved, see bug 141 (closed)
      paintScale->PostPaintEvent(_Arc, x1, y1, x2-x1, y2-y1, 0, 16*360); // erase old painting
    State++;
    x2 = gx;
    y2 = gy;
    paintScale->PostPaintEvent(_Arc, x1, y1, x2-x1, y2-y1, 0, 16*360);
  }
  else { x2 = gx; y2 = gy; }

  if(drawn) { untested();
    p->PostPaintEvent(_Ellipse, cx+13, cy, 18, 12,0,0,true);  // erase old cursor symbol
    if(filled) { untested();
      p->PostPaintEvent(_Line, cx+14, cy+7, cx+20, cy+1,0,0,true);
      p->PostPaintEvent(_Line, cx+25, cy+2, cx+18, cy+9,0,0,true);
      p->PostPaintEvent(_Line, cx+29, cy+4, cx+23, cy+10,0,0,true);
    }
  }
  cx = x;
  cy = y;
  p->PostPaintEvent(_Ellipse, cx+13, cy, 18, 12,0,0,true);  // paint new cursor symbol
  if(filled) { untested();
    p->PostPaintEvent(_Line, cx+14, cy+7, cx+20, cy+1,0,0,true);
    p->PostPaintEvent(_Line, cx+25, cy+2, cx+18, cy+9,0,0,true);
    p->PostPaintEvent(_Line, cx+29, cy+4, cx+23, cy+10,0,0,true);
  }
}
#endif

// --------------------------------------------------------------------------
#if 0
bool Ellipse::MousePressing()
{ untested();
  State++;
  if(State == 1) { untested();
    x1 = x2;
    y1 = y2;    // first corner is determined
  }
  else { untested();
    if(x1 < x2) { cx = x1; x2 = x2-x1; } // cx/cy to upper left corner
    else { cx = x2; x2 = x1-x2; }
    if(y1 < y2) { cy = y1; y2 = y2-y1; }
    else { cy = y2; y2 = y1-y2; }
    x1 = y1 = 0;
    State = 0;
    return true;    // painting is ready
  }
  return false;
}
#endif

// --------------------------------------------------------------------------
// Checks if the coordinates x/y point to the painting.
bool Ellipse::getSelected(float fX, float fY, float w)
{ untested();
	 auto cx=Element::cx();
     auto cy=Element::cy();

  float fX2 = float(x2);
  float fY2 = float(y2);
  fX -= float(cx) + fX2/2.0;
  fY -= float(cy) + fY2/2.0;

  if(filled) { untested();
    float a = 2.0 * fX / fX2;  a *= a;
    float b = 2.0 * fY / fY2;  b *= b;

    if(a+b > 1.0)
      return false;
  }
  else { untested();
    float a1 = fX / (fX2/2.0 - w);  a1 *= a1;
    float a2 = fX / (fX2/2.0 + w);  a2 *= a2;
    float b1 = fY / (fY2/2.0 - w);  b1 *= b1;
    float b2 = fY / (fY2/2.0 + w);  b2 *= b2;

    if(a1+b1 < 1.0)  return false;
    if(a2+b2 > 1.0)  return false;
  }

  return true;
}

// --------------------------------------------------------------------------
// Rotates around the center.
void Ellipse::rotate()
{ untested();
	//??
//  _cy += (y2-x2) >> 1;
//  _cx += (x2-y2) >> 1;

  std::swap(x2, y2);
}

// --------------------------------------------------------------------------
// Mirrors about center line.
void Ellipse::mirrorX()
{ untested();
  // nothing to do
}

// --------------------------------------------------------------------------
// Mirrors about center line.
void Ellipse::mirrorY()
{ untested();
  // nothing to do
}

// --------------------------------------------------------------------------
// Calls the property dialog for the painting and changes them accordingly.
// If there were changes, it returns 'true'.
bool Ellipse::Dialog()
{ untested();
  bool changed = false;

  FillDialog *d = new FillDialog(QObject::tr("Edit Ellipse Properties"));
  misc::setPickerColor(d->ColorButt, Pen.color());
  d->LineWidth->setText(QString::number(Pen.width()));
  d->StyleBox->setCurrentIndex(Pen.style()-1);
  misc::setPickerColor(d->FillColorButt, Brush.color());
  d->FillStyleBox->setCurrentIndex(Brush.style());
  d->CheckFilled->setChecked(filled);
  d->slotCheckFilled(filled);

  if(d->exec() == QDialog::Rejected) { untested();
    delete d;
    return false;
  }

  /// \todo deduplicate
  QColor penColor = misc::getWidgetBackgroundColor(d->ColorButt);
  if(Pen.color() != penColor) { untested();
    Pen.setColor(penColor);
    changed = true;
  }
  if(Pen.width()  != d->LineWidth->text().toInt()) { untested();
    Pen.setWidth(d->LineWidth->text().toInt());
    changed = true;
  }
  if(Pen.style()  != (Qt::PenStyle)(d->StyleBox->currentIndex()+1)) { untested();
    Pen.setStyle((Qt::PenStyle)(d->StyleBox->currentIndex()+1));
    changed = true;
  }
  if(filled != d->CheckFilled->isChecked()) { untested();
    filled = d->CheckFilled->isChecked();
    changed = true;
  }
  QColor brushColor = misc::getWidgetBackgroundColor(d->FillColorButt);
  if(Brush.color() != brushColor) { untested();
    Brush.setColor(brushColor);
    changed = true;
  }
  if(Brush.style()  != d->FillStyleBox->currentIndex()) { untested();
    Brush.setStyle((Qt::BrushStyle)d->FillStyleBox->currentIndex());
    changed = true;
  }

  delete d;
  return changed;
}

} // namespace
