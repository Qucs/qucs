/***************************************************************************
                          qucsview.cpp  -  description
                             -------------------
    begin                : Thu Aug 28 18:17:41 CEST 2003
    copyright            : (C) 2003 by Michael Margraf
    email                : margraf@mwt.ee.tu-berlin.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "qucsview.h"
#include "componentdialog.h"
#include "diagramdialog.h"

#include <qinputdialog.h>


//QucsView::QucsView(QWidget *parent, const char *name, WFlags f) : QScrollView(parent,name,f)
QucsView::QucsView(QWidget *parent) : QScrollView(parent)
{
  /** connect doc with the view*/
//  connect(doc, SIGNAL(documentChanged()), this, SLOT(slotDocumentChanged()));

// #########################################################################################
//  enableClipper(TRUE);
  setVScrollBarMode(QScrollView::AlwaysOn);
  setHScrollBarMode(QScrollView::AlwaysOn);
//  viewport()->setBackgroundMode(PaletteBase);   // white background (typically)
  viewport()->setPaletteBackgroundColor(QColor(255,250,225));
  resizeContents(800,800);
  viewport()->setMouseTracking(true);

  MouseMoveAction = &QucsView::MouseDoNothing;
  MousePressAction = &QucsView::MouseDoNothing;
  MouseReleaseAction = &QucsView::MouseDoNothing;
  MouseDoubleClickAction = &QucsView::MouseDoNothing;

  Scale   = 1.0; // no scaling
  selComp = 0;  // no component is selected
  selDiag = 0;  // no diagram is selected

  Docs.setAutoDelete(true);
}


QucsView::~QucsView()
{
}

// -----------------------------------------------------------
// this function is called when the content (schematic or data display) has to be draw
void QucsView::drawContents(QPainter *p, int, int, int, int)
{
  p->scale(Scale, Scale);

  if(Docs.current()->GridOn) {
    int x, y, x1 = contentsX(), y1 = contentsY();
    int x2 = x1+int(visibleWidth()/Scale), y2 = y1+int(visibleHeight()/Scale);
    int dx = 2*Docs.current()->GridX, dy = 2*Docs.current()->GridY;
    Docs.current()->setOnGrid(x1, y1);

    p->setPen(QPen(QPen::black,1));
    for(x=x1; x<x2; x+=dx)
      for(y=y1; y<y2; y+=dy) {
        p->drawPoint(x,y);
      }
  }

  Docs.current()->paint(p);
//  drawn = false;
}

// -----------------------------------------------------------
double QucsView::Zoom(double s)
{
  Scale *= s;
  resizeContents(int(contentsWidth()*s),int(contentsHeight()*s));
  return Scale;
}

// -----------------------------------------------------------
void QucsView::MouseDoNothing(QMouseEvent*)
{
}

// *************************************************************************************
// **********                                                                 **********
// **********              Functions for serving mouse moving                 **********
// **********                                                                 **********
// *************************************************************************************
void QucsView::contentsMouseMoveEvent(QMouseEvent *Event)
{
  (this->*MouseMoveAction)(Event);
}

// -----------------------------------------------------------
void QucsView::MMoveWire2(QMouseEvent *Event)
{
  QPainter painter(viewport());
  painter.translate(-contentsX(), -contentsY());  // contents to viewport transformation
//  painter.scale(Scale, Scale);
  painter.setPen(QPen(black, 0, Qt::DotLine));
  painter.setRasterOp(Qt::NotROP);  // background should not be erased

  if(drawn)
    if(MAx1 == 0) {
      painter.drawLine(MAx3, MAy3, MAx3, MAy2); // erase old
      painter.drawLine(MAx3, MAy2, MAx2, MAy2); // erase old
    }
    else {
      painter.drawLine(MAx3, MAy3, MAx2, MAy3); // erase old
      painter.drawLine(MAx2, MAy3, MAx2, MAy2); // erase old
    }
  else drawn = true;

  MAx2  = Event->pos().x();
  MAy2  = Event->pos().y();
  Docs.current()->setOnGrid(MAx2, MAy2);

  if(MAx1 == 0) {
    painter.drawLine(MAx3, MAy3, MAx3, MAy2); // paint
    painter.drawLine(MAx3, MAy2, MAx2, MAy2); // paint
  }
  else {
    painter.drawLine(MAx3, MAy3, MAx2, MAy3); // paint
    painter.drawLine(MAx2, MAy3, MAx2, MAy2); // paint
  }
}

