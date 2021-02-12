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

//  The LegacyTaskElement class constitutes a baseclass for legacy TaskElements
//  this one is a copy of the legacy Component class, the former
//  baseclass for LegacyTaskElements. As such, it contains unneeded stuff.
//
//  !! Inherit from the TaskElement class in new code !!

#include <stdlib.h>
#include <cmath>

#include "node.h"
#include "qucs_app.h"
#include "schematic_doc.h"
#include "viewpainter.h"
#include "module.h"
#include "misc.h"
#include "property.h"
#include "legacy_task_element.h"

#include <QPen>
#include <QString>
#include <QMessageBox>
#include <QPainter>
#include <QDebug>

#include "../legacy/obsolete_paintings.h" // BUG

LegacyTaskElement::LegacyTaskElement(LegacyTaskElement const& p)
  : TaskElement(p)
{
  qDebug() << "component copy";

  assert(!Props.count());
  for(auto i : p.Props){
    Props.append(new Property(*i));
  }
  for(auto i : p.Texts){
    Texts.append(new Text(*i));
  }
  isActive = COMP_IS_ACTIVE; // bug.
}

LegacyTaskElement::LegacyTaskElement() : TaskElement()
{
  isActive = COMP_IS_ACTIVE;
  showName = true;

  tx = 0;
  ty = 0;

  // Props.setAutoDelete(true);
}
/*--------------------------------------------------------------------------*/
index_t LegacyTaskElement::param_count() const
{
	return Props.size();
}
/*--------------------------------------------------------------------------*/
std::string LegacyTaskElement::param_name(index_t n) const
{
	assert(n < param_count());
	assert(Props.at(n));
	return Props.at(n)->Name.toStdString();
}
/*--------------------------------------------------------------------------*/
std::string LegacyTaskElement::param_value(index_t n) const
{
	assert(n < param_count());
	assert(Props.at(n));
	return Props.at(n)->Value.toStdString();
}
/*--------------------------------------------------------------------------*/
// Size of component text.
int LegacyTaskElement::textSize(int& _dx, int& _dy)
{
#if 1 // obsolete
  // get size of text using the screen-compatible metric
  QFontMetrics metrics(QString_(QucsSettings.font), 0);

  int tmp, count=0;
  _dx = _dy = 0;
  if(showName) {
    _dx = metrics.horizontalAdvance(QString_(label()));
    _dy = metrics.height();
    count++;
  }else{
  }
  for(Property *pp = Props.first(); pp != 0; pp = Props.next()){
    if(pp->display) {
      // get width of text
      tmp = metrics.horizontalAdvance(pp->Name+"="+pp->Value);
      if(tmp > _dx) {
	_dx = tmp;
      }else{
      }
      _dy += metrics.height();
      count++;
    }
  }
  return count;
#endif
  return 0;
}
/*--------------------------------------------------------------------------*/
Widget* LegacyTaskElement::schematicWidget(QucsDoc* Doc) const
{ untested();
  trace0("Component::editElement");
  incomplete();
  return nullptr;

  // return new TaskElementDialog(Doc); // memory leak?
}
/*--------------------------------------------------------------------------*/

// -------------------------------------------------------

// -------------------------------------------------------
// obsolete?
//void TaskElement::setCenter(int x, int y, bool relative)
//{
//  if(relative) {
//    _cx += x;  _cy += y;
//  } else {
//    _cx = x;  _cy = y;
//  }
//}

//// -------------------------------------------------------
//void TaskElement::getCenter(int& x, int& y)
//{
//  unreachable();
//  x = cx();
//  y = cy();
//}
//
#if 0
// -------------------------------------------------------
int LegacyTaskElement::getTextSelected(int x_, int y_, float Corr)
{
  x_ -= cx();
  y_ -= cy();
  if(x_ < tx) return -1;
  if(y_ < ty) return -1;

  x_ -= tx;
  y_ -= ty;
  int w, dy = int(float(y_) * Corr);  // correction for font scaling
  // use the screen-compatible metric
  QFontMetrics  metrics(QucsSettings.font, 0);
  if(showName) {
    w  = metrics.horizontalAdvance(Name);
    if(dy < 1) {
      if(x_ < w) return 0;
      return -1;
    }
    dy--;
  }

  Property *pp;
  for(pp = Props.first(); pp != 0; pp = Props.next())
    if(pp->display)
      if((dy--) < 1) break;
  if(!pp) return -1;

  // get width of text
  w = metrics.horizontalAdvance(pp->Name+"="+pp->Value);
  if(x_ > w) return -1; // clicked past the property text end - selection invalid
  return Props.at()+1;  // number the property
  return 0;
}
// -------------------------------------------------------
bool TaskElement::getSelected(int x_, int y_)
{
  x_ -= cx();
  y_ -= cy();
  if(x_ >= x1) if(x_ <= x2) if(y_ >= y1) if(y_ <= y2)
    return true;

  return false;
}
#endif

