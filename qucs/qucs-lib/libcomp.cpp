/***************************************************************************
                                 libcomp.cpp
                              ------------------
    begin                : yes
    copyright            : (C) 2016 Felix Salfelder
    email                : felix@salfelder.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "libcomp.h"
#include <QPainter>

void QucsLibComponent::draw(QWidget& w) const
{
  QPainter Painter(&w);
  Painter.drawText(2, 2, 0, 0, Qt::AlignLeft | Qt::TextDontClip, "PaintText");

  QFontMetrics metrics(QucsSettings.font, 0);

  // BUG: this is no a warning. just a symbol.
  Painter.drawText(2, metrics.height(), 0, 0, Qt::AlignLeft | Qt::TextDontClip, "Warning");

  int DragNDropWidth=0; // BUG: a symbol does not have a DragNdropWidth
  int dx = (x2-x1)/2 + TextWidth - DragNDropWidth/2;
  if(dx < 2)  dx = 2;
  Painter.drawText(dx, y2-y1+2, 0, 0, Qt::AlignLeft | Qt::TextDontClip, "DragNDropText");

  // paint all lines
  for(int i=0; i<Lines.size(); i++) {
    Line *pl = Lines.at(i);
    Painter.setPen(pl->style);
    Painter.drawLine(cx+pl->x1, cy+pl->y1, cx+pl->x2, cy+pl->y2);
  }

  // paint all arcs
  for(int i=0; i<Arcs.size(); i++) {
    Arc *pc = Arcs.at(i);
    Painter.setPen(pc->style);
    Painter.drawArc(cx+pc->x, cy+pc->y, pc->w, pc->h, pc->angle, pc->arclen);
  }

  // paint all rectangles
  for(int i=0; i<Rects.size(); i++) {
    Area *pa = Rects.at(i);
    Painter.setPen(pa->Pen);
    Painter.setBrush(pa->Brush);
    Painter.drawRect(cx+pa->x, cy+pa->y, pa->w, pa->h);
  }

  // paint all ellipses
  for(int i=0; i<Ellips.size(); i++) {
    Area *pa = Ellips.at(i);
    Painter.setPen(pa->Pen);
    Painter.setBrush(pa->Brush);
    Painter.drawEllipse(cx+pa->x, cy+pa->y, pa->w, pa->h);
  }

  Painter.setPen(QPen(Qt::black,1));
  QFont Font = Painter.font();   // save current font
  Font.setWeight(QFont::Light);
  // write all text
  for(int i=0; i<Texts.size(); i++) {
    Text *pt = Texts.at(i);
    Font.setPointSizeF(pt->Size);
    Painter.setFont(Font);
    Painter.setPen(pt->Color);
    Painter.drawText(cx+pt->x, cy+pt->y, 0, 0, Qt::TextDontClip, pt->s);
  }
}

// this creates a strange pseudo-symbol. it can't be used. but hey, it's there!
// what is needed to clean this up?!
QucsLibComponent::QucsLibComponent(const QString& Lib_, const QString& Comp_)
	: LibraryName(Lib_), ComponentName(Comp_),
  cx(0), cy(0), x1(0), x2(0), y1(0), y2(0)
{
  // Warning.clear();

  int PortNo = 0;
  QString Comp = ModelString.section(' ', 0,0);
  Comp.remove(0, 1);  // remove '<'

  QString FirstProp = ModelString.section('"', 1,1);

  // BUG: that's what dictionaries are for!
  // (postponed)
  if(Comp == "_BJT") {
    Lines.append(new Line(-10,-15,-10, 15,QPen(Qt::darkBlue,3)));
    Lines.append(new Line(-30,  0,-10,  0,QPen(Qt::darkBlue,2)));
    Lines.append(new Line(-10, -5,  0,-15,QPen(Qt::darkBlue,2)));
    Lines.append(new Line(  0,-15,  0,-30,QPen(Qt::darkBlue,2)));
    Lines.append(new Line(-10,  5,  0, 15,QPen(Qt::darkBlue,2)));
    Lines.append(new Line(  0, 15,  0, 30,QPen(Qt::darkBlue,2)));

    if(FirstProp == "npn") {
      Lines.append(new Line( -6, 15,  0, 15,QPen(Qt::darkBlue,2)));
      Lines.append(new Line(  0,  9,  0, 15,QPen(Qt::darkBlue,2)));
    }
    else {
      Lines.append(new Line( -5, 10, -5, 16,QPen(Qt::darkBlue,2)));
      Lines.append(new Line( -5, 10,  1, 10,QPen(Qt::darkBlue,2)));
    }

    Arcs.append(new struct Arc(-34, -4, 8, 8, 0, 16*360,
                               QPen(Qt::red,1)));
    Arcs.append(new struct Arc(-4, -34, 8, 8, 0, 16*360,
                               QPen(Qt::red,1)));
    Arcs.append(new struct Arc(-4, 26, 8, 8, 0, 16*360,
                               QPen(Qt::red,1)));

    PortNo = 3;
    x1 = -34; y1 = -34;
    x2 =   4; y2 =  34;
  }
  else if(Comp == "JFET") {
    Lines.append(new Line(-10,-15,-10, 15,QPen(Qt::darkBlue,3)));
    Lines.append(new Line(-30,  0,-10,  0,QPen(Qt::darkBlue,2)));
    Lines.append(new Line(-10,-10,  0,-10,QPen(Qt::darkBlue,2)));
    Lines.append(new Line(  0,-10,  0,-30,QPen(Qt::darkBlue,2)));
    Lines.append(new Line(-10, 10,  0, 10,QPen(Qt::darkBlue,2)));
    Lines.append(new Line(  0, 10,  0, 30,QPen(Qt::darkBlue,2)));

    Lines.append(new Line( -4, 24,  4, 20,QPen(Qt::darkBlue,2)));

    if(FirstProp == "nfet") {
      Lines.append(new Line(-16, -5,-11,  0,QPen(Qt::darkBlue,2)));
      Lines.append(new Line(-16,  5,-11,  0,QPen(Qt::darkBlue,2)));
    }
    else {
      Lines.append(new Line(-18, 0,-13, -5,QPen(Qt::darkBlue,2)));
      Lines.append(new Line(-18, 0,-13,  5,QPen(Qt::darkBlue,2)));
    }

    Arcs.append(new struct Arc(-34, -4, 8, 8, 0, 16*360,
                               QPen(Qt::red,1)));
    Arcs.append(new struct Arc(-4, -34, 8, 8, 0, 16*360,
                               QPen(Qt::red,1)));
    Arcs.append(new struct Arc(-4, 26, 8, 8, 0, 16*360,
                               QPen(Qt::red,1)));

    PortNo = 3;
    x1 = -30; y1 = -30;
    x2 =   4; y2 =  30;
  }
  else if(Comp == "_MOSFET") {
    Lines.append(new Line(-14,-13,-14, 13,QPen(Qt::darkBlue,3)));
    Lines.append(new Line(-30,  0,-14,  0,QPen(Qt::darkBlue,2)));

    Lines.append(new Line(-10,-11,  0,-11,QPen(Qt::darkBlue,2)));
    Lines.append(new Line(  0,-11,  0,-30,QPen(Qt::darkBlue,2)));
    Lines.append(new Line(-10, 11,  0, 11,QPen(Qt::darkBlue,2)));
    Lines.append(new Line(  0,  0,  0, 30,QPen(Qt::darkBlue,2)));
    Lines.append(new Line(-10,  0,  0,  0,QPen(Qt::darkBlue,2)));

    Lines.append(new Line(-10,-16,-10, -7,QPen(Qt::darkBlue,3)));
    Lines.append(new Line(-10,  7,-10, 16,QPen(Qt::darkBlue,3)));

    if(FirstProp == "nfet") {
      Lines.append(new Line( -9,  0, -4, -5,QPen(Qt::darkBlue,2)));
      Lines.append(new Line( -9,  0, -4,  5,QPen(Qt::darkBlue,2)));
    }
    else {
      Lines.append(new Line( -1,  0, -6, -5,QPen(Qt::darkBlue,2)));
      Lines.append(new Line( -1,  0, -6,  5,QPen(Qt::darkBlue,2)));
    }

    // depletion or enhancement MOSFET ?
    if((ModelString.section('"', 3,3).trimmed().at(0) == '-') ==
       (FirstProp == "nfet"))
      Lines.append(new Line(-10, -8,-10,  8,QPen(Qt::darkBlue,3)));
    else
      Lines.append(new Line(-10, -4,-10,  4,QPen(Qt::darkBlue,3)));

    Arcs.append(new struct Arc(-34, -4, 8, 8, 0, 16*360,
                               QPen(Qt::red,1)));
    Arcs.append(new struct Arc(-4, -34, 8, 8, 0, 16*360,
                               QPen(Qt::red,1)));
    Arcs.append(new struct Arc(-4, 26, 8, 8, 0, 16*360,
                               QPen(Qt::red,1)));

    PortNo = 3;
    x1 = -34; y1 = -34;
    x2 =   4; y2 =  34;
  }
  else if(Comp == "Diode") {
    Lines.append(new Line(-30,  0, 30,  0,QPen(Qt::darkBlue,2)));
    Lines.append(new Line( -6, -9, -6,  9,QPen(Qt::darkBlue,2)));
    Lines.append(new Line(  6, -9,  6,  9,QPen(Qt::darkBlue,2)));
    Lines.append(new Line( -6,  0,  6, -9,QPen(Qt::darkBlue,2)));
    Lines.append(new Line( -6,  0,  6,  9,QPen(Qt::darkBlue,2)));

    Arcs.append(new struct Arc(-34, -4, 8, 8, 0, 16*360,
                               QPen(Qt::red,1)));
    Arcs.append(new struct Arc(26, -4, 8, 8, 0, 16*360,
                               QPen(Qt::red,1)));

    PortNo = 2;
    x1 = -34; y1 = -9;
    x2 =  34; y2 =  9;
  }
  else if(Comp == "SUBST") {
    Lines.append(new Line(-30,-16, 30,-16,QPen(Qt::darkBlue,2)));
    Lines.append(new Line(-30,-12, 30,-12,QPen(Qt::darkBlue,2)));
    Lines.append(new Line(-30, 16, 30, 16,QPen(Qt::darkBlue,2)));
    Lines.append(new Line(-30, 12, 30, 12,QPen(Qt::darkBlue,2)));
    Lines.append(new Line(-30,-16,-30, 16,QPen(Qt::darkBlue,2)));
    Lines.append(new Line( 30,-16, 30, 16,QPen(Qt::darkBlue,2)));

    Lines.append(new Line(-30,-16, 16,-40,QPen(Qt::darkBlue,2)));
    Lines.append(new Line( 30,-16, 80,-40,QPen(Qt::darkBlue,2)));
    Lines.append(new Line( 30,-12, 80,-36,QPen(Qt::darkBlue,2)));
    Lines.append(new Line( 30, 12, 80,-16,QPen(Qt::darkBlue,2)));
    Lines.append(new Line( 30, 16, 80,-12,QPen(Qt::darkBlue,2)));
    Lines.append(new Line( 16,-40, 80,-40,QPen(Qt::darkBlue,2)));
    Lines.append(new Line( 80,-40, 80,-12,QPen(Qt::darkBlue,2)));

    Lines.append(new Line(-30,  0,-18,-12,QPen(Qt::darkBlue,1)));
    Lines.append(new Line(-22, 12,  2,-12,QPen(Qt::darkBlue,1)));
    Lines.append(new Line( -2, 12, 22,-12,QPen(Qt::darkBlue,1)));
    Lines.append(new Line( 18, 12, 30,  0,QPen(Qt::darkBlue,1)));

    Lines.append(new Line( 30,  1, 37,  8,QPen(Qt::darkBlue,1)));
    Lines.append(new Line( 37,-15, 52,  0,QPen(Qt::darkBlue,1)));
    Lines.append(new Line( 52,-22, 66, -8,QPen(Qt::darkBlue,1)));
    Lines.append(new Line( 66,-30, 80,-16,QPen(Qt::darkBlue,1)));

    PortNo = 0;
    x1 = -34; y1 =-44;
    x2 =  84; y2 = 20;
  }
  else {
//	 BUG: must throw if dictionary lookup fails!
//	 (this does not work)
  //   Warning = tr("Warning: Symbol '%1' missing in Qucs Library.\n"
  //                "Drag and Drop may still work.\n"
  //                "Please contact the developers.").arg(Comp);
  }

  x1 -= 4;   // enlarge component boundings a little
  x2 += 4;
  y1 -= 4;
  y2 += 4;
  cx  = -x1 + TextWidth;
  cy  = -y1;

  int DragNDropWidth=0; // what does this do?
                        // does not seem to be a symbol property...

  int dx = x2-x1 + TextWidth;
  if((x2-x1) < DragNDropWidth)
    dx = (x2-x1 + DragNDropWidth)/2 + TextWidth;
  if(dx < DragNDropWidth)
    dx = DragNDropWidth;
  // FIXME: w.setMinimumSize(dx, y2-y1 + TextHeight+4);
  if(width() > dx)  dx = width();
  // resize(dx, y2-y1 + TextHeight+4); FIXME: resize what?!
  // update(); fixme: update who?
  PortNumber = PortNo;
}