// -----------------------------------------------------------
void QucsView::MMoveWire1(QMouseEvent *Event)
{
  QPainter painter(viewport());
  painter.translate(-contentsX(), -contentsY());  // contents to viewport transformation
  painter.setPen(QPen(black, 0, Qt::DotLine));
  painter.setRasterOp(Qt::NotROP);  // background should not be erased

  if(drawn) {
    painter.drawLine(MAx1, MAy3, MAx2, MAy3); // erase old
    painter.drawLine(MAx3, MAy1, MAx3, MAy2);
  }
  drawn = true;

  MAx3  = Event->pos().x();
  MAy3  = Event->pos().y();
  Docs.current()->setOnGrid(MAx3, MAy3);
  
  MAx1  = contentsX();
  MAy1  = contentsY();
  MAx2  = MAx1 + visibleWidth();
  MAy2  = MAy1 + visibleHeight();

  painter.drawLine(MAx1, MAy3, MAx2, MAy3); // paint
  painter.drawLine(MAx3, MAy1, MAx3, MAy2);
}

// -----------------------------------------------------------
void QucsView::MMoveComponent(QMouseEvent *Event)
{
  if(selComp == 0) return;

  QPainter painter(viewport());
  painter.translate(-contentsX(), -contentsY());  // contents to viewport transformation
  painter.scale(Scale, Scale);
  painter.setPen(QPen(black, 0, Qt::DotLine));
  painter.setRasterOp(Qt::NotROP);  // background should not be erased

  int x=int(Event->pos().x()/Scale), y=int(Event->pos().y()/Scale);

  if(drawn) selComp->paintScheme(&painter); // erase old scheme
  drawn = true;
  Docs.current()->setOnGrid(x, y);
  selComp->setCenter(x, y);
  selComp->paintScheme(&painter); // paint scheme at new position
}

// -----------------------------------------------------------
void QucsView::MMoveDiagram(QMouseEvent *Event)
{
  if(selDiag == 0) return;

  QPainter painter(viewport());
  painter.translate(-contentsX(), -contentsY());  // contents to viewport transformation
  painter.scale(Scale, Scale);
  painter.setPen(QPen(black, 0, Qt::DotLine));
  painter.setRasterOp(Qt::NotROP);  // background should not be erased

  int x=int(Event->pos().x()/Scale), y=int(Event->pos().y()/Scale);

  if(drawn) selDiag->paintScheme(&painter); // erase old scheme
  drawn = true;
  Docs.current()->setOnGrid(x, y);
  selDiag->setCenter(x, y);
  selDiag->paintScheme(&painter); // paint scheme at new position
}

// -----------------------------------------------------------
void QucsView::MMoveSelect(QMouseEvent *Event)
{
  QPainter painter(viewport());
  painter.translate(-contentsX(), -contentsY());  // contents to viewport transformation
  painter.setPen(QPen(black, 0, Qt::DotLine));
  painter.setRasterOp(Qt::NotROP);  // background should not be erased

  if(drawn) painter.drawRect(MAx1, MAy1, MAx2, MAy2);   // erase old rectangle
  drawn = true;
  MAx2 = Event->pos().x() - MAx1;
  MAy2 = Event->pos().y() - MAy1;
  painter.drawRect(MAx1, MAy1, MAx2, MAy2); // paint new rectangle
}

