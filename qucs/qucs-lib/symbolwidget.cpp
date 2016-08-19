/***************************************************************************
                               symbolwidget.cpp
                              ------------------
    begin                : Sat May 28 2005
    copyright            : (C) 2005 by Michael Margraf
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


#include <assert.h>
#include <cmath>
#include <limits.h>

#include <QPainter>
#include <QPaintEvent>
#include <QPixmap>
#include <QMouseEvent>
#include <QTextIStream>
#include <QMimeData>
#include <QDrag>
#include <QTextStream>

#include "symbolwidget.h"

/*!
 * \file symbolwidget.cpp
 * \brief Implementation of the SymbolWidget class.
 */

const char *empty_xpm[] = {  // for drag n'drop
"1 1 1 1", "  c None", " "};


SymbolWidget::SymbolWidget(QWidget *parent)
    : QWidget(parent), symbol(NULL)
{

  // Text_x = Text_y = 0;
  PaintText = tr("Symbol:");
  QFontMetrics  metrics(QucsSettings.font, 0); // use the the screen-compatible metric
  TextWidth = metrics.width(PaintText) + 4;    // get size of text

  DragNDropText = tr("! Drag n'Drop me !");
  DragNDropWidth = metrics.width(DragNDropText);    // get size of text
  TextHeight = metrics.lineSpacing();

  ///setPaletteBackgroundColor(Qt::white);
  QPalette palette;
  palette.setColor(backgroundRole(), Qt::white);
  setPalette(palette);
  setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
}

void SymbolWidget::attachSymbol(Symbol* s) // bug, must be const* symbol!
{
  if(symbol){
    delete symbol;
  }else if(QucsLibComponent* c=dynamic_cast<QucsLibComponent*>(s)){
    symbol = c;
  }else{
    // incomplete. the code does not yet work for all kinds of symbols.
    assert(false);
  }
}

SymbolWidget::~SymbolWidget()
{
  attachSymbol(NULL);
}

// ************************************************************
// now these are likely unnecessary...
QString SymbolWidget::theModel() const
{
  if(symbol){
    return symbol->theModel();
  }
  return "no symbol";
}
QString SymbolWidget::modelString() const
{
  if(symbol){
    return symbol->modelString();
  }
  return "no symbol";
}
QString SymbolWidget::verilogModelString() const
{
  if(symbol){
    return symbol->verilogModelString();
  }
  return "no symbol";
}
QString SymbolWidget::vHDLModelString() const
{
  if(symbol){
    return symbol->vHDLModelString();
  }
  return "no symbol";
}

// ************************************************************
void SymbolWidget::mouseMoveEvent(QMouseEvent*)
{

  QDrag *drag = new QDrag(this);
  QMimeData *mimeData = new QMimeData;

  mimeData->setText("QucsComponent:"+theModel());
  drag->setMimeData(mimeData);
  drag->setPixmap( QPixmap(empty_xpm));
  drag->setHotSpot(QPoint(drag->pixmap().width()/2,drag->pixmap().height()));
  drag->exec();

}

