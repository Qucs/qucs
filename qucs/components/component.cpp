/***************************************************************************
                               component.cpp
                              ---------------
    begin                : Sat Aug 23 2003
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
#include <stdlib.h>
#include <cmath>

#include "componentdialog.h"
#include "components.h"
#include "node.h"
#include "qucs.h"
#include "schematic.h"
#include "viewpainter.h"
#include "module.h"
#include "misc.h"
#include "exception.h"
#include "trace.h"
#include "qt_compat.h"

#include <QPen>
#include <QString>
#include <QMessageBox>
#include <QPainter>
#include <QDebug>

/*!
 * \file component.cpp
 * \brief Implementation of the Component class.
 */


/*!
 * \class Component
 * \brief The Component class implements a generic analog component
 */
Component::Component()
{
  Type = isAnalogComponent;

  mirroredX = false;
  rotated = 0;
  isSelected = false;
  isActive = COMP_IS_ACTIVE;
  showName = true;

  cx = 0;
  cy = 0;
  set_qucs_text_position(0, 0);

  containingSchematic = NULL;
}

// -------------------------------------------------------
Component* Component::newOne()
{ untested();
  return new Component();
}

// -------------------------------------------------------
void Component::Bounding(int& _x1, int& _y1, int& _x2, int& _y2)
{ untested();
  _x1 = x1+cx;
  _y1 = y1+cy;
  _x2 = x2+cx;
  _y2 = y2+cy;
}

// -------------------------------------------------------

qucs::Property &Component::prop(int n)
{
  assert (n < int(Props.size()));
  auto p = Props.begin();
  while (n-- > 0 && p != Props.end()){
    ++p;
    assert(p!=Props.end());
  }

  return *p;
}

const qucs::Property &Component::prop(int n) const
{ untested();
  auto p = Props.begin();
  while (n-- > 0 && p != Props.end())
    ++p;
  assert(p != Props.end());
  return *p;
}

qucs::Port &Component::port(int n)
{
  auto p = Ports.begin();
  while (n-- > 0 && p != Ports.end())
    ++p;
  assert(p != Ports.end());
  return *p;
}

const qucs::Port &Component::port(int n) const
{ untested();
  auto p = Ports.begin();
  while (n-- > 0 && p != Ports.end())
    ++p;
  assert(p != Ports.end());
  return *p;
}

// -------------------------------------------------------
// Size of component text.
int Component::textSize(int& _dx, int& _dy)
{
  // get size of text using the screen-compatible metric
  QFontMetrics metrics(QucsSettings.font, 0);

  int tmp, count=0;
  _dx = _dy = 0;
  if(showName) {
    _dx = metrics.horizontalAdvance(Name);
    _dy = metrics.height();
    count++;
  }
  for(auto pp = Props.begin(); pp != Props.end(); ++pp)
    if(pp->display) {
      // get width of text
      tmp = metrics.horizontalAdvance(pp->Name+"="+pp->Value);
      if(tmp > _dx)  _dx = tmp;
      _dy += metrics.height();
      count++;
    }
  return count;
}

// -------------------------------------------------------
// Boundings including the component text.
void Component::entireBounds(int& _x1, int& _y1, int& _x2, int& _y2, float Corr)
{
  _x1 = x1+cx;
  _y1 = y1+cy;
  _x2 = x2+cx;
  _y2 = y2+cy;

  // text boundings
  if(tx() < x1) _x1 = tx()+cx;
  if(ty() < y1) _y1 = ty()+cy;

  int dx, dy, ny;
  ny = textSize(dx, dy);
  dy = int(float(ny) / Corr);  // correction for unproportional font scaling

  if((tx()+dx) > x2) _x2 = tx()+dx+cx;
  if((ty()+dy) > y2) _y2 = ty()+dy+cy;
}

// -------------------------------------------------------
void Component::setCenter(int x, int y, bool relative)
{ untested();
  if(relative) { untested(); cx += x;  cy += y; }
  else { untested(); cx = x;  cy = y; }
}

// -------------------------------------------------------
void Component::getCenter(int& x, int& y)
{ untested();
  x = cx;
  y = cy;
}

// -------------------------------------------------------
int Component::getTextSelected(int x_, int y_, float Corr)
{ untested();
  x_ -= cx;
  y_ -= cy;
  if(x_ < tx()) return -1;
  if(y_ < ty()) return -1;

  x_ -= tx();
  y_ -= ty();
  int w, dy = int(float(y_) * Corr);  // correction for font scaling
  // use the screen-compatible metric
  QFontMetrics  metrics(QucsSettings.font, 0);
  if(showName) { untested();
    w  = metrics.horizontalAdvance(Name);
    if(dy < 1) { untested();
      if(x_ < w) return 0;
      return -1;
    }
    dy--;
  }

  auto pp = Props.begin();
  int idx = 0;
  for( ; pp != Props.end(); ++pp, ++idx)
    if(pp->display)
      if((dy--) < 1) break;
  if(pp == Props.end()) return -1;

  // get width of text
  w = metrics.horizontalAdvance(pp->Name+"="+pp->Value);
  if(x_ > w) return -1; // clicked past the property text end - selection invalid
  return idx+1;  // number the property
}

// -------------------------------------------------------
bool Component::getSelected(int x_, int y_)
{ untested();
  x_ -= cx;
  y_ -= cy;
  if(x_ >= x1) if(x_ <= x2) if(y_ >= y1) if(y_ <= y2)
    return true;

  return false;
}