// *************************************************************************************
// **********                                                                 **********
// **********          Functions for serving mouse button clicking            **********
// **********                                                                 **********
// *************************************************************************************
void QucsView::contentsMousePressEvent(QMouseEvent *Event)
{
  (this->*MousePressAction)(Event);
}

// -----------------------------------------------------------
void QucsView::MPressLabel(QMouseEvent *Event)
{
  int x = int(Event->pos().x()/Scale);
  int y = int(Event->pos().y()/Scale);
  Wire *ptr = Docs.current()->selectedWire(x, y);
  if(ptr == 0) return;

  bool OK;
  QString Name = QInputDialog::getText("Insert Nodename", "Enter the label:", QLineEdit::Normal,
                                       QString::null, &OK, this);
  if(OK && !Name.isEmpty()) {
    ptr->Name = Name;
    ptr->delta = x-ptr->x1 + y-ptr->y1;
    ptr->nx = x + 30;
    ptr->ny = y - 30;
    viewport()->repaint();
    Docs.current()->setChanged(true);
  }
}

// -----------------------------------------------------------
void QucsView::MPressSelect(QMouseEvent *Event)
{
  bool Ctrl;
  if(Event->state() & ControlButton) Ctrl = true;
  else Ctrl = false;
  if(!Docs.current()->selectComponent(int(Event->pos().x()/Scale), int(Event->pos().y()/Scale), Ctrl)) {
    MAx1 = Event->pos().x();  // if not clicking on a component => open a rectangle
    MAy1 = Event->pos().y();
    MAx2 = MAx1;
    MAy2 = MAy1;
    MouseReleaseAction = &QucsView::MReleaseSelect;
    MouseMoveAction = &QucsView::MMoveSelect;
  }
  viewport()->repaint();
}

// -----------------------------------------------------------
void QucsView::MPressDelete(QMouseEvent *Event)
{
  if(Docs.current()->selectComponent(int(Event->pos().x()/Scale), int(Event->pos().y()/Scale), true)) {
    Docs.current()->deleteComponents();
    viewport()->repaint();
  }
}

// -----------------------------------------------------------
void QucsView::MPressActivate(QMouseEvent *Event)
{
  Docs.current()->activateComponent(int(Event->pos().x()/Scale), int(Event->pos().y()/Scale));
  viewport()->repaint();
}

// -----------------------------------------------------------
void QucsView::MPressComponent(QMouseEvent *Event)
{
    QPainter painter(viewport());
    painter.translate(-contentsX(), -contentsY());
    painter.setPen(QPen(black, 0, Qt::DotLine));
    painter.setRasterOp(Qt::NotROP);  // background should not be erased

    painter.scale(Scale, Scale);
    
  switch(Event->button()) { // left mouse button inserts the component into the schematic
  case Qt::LeftButton :
    if(selComp == 0) break;
    Docs.current()->insertComponent(selComp);
    viewport()->repaint();
    selComp = selComp->newOne(); // the component is used, so create a new one
    selComp->paintScheme(&painter);
    break;

  case Qt::RightButton :  // right mouse button rotates the component
    if(selComp == 0) break;
    selComp->paintScheme(&painter); // erase old component scheme
    selComp->rotate();
    selComp->paintScheme(&painter); // paint new component scheme
    break;

  default:    // avoids compiler warnings
    break;
  }
}

// -----------------------------------------------------------
void QucsView::MPressDiagram(QMouseEvent *)
{
  QPainter painter(viewport());
  painter.translate(-contentsX(), -contentsY());
  painter.setPen(QPen(black, 0, Qt::DotLine));
  painter.setRasterOp(Qt::NotROP);  // background should not be erased

  painter.scale(Scale, Scale);

  if(selDiag == 0) return; //break;
  Docs.current()->Diags.append(selDiag);

  Docs.current()->setChanged(true);   // document has been changed

  DiagramDialog *d = new DiagramDialog(selDiag, &ProjDir);
  d->exec();
  delete d;

  viewport()->repaint();
  selDiag = selDiag->newOne(); // the component is used, so create a new one
  selDiag->paintScheme(&painter);
}