// -------------------------------------------------------
void LegacyTaskElement::paint(ViewPainter *p) const
{ untested();
	int x2=0; int y2=0; //?
  int cx = center().first;
  int cy = center().second;
  int x, y, a, b, xb, yb;
  QFont f = p->font();   // save current font
  QFont newFont = f;
  {   // is simulation component (dc, ac, ...)
    unsigned size;

#if 1
//    if(!Texts.isEmpty()){
//      size = Texts.first()->Size;
//    }else{
      // possibly incomplete.
      size = 20;
//    }
	 auto scale = 1;
    newFont.setPointSizeF(scale * size);
    newFont.setWeight(QFont::DemiBold);
    p->setFont(newFont);
    p->map(0, 0, x, y);

    p->setPen(QPen(Qt::darkBlue,2));
    a = b = 0;
    QRect t;
    foreach(Text *pt, Texts) {
      t.setRect(x, y+b, 0, 0);
      p->drawText(t, Qt::AlignLeft|Qt::TextDontClip, pt->s, &_bb);
      b += _bb.height();
      if(a < _bb.width())  a = _bb.width();
   }
    xb = a + int(12.0*scale);
    yb = b + int(10.0*scale);
    x2 = x1+25 + int(float(a) / scale);
    y2 = y1+23 + int(float(b) / scale);
    if(ty < y2+1) if(ty > y1-_bb.height())  ty = y2 + 1;

    p->map(-1, 0, x, y);
    p->map(-6, -5, a, b);
    p->drawRect(a, b, xb, yb);
    p->drawLine(x,      y+yb, a,      b+yb);
    p->drawLine(x+xb-1, y+yb, x,      y+yb);
    p->drawLine(x+xb-1, y+yb, a+xb,   b+yb);
    p->drawLine(x+xb-1, y+yb, x+xb-1, y);
    p->drawLine(x+xb-1, y,    a+xb,   b);
#endif
  }

  // restore old font
  p->setFont(f);

  p->setPen(QPen(Qt::black,1));
  p->map(tx, ty, x, y);
  if(showName) {
    p->drawText(x, y, 0, 0, Qt::TextDontClip, "incomplete");
    y += p->LineSpacing;
  }
  // write all properties
#if 1
  for(Property *p4 = Props.first(); p4 != 0; p4 = Props.next()){
    if(p4->display) {
      p->drawText(x, y, 0, 0, Qt::TextDontClip, p4->Name+"="+p4->Value);
      y += p->LineSpacing;
    }
  }
#endif

  if(isActive == COMP_IS_OPEN)
    p->setPen(QPen(Qt::red,0));
  else if(isActive & COMP_IS_SHORTEN)
    p->setPen(QPen(Qt::darkGreen,0));
  if(isActive != COMP_IS_ACTIVE) {
    p->drawRect(+x1, +y1, x2-x1+1, y2-y1+1);
    p->drawLine(+x1, +y1, +x2, +y2);
    p->drawLine(+x1, +y2, +x2, +y1);
  }

#if 0
  // draw component bounding box
  if(isSelected()) {
    p->setPen(QPen(Qt::darkGray,3));
    p->drawRoundRect(cx+x1, cy+y1, x2-x1, y2-y1);
  }
#endif
} // TaskElement::paint

rect_t LegacyTaskElement::bounding_rect() const
{
	incomplete();
	return rect_t(_bb.marginsAdded(QMargins(5,5,10,10)));
}
// -------------------------------------------------------
// For output on a printer device.
void TaskElement::print(ViewPainter *p, float FontScale)
{
#if 0
  foreach(Text *pt, Texts)
    pt->Size *= FontScale;

  paint(p);

 foreach(Text *pt, Texts)
    pt->Size /= FontScale;
#endif
}
// -------------------------------------------------------
QString TaskElement::netlist()
{
  incomplete(); // remove.
  return "obsolete";
}
// -------------------------------------------------------
bool LegacyTaskElement::getPen(const QString& s, QPen& Pen, int i)
{
	unreachable();
	bool ok;
	QString n;

	n = s.section(' ',i,i);    // color
	QColor co;
	co.setNamedColor(n);
	Pen.setColor(co);
	if(!Pen.color().isValid()) return false;

	i++;
	n = s.section(' ',i,i);    // thickness
	Pen.setWidth(n.toInt(&ok));
	if(!ok) return false;

	i++;
	n = s.section(' ',i,i);    // line style
	Pen.setStyle((Qt::PenStyle)n.toInt(&ok));
	if(!ok) return false;

	return true;
}

// ---------------------------------------------------------------------
bool LegacyTaskElement::getBrush(const QString& s, QBrush& Brush, int i)
{
	unreachable();
	bool ok;
	QString n;

	n = s.section(' ',i,i);    // fill color
	QColor co;
	co.setNamedColor(n);
	Brush.setColor(co);
	if(!Brush.color().isValid()) return false;

	i++;
	n = s.section(' ',i,i);    // fill style
	Brush.setStyle((Qt::BrushStyle)n.toInt(&ok));
	if(!ok) return false;

	i++;
	n = s.section(' ',i,i);    // filled
	if(n.toInt(&ok) == 0) Brush.setStyle(Qt::NoBrush);
	if(!ok) return false;

	return true;
}

// ---------------------------------------------------------------------
#if 0
Property * LegacyTaskElement::getProperty(const QString& name)
{
  incomplete();
  for(Property *pp = Props.first(); pp != 0; pp = Props.next()){
    if(pp->Name == name) {
      return pp;
    }else{
    }
  }
  return NULL;
}
#endif

// ---------------------------------------------------------------------

// do something with Dialog Buttons
// void LegacyTaskElement::dialgButtStuff(ComponentDialog& /*d*/)const
// {
//   incomplete();
//   // d.disableButtons();
// }
//
// BUG, tmp.
//void SchematicModel::simpleInserttaskElement(LegacyTaskElement *)
//{
//  unreachable();
//}