// -------------------------------------------------------
void LegacyComponent::paint(ViewPainter *p)
{ untested();
  int x, y, a, b, xb, yb;
  QFont f = p->Painter->font();   // save current font
  QFont newFont = f;
  if(Model.at(0) == '.') {   // is simulation component (dc, ac, ...)
    newFont.setPointSizeF(p->Scale * Texts.front().Size);
    newFont.setWeight(QFont::DemiBold);
    p->Painter->setFont(newFont);
    p->map(cx, cy, x, y);

    p->Painter->setPen(QPen(Qt::darkBlue,2));
    a = b = 0;
    QRect r, t;
    for(auto pt = Texts.begin(); pt != Texts.end(); ++pt) { untested();
      t.setRect(x, y+b, 0, 0);
      p->Painter->drawText(t, Qt::AlignLeft|Qt::TextDontClip, pt->s, &r);
      b += r.height();
      if(a < r.width())  a = r.width();
    }
    xb = a + int(12.0*p->Scale);
    yb = b + int(10.0*p->Scale);
    x2 = x1+25 + int(float(a) / p->Scale);
    y2 = y1+23 + int(float(b) / p->Scale);
    if(ty() < y2+1) { untested();
      if(ty() > y1-r.height()){ untested();
	set_qucs_text_position(tx(), y2 + 1);
      }else{ untested();
      }
    }else{ untested();
    }

    p->map(cx-1, cy, x, y);
    p->map(cx-6, cy-5, a, b);
    p->Painter->drawRect(a, b, xb, yb);
    p->Painter->drawLine(x,      y+yb, a,      b+yb);
    p->Painter->drawLine(x+xb-1, y+yb, x,      y+yb);
    p->Painter->drawLine(x+xb-1, y+yb, a+xb,   b+yb);
    p->Painter->drawLine(x+xb-1, y+yb, x+xb-1, y);
    p->Painter->drawLine(x+xb-1, y,    a+xb,   b);
  }
  else {    // normal components go here

    // paint all lines
    for(auto p1 = Lines.begin(); p1 != Lines.end(); ++p1) { untested();
      p->Painter->setPen(p1->style);
      p->drawLine(cx+p1->x1, cy+p1->y1, cx+p1->x2, cy+p1->y2);
    }

    // paint all arcs
    for(auto p3 = Arcs.begin(); p3 != Arcs.end(); ++p3) { untested();
      p->Painter->setPen(p3->style);
      p->drawArc(cx+p3->x, cy+p3->y, p3->w, p3->h, p3->angle, p3->arclen);
    }

    // paint all rectangles
    for(auto pa = Rects.begin(); pa != Rects.end(); ++pa) { untested();
      p->Painter->setPen(pa->Pen);
      p->Painter->setBrush(pa->Brush);
      p->drawRect(cx+pa->x, cy+pa->y, pa->w, pa->h);
    }

    // paint all ellipses
    for(auto pa = Ellips.begin(); pa != Ellips.end(); ++pa) { untested();
      p->Painter->setPen(pa->Pen);
      p->Painter->setBrush(pa->Brush);
      p->drawEllipse(cx+pa->x, cy+pa->y, pa->w, pa->h);
    }
    p->Painter->setBrush(Qt::NoBrush);

    newFont.setWeight(QFont::Light);

    // keep track of painter state
    p->Painter->save();

    // write all text
    for(auto pt = Texts.begin(); pt != Texts.end(); ++pt) { untested();
      p->Painter->setWorldTransform(
          QTransform(pt->mCos, -pt->mSin, pt->mSin, pt->mCos,
                     p->DX + float(cx+pt->x) * p->Scale,
                     p->DY + float(cy+pt->y) * p->Scale));
      newFont.setPointSizeF(p->Scale * pt->Size);
      newFont.setOverline(pt->over);
      newFont.setUnderline(pt->under);
      p->Painter->setFont(newFont);
      p->Painter->setPen(pt->Color);
      if (0) { untested();
	p->Painter->drawText(0, 0, 0, 0, Qt::AlignLeft|Qt::TextDontClip, pt->s);
      } else { untested();
	int w, h;
	w = p->drawTextMapped (pt->s, 0, 0, &h);
    Q_UNUSED(w);
      }
    }

    // restore painter state
    p->Painter->restore();
  }

  // restore old font
  p->Painter->setFont(f);

  p->Painter->setPen(QPen(Qt::black,1));
  p->map(cx+tx(), cy+ty(), x, y);
  if(showName) { untested();
    p->Painter->drawText(x, y, 0, 0, Qt::TextDontClip, Name);
    y += p->LineSpacing;
  }
  // write all properties
  for(auto p4 = Props.begin(); p4 != Props.end(); ++p4)
    if(p4->display) { untested();
      p->Painter->drawText(x, y, 0, 0, Qt::TextDontClip, p4->Name+"="+p4->Value);
      y += p->LineSpacing;
    }

  if(isActive == COMP_IS_OPEN)
    p->Painter->setPen(QPen(Qt::red,0));
  else if(isActive & COMP_IS_SHORTEN)
    p->Painter->setPen(QPen(Qt::darkGreen,0));
  if(isActive != COMP_IS_ACTIVE) { untested();
    p->drawRect(cx+x1, cy+y1, x2-x1+1, y2-y1+1);
    p->drawLine(cx+x1, cy+y1, cx+x2, cy+y2);
    p->drawLine(cx+x1, cy+y2, cx+x2, cy+y1);
  }

  // draw component bounding box
  if(isSelected) { untested();
    p->Painter->setPen(QPen(Qt::darkGray,3));
    p->drawRoundRect(cx+x1, cy+y1, x2-x1, y2-y1);
  }
}

// -------------------------------------------------------
// Paints the component when moved with the mouse.
void LegacyComponent::paintScheme(Schematic *p)
{ untested();
  // qDebug() << "paintScheme" << Model;
  if(Model.at(0) == '.') {   // is simulation component (dc, ac, ...)
    int a, b, xb, yb;
    QFont newFont = p->font();

    float Scale =
          ((Schematic*)QucsMain->DocumentTab->currentWidget())->Scale;
    newFont.setPointSizeF(float(Scale) * QucsSettings.largeFontSize);
    newFont.setWeight(QFont::DemiBold);
    // here the font metric is already the screen metric, since the font
    // is the current font the painter is using
    QFontMetrics  metrics(newFont);

    a = b = 0;
    QSize r;
    for(auto pt = Texts.begin(); pt != Texts.end(); ++pt) { untested();
      r = metrics.size(0, pt->s);
      b += r.height();
      if(a < r.width())  a = r.width();
    }
    xb = a + int(12.0*Scale);
    yb = b + int(10.0*Scale);
    x2 = x1+25 + int(float(a) / Scale);
    y2 = y1+23 + int(float(b) / Scale);
    if(ty() < y2+1) { untested();
     if(ty() > y1-r.height()) { untested();
       set_qucs_text_position(tx(), y2 + 1);
      }else{ untested();
      }
    }else{ untested();
    }

    p->PostPaintEvent(_Rect,cx-6, cy-5, xb, yb);
    p->PostPaintEvent(_Line,cx-1, cy+yb, cx-6, cy+yb-5);
    p->PostPaintEvent(_Line,cx+xb-2, cy+yb, cx-1, cy+yb);
    p->PostPaintEvent(_Line,cx+xb-2, cy+yb, cx+xb-6, cy+yb-5);
    p->PostPaintEvent(_Line,cx+xb-2, cy+yb, cx+xb-2, cy);
    p->PostPaintEvent(_Line,cx+xb-2, cy, cx+xb-6, cy-5);
    return;
  }

  // paint all lines
  for (auto p1 = Lines.begin(); p1 != Lines.end(); ++p1)
    p->PostPaintEvent(_Line,cx+p1->x1, cy+p1->y1, cx+p1->x2, cy+p1->y2);

  // paint all ports
  for (auto p2 = Ports.begin(); p2 != Ports.end(); ++p2)
    if(p2->avail) p->PostPaintEvent(_Ellipse,cx+p2->x-4, cy+p2->y-4, 8, 8);

  // paint all arcs
  for (auto p3 = Arcs.begin(); p3 != Arcs.end(); ++p3)
    p->PostPaintEvent(_Arc,cx+p3->x, cy+p3->y, p3->w, p3->h, p3->angle, p3->arclen);

  // paint all rectangles
  for (auto pa = Rects.begin(); pa != Rects.end(); ++pa)
    p->PostPaintEvent(_Rect,cx+pa->x, cy+pa->y, pa->w, pa->h);

  // paint all ellipses
  for (auto pa = Ellips.begin(); pa != Ellips.end(); ++pa)
    p->PostPaintEvent(_Ellipse,cx+pa->x, cy+pa->y, pa->w, pa->h);
}

// -------------------------------------------------------
// For output on a printer device.
void LegacyComponent::print(ViewPainter *p, float FontScale)
{ untested();
  for(auto pt = Texts.begin(); pt != Texts.end(); ++pt)
    pt->Size *= FontScale;

  paint(p);

  for(auto pt = Texts.begin(); pt != Texts.end(); ++pt)
    pt->Size /= FontScale;
}