// -----------------------------------------------------------
// Is called if starting point of wire is pressed
void QucsView::MPressWire1(QMouseEvent *Event)
{
  if(Event->button() != Qt::LeftButton) return;
  
  QPainter painter(viewport());
  painter.translate(-contentsX(), -contentsY());  // contents to viewport transformation
  painter.setPen(QPen(black, 0, Qt::DotLine));
  painter.setRasterOp(Qt::NotROP);  // background should not be erased

  if(drawn) {
    painter.drawLine(MAx1, MAy3, MAx2, MAy3); // erase old mouse cross
    painter.drawLine(MAx3, MAy1, MAx3, MAy2);
  }
  drawn = false;

  MAx1 = 0;   // paint wire corner first up, then left/right
  MAx2 = MAx3;
  MAy2 = MAy3;
  MouseMoveAction = &QucsView::MMoveWire2;
  MousePressAction = &QucsView::MPressWire2;
  MouseDoubleClickAction = &QucsView::MDoubleClickWire2;
}

// -----------------------------------------------------------
// Is called if ending point of wire is pressed
void QucsView::MPressWire2(QMouseEvent *Event)
{
  QPainter painter(viewport());
  painter.translate(-contentsX(), -contentsY());  // contents to viewport transformation
  painter.setPen(QPen(black, 0, Qt::DotLine));
  painter.setRasterOp(Qt::NotROP);  // background should not be erased

  switch(Event->button()) {
  case Qt::LeftButton :
    if(MAx1 == 0) {
      if(MAx2 != MAx3) {
        if(Docs.current()->insertWire(new Wire(int(MAx3/Scale), int(MAy2/Scale), int(MAx2/Scale), int(MAy2/Scale))) & 2) {
          MouseMoveAction = &QucsView::MMoveWire1;  // if last port connected, then start a new wire
          MousePressAction = &QucsView::MPressWire1;
          MouseDoubleClickAction = &QucsView::MouseDoNothing;
        }
        if(MAy2 != MAy3) Docs.current()->insertWire(new Wire(int(MAx3/Scale), int(MAy3/Scale), int(MAx3/Scale), int(MAy2/Scale)));
      }
      else if(MAy2 != MAy3)
        if(Docs.current()->insertWire(new Wire(int(MAx3/Scale), int(MAy3/Scale), int(MAx3/Scale), int(MAy2/Scale))) & 2) {
          MouseMoveAction = &QucsView::MMoveWire1;  // if last port connected, then start a new wire
          MousePressAction = &QucsView::MPressWire1;
          MouseDoubleClickAction = &QucsView::MouseDoNothing;
        }
    }
    else {
      if(MAy2 != MAy3) {
        if(Docs.current()->insertWire(new Wire(int(MAx2/Scale), int(MAy3/Scale), int(MAx2/Scale), int(MAy2/Scale))) & 2) {
          MouseMoveAction = &QucsView::MMoveWire1;  // if last port connected, then start a new wire
          MousePressAction = &QucsView::MPressWire1;
          MouseDoubleClickAction = &QucsView::MouseDoNothing;
        }
        if(MAx2 != MAx3) Docs.current()->insertWire(new Wire(int(MAx3/Scale), int(MAy3/Scale), int(MAx2/Scale), int(MAy3/Scale)));
      }
      else if(MAx2 != MAx3)
        if(Docs.current()->insertWire(new Wire(int(MAx3/Scale), int(MAy3/Scale), int(MAx2/Scale), int(MAy3/Scale))) & 2) {
          MouseMoveAction = &QucsView::MMoveWire1;  // if last port connected, then start a new wire
          MousePressAction = &QucsView::MPressWire1;
          MouseDoubleClickAction = &QucsView::MouseDoNothing;
        }
    }
    viewport()->repaint();
    drawn = false;
    MAx3 = MAx2;
    MAy3 = MAy2;
    break;

  case Qt::RightButton :  // right mouse button changes the wire corner
    if(MAx1 == 0) {
      painter.drawLine(MAx3, MAy3, MAx3, MAy2); // erase old
      painter.drawLine(MAx3, MAy2, MAx2, MAy2); // erase old
    }
    else {
      painter.drawLine(MAx3, MAy3, MAx2, MAy3); // erase old
      painter.drawLine(MAx2, MAy3, MAx2, MAy2); // erase old
    }

    MAx2  = Event->pos().x();
    MAx2 -= MAx2 % Docs.current()->GridX - (Docs.current()->GridX >> 1);
    MAy2  = Event->pos().y();
    MAy2 -= MAy2 % Docs.current()->GridY - (Docs.current()->GridY >> 1);

    MAx1 ^= 1;    // change the painting direction of wire corner
    if(MAx1 == 0) {
      painter.drawLine(MAx3, MAy3, MAx3, MAy2); // paint
      painter.drawLine(MAx3, MAy2, MAx2, MAy2); // paint
    }
    else {
      painter.drawLine(MAx3, MAy3, MAx2, MAy3); // paint
      painter.drawLine(MAx2, MAy3, MAx2, MAy2); // paint
    }
    break;

  default:    // avoids compiler warnings
    break;
  }
}