// ************************************************************
void SymbolWidget::paintEvent(QPaintEvent*)
{
  if(symbol){
    symbol->draw(*this);
  }else{
	  // cannot draw anything
	  // this might not hurt anybody.
  }
}
// ---------------------------------------------------------------------
int QucsLibComponent::analyseLine(const QString& Row)
{
  QPen Pen;
  QBrush Brush;
  QColor Color;
  QString s;
  int i1, i2, i3, i4, i5, i6;

  s = Row.section(' ',0,0);    // component type
  if(s == ".PortSym") {  // here: ports are open nodes
    if(!getCompLineIntegers(Row, &i1, &i2, &i3))  return -1;
    Arcs.append(new struct Arc(i1-4, i2-4, 8, 8, 0, 16*360,
                               QPen(Qt::red,1)));

    if((i1-4) < x1)  x1 = i1-4;  // keep track of component boundings
    if((i1+4) > x2)  x2 = i1+4;
    if((i2-4) < y1)  y1 = i2-4;
    if((i2+4) > y2)  y2 = i2+4;
    return 0;   // do not count Ports
  }
  else if(s == "Line") {
    if(!getCompLineIntegers(Row, &i1, &i2, &i3, &i4))  return -1;
    if(!getPen(Row, Pen, 5))  return -1;
    i3 += i1;
    i4 += i2;
    Lines.append(new Line(i1, i2, i3, i4, Pen));

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
    if(!getCompLineIntegers(Row, &i1, &i2, &i3, &i4, &i5, &i6))  return -1;
    if(!getPen(Row, Pen, 7))  return -1;
    Arcs.append(new struct Arc(i1, i2, i3, i4, i5, i6, Pen));

    if(i1 < x1)  x1 = i1;  // keep track of component boundings
    if(i1+i3 > x2)  x2 = i1+i3;
    if(i2 < y1)  y1 = i2;
    if(i2+i4 > y2)  y2 = i2+i4;
    return 1;
  }
  else if(s == ".ID") {
    if(!getCompLineIntegers(Row, &i1, &i2))  return -1;
    Text_x = i1;
    Text_y = i2;
    Prefix = Row.section(' ',3,3);
    if(Prefix.isEmpty())  Prefix = "X";
    return 0;   // do not count IDs
  }
  else if(s == "Arrow") {
    if(!getCompLineIntegers(Row, &i1, &i2, &i3, &i4, &i5, &i6))  return -1;
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

    Lines.append(new Line(i1, i2, i3, i4, Pen));   // base line

    double w = beta+phi;
    i5 = i3-int(Length*cos(w));
    i6 = i4-int(Length*sin(w));
    Lines.append(new Line(i3, i4, i5, i6, Pen)); // arrow head
    if(i5 < x1)  x1 = i5;  // keep track of component boundings
    if(i5 > x2)  x2 = i5;
    if(i6 < y1)  y1 = i6;
    if(i6 > y2)  y2 = i6;

    w = phi-beta;
    i5 = i3-int(Length*cos(w));
    i6 = i4-int(Length*sin(w));
    Lines.append(new Line(i3, i4, i5, i6, Pen));
    if(i5 < x1)  x1 = i5;  // keep track of component boundings
    if(i5 > x2)  x2 = i5;
    if(i6 < y1)  y1 = i6;
    if(i6 > y2)  y2 = i6;

    return 1;
  }
  else if(s == "Ellipse") {
    if(!getCompLineIntegers(Row, &i1, &i2, &i3, &i4))  return -1;
    if(!getPen(Row, Pen, 5))  return -1;
    if(!getBrush(Row, Brush, 8))  return -1;
    Ellips.append(new Area(i1, i2, i3, i4, Pen, Brush));

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
    if(!getCompLineIntegers(Row, &i1, &i2, &i3, &i4))  return -1;
    if(!getPen(Row, Pen, 5))  return -1;
    if(!getBrush(Row, Brush, 8))  return -1;
    Rects.append(new Area(i1, i2, i3, i4, Pen, Brush));

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
    if(!getCompLineIntegers(Row, &i1, &i2, &i3))  return -1;
    Color.setNamedColor(Row.section(' ',4,4));
    if(!Color.isValid()) return -1;

    s = Row.mid(Row.indexOf('"')+1);    // Text (can contain " !!!)
    s = s.left(s.length()-1);
    if(s.isEmpty()) return -1;
    s.replace("\\n", "\n");
    s.replace("\\\\", "\\");

    Texts.append(new Text(i1, i2, s, Color, float(i3)));

    QFont Font(QucsSettings.font);
    Font.setPointSizeF(float(i3));
    QFontMetrics  metrics(Font, 0);  // use the the screen-compatible metric
    QSize r = metrics.size(0, s);  // get size of text
    i3 = i1 + r.width();
    i4 = i2 + r.height();

    if(i1 < x1)  x1 = i1;  // keep track of component boundings
    if(i3 > x2)  x2 = i3;
    if(i2 < y1)  y1 = i2;
    if(i4 > y2)  y2 = i4;
  }

  return 0;
}


// ---------------------------------------------------------------------
bool QucsLibComponent::getPen(const QString& s, QPen& Pen, int i)
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
bool QucsLibComponent::getBrush(const QString& s, QBrush& Brush, int i)
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

// vim:ts=8:sw=2:noet