// -------------------------------------------------------
// Rotates the component 90 counter-clockwise around its center
void LegacyComponent::rotate()
{
  // Port count only available after recreate, createSymbol
  if ((Model != "Sub") && (Model !="VHDL") && (Model != "Verilog")) // skip port count
    if(Ports.size() < 1) return;  // do not rotate components without ports
  int tmp, dx, dy;

  // rotate all lines
  for (auto p1 = Lines.begin(); p1 != Lines.end(); ++p1) {
    tmp = -p1->x1;
    p1->x1 = p1->y1;
    p1->y1 = tmp;
    tmp = -p1->x2;
    p1->x2 = p1->y2;
    p1->y2 = tmp;
  }

  // rotate all ports
  for (auto p2 = Ports.begin(); p2 != Ports.end(); ++p2) {
    tmp = -p2->x;
    p2->x = p2->y;
    p2->y = tmp;
  }

  // rotate all arcs
  for (auto p3 = Arcs.begin(); p3 != Arcs.end(); ++p3) {
    tmp = -p3->x;
    p3->x = p3->y;
    p3->y = tmp - p3->w;
    tmp = p3->w;
    p3->w = p3->h;
    p3->h = tmp;
    p3->angle += 16*90;
    if(p3->angle >= 16*360) p3->angle -= 16*360;;
  }

  // rotate all rectangles
  for (auto pa = Rects.begin(); pa != Rects.end(); ++pa) { untested();
    tmp = -pa->x;
    pa->x = pa->y;
    pa->y = tmp - pa->w;
    tmp = pa->w;
    pa->w = pa->h;
    pa->h = tmp;
  }

  // rotate all ellipses
  for (auto pa = Ellips.begin(); pa != Ellips.end(); ++pa) {
    tmp = -pa->x;
    pa->x = pa->y;
    pa->y = tmp - pa->w;
    tmp = pa->w;
    pa->w = pa->h;
    pa->h = tmp;
  }

  // rotate all text
  float ftmp;
  for(auto pt = Texts.begin(); pt != Texts.end(); ++pt) {
    tmp = -pt->x;
    pt->x = pt->y;
    pt->y = tmp;

    ftmp = -pt->mSin;
    pt->mSin = pt->mCos;
    pt->mCos = ftmp;
  }

  tmp = -x1;   // rotate boundings
  x1  = y1; y1 = -x2;
  x2  = y2; y2 = tmp;

  tmp = -tx();    // rotate text position
  set_qucs_text_position(ty(), -tx());
  // use the screen-compatible metric
  QFontMetrics  metrics(QucsSettings.font, 0);   // get size of text
  dx = dy = 0;
  if(showName) {
    dx = metrics.horizontalAdvance(Name);
    dy = metrics.lineSpacing();
  }
  for(auto pp = Props.begin(); pp != Props.end(); ++pp)
    if(pp->display) {
      // get width of text
      tmp = metrics.horizontalAdvance(pp->Name+"="+pp->Value);
      if(tmp > dx) dx = tmp;
      dy += metrics.lineSpacing();
    }
  if(tx() > x2){
    // rotate text position
    set_qucs_text_position(tx(), y1-ty()+y2);
  } else if(ty() < y1){
    set_qucs_text_position(tx(), ty() - dy);
  }else if(tx() < x1) {
    set_qucs_text_position(tx() + dy - dx, y1-ty()+y2);
  }else{
    set_qucs_text_position(tx(), ty() - dx);
  }

  rotated++;  // keep track of what's done
  rotated &= 3;
}

// -------------------------------------------------------
// Mirrors the component about the x-axis.
void LegacyComponent::mirrorX()
{
  // Port count only available after recreate, createSymbol
  if ((Model != "Sub") && (Model !="VHDL") && (Model != "Verilog")) // skip port count
    if(Ports.size() < 1) return;  // do not rotate components without ports

  // mirror all lines
  for (auto p1 = Lines.begin(); p1 != Lines.end(); ++p1) {
    p1->y1 = -p1->y1;
    p1->y2 = -p1->y2;
  }

  // mirror all ports
  for (auto p2 = Ports.begin(); p2 != Ports.end(); ++p2)
    p2->y = -p2->y;

  // mirror all arcs
  for (auto p3 = Arcs.begin(); p3 != Arcs.end(); ++p3) {
    p3->y = -p3->y - p3->h;
    if(p3->angle > 16*180) p3->angle -= 16*360;
    p3->angle  = -p3->angle;    // mirror
    p3->angle -= p3->arclen;    // go back to end of arc
    if(p3->angle < 0) p3->angle += 16*360;  // angle has to be > 0
  }

  // mirror all rectangles
  for (auto pa = Rects.begin(); pa != Rects.end(); ++pa)
    pa->y = -pa->y - pa->h;

  // mirror all ellipses
  for (auto pa = Ellips.begin(); pa != Ellips.end(); ++pa)
    pa->y = -pa->y - pa->h;

  QFont f = QucsSettings.font;
  // mirror all text
  for(auto pt = Texts.begin(); pt != Texts.end(); ++pt) {
    f.setPointSizeF(pt->Size);
    // use the screen-compatible metric
    QFontMetrics  smallMetrics(f, 0);
    QSize s = smallMetrics.size(0, pt->s);   // use size for more lines
    pt->y = -pt->y - int(pt->mCos)*s.height() + int(pt->mSin)*s.width();
  }

  int tmp = y1;
  y1  = -y2; y2 = -tmp;   // mirror boundings
  // use the screen-compatible metric
  QFontMetrics  metrics(QucsSettings.font, 0);   // get size of text
  int dy = 0;
  if(showName)
    dy = metrics.lineSpacing();   // for "Name"
  for(auto pp = Props.begin(); pp != Props.end(); ++pp)
    if(pp->display)  dy += metrics.lineSpacing();
  if((tx() > x1) && (tx() < x2)) {
    // mirror text position
    set_qucs_text_position(tx(), -ty()-dy);
  }else{
    set_qucs_text_position(tx(), y1+ty()+y2);
  }

  mirroredX = !mirroredX;    // keep track of what's done
  rotated += rotated << 1;
  rotated &= 3;
}

// -------------------------------------------------------
// Mirrors the component about the y-axis.
void LegacyComponent::mirrorY()
{
  // Port count only available after recreate, createSymbol
  if ((Model != "Sub") && (Model !="VHDL") && (Model != "Verilog")) // skip port count
    if(Ports.size() < 1) return;  // do not rotate components without ports

  // mirror all lines
  for (auto p1 = Lines.begin(); p1 != Lines.end(); ++p1) {
    p1->x1 = -p1->x1;
    p1->x2 = -p1->x2;
  }

  // mirror all ports
  for (auto p2 = Ports.begin(); p2 != Ports.end(); ++p2)
    p2->x = -p2->x;

  // mirror all arcs
  for (auto p3 = Arcs.begin(); p3 != Arcs.end(); ++p3) {
    p3->x = -p3->x - p3->w;
    p3->angle = 16*180 - p3->angle - p3->arclen;  // mirror
    if(p3->angle < 0) p3->angle += 16*360;   // angle has to be > 0
  }

  // mirror all rectangles
  for (auto pa = Rects.begin(); pa != Rects.end(); ++pa)
    pa->x = -pa->x - pa->w;

  // mirror all ellipses
  for (auto pa = Ellips.begin(); pa != Ellips.end(); ++pa)
    pa->x = -pa->x - pa->w;

  int tmp;
  QFont f = QucsSettings.font;
  // mirror all text
  for(auto pt = Texts.begin(); pt != Texts.end(); ++pt) {
    f.setPointSizeF(pt->Size);
    // use the screen-compatible metric
    QFontMetrics  smallMetrics(f, 0);
    QSize s = smallMetrics.size(0, pt->s);   // use size for more lines
    pt->x = -pt->x - int(pt->mSin)*s.height() - int(pt->mCos)*s.width();
  }

  tmp = x1;
  x1  = -x2; x2 = -tmp;   // mirror boundings
  // use the screen-compatible metric
  QFontMetrics  metrics(QucsSettings.font, 0);   // get size of text
  int dx = 0;
  if(showName)
    dx = metrics.horizontalAdvance(Name);
  for(auto pp = Props.begin(); pp != Props.end(); ++pp)
    if(pp->display) {
      // get width of text
      tmp = metrics.horizontalAdvance(pp->Name+"="+pp->Value);
      if(tmp > dx)  dx = tmp;
    }
  if((ty() > y1) && (ty() < y2)){
    // mirror text position
    set_qucs_text_position(-tx()-dx, ty());
  }else{
    set_qucs_text_position(x1+tx()+x2, ty());
  }

  mirroredX = !mirroredX;   // keep track of what's done
  rotated += rotated << 1;
  rotated += 2;
  rotated &= 3;
}

// -------------------------------------------------------
QString Component::netlist()
{
  QString s = Model+":"+Name;
  int i=-1;
  // output all node names
  // This only works in cases where the resistor would be a series
  // with the component, as for the other components, they're accounted
  // as a resistor as well, and the changes were made to their .cpp
  for(auto p1 = Ports.begin(); p1 != Ports.end(); ++p1){
    i++;
    s += " " + p1->getConnection()->Name;   // node names
  }

  // output all properties
  for(auto p2 = Props.begin(); p2 != Props.end(); ++p2) {
    if ( (p2->Name != "Symbol") && (p2->Name != "File") ){
      s += " " + p2->Name + "=\"" + p2->Value + "\"";
    }else{
      // BUG: what is this?
      // doing name dependent stuff
    }
  }

  s += '\n';

  return s;
}