// *************************************************************************************
// **********                                                                 **********
// **********          Functions for serving mouse button releasing           **********
// **********                                                                 **********
// *************************************************************************************
void QucsView::contentsMouseReleaseEvent(QMouseEvent *Event)
{
  (this->*MouseReleaseAction)(Event);
}

// -----------------------------------------------------------
void QucsView::MReleaseSelect(QMouseEvent *Event)
{
  QPainter painter(viewport());
  painter.translate(-contentsX(), -contentsY());  // contents to viewport transformation
  painter.setPen(QPen(black, 0, Qt::DotLine));
  painter.setRasterOp(Qt::NotROP);  // background should not be erased

  if(drawn) painter.drawRect(MAx1, MAy1, MAx2, MAy2);   // erase old rectangle
  drawn = false;
  MAx2 = Event->pos().x();
  MAy2 = Event->pos().y();

  bool Ctrl;
  if(Event->state() & ControlButton) Ctrl = true;
  else Ctrl = false;
  Docs.current()->selectComponents(MAx1, MAy1, MAx2, MAy2, Ctrl); // selects all components within
                                                                  // the rectangle
  MouseMoveAction = &QucsView::MouseDoNothing;
  MouseReleaseAction = &QucsView::MouseDoNothing;
  viewport()->repaint();
}

// *************************************************************************************
// **********                                                                 **********
// **********       Functions for serving mouse button double clicking        **********
// **********                                                                 **********
// *************************************************************************************
void QucsView::contentsMouseDoubleClickEvent(QMouseEvent *Event)
{
  (this->*MouseDoubleClickAction)(Event);
}

// -----------------------------------------------------------
void QucsView::MDoubleClickSelect(QMouseEvent *Event)
{
  Component *c = Docs.current()->selectedComponent(int(Event->pos().x()/Scale), int(Event->pos().y()/Scale));
  if(c != 0) if(!c->Model.isEmpty()) {
    ComponentDialog *d = new ComponentDialog(c, this);
    d->exec();
    viewport()->repaint();
    return;
  }

  // if no component was double clicked, try the wires
  MPressLabel(Event);
}

// -----------------------------------------------------------
void QucsView::MDoubleClickWire2(QMouseEvent *Event)
{
  MPressWire2(Event);

  MouseMoveAction = &QucsView::MMoveWire1;
  MousePressAction = &QucsView::MPressWire1;
//  MouseReleaseAction = &QucsView::MouseDoNothing;
  MouseDoubleClickAction = &QucsView::MouseDoNothing;
}
