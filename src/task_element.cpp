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

// TODO: move
#include <stdlib.h>
#include <cmath>

#include "misc.h"
#include "module.h"
#include "node.h"
#include "property.h"
#include "qucs_app.h"
#include "qucs_globals.h"
#include "viewpainter.h"
#include "widget.h"
#include "task_element.h"
#include "qt_compat.h"

#include <QPen>
#include <QString>
#include <QMessageBox>
#include <QPainter>
#include <QDebug>

namespace qucs {

TaskElement::TaskElement(TaskElement const& p)
  : Element(p),
    tx(p.tx),
    ty(p.ty),
    showName(p.showName)
{
  qDebug() << "component copy";

//  assert(!Props.count());
//  for(auto i : p.Props){
//    Props.append(new Property(*i));
//  }
}

/*!
 * \class Component
 * \brief The TaskElement class constitutes a baseclass for commands.
 *        this one is a copy of the legacy Component class, the former
 *        baseclass for TaskElements. As such, it contains unneeded stuff.
 */
TaskElement::TaskElement() : Element()
{
  showName = true;

  tx = 0;
  ty = 0;

  // Props.setAutoDelete(true);
}
/*--------------------------------------------------------------------------*/
ElementList* TaskElement::scope()
{
	if(auto o=dynamic_cast<Symbol*>(owner())){
		return o->subckt();
	}else{ untested();
		return nullptr;
	}
}
/*--------------------------------------------------------------------------*/
Widget* TaskElement::schematicWidget() const
{ untested();
	trace0("TE::schematicWidget");

	Object const* w = widget_dispatcher["TaskElementDialog"];
	auto ww = dynamic_cast<Widget const*>(w);

	assert(ww);
	Widget* clone = ww->clone();
	assert(clone);
	return clone;
}
/*--------------------------------------------------------------------------*/

// -------------------------------------------------------
// Boundings including the component text.
void TaskElement::entireBounds(int&, int&, int&, int&, float)
{
  // qt does that.
  assert(false);
}

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
// -------------------------------------------------------
#if 0
int TaskElement::getTextSelected(int x_, int y_, float Corr)
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
#endif
/* -------------------------------------------------------------------------------- */
std::string TaskElement::param_value_by_name(std::string const& n) const
{
	if(n=="$xposition"){ untested();
		return std::to_string(cx());
	}else if(n=="$yposition"){ untested();
		return std::to_string(cy());
	}else if(n=="$mfactor"){
		return "1";
	}else if(n=="$active"){
		return "1";
//		return std::to_string(isActive); // move to element? it does not have params (yet).
	}else{ untested();
		trace1("TaskElement::paramValue", n);
		incomplete();
		return "incomplete";
	}
}
/* -------------------------------------------------------------------------------- */
rect_t TaskElement::bounding_rect() const
{
	incomplete();
	int m=5;
	return rect_t(-m,-m,120+2*m,50+2*m);
}
/* -------------------------------------------------------------------------------- */
void TaskElement::paint(ViewPainter *p) const
{
	int x2=0; int y2=0; //?
  int cx = 0; // center().first;
  int cy = 0; // center().second;
  int x, y, a, b, xb, yb;
  QFont f = p->font();   // save current font
  QFont newFont = f;
  {
    unsigned size;

#if 1
//    if(!Texts.isEmpty()){
//      size = Texts.first()->Size;
//    }else{
      // possibly incomplete.
      size = 20;
//    }
    newFont.setPointSizeF(size);
    newFont.setWeight(QFont::DemiBold);
    p->setFont(newFont);
    p->map(cx, cy, x, y);

	 std::string desc_short = caption();

    p->setPen(QPen(Qt::darkBlue,2));
    a = b = 0;
    QRect t, r;
    t.setRect(x, y+b, 0, 0);
    p->drawText(t, Qt::AlignLeft|Qt::TextDontClip, QString_(desc_short), &r);

//    foreach(Text *pt, Texts) {
//      p->drawText(t, Qt::AlignLeft|Qt::TextDontClip, pt->s, &r);
//      b += r.height();
//      if(a < r.width())  a = r.width();
//    }
//
	  if(a < r.width()){
		  a = r.width();
	  }else{
	  }
	  if(b < r.height()){
		  b = r.height();
	  }else{
	  }

	 int scale = 10;
    xb = a + 10;
    yb = b + 5;
    x2 = x1+25 + int(float(a) / scale);
    y2 = y1+23 + int(float(b) / scale);
    if(ty < y2+1 && ty > y1-r.height()){
		 ty = y2 + 1;
	 }else{
	 }

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
#if 0
  for(Property *p4 = Props.first(); p4 != 0; p4 = Props.next()){
    if(p4->display) {
      p->drawText(x, y, 0, 0, Qt::TextDontClip, p4->Name+"="+p4->Value);
      y += p->LineSpacing;
    }
  }
#endif

#if 0
  if(isActive == COMP_IS_OPEN)
    p->setPen(QPen(Qt::red,0));
  else if(isActive & COMP_IS_SHORTEN)
    p->setPen(QPen(Qt::darkGreen,0));
  if(isActive != COMP_IS_ACTIVE) {
    p->drawRect(+x1, +y1, x2-x1+1, y2-y1+1);
    p->drawLine(+x1, +y1, +x2, +y2);
    p->drawLine(+x1, +y2, +x2, +y1);
  }
#endif
} // TaskElement::paint

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
bool TaskElement::getPen(const QString& s, QPen& Pen, int i)
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
bool TaskElement::getBrush(const QString& s, QBrush& Brush, int i)
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
Property * TaskElement::getProperty(const QString& name)
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
void TaskElement::dialgButtStuff(ComponentDialog& /*d*/)const
{
  incomplete();
  // d.disableButtons();
}

} // qucs