// -------------------------------------------------------
QString Component::getNetlist()
{
  switch(isActive) {
    case COMP_IS_ACTIVE:
      return netlist();
    case COMP_IS_OPEN:
      return QString("");
  }

  // Component is shortened.
  int z=0;
  auto iport = Ports.begin();
  QString Node1 = iport->getConnection()->Name;
  QString s = "";
  ++iport;
  for ( ; iport != Ports.end(); ++iport)
    s += "R:" + Name + "." + QString::number(z++) + " " +
      Node1 + " " + iport->getConnection()->Name + " R=\"0\"\n";
  return s;
}

// -------------------------------------------------------
QString Component::verilogCode(int)
{ untested();
  return QString("");   // no digital model
}

// -------------------------------------------------------
QString Component::get_Verilog_Code(int NumPorts)
{ untested();
  switch(isActive) {
    case COMP_IS_OPEN:
      return QString("");
    case COMP_IS_ACTIVE:
      return verilogCode(NumPorts);
  }

  // Component is shortened.
  auto iport = Ports.begin();
  QString Node1 = iport->getConnection()->Name;
  QString s = "";
  ++iport;
  for ( ; iport != Ports.end(); ++iport)
    s += "  assign " + iport->getConnection()->Name + " = " + Node1 + ";\n";
  return s;
}

// -------------------------------------------------------
QString Component::vhdlCode(int)
{ untested();
  return QString("");   // no digital model
}

// -------------------------------------------------------
QString Component::get_VHDL_Code(int NumPorts)
{ untested();
  switch(isActive) {
    case COMP_IS_OPEN:
      return QString("");
    case COMP_IS_ACTIVE:
      return vhdlCode(NumPorts);
  }

  // Component is shortened.
  // This puts the signal of the second port onto the first port.
  // This is logically correct for the inverter only, but makes
  // some sense for the gates (OR, AND etc.).
  // Has anyone a better idea?
  QString Node1 = port(0).getConnection()->Name;
  return "  " + Node1 + " <= " + port(1).getConnection()->Name + ";\n";
}

void Component::set_port_by_index(int num, std::string const& ext_name)
{
  if(_portvalues.size()<size_t(num+1)) {
    _portvalues.resize(num+1);
  }
  _portvalues[num]=ext_name;
}

void Component::check_node_positions(Schematic* schematic)
{
  assert(schematic);
  int i=0;
  std::string msg="";
  for(auto p=Ports.begin();p!=Ports.end();p++) {
    int chx = cx+p->x;
    int chy = cy+p->y;
    try {
      if(i<(int)_portvalues.size()) {
        if(schematic->nodename_at(chx,chy) != _portvalues[i]) {
          msg="Port "+std::to_string(i);
          msg+=" of component "+Name.toStdString();
          msg+=" at position ("+std::to_string(chx)+","+std::to_string(chy)+")!";
          msg+="\n";
          msg+="Should be connected to "+_portvalues[i]+" but isn't!";
          msg+="\n";
          schematic->warn(0,msg);
        }
      } else {
        msg="No node assignment for port "+std::to_string(i);
        msg+=" of component "+Name.toStdString();
        msg+=" at position ("+std::to_string(chx)+","+std::to_string(chy)+")";
        msg+="\n";
        schematic->warn(0,msg);
      }
    }
    catch (const std::out_of_range& e)
    {
      msg="Port "+std::to_string(i);
      msg+=" of component "+Name.toStdString();
      msg+=" not found at ("+std::to_string(chx)+","+std::to_string(chy)+")!\n";
      msg+="Exception:"+std::string(e.what());
      msg+="\n";
      schematic->warn(0,msg);
    }
    i++;
  }
}

// Attributes
std::string Component::attr_get() const
{
  std::string ret;
  ret += "qucs_mirrored=";
  ret += std::to_string(mirroredX);
  ret += ", qucs_rotated=";
  ret += std::to_string(rotated);
  if(_attr.size()){ untested();
    ret += ", " + _attr;
  }else{
  }
  return ret;
}

void Component::set_attribute(std::string name, std::string value)
{
  if(name == "qucs_dev") {
    set_dev_type(value);
  }
  else
  if(name == "qucs_mirrored") {
    set_qucs_mirrored(std::stoi(value));
  }
  else
  if(name == "qucs_rotated"){
    set_qucs_rotated(std::stoi(value));
  }
}

void Component::apply_qucs_values()
{
  if(_qucs_mirrored){
    mirrorX();
  }
  if(rotated > _qucs_rotated)
    _qucs_rotated += 4;
  for(int z=rotated; z<_qucs_rotated; z++){
    rotate();
  }
  if(Ports.size()) {
    cx = _qucs_x1 - Ports.begin()->x;
    cy = _qucs_y1 - Ports.begin()->y;
  } else {
    cx = _qucs_x1;
    cy = _qucs_y1;
  }
}

// -------------------------------------------------------
// number of parameters
int Component::param_count() const
{
  return Props.size();
}

// whether a parameter is shown in a dump
bool Component::param_is_printable(int) const
{
  // incomplete(); // props(i)->is_attribute?
  return true;
}

std::string Component::param_name(int i) const
{
  assert(i<int(Props.size()));
  auto it = Props.begin();
  // BUG: Missing random access
  std::advance(it, i);
  return it->Name.toStdString();
}

std::string Component::param_value(int i) const
{
  assert(i<int(Props.size()));
  auto it = Props.begin();
  // BUG: Missing random access
  std::advance(it, i);
  return it->Value.toStdString();
}

void Component::set_param_by_index(int i, std::string const& Value)
{ untested();
  (void)i;
  (void)Value;
  incomplete();
//  auto it = Props.begin();
//  std::advance(it, i);
//  it->Value = Value;
}

void Component::set_param_by_name(std::string const& name, std::string const& v)
{
  for(auto ap = Props.begin(); ap!=Props.end(); ap++) {
    if(ap->Name.toStdString() == name) {
      ap->Value = QString::fromStdString(v);
    }
  }
}

void Component::set_dev_type(std::string const& type)
{
  Model = QString::fromStdString(type);
}

void Component::set_label(std::string const& name)
{
  Name = QString::fromStdString(name);
}

void Component::set_port_by_name(std::string const&, std::string const&)
{ incomplete();
  // BUG: missing port name
  // BUG: using position for connection making, are we?
}


