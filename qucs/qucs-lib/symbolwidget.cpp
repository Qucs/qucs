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

void SymbolWidget::attachSymbol(Symbol const* s)
{
  if(symbol){
    delete symbol;
  }

  AdjustWidgetSize(*s);
  qDebug() << "attaching" << s->width() << s->height();

  if(QucsLibComponent const* c=dynamic_cast<QucsLibComponent const*>(s)){
    symbol = c;
  }else{
    assert(false && "this is not a QucsLibComponent");
    // incomplete. the code does not yet work for all kinds of symbols.
  }

}

inline void SymbolWidget::AdjustWidgetSize(Symbol const& s)
{
  SymbolWidget& w(*this);
  unsigned dx = s.width() + TextWidth;
  if(s.width() < w.DragNDropWidth){
    dx = (s.width() + w.DragNDropWidth)/2 + TextWidth;
  }
  if(dx < w.DragNDropWidth){
    dx = w.DragNDropWidth;
  }
  setMinimumSize(dx, s.height() + TextHeight+4);
  if(unsigned(w.width()) > dx){
    dx = w.width();
  }
  resize(dx, s.height() + TextHeight+4);
  update(); // what does it do?
}
// ************************************************************

SymbolWidget::~SymbolWidget()
{
  if(symbol){
    // incomplete: detach/check attach count first
    delete symbol;
  }
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
  QPainter Painter(this);
  Painter.drawText(2, 2, 0, 0, Qt::AlignLeft | Qt::TextDontClip, PaintText);
  if(symbol){
    symbol->draw(Painter);
  }else{
    // cannot draw anything
    // this might not hurt anybody.
  }
}
// ---------------------------------------------------------------------

// vim:ts=8:sw=2:noet
