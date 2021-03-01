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
#include "qucs_app.h"
#include "qucs_globals.h"
#include "module.h"
#include "../legacy_painting.h"
#include "viewpainter.h"
#include "some_font_stuff.h"

#include <QPainter>

namespace{

using namespace qucs;

class PortSymbol : public LegacyPainting  {
public:
  PortSymbol();
  ~PortSymbol();
  Element* clone() const{
	  return new PortSymbol(*this);
  }
  void setSomeArgsHack(int cx_, int cy_, const QString& numberStr_,  const QString& nameStr_);

  void getCenter(int&, int&);
//  void setCenter(int, int, bool relative=false);

  bool load(const QString&);
  QString save();
  void paint(ViewPainter*);
  void Bounding(int&, int&, int&, int&);

  void rotate();
  void mirrorX();
  void mirrorY();

  int Angel;
  QString numberStr, nameStr;
};

PortSymbol D;
Dispatcher<Element>::INSTALL p(&element_dispatcher, "PortSym", &D);
Module::INSTALL pp("paintings", &D);

PortSymbol::PortSymbol() : LegacyPainting()
{
	// setTypeName(".Portsym");
  Name = ".PortSym ";
}

void PortSymbol::setSomeArgsHack(int cx_, int cy_, const QString& numberStr_,
                                         const QString& nameStr_)
{
	unreachable();
  Name = ".PortSym ";
  setCenter(pos_t(cx_, cy_));

  Angel = 0;
  nameStr = nameStr_;
  numberStr = numberStr_;
  // get size of text using the screen-compatible metric
  FontMetrics metrics;
  QSize r = metrics.size(0, nameStr);
  x1 = -r.width() - 8;
  y1 = -((r.height() + 8) >> 1);
  x2 = 8 - x1;
  y2 = r.height() + 8;
}
/*--------------------------------------------------------------------------*/
PortSymbol::~PortSymbol()
{
}
/*--------------------------------------------------------------------------*/
void PortSymbol::paint(ViewPainter *p)
{
	auto cx=0;
	auto cy=0;

  // keep track of painter state
  p->save();

  p->setPen(QPen(Qt::red,1));  // like open node
  p->drawEllipse(cx-4, cy-4, 8, 8);

  QSize r = QSize(5, 10); // p->fontMetrics().size(0, nameStr);
  int Unit = int(8.0 * p->Scale);
  x1 = -r.width() - Unit;
  y1 = -((r.height() + Unit) >> 1);
  x2 = Unit - x1;
  y2 = r.height() + Unit;

  auto wm = p->worldMatrix();
#if 0
  QMatrix Mat(1.0, 0.0, 0.0, 1.0, p->DX + float(cx) * p->Scale,
				   p->DY + float(cy) * p->Scale);
  p->setWorldMatrix(Mat);

  int tmp, tx, ty;
  tx = x1 + (Unit >> 1);
  ty = y1 + (Unit >> 1);
  switch(Angel) {
    case 90:
      x1 = y1;
      y1 = -Unit;
      tmp = x2;  x2 = y2;  y2 = tmp;
      p->rotate(-90.0); // automatically enables transformation
      break;
    case 180:
      x1 = -Unit;
      tx = Unit >> 1;
      break;
    case 270:
      tx = Unit >> 1;
      tmp = x1;  x1 = y1;  y1 = tmp;
      tmp = x2;  x2 = y2;  y2 = tmp;
      p->rotate(-90.0); // automatically enables transformation
      break;
  }

  p->setPen(Qt::black);
  p->drawText(tx, ty, 0, 0, Qt::TextDontClip, nameStr);


#endif
  p->setWorldMatrix(wm);
  p->setWorldMatrixEnabled(false);

  // restore painter state
  p->restore();

  x1 = int(float(x1) / p->Scale);
  x2 = int(float(x2) / p->Scale);
  y1 = int(float(y1) / p->Scale);
  y2 = int(float(y2) / p->Scale);

  p->setPen(Qt::lightGray);
  p->drawRect(cx+x1, cy+y1, x2, y2);

#if 0
  if(isSelected()) {
    p->setPen(QPen(Qt::darkGray,3));
    p->drawRoundRect(cx+x1-4, cy+y1-4, x2+8, y2+8);
  }
#endif
}

// --------------------------------------------------------------------------
void PortSymbol::getCenter(int& x, int &y)
{
  x = cx();
  y = cy();
}

// --------------------------------------------------------------------------

// --------------------------------------------------------------------------
bool PortSymbol::load(const QString& s)
{
  bool ok;

  QString n;
  n  = s.section(' ',1,1);    // cx
  int cx = n.toInt(&ok);
  if(!ok) return false;

  n  = s.section(' ',2,2);    // cy
  int cy = n.toInt(&ok);
  if(!ok) return false;

  setCenter(pos_t(cx, cy));

  numberStr  = s.section(' ',3,3);    // number
  if(numberStr.isEmpty()) return false;

  n  = s.section(' ',4,4);      // Angel
  if(n.isEmpty()) return true;  // be backward-compatible
  Angel = n.toInt(&ok);
  if(!ok) return false;

  return true;
}

// --------------------------------------------------------------------------
QString PortSymbol::save()
{
	auto cx=Element::cx();
	auto cy=Element::cy();

  QString s = Name+QString::number(cx)+" "+QString::number(cy)+" ";
  s += numberStr+" "+QString::number(Angel);
  return s;
}

// --------------------------------------------------------------------------
#if 0
QString PortSymbol::saveCpp()
{
	 auto cx=Element::cx();
     auto cy=Element::cy();

  QString s =
    QString ("new Port (%1, %2)").
    arg(cx).arg(cy);
  s = "Ports.append (" + s + "); /* " + nameStr + " */";
  return s;
}
QString PortSymbol::saveJSON()
{
	 auto cx=Element::cx();
     auto cy=Element::cy();

  QString s = QString ("{\"type\" : \"portsymbol\", "
                       "\"x\" : %1, \"y\" : %2},").arg(cx).arg(cy);
  return s;
}
#endif

// --------------------------------------------------------------------------
void PortSymbol::Bounding(int& _x1, int& _y1, int& _x2, int& _y2)
{
	auto cx=0;
	auto cy=0;

  _x1 = cx+x1;     _y1 = cy+y1;
  _x2 = cx+x1+x2;  _y2 = cy+y1+y2;
}

// --------------------------------------------------------------------------
// Rotates around the center.
void PortSymbol::rotate()
{
  if(Angel < 270)  Angel += 90;
  else  Angel = 0;
}

// --------------------------------------------------------------------------
// Mirrors about connection node (not center line !).
void PortSymbol::mirrorX()
{
  if(Angel == 90)  Angel = 270;
  else  if(Angel == 270)  Angel = 90;
}

// --------------------------------------------------------------------------
// Mirrors about connection node (not center line !).
void PortSymbol::mirrorY()
{
  if(Angel == 0)  Angel = 180;
  else  if(Angel == 180)  Angel = 0;
}
}