// -------------------------------------------------------
// save a component
// FIXME: part of corresponding SchematicSerializer implementation
// BUG: c must be const (cannot because of QT3)
void Schematic::saveComponent(QTextStream& s, Component /*const*/ * c) const
{
#if XML
  QDomDocument doc;
  QDomElement el = doc.createElement (Model);
  doc.appendChild (el);
  el.setTagName (Model);
  el.setAttribute ("inst", Name.isEmpty() ? "*" : Name);
  el.setAttribute ("display", isActive | (showName ? 4 : 0));
  el.setAttribute ("cx", cx);
  el.setAttribute ("cy", cy);
  el.setAttribute ("tx", tx);
  el.setAttribute ("ty", ty);
  el.setAttribute ("mirror", mirroredX);
  el.setAttribute ("rotate", rotated);

  for (qucs::Property *pr = Props.front(); pr != 0; pr = Props.next()) { untested();
    el.setAttribute (pr->Name, (pr->display ? "1@" : "0@") + pr.Value);
  }
  qDebug (doc.toString());
#endif
  // s << "  "; ??
  s << "<" << c->obsolete_model_hack();

  s << " ";
  if(c->name().isEmpty()){ untested();
    s << "*";
  }else{
    s << c->name();
  }
  s << " ";

  int i=0;
  if(!c->showName){
    i = 4;
  }
  i |= c->isActive;
  s << QString::number(i);
  s << " "+QString::number(c->cx)+" "+QString::number(c->cy);
  s << " "+QString::number(c->tx())+" "+QString::number(c->ty());
  s << " ";
  if(c->mirroredX){
    s << "1";
  }else{
    s << "0";
  }
  s << " " << QString::number(c->rotated);

  // write all properties
  // FIXME: ask component for properties, not for dictionary
  for(auto p1 = c->Props.begin(); p1 != c->Props.end(); ++p1) {
    if(p1->Description.isEmpty()){
      s << " \""+p1->Name+"="+p1->Value+"\"";   // e.g. for equations
    }else{
      s << " \""+p1->Value+"\"";
    }
    s << " ";
    if(p1->display){
      s << "1";
    }else{
      s << "0";
    }
  }

  s << ">";
}
// -------------------------------------------------------
// FIXME: must be Component* SchematicParser::loadComponent(Stream&, Component*);
bool Schematic::loadComponent(const QString& _s, const std::shared_ptr<Component> &c) const
{
  bool ok;
  int  ttx, tty, tmp;
  QString s = _s;

  if(s.at(0) != '<'){ untested();
    return false;
  }else if(s.at(s.length()-1) != '>'){ untested();
    return false;
  }
  s = s.mid(1, s.length()-2);   // cut off start and end character

  QString label=s.section(' ',1,1);
  c->setName(label);

  QString n;
  n  = s.section(' ',2,2);      // isActive
  tmp = n.toInt(&ok);
  if(!ok){ untested();
    return false;
  }
  c->isActive = tmp & 3;

  if(tmp & 4){
    c->showName = false;
  }else{
    // use default, e.g. never show name for GND (bug?)
  }

  n  = s.section(' ',3,3);    // cx
  c->cx = n.toInt(&ok);
  if(!ok) return false;

  n  = s.section(' ',4,4);    // cy
  c->cy = n.toInt(&ok);
  if(!ok) return false;

  n  = s.section(' ',5,5);    // tx
  ttx = n.toInt(&ok);
  if(!ok) return false;

  n  = s.section(' ',6,6);    // ty
  tty = n.toInt(&ok);
  if(!ok) return false;

  if(c->obsolete_model_hack().at(0) != '.') {  // is simulation component (dc, ac, ...) ?

    n  = s.section(' ',7,7);    // mirroredX
    if(n.toInt(&ok) == 1){
      c->mirrorX();
    }
    if(!ok) return false;

    n  = s.section(' ',8,8);    // rotated
    tmp = n.toInt(&ok);
    if(!ok) return false;
    if(c->rotated > tmp)  // neccessary because of historical flaw in ...
      tmp += 4;        // ... components like "volt_dc"
    for(int z=c->rotated; z<tmp; z++){
      c->rotate();
    }
  }

  // restore text position (was changed by rotate/mirror)
  c->set_qucs_text_position(ttx, tty);

  QString Model = c->obsolete_model_hack(); // BUG: don't use names

  unsigned int counts = s.count('"');
  // FIXME. use c->paramCount()
  if(Model == "Sub"){
    tmp = 2;   // first property (File) already exists
  }else if(Model == "Lib"){
    tmp = 3;
  }else if(Model == "EDD"){
    tmp = 5;
  }else if(Model == "RFEDD")
    tmp = 8;
  else if(Model == "VHDL")
    tmp = 2;
  else if(Model == "MUTX")
    tmp = 5; // number of properties for the default MUTX (2 inductors)
  else tmp = counts + 1;    // "+1" because "counts" could be zero

  /// BUG FIXME. dont use Component parameter dictionary.
  for(; tmp<=(int)counts/2; tmp++){
    c->Props.push_back(qucs::Property("p", "", true, " "));
  }

  // BUG this is weird ...

  // load all properties
  unsigned int z = 0;
  for(auto p1 = c->Props.begin(); p1 != c->Props.end(); ++p1) {
    trace1("parseLegacy", Model);
    z++;
    n = s.section('"',z,z);    // property value
    z++;
    //qDebug() << "LOAD: " << p1->Description;

    // not all properties have to be mentioned (backward compatible)
    if(z > counts) {

      if(p1->Description.isEmpty()){ untested();
	trace2("erasing prop", p1->Name, p1->Value);
        c->Props.erase(p1++);    // remove if allocated in vain
      }

      if(Model == "Diode") { // BUG: don't use names
	if(counts < 56) {  // backward compatible
          counts >>= 1;
          p1 = c->Props.begin();
          for(int i = 0; i < int(counts)-1 && p1 != c->Props.end(); ++i)
            ++p1;
          for(; p1 != c->Props.begin(); ) {
            if(counts-- < 19){
              break;
	    }
            auto p1prev = p1;
            --p1prev;
            p1->Value = p1prev->Value;
            --p1;
          }

          c->prop(17).Value = c->prop(11).Value;
          p1->Value = "0";
        }
      }
      else if(Model == "AND" || Model == "NAND" || Model == "NOR" ||
	      Model == "OR" ||  Model == "XNOR"|| Model == "XOR") { untested();
	if(counts < 10) {   // backward compatible
          counts >>= 1;
          p1 = c->Props.begin();
          for(int i = 0; i < int(counts) && p1 != c->Props.end(); ++i)
            ++p1;
          for(; p1 != c->Props.begin(); ) { untested();
            if(counts-- < 4)
              break;
            auto p1prev = p1;
            --p1prev;
            p1->Value = p1prev->Value;
            --p1;
          }
          p1->Value = "10";
	}
      }
      else if(Model == "Buf" || Model == "Inv") { untested();
	if(counts < 8) {   // backward compatible
          counts >>= 1;
          p1 = c->Props.begin();
          for(int i = 0; i < int(counts) && p1 != c->Props.end(); ++i)
            ++p1;
          for(; p1 != c->Props.begin(); ) { untested();
            if(counts-- < 3)
              break;
            auto p1prev = p1;
            --p1prev;
            p1->Value = p1prev->Value;
            --p1;
          }
          p1->Value = "10";
        }
      }else{
      }

      return true;
    }

    // for equations
    if(Model != "EDD" && Model != "RFEDD" && Model != "RFEDD2P") {
      if(p1->Description.isEmpty()) {
      	// unknown number of properties ?
        p1->Name = n.section('=',0,0);
        n = n.section('=',1);
        // allocate memory for a new property (e.g. for equations)
        if((unsigned int)c->Props.size() < (counts>>1)) {
          auto p1next = p1;
          ++p1next;
          c->Props.insert(p1next, qucs::Property("y", "1", true));
        }
      }else{
      }
    }else{
    }

    if(Model == "R" && z == 6 && counts == 6) {    // backward compatible
      c->Props.back().Value = n;
      return true;
    }else if(Model == "Lib"){
      trace1("Lib??", c->Props.size());
    }else{
    }
    p1->Value = n;

    n  = s.section('"',z,z);    // display
    p1->display = (n.at(1) == '1');
  }

  return true;
}

// -------------------------------------------------------

// *******************************************************************
// ***  The following functions are used to load the schematic symbol
// ***  from file. (e.g. subcircuit, library component)

int LegacyComponent::analyseLine(const QString& Row, int numProps)
{
  QPen Pen;
  QBrush Brush;
  QColor Color;
  QString s;
  int i1, i2, i3, i4, i5, i6;

  s = Row.section(' ',0,0);    // component type
  if((s == "PortSym") || (s == ".PortSym")) {  // backward compatible
    if(!getIntegers(Row, &i1, &i2, &i3))
      return -1;
    for(i6 = Ports.size(); i6<i3; i6++)  // if ports not in numerical order
      Ports.push_back(qucs::Port(0, 0, false));

    auto po = Ports.begin();
    for(int i = 0; i < (i3-1) && po != Ports.end(); ++i)
      ++po;
    po->x  = i1;
    po->y = i2;
    po->avail = true;

    if(i1 < x1)  x1 = i1;  // keep track of component boundings
    if(i1 > x2)  x2 = i1;
    if(i2 < y1)  y1 = i2;
    if(i2 > y2)  y2 = i2;
    return 0;   // do not count Ports
  }
  else if(s == "Line") {
    if(!getIntegers(Row, &i1, &i2, &i3, &i4))  return -1;
    if(!getPen(Row, Pen, 5))  return -1;
    i3 += i1;
    i4 += i2;
    Lines.push_back(qucs::Line(i1, i2, i3, i4, Pen));

    if(i1 < x1)  x1 = i1;  // keep track of component boundings
    if(i1 > x2)  x2 = i1;
    if(i2 < y1)  y1 = i2;
    if(i2 > y2)  y2 = i2;
    if(i3 < x1)  x1 = i3;
    if(i3 > x2)  x2 = i3;
    if(i4 < y1)  y1 = i4;
    if(i4 > y2)  y2 = i4;
    return 1;
  }
  else if(s == "EArc") {
    if(!getIntegers(Row, &i1, &i2, &i3, &i4, &i5, &i6))
      return -1;
    if(!getPen(Row, Pen, 7))  return -1;
    Arcs.push_back(qucs::Arc(i1, i2, i3, i4, i5, i6, Pen));

    if(i1 < x1)  x1 = i1;  // keep track of component boundings
    if(i1+i3 > x2)  x2 = i1+i3;
    if(i2 < y1)  y1 = i2;
    if(i2+i4 > y2)  y2 = i2+i4;
    return 1;
  }
  else if(s == ".ID") {
    if(!getIntegers(Row, &i1, &i2))  return -1;
    set_qucs_text_position(i1, i2);
    Name = Row.section(' ',3,3);
    if(Name.isEmpty())  Name = "SUB";

    i1 = 1;
    auto pp = Props.begin();
    for(int i = 0; i < (numProps-1) && pp != Props.end(); ++i)
      ++pp;
    for(;;) {
      s = Row.section('"', i1,i1);
      if(s.isEmpty()){
	break;
      }else{
      }

      assert(pp!=Props.end());
      ++pp;
      if(pp == Props.end()) {
        Props.push_back(qucs::Property());
        pp = --Props.end();
	assert(pp!=Props.end());
        pp->display = (s.at(0) == '1');
        pp->Value = s.section('=', 2,2);
      }else{
      }
      assert(pp!=Props.end());

      pp->Name  = s.section('=', 1,1);
      pp->Description = s.section('=', 3,3);
      if(pp->Description.isEmpty())
        pp->Description = " ";

      i1 += 2;
    }

    if (pp!=Props.end()){
      ++pp;
    }else{
    }
    Props.erase(pp, Props.end());
    return 0;   // do not count IDs
  }
  else if(s == "Arrow") {
    if(!getIntegers(Row, &i1, &i2, &i3, &i4, &i5, &i6))  return -1;
    if(!getPen(Row, Pen, 7))  return -1;

    double beta   = atan2(double(i6), double(i5));
    double phi    = atan2(double(i4), double(i3));
    double Length = sqrt(double(i6*i6 + i5*i5));

    i3 += i1;
    i4 += i2;
    if(i1 < x1)  x1 = i1;  // keep track of component boundings
    if(i1 > x2)  x2 = i1;
    if(i3 < x1)  x1 = i3;
    if(i3 > x2)  x2 = i3;
    if(i2 < y1)  y1 = i2;
    if(i2 > y2)  y2 = i2;
    if(i4 < y1)  y1 = i4;
    if(i4 > y2)  y2 = i4;

    Lines.push_back(qucs::Line(i1, i2, i3, i4, Pen));   // base line

    double w = beta+phi;
    i5 = i3-int(Length*cos(w));
    i6 = i4-int(Length*sin(w));
    Lines.push_back(qucs::Line(i3, i4, i5, i6, Pen)); // arrow head
    if(i5 < x1)  x1 = i5;  // keep track of component boundings
    if(i5 > x2)  x2 = i5;
    if(i6 < y1)  y1 = i6;
    if(i6 > y2)  y2 = i6;

    w = phi-beta;
    i5 = i3-int(Length*cos(w));
    i6 = i4-int(Length*sin(w));
    Lines.push_back(qucs::Line(i3, i4, i5, i6, Pen));
    if(i5 < x1)  x1 = i5;  // keep track of component boundings
    if(i5 > x2)  x2 = i5;
    if(i6 < y1)  y1 = i6;
    if(i6 > y2)  y2 = i6;

    return 1;
  }
  else if(s == "Ellipse") {
    if(!getIntegers(Row, &i1, &i2, &i3, &i4))  return -1;
    if(!getPen(Row, Pen, 5))  return -1;
    if(!getBrush(Row, Brush, 8))  return -1;
    Ellips.push_back(qucs::Area(i1, i2, i3, i4, Pen, Brush));

    if(i1 < x1)  x1 = i1;  // keep track of component boundings
    if(i1 > x2)  x2 = i1;
    if(i2 < y1)  y1 = i2;
    if(i2 > y2)  y2 = i2;
    if(i1+i3 < x1)  x1 = i1+i3;
    if(i1+i3 > x2)  x2 = i1+i3;
    if(i2+i4 < y1)  y1 = i2+i4;
    if(i2+i4 > y2)  y2 = i2+i4;
    return 1;
  }
  else if(s == "Rectangle") {
    if(!getIntegers(Row, &i1, &i2, &i3, &i4))  return -1;
    if(!getPen(Row, Pen, 5))  return -1;
    if(!getBrush(Row, Brush, 8))  return -1;
    Rects.push_back(qucs::Area(i1, i2, i3, i4, Pen, Brush));

    if(i1 < x1)  x1 = i1;  // keep track of component boundings
    if(i1 > x2)  x2 = i1;
    if(i2 < y1)  y1 = i2;
    if(i2 > y2)  y2 = i2;
    if(i1+i3 < x1)  x1 = i1+i3;
    if(i1+i3 > x2)  x2 = i1+i3;
    if(i2+i4 < y1)  y1 = i2+i4;
    if(i2+i4 > y2)  y2 = i2+i4;
    return 1;
  }
  else if(s == "Text") {  // must be last in order to reuse "s" *********
    if(!getIntegers(Row, &i1, &i2, &i3, 0, &i4))  return -1;
    Color.setNamedColor(Row.section(' ',4,4));
    if(!Color.isValid()) return -1;

    s = Row.mid(Row.indexOf('"')+1);    // Text (can contain " !!!)
    s = s.left(s.length()-1);
    if(s.isEmpty()) return -1;
    misc::convert2Unicode(s);

    Texts.push_back(qucs::Text(i1, i2, s, Color, float(i3),
                          float(cos(float(i4)*pi/180.0)),
                          float(sin(float(i4)*pi/180.0))));

    QFont Font(QucsSettings.font);
    Font.setPointSizeF(float(i3));
    QFontMetrics  metrics(Font, 0); // use the screen-compatible metric
    QSize r = metrics.size(0, s);    // get size of text
    i3 = i1 + int(float(r.width())  * Texts.back().mCos)
            + int(float(r.height()) * Texts.back().mSin);
    i4 = i2 + int(float(r.width())  * -Texts.back().mSin)
            + int(float(r.height()) * Texts.back().mCos);

    if(i1 < x1)  x1 = i1;  // keep track of component boundings
    if(i2 < y1)  y1 = i2;
    if(i1 > x2)  x2 = i1;
    if(i2 > y2)  y2 = i2;

    if(i3 < x1)  x1 = i3;
    if(i4 < y1)  y1 = i4;
    if(i3 > x2)  x2 = i3;
    if(i4 > y2)  y2 = i4;
    return 1;
  }

  return 0;
}

// ---------------------------------------------------------------------
bool Component::getIntegers(const QString& s, int *i1, int *i2, int *i3,
			     int *i4, int *i5, int *i6)
{
  bool ok;
  QString n;

  if(!i1) return true;
  n  = s.section(' ',1,1);
  *i1 = n.toInt(&ok);
  if(!ok) return false;

  if(!i2) return true;
  n  = s.section(' ',2,2);
  *i2 = n.toInt(&ok);
  if(!ok) return false;

  if(!i3) return true;
  n  = s.section(' ',3,3);
  *i3 = n.toInt(&ok);
  if(!ok) return false;

  if(i4) {
    n  = s.section(' ',4,4);
    *i4 = n.toInt(&ok);
    if(!ok) return false;
  }

  if(!i5) return true;
  n  = s.section(' ',5,5);
  *i5 = n.toInt(&ok);
  if(!ok) return false;

  if(!i6) return true;
  n  = s.section(' ',6,6);
  *i6 = n.toInt(&ok);
  if(!ok) return false;

  return true;
}

// ---------------------------------------------------------------------
bool Component::getPen(const QString& s, QPen& Pen, int i)
{
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
bool Component::getBrush(const QString& s, QBrush& Brush, int i)
{
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
qucs::Property &Component::getProperty(const QString& name)
{
  for(auto pp = Props.begin(); pp != Props.end(); ++pp) {
    if(pp->Name == name) {
      return *pp;
    }
  }
  Props.push_back(qucs::Property());
  Props.back().Name = name;
  return Props.back();
}

// ---------------------------------------------------------------------
// BUG: misleading name. there is no copying here.
void LegacyComponent::copyComponent(const Component &c)
{ untested();
  Component::copyComponent(c);
  Type = c.Type;
  x1 = c.x1;
  y1 = c.y1;
  x2 = c.x2;
  y2 = c.y2;

  isActive = c.isActive;
  rotated  = c.rotated;
  mirroredX = c.mirroredX;

  Props  = c.Props;
  Ports  = c.Ports;

  if(auto l=dynamic_cast<LegacyComponent const*>(&c)) {
    Lines  = l->Lines;
    Arcs   = l->Arcs;
    Rects  = l->Rects;
    Ellips = l->Ellips;
    Texts  = l->Texts;
  }else{
  }
}


// ***********************************************************************
// ********                                                       ********
// ********          Functions of class MultiViewComponent        ********
// ********                                                       ********
// ***********************************************************************
void MultiViewComponent::recreate(Schematic *Doc)
{
  ComponentList::holder holder;
  if(Doc) { untested();
    auto i = Doc->Components->find(this);
    assert(i != Doc->Components->end());
    holder = i.ref();
  }

  // The problem is that Schematic::insertComponentNodes, which is called by Doc->insertRawComponent,
  // adds new nodes all the time, but the old nodes are not removed when the port is deleted.
  // A second problem is that the component is added to the ElementList, even if it exists already.
  // This causes weird behavior on move, as the component is selected twice, it will also move
  // at double speed.
  
  //Removes all connections to this component's ports, and deletes dangling nodes.
  //Code is very similar to Schematic::setCompPorts
  if(holder)
  { untested();
    std::list<std::shared_ptr<WireLabel> > LabelCache;

    for (auto pp = holder->Ports.begin(); pp != holder->Ports.end(); ++pp)
    { untested();
      auto pcc = pp->getConnection();
      pcc->removeConnection(holder);  // delete connections
      switch(pcc->refcount()) {
      case 0:
          { untested();
            auto pl = pcc->Label;
            if(pl)
            { untested();
              LabelCache.push_back(pl);
              pl->cx = pp->x + holder->cx;
              pl->cy = pp->y + holder->cy;
            }
            Doc->Nodes->erase(pcc);
          }
          break;
      case 2:
          Doc->oneTwoWires(pcc); // try to connect two wires to one
      default:
          ;
      }
    }
  }

  Ellips.clear();
  Texts.clear();
  Ports.clear();
  Lines.clear();
  Rects.clear();
  Arcs.clear();
  createSymbol();

  bool mmir = mirroredX;
  int  rrot = rotated;
  if (mmir && rrot==2) // mirrorX and rotate 180 = mirrorY
    mirrorY();
  else  {
    if(mmir)
      mirrorX();   // mirror
    if (rrot)
      for(int z=0; z<rrot; z++)  rotate(); // rotate
  }

  rotated = rrot;   // restore properties (were changed by rotate/mirror)
  mirroredX = mmir;

  if(Doc) { untested();
    Doc->insertRawComponent(holder);
  }
}


// ***********************************************************************
// ********                                                       ********
// ********            Functions of class GateComponent           ********
// ********               BUG: wrong file.                        ********
// ********                                                       ********
// ***********************************************************************
bool GateComponent::param_is_printable(int i) const
{
  std::string pname = param_name(i);
  if(pname=="Symbol") {
    return false;
  }else{
    return Component::param_is_printable(i);
  }
}

GateComponent::GateComponent()
{
  Type = isComponent;   // both analog and digital
  Name  = "Y";

  // the list order must be preserved !!!
  Props.push_back(qucs::Property("in", "2", false,
		QObject::tr("number of input ports")));
  Props.push_back(qucs::Property("V", "1 V", false,
		QObject::tr("voltage of high level")));
  Props.push_back(qucs::Property("t", "0", false,
		QObject::tr("delay time")));
  Props.push_back(qucs::Property("TR", "10", false,
		QObject::tr("transfer function scaling factor")));

  // this must be the last property in the list !!!
  Props.push_back(qucs::Property("Symbol", "old", false,
		QObject::tr("schematic symbol")+" [old, DIN40900]"));
}

// -------------------------------------------------------
QString GateComponent::netlist()
{
  QString s = Model+":"+Name;

  // output all node names
  for (auto pp = Ports.begin(); pp != Ports.end(); ++pp)
    s += " "+pp->getConnection()->Name;   // node names

  // output all properties
  s += " " + prop(0).Name + "=\"" + prop(0).Value + "\"";
  s += " " + prop(1).Name + "=\"" + prop(1).Value + "\"";
  s += " " + prop(2).Name + "=\"" + prop(2).Value + "\"\n";
  return s;
}

// -------------------------------------------------------
QString GateComponent::vhdlCode(int NumPorts)
{ untested();
  auto iport = Ports.begin();
  ++iport;
  QString s = "  " + iport->getConnection()->Name + " <= ";  // output port

  // xnor NOT defined for std_logic, so here use not and xor
  if (Model == "XNOR") { untested();
    QString Op = " xor ";

    // first input port
    ++iport;
    QString rhs = iport->getConnection()->Name;

    // output all input ports with node names
    while(++iport != Ports.end()) { untested();
      rhs = "not ((" + rhs + ")" + Op + iport->getConnection()->Name + ")";
    }
    s += rhs;
  }
  else { untested();
    QString Op = ' ' + Model.toLower() + ' ';
    if(Model.at(0) == 'N') { untested();
      s += "not (";    // nor, nand is NOT assoziative !!! but xnor is !!!
      Op = Op.remove(1, 1);
    }

    ++iport;
    s += iport->getConnection()->Name;   // first input port

    // output all input ports with node names
    while(++iport != Ports.end()) { untested();
      s += Op + iport->getConnection()->Name;
    }
    if(Model.at(0) == 'N')
      s += ')';
  }

  if(NumPorts <= 0) { // no truth table simulation ?
    QString td = prop(2).Value;        // delay time
    if(!misc::VHDL_Delay(td, Name)) return td;
    s += td;
  }

  s += ";\n";
  return s;
}

// -------------------------------------------------------
QString GateComponent::verilogCode(int NumPorts)
{ untested();
  bool synthesize = true;
  auto iport = Ports.begin();
  ++iport;
  QString s("");

  if(synthesize) { untested();
    QString op = Model.toLower();
    if(op == "and" || op == "nand")
      op = "&";
    else if (op == "or" || op == "nor")
      op = "|";
    else if (op == "xor")
      op = "^";
    else if (op == "xnor")
      op = "^~";

    s = "  assign";

    if(NumPorts <= 0) { // no truth table simulation ?
      QString td = prop(2).Value;        // delay time
      if(!misc::Verilog_Delay(td, Name)) return td;
      s += td;
    }
    s += " " + iport->getConnection()->Name + " = ";  // output port
    if(Model.at(0) == 'N') s += "~(";

    ++iport;
    s += iport->getConnection()->Name;   // first input port

    // output all input ports with node names
    while (++iport != Ports.end()) { untested();
      s += " " + op + " " + iport->getConnection()->Name;
    }

    if(Model.at(0) == 'N') s += ")";
    s += ";\n";
  }
  else { untested();
    s = "  " + Model.toLower();

    if(NumPorts <= 0) { // no truth table simulation ?
      QString td = prop(2).Value;        // delay time
      if(!misc::Verilog_Delay(td, Name)) return td;
      s += td;
    }
    s += " " + Name + " (" + iport->getConnection()->Name;  // output port

    ++iport;
    s += ", " + iport->getConnection()->Name;   // first input port

    // output all input ports with node names
    while (++iport != Ports.end()) { untested();
      s += ", " + iport->getConnection()->Name;
    }

    s += ");\n";
  }
  return s;
}

// -------------------------------------------------------
void GateComponent::createSymbol()
{
  int Num = Props.front().Value.toInt();
  if(Num < 2) Num = 2;
  else if(Num > 8) Num = 8;
  Props.front().Value = QString::number(Num);

  int xl, xr, y = 10*Num, z;
  x1 = -30; y1 = -y-3;
  x2 =  30; y2 =  y+3;

  set_qucs_text_position(x1+4, y2+4);

  z = 0;
  if(Model.at(0) == 'N')  z = 1;

  if(Props.back().Value.at(0) == 'D') {  // DIN symbol
    xl = -15;
    xr =  15;
    Lines.push_back(qucs::Line( 15,-y, 15, y,QPen(Qt::darkBlue,2)));
    Lines.push_back(qucs::Line(-15,-y, 15,-y,QPen(Qt::darkBlue,2)));
    Lines.push_back(qucs::Line(-15, y, 15, y,QPen(Qt::darkBlue,2)));
    Lines.push_back(qucs::Line(-15,-y,-15, y,QPen(Qt::darkBlue,2)));
    Lines.push_back(qucs::Line( 15, 0, 30, 0,QPen(Qt::darkBlue,2)));

    if(Model.at(z) == 'O') {
      Lines.push_back(qucs::Line(-11, 6-y,-6, 9-y,QPen(Qt::darkBlue,0)));
      Lines.push_back(qucs::Line(-11,12-y,-6, 9-y,QPen(Qt::darkBlue,0)));
      Lines.push_back(qucs::Line(-11,14-y,-6,14-y,QPen(Qt::darkBlue,0)));
      Lines.push_back(qucs::Line(-11,16-y,-6,16-y,QPen(Qt::darkBlue,0)));
      Texts.push_back(qucs::Text( -4, 3-y, "1", Qt::darkBlue, 15.0));
    }
    else if(Model.at(z) == 'A')
      Texts.push_back(qucs::Text( -10, 3-y, "&", Qt::darkBlue, 15.0));
    else if(Model.at(0) == 'X') { untested();
      if(Model.at(1) == 'N') { untested();
	Ellips.push_back(qucs::Area(xr,-4, 8, 8,
                  QPen(Qt::darkBlue,0), QBrush(Qt::darkBlue)));
        Texts.push_back(qucs::Text( -11, 3-y, "=1", Qt::darkBlue, 15.0));
      }
      else
        Texts.push_back(qucs::Text( -11, 3-y, "=1", Qt::darkBlue, 15.0));
    }
  }
  else {   // old symbol

    if(Model.at(z) == 'O')  xl = 10;
    else  xl = -10;
    xr = 10;
    Lines.push_back(qucs::Line(-10,-y,-10, y,QPen(Qt::darkBlue,2)));
    Lines.push_back(qucs::Line( 10, 0, 30, 0,QPen(Qt::darkBlue,2)));
    Arcs.push_back(qucs::Arc(-30,-y, 40, 30, 0, 16*90,QPen(Qt::darkBlue,2)));
    Arcs.push_back(qucs::Arc(-30,y-30, 40, 30, 0,-16*90,QPen(Qt::darkBlue,2)));
    Lines.push_back(qucs::Line( 10,15-y, 10, y-15,QPen(Qt::darkBlue,2)));

    if(Model.at(0) == 'X') {
      Lines.push_back(qucs::Line(-5, 0, 5, 0,QPen(Qt::darkBlue,1)));
      if(Model.at(1) == 'N') {
        Lines.push_back(qucs::Line(-5,-3, 5,-3,QPen(Qt::darkBlue,1)));
        Lines.push_back(qucs::Line(-5, 3, 5, 3,QPen(Qt::darkBlue,1)));
      }
      else {
        Arcs.push_back(qucs::Arc(-5,-5, 10, 10, 0, 16*360,QPen(Qt::darkBlue,1)));
        Lines.push_back(qucs::Line( 0,-5, 0, 5,QPen(Qt::darkBlue,1)));
      }
    }
  }

  if(Model.at(0) == 'N')
    Ellips.push_back(qucs::Area(xr,-4, 8, 8,
                  QPen(Qt::darkBlue,0), QBrush(Qt::darkBlue)));

  Ports.push_back(qucs::Port( 30,  0));
  y += 10;
  for(z=0; z<Num; z++) {
    y -= 20;
    Ports.push_back(qucs::Port(-30, y));
    if(xl == 10) if((z == 0) || (z == Num-1)) {
      Lines.push_back(qucs::Line(-30, y, 9, y,QPen(Qt::darkBlue,2)));
      continue;
    }
    Lines.push_back(qucs::Line(-30, y, xl, y,QPen(Qt::darkBlue,2)));
  }
}


// ***********************************************************************
// ********                                                       ********
// ******** The following function does not belong to any class.  ********
// ******** It creates a symbol by getting the identification     ********
// ******** string used in the schematic file and for copy/paste. ********
// ********                                                       ********
// ***********************************************************************

// FIXME:
// must be Component* SomeParserClass::getComponent(QString& Line)
// better: Component* SomeParserClass::getComponent(SomeDataStream& s)
std::shared_ptr<Component> getComponentFromName(QString& Line, Schematic* p)
{
  std::shared_ptr<Component> c;

  Line = Line.trimmed();
  if(Line.at(0) != '<') { untested();
    QMessageBox::critical(0, QObject::tr("Error"),
			QObject::tr("Format Error:\nWrong line start!"));
    return 0;
  }

  QString cstr = Line.section (' ',0,0); // component type
  cstr.remove (0,1);    // remove leading "<"
  c = Module::getComponent(cstr);

  if(!c) { untested();
    /// \todo enable user to load partial schematic, skip unknown components
      if (QucsMain!=0) { untested();
          QMessageBox* msg = new QMessageBox(QMessageBox::Warning,QObject::tr("Warning"),
                                             QObject::tr("Format Error:\nUnknown component!\n"
                                                         "%1\n\n"
                                                         "Do you want to load schematic anyway?\n"
                                                         "Unknown components will be replaced \n"
                                                         "by dummy subcircuit placeholders.").arg(cstr),
                                             QMessageBox::Yes|QMessageBox::No);
          int r = msg->exec();
          delete msg;
          if (r == QMessageBox::Yes) { untested();
              c.reset(new Subcircuit());
              // Hack: insert dummy File property before the first property
              int pos1 = Line.indexOf('"');
              QString filestr = QString("\"%1.sch\" 1 ").arg(cstr);
              Line.insert(pos1,filestr);
          } else return 0;
      } else { untested();
          QString err_msg = QString("Schematic loading error! Unknown device %1").arg(cstr);
          qCritical()<<err_msg;
          return 0;
      }

  }

  // BUG: don't use schematic.
  if(!p){
  }else if(!p->loadComponent(Line, c)) { untested();
    QMessageBox::critical(0, QObject::tr("Error"),
	QObject::tr("Format Error:\nWrong 'component' line format!"));
    return 0;
  }

  cstr = c->name();   // is perhaps changed in "recreate" (e.g. subcircuit)
  int x = c->tx(), y = c->ty();
  c->setSchematic (p);
  c->recreate(0);
  c->obsolete_name_override_hack(cstr);
  c->set_qucs_text_position(x, y);
  return c;
}

// do something with Dialog Buttons
void Component::dialgButtStuff(ComponentDialog& d)const
{ untested();
  d.disableButtons();
}

// vim:ts=8:sw=2:noet
