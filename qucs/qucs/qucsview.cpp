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
#include <qclipboard.h>
#include <qapplication.h>
#include <qmessagebox.h>

#include <limits.h>


//QucsView::QucsView(QWidget *parent, const char *name, WFlags f) : QScrollView(parent,name,f)
QucsView::QucsView(QWidget *parent) : QScrollView(parent)
{
  /** connect doc with the view*/
//  connect(doc, SIGNAL(documentChanged()), this, SLOT(slotDocumentChanged()));

// #########################################################################################
//  enableClipper(TRUE);
//  setWFlags(0);//Qt::WNoAutoErase); //Qt::WStaticContents);
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

//  Scale   = 1.0; // no scaling
//  xShift = yShift = 0;
  selComp = 0;  // no component is selected
  selDiag = 0;  // no diagram is selected

  Docs.setAutoDelete(true);

  // .......................................................................
  // to repair some strange  scrolling artefacts
  connect(this, SIGNAL(horizontalSliderReleased()), viewport(), SLOT(repaint()));
  connect(this, SIGNAL(verticalSliderReleased()), viewport(), SLOT(repaint()));
}


QucsView::~QucsView()
{
}

// -----------------------------------------------------------
// this function is called when the content (schematic or data display) has to be draw
void QucsView::drawContents(QPainter *p, int, int, int, int)
{
  Docs.current()->paintGrid(p, contentsX(), contentsY(), visibleWidth(), visibleHeight());

  p->scale(Docs.current()->Scale, Docs.current()->Scale);
  p->translate(-Docs.current()->ViewX1, -Docs.current()->ViewY1);
  Docs.current()->paint(p);
//  drawn = false;
}

// -----------------------------------------------------------
double QucsView::Zoom(double s)
{
  Docs.current()->Scale *= s;
  resizeContents(int(contentsWidth()*s),int(contentsHeight()*s));
  return Docs.current()->Scale;
}

// -----------------------------------------------------------
bool QucsView::pasteElements()
{
  QClipboard *cb = QApplication::clipboard();   // get system clipboard
  QString s = cb->text(QClipboard::Clipboard);
  QTextStream stream(&s, IO_ReadOnly);
  movingElements.clear();
  if(!Docs.current()->paste(&stream, &movingElements)) return false;

//  viewport()->repaint();

  Element *pe;
  int xmax, xmin, ymax, ymin;
  xmin = ymin = INT_MAX;
  xmax = ymax = INT_MIN;
  // First, get the max and min coordinates of all selected elements.
  for(pe = movingElements.first(); pe != 0; pe = movingElements.next()) {
    if(pe->Type == isWire) {
      if(pe->x1 < xmin) xmin = pe->x1;
      if(pe->x2 > xmax) xmax = pe->x2;
      if(pe->y1 < ymin) ymin = pe->y1;
      if(pe->y2 > ymax) ymax = pe->y2;
    }
    else {
      if(pe->cx < xmin) xmin = pe->cx;
      if(pe->cx > xmax) xmax = pe->cx;
      if(pe->cy < ymin) ymin = pe->cy;
      if(pe->cy > ymax) ymax = pe->cy;
    }
  }

  xmin = -((xmax+xmin) >> 1);   // calculate midpoint
  ymin = -((ymax+ymin) >> 1);
  Docs.current()->setOnGrid(xmin, ymin);

  // moving with mouse cursor in the midpoint
  for(pe = movingElements.first(); pe != 0; pe = movingElements.next())
    pe->setCenter(xmin, ymin, true);

  return true;
}


// -----------------------------------------------------------
// Enlarge the viewport area if the coordinates x1-x2/y1-y2 exceeds the visible area.
void QucsView::enlargeView(int x1, int y1, int x2, int y2)
{
  int dx=0, dy=0;
  QucsDoc *d = Docs.current();

  if(x1 < d->ViewX1) { dx = d->ViewX1-x1+40; d->ViewX1 = x1-40; }
  if(y1 < d->ViewY1) { dy = d->ViewY1-y1+40; d->ViewY1 = y1-40; }
  if(x2 > d->ViewX2) d->ViewX2 = x2+40;
  if(y2 > d->ViewY2) d->ViewY2 = y2+40;

  resizeContents(d->ViewX2-d->ViewX1,d->ViewY2-d->ViewY1);
  scrollBy(dx,dy);
}

// -----------------------------------------------------------
void QucsView::setPainter(QPainter *p, QucsDoc *d)
{
  p->translate(-contentsX(), -contentsY());  // contents to viewport transformation
  p->scale(d->Scale, d->Scale);
  p->translate(-d->ViewX1, -d->ViewY1);
  p->setPen(QPen(black, 0, Qt::DotLine));
  p->setRasterOp(Qt::NotROP);  // background should not be erased
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
  QucsDoc *d = Docs.current();
  QPainter painter(viewport());
  setPainter(&painter, d);

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

  MAx2  = int(Event->pos().x()/d->Scale) + d->ViewX1;
  MAy2  = int(Event->pos().y()/d->Scale) + d->ViewY1;
  d->setOnGrid(MAx2, MAy2);

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
  QucsDoc *d = Docs.current();
  QPainter painter(viewport());
  setPainter(&painter, d);

  if(drawn) {
    painter.drawLine(MAx1, MAy3, MAx2, MAy3); // erase old
    painter.drawLine(MAx3, MAy1, MAx3, MAy2);
  }
  drawn = true;

  MAx3  = int(Event->pos().x()/d->Scale) + d->ViewX1;
  MAy3  = int(Event->pos().y()/d->Scale) + d->ViewY1;
  d->setOnGrid(MAx3, MAy3);
  
  MAx1  = contentsX()+d->ViewX1;
  MAy1  = contentsY()+d->ViewY1;
  MAx2  = MAx1 + visibleWidth();
  MAy2  = MAy1 + visibleHeight();

  painter.drawLine(MAx1, MAy3, MAx2, MAy3); // paint
  painter.drawLine(MAx3, MAy1, MAx3, MAy2);
}

// -----------------------------------------------------------
void QucsView::MMoveComponent(QMouseEvent *Event)
{
  if(selComp == 0) return;

  QucsDoc *d = Docs.current();
  QPainter painter(viewport());
  setPainter(&painter, d);

  int x=int(Event->pos().x()/d->Scale) + d->ViewX1;
  int y=int(Event->pos().y()/d->Scale) + d->ViewY1;

  if(drawn) selComp->paintScheme(&painter); // erase old scheme
  drawn = true;
  d->setOnGrid(x, y);
  selComp->setCenter(x, y);
  selComp->paintScheme(&painter); // paint scheme at new position
}

// -----------------------------------------------------------
void QucsView::MMoveDiagram(QMouseEvent *Event)
{
  if(selDiag == 0) return;

  QucsDoc *d = Docs.current();
  QPainter painter(viewport());
  setPainter(&painter, d);

  int x=int(Event->pos().x()/d->Scale) + d->ViewX1;
  int y=int(Event->pos().y()/d->Scale) + d->ViewY1;

  if(drawn) selDiag->paintScheme(&painter); // erase old scheme
  drawn = true;
  d->setOnGrid(x, y);
  selDiag->setCenter(x, y);
  selDiag->paintScheme(&painter); // paint scheme at new position
}

// -----------------------------------------------------------
void QucsView::MMoveSelect(QMouseEvent *Event)
{
  QucsDoc *d = Docs.current();
  QPainter painter(viewport());
  setPainter(&painter, d);

  if(drawn) painter.drawRect(MAx1, MAy1, MAx2, MAy2);   // erase old rectangle
  drawn = true;
  MAx2 = int(Event->pos().x()/d->Scale) + d->ViewX1 - MAx1;
  MAy2 = int(Event->pos().y()/d->Scale) + d->ViewY1 - MAy1;
  painter.drawRect(MAx1, MAy1, MAx2, MAy2); // paint new rectangle
}

// -----------------------------------------------------------
// Moves components by keeping the mouse button pressed.
void QucsView::MMoveMoving(QMouseEvent *Event)
{
  QucsDoc *d = Docs.current();
  QPainter painter(viewport());
  setPainter(&painter, d);

  MAx2 = int(Event->pos().x()/d->Scale) + d->ViewX1;
  MAy2 = int(Event->pos().y()/d->Scale) + d->ViewY1;
  
  d->setOnGrid(MAx2, MAy2);
  MAx1 = MAx2 - MAx1;
  MAy1 = MAy2 - MAy1;

  movingElements.clear();
  d->copySelectedElements(&movingElements);
  viewport()->repaint();

  Wire *pw;
  // Changes the position of all moving elements by dx/dy
  for(Element *pe = movingElements.first(); pe != 0; pe = movingElements.next()) {
    if(pe->Type == isWire) {
      pw = (Wire*)pe;   // connecting wires are only moved with one node

      if(int(pw->Port1) > 3) {
        pw->x1 += MAx1;
        pw->y1 += MAy1;
      }
      else {
        if(int(pw->Port1) & 1) pw->x1 += MAx1;
        if(int(pw->Port1) & 2) pw->y1 += MAy1;
      }

      if(int(pw->Port2) > 3) {
        pw->x2 += MAx1;
        pw->y2 += MAy1;
      }
      else {
        if(int(pw->Port2) & 1) pw->x2 += MAx1;
        if(int(pw->Port2) & 2) pw->y2 += MAy1;
      }
    }
    else pe->setCenter(MAx1, MAy1, true);
    
    pe->paintScheme(&painter);
  }

  drawn = true;
  MAx1 = MAx2;
  MAy1 = MAy2;
  MouseMoveAction = &QucsView::MMoveMoving2;
  MouseReleaseAction = &QucsView::MReleaseMoving;

}

// -----------------------------------------------------------
// Moves components by keeping the mouse button pressed.
void QucsView::MMoveMoving2(QMouseEvent *Event)
{
  QucsDoc *d = Docs.current();
  QPainter painter(viewport());
  setPainter(&painter, d);

  MAx2 = int(Event->pos().x()/d->Scale) + d->ViewX1;
  MAy2 = int(Event->pos().y()/d->Scale) + d->ViewY1;

  if(drawn) // erase old scheme
    for(Element *p1 = movingElements.first(); p1 != 0; p1 = movingElements.next())
      p1->paintScheme(&painter);

  drawn = true;
  d->setOnGrid(MAx2, MAy2);
  MAx1 = MAx2 - MAx1;
  MAy1 = MAy2 - MAy1;

  Wire *pw;
  for(Element *pe = movingElements.first(); pe != 0; pe = movingElements.next()) {
    if(pe->Type == isWire) {
      pw = (Wire*)pe;   // connecting wires are not moved completely

      if(int(pw->Port1) > 3) { pw->x1 += MAx1;  pw->y1 += MAy1; }
      else {  if(int(pw->Port1) & 1) pw->x1 += MAx1;
              if(int(pw->Port1) & 2) pw->y1 += MAy1; }

      if(int(pw->Port2) > 3) { pw->x2 += MAx1;  pw->y2 += MAy1; }
      else {  if(int(pw->Port2) & 1) pw->x2 += MAx1;
              if(int(pw->Port2) & 2) pw->y2 += MAy1; }
    }
    else pe->setCenter(MAx1, MAy1, true);

    pe->paintScheme(&painter);
  }

  MAx1 = MAx2;
  MAy1 = MAy2;
}

// -----------------------------------------------------------
// Moves components after paste from clipboard.
void QucsView::MMovePaste(QMouseEvent *Event)
{
  QucsDoc *d = Docs.current();
  QPainter painter(viewport());
  setPainter(&painter, d);

  MAx1 = int(Event->pos().x()/d->Scale) + d->ViewX1;
  MAy1 = int(Event->pos().y()/d->Scale) + d->ViewY1;
  d->setOnGrid(MAx1, MAy1);

  for(Element *pe = movingElements.first(); pe != 0; pe = movingElements.next()) {
    pe->setCenter(MAx1, MAy1, true);
    pe->paintScheme(&painter);
  }

  drawn = true;
  MouseMoveAction = &QucsView::MMoveMoving2;
  MouseReleaseAction = &QucsView::MReleasePaste;
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
  int x = int(Event->pos().x()/Docs.current()->Scale)+Docs.current()->ViewX1;
  int y = int(Event->pos().y()/Docs.current()->Scale)+Docs.current()->ViewY1;
  Wire *pw = Docs.current()->selectedWire(x, y);
  if(pw == 0) return;

  QString Name("");
  Wire *pw2 = Docs.current()->getWireLabel(pw);   // is wire line already labeled ?
  if(pw2 != 0) Name = pw2->Name;
  
  bool OK;
  Name = QInputDialog::getText("Insert Nodename", "Enter the label:", QLineEdit::Normal,
                                       Name, &OK, this);
  if(OK && !Name.isEmpty()) {
    if(Name.at(0) == '_') Name.remove(0,1);   // label must not start with '_'
    if(pw2 != 0) pw2->Name = "";  // delete old name
    pw->Name = Name;
    pw->delta = x-pw->x1 + y-pw->y1;
    pw->nx = x + 30;
    pw->ny = y - 30;
    enlargeView(pw->nx, pw->ny, pw->nx, pw->ny);
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

  MAx1 = int(double(Event->pos().x())/Docs.current()->Scale)+Docs.current()->ViewX1;
  MAy1 = int(double(Event->pos().y())/Docs.current()->Scale)+Docs.current()->ViewY1;
  focusElement = Docs.current()->selectElement(MAx1, MAy1, Ctrl);
  if(focusElement == 0) {
    MAx2 = 0;  // if not clicking on a component => open a rectangle
    MAy2 = 0;
    MouseReleaseAction = &QucsView::MReleaseSelect2;
    MouseMoveAction = &QucsView::MMoveSelect;
    MousePressAction = &QucsView::MouseDoNothing;
    MouseDoubleClickAction = &QucsView::MouseDoNothing;
  }
  else {  // component could be moved
    Docs.current()->setOnGrid(MAx1, MAy1);
    MouseMoveAction = &QucsView::MMoveMoving;
    MousePressAction = &QucsView::MouseDoNothing;
    MouseDoubleClickAction = &QucsView::MouseDoNothing;
  }
  viewport()->repaint();
}

// -----------------------------------------------------------
void QucsView::MPressDelete(QMouseEvent *Event)
{
  Element *e
    = Docs.current()->selectElement(int(Event->pos().x()/Docs.current()->Scale)+Docs.current()->ViewX1,
                                    int(Event->pos().y()/Docs.current()->Scale)+Docs.current()->ViewY1,false);
  if(e != 0) {
    e->isSelected = true;
    Docs.current()->deleteElements();
    viewport()->repaint();
  }
}

// -----------------------------------------------------------
void QucsView::MPressActivate(QMouseEvent *Event)
{
  MAx1 = int(Event->pos().x()/Docs.current()->Scale) + Docs.current()->ViewX1;
  MAy1 = int(Event->pos().y()/Docs.current()->Scale) + Docs.current()->ViewY1;
  if(!Docs.current()->activateComponent(MAx1, MAy1)) {
    MAx2 = 0;  // if not clicking on a component => open a rectangle
    MAy2 = 0;
    MousePressAction = &QucsView::MouseDoNothing;
    MouseReleaseAction = &QucsView::MReleaseActivate;
    MouseMoveAction = &QucsView::MMoveSelect;
  }

  viewport()->repaint();
}

// -----------------------------------------------------------
void QucsView::MPressMirrorX(QMouseEvent *Event)
{
  // no use in mirroring wires or diagrams
  Component *c
    = Docs.current()->selectedComponent(int(Event->pos().x()/Docs.current()->Scale)+Docs.current()->ViewX1,
                                        int(Event->pos().y()/Docs.current()->Scale)+Docs.current()->ViewY1);
  if(c == 0) return;
  
  if(c->Ports.count() < 1) return;  // do not mirror components without ports
  c->mirrorX();
  Docs.current()->setCompPorts(c);

  viewport()->repaint();
}

// -----------------------------------------------------------
void QucsView::MPressMirrorY(QMouseEvent *Event)
{
  // no use in mirroring wires or diagrams
  Component *c
    = Docs.current()->selectedComponent(int(Event->pos().x()/Docs.current()->Scale)+Docs.current()->ViewX1,
                                        int(Event->pos().y()/Docs.current()->Scale)+Docs.current()->ViewY1);
  if(c == 0) return;

  if(c->Ports.count() < 1) return;  // do not mirror components without ports
  c->mirrorY();
  Docs.current()->setCompPorts(c);

  viewport()->repaint();
}

// -----------------------------------------------------------
void QucsView::MPressRotate(QMouseEvent *Event)
{
  QucsDoc *d = Docs.current();

  Element *e = d->selectElement(int(Event->pos().x()/Docs.current()->Scale)+Docs.current()->ViewX1,
                                int(Event->pos().y()/Docs.current()->Scale)+Docs.current()->ViewY1,false);
  if(e == 0) return;
  
  int x1, y1, x2, y2;
  e->isSelected = false;
  switch(e->Type) {
    case isComponent: if(((Component*)e)->Ports.count() < 1)
                        break;  // do not rotate components without ports
                      ((Component*)e)->rotate();
                      d->setCompPorts((Component*)e);
                      // enlarge viewarea if component lies outside the view
                      ((Component*)e)->entireBounds(x1,y1,x2,y2);
                      enlargeView(x1, y1, x2, y2);
                      break;
    case isWire:      d->Wires.setAutoDelete(false);
                      d->deleteWire((Wire*)e);
                      ((Wire*)e)->rotate();
                      d->setOnGrid(((Wire*)e)->x1, ((Wire*)e)->y1);
                      d->setOnGrid(((Wire*)e)->x2, ((Wire*)e)->y2);
                      d->insertWire((Wire*)e);
                      d->Wires.setAutoDelete(true);
                      enlargeView(e->x1, e->y1, e->x2, e->y2);
    default:          ;
  }
  viewport()->repaint();
}

// -----------------------------------------------------------
void QucsView::MPressComponent(QMouseEvent *Event)
{
  QucsDoc *d = Docs.current();
  QPainter painter(viewport());
  setPainter(&painter, d);
    
  int x1, y1, x2, y2;
  switch(Event->button()) { // left mouse button inserts the component into the schematic
  case Qt::LeftButton :
    if(selComp == 0) break;
    d->insertComponent(selComp);

    selComp->entireBounds(x1,y1,x2,y2);   // enlarge viewarea if component lies outside the view
    enlargeView(x1, y1, x2, y2);
    
    viewport()->repaint();
    selComp = selComp->newOne(); // the component is used, so create a new one
    selComp->paintScheme(&painter);
    break;

  case Qt::RightButton :  // right mouse button rotates the component
    if(selComp == 0) break;
    if(selComp->Ports.count() == 0) break;  // do not rotate components without ports
    selComp->paintScheme(&painter); // erase old component scheme
    selComp->rotate();
    selComp->paintScheme(&painter); // paint new component scheme
    break;

  default: ;   // avoids compiler warnings
  }
}

// -----------------------------------------------------------
void QucsView::MPressDiagram(QMouseEvent *Event)
{
  QucsDoc *d = Docs.current();
  QPainter painter(viewport());
  setPainter(&painter, d);

  if(selDiag == 0) return;
  selDiag->setCenter(int(Event->pos().x()/d->Scale) + d->ViewX1,
                     int(Event->pos().y()/d->Scale) + d->ViewY1);
  d->Diags.append(selDiag);

  enlargeView(selDiag->cx, selDiag->cy-selDiag->y2, selDiag->cx+selDiag->x2, selDiag->cy);
  d->setChanged(true);   // document has been changed

  DiagramDialog *dia = new DiagramDialog(selDiag, d->DataSet);
  dia->exec();
  delete dia;

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
  setPainter(&painter, Docs.current());

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
  QucsDoc *d = Docs.current();
  QPainter painter(viewport());
  setPainter(&painter, d);

  switch(Event->button()) {
  case Qt::LeftButton :
    if(MAx1 == 0) { // which wire direction first ?
      if(MAx2 != MAx3) {
        if(d->insertWire(new Wire(MAx3, MAy2, MAx2, MAy2)) & 2) {
          MouseMoveAction = &QucsView::MMoveWire1;  // if last port connected, then start a new wire
          MousePressAction = &QucsView::MPressWire1;
          MouseDoubleClickAction = &QucsView::MouseDoNothing;
        }
        if(MAy2 != MAy3) d->insertWire(new Wire(MAx3, MAy3, MAx3, MAy2));
      }
      else if(MAy2 != MAy3)
        if(d->insertWire(new Wire(MAx3, MAy3, MAx3, MAy2)) & 2) {
          MouseMoveAction = &QucsView::MMoveWire1;  // if last port connected, then start a new wire
          MousePressAction = &QucsView::MPressWire1;
          MouseDoubleClickAction = &QucsView::MouseDoNothing;
        }
    }
    else {
      if(MAy2 != MAy3) {
        if(d->insertWire(new Wire(MAx2, MAy3, MAx2, MAy2)) & 2) {
          MouseMoveAction = &QucsView::MMoveWire1;  // if last port connected, then start a new wire
          MousePressAction = &QucsView::MPressWire1;
          MouseDoubleClickAction = &QucsView::MouseDoNothing;
        }
        if(MAx2 != MAx3) d->insertWire(new Wire(MAx3, MAy3, MAx2, MAy3));
      }
      else if(MAx2 != MAx3)
        if(d->insertWire(new Wire(MAx3, MAy3, MAx2, MAy3)) & 2) {
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

    MAx2  = int(Event->pos().x()/d->Scale) + d->ViewX1;
    MAy2  = int(Event->pos().y()/d->Scale) + d->ViewY1;
    d->setOnGrid(MAx2, MAy2);

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

  default: ;    // avoids compiler warnings
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
  bool ctrl;
  if(Event->state() & ControlButton) ctrl = true;
  else ctrl = false;
  
  if(!ctrl) Docs.current()->deselectElements(focusElement);

  if(focusElement != 0)
    if(Event->button() == LeftButton)
      if(focusElement->Type == isWire) {
        Docs.current()->selectWireLine(focusElement, ((Wire*)focusElement)->Port1, ctrl);
        Docs.current()->selectWireLine(focusElement, ((Wire*)focusElement)->Port2, ctrl);
      }

  MousePressAction = &QucsView::MPressSelect;
  MouseReleaseAction = &QucsView::MReleaseSelect;
  MouseDoubleClickAction = &QucsView::MDoubleClickSelect;
  MouseMoveAction = &QucsView::MouseDoNothing;   // no element moving
  viewport()->repaint();
}

// -----------------------------------------------------------
// Is called after the rectangle for selection is released.
void QucsView::MReleaseSelect2(QMouseEvent *Event)
{
  if(Event->button() != Qt::LeftButton) return;

  QucsDoc *d = Docs.current();
  QPainter painter(viewport());
  setPainter(&painter, d);

  bool Ctrl;
  if(Event->state() & ControlButton) Ctrl = true;
  else Ctrl = false;
  
   // selects all components within the rectangle
  d->selectComponents(MAx1, MAy1, MAx1+MAx2, MAy1+MAy2, Ctrl);
/*  if(d->selectComponents(MAx1, MAy1, MAx2, MAy2, Ctrl) > 0)
    emit CompsSelected(true);
  else emit CompsSelected(false);*/
  
  MouseMoveAction = &QucsView::MouseDoNothing;
  MousePressAction = &QucsView::MPressSelect;
  MouseReleaseAction = &QucsView::MReleaseSelect;
  MouseDoubleClickAction = &QucsView::MDoubleClickSelect;
  viewport()->repaint();
  drawn = false;
}

// -----------------------------------------------------------
void QucsView::MReleaseActivate(QMouseEvent *Event)
{
  if(Event->button() != Qt::LeftButton) return;

  QucsDoc *d = Docs.current();
  QPainter painter(viewport());
  setPainter(&painter, d);

  if(drawn) painter.drawRect(MAx1, MAy1, MAx2, MAy2);   // erase old rectangle
  drawn = false;

   // activates all components within the rectangle
  d->activateComps(MAx1, MAy1, MAx1+MAx2, MAy1+MAy2);

  MouseMoveAction = &QucsView::MouseDoNothing;
  MousePressAction = &QucsView::MPressActivate;
  MouseReleaseAction = &QucsView::MouseDoNothing;
  MouseDoubleClickAction = &QucsView::MouseDoNothing;
  viewport()->repaint();
}

// -----------------------------------------------------------
void QucsView::MReleaseMoving(QMouseEvent *Event)
{
  if(Event->button() != Qt::LeftButton) return;
  
  int x1, y1, x2, y2;
  // insert all moved elements into document and enlarge viewarea if component lies outside the view
  for(Element *pe = movingElements.first(); pe!=0; pe = movingElements.next()) {
    switch(pe->Type) {
      case isWire:    if(pe->x1 == pe->x2) if(pe->y1 == pe->y2) break;
                      Docs.current()->insertWire((Wire*)pe);
                      enlargeView(pe->x1, pe->y1, pe->x2, pe->y2);
                      break;
      case isDiagram: Docs.current()->Diags.append((Diagram*)pe);
                      enlargeView(pe->cx, pe->cy-pe->y2, pe->cx+pe->x2, pe->cy);
                      Docs.current()->setChanged(true);
                      break;
      default:        Docs.current()->insertRawComponent((Component*)pe);
                      ((Component*)pe)->entireBounds(x1,y1,x2,y2);
                      enlargeView(x1, y1, x2, y2);
                      break;
    }
    pe->isSelected = false;
  }
  
  movingElements.clear();

  MouseMoveAction = &QucsView::MouseDoNothing;
  MousePressAction = &QucsView::MPressSelect;
  MouseReleaseAction = &QucsView::MReleaseSelect;
  MouseDoubleClickAction = &QucsView::MDoubleClickSelect;

  drawn = false;
  viewport()->repaint();
}

// -----------------------------------------------------------
void QucsView::MReleasePaste(QMouseEvent *)
{
  int x1, y1, x2, y2;
  // insert all moved elements into document
  for(Element *pe = movingElements.first(); pe!=0; pe = movingElements.next()) {
    switch(pe->Type) {
      case isWire:    if(pe->x1 == pe->x2) if(pe->y1 == pe->y2) break;
                      Docs.current()->insertWire((Wire*)pe);
                      enlargeView(pe->x1, pe->y1, pe->x2, pe->y2);
                      break;
      case isDiagram: Docs.current()->Diags.append((Diagram*)pe);
                      enlargeView(pe->cx, pe->cy-pe->y2, pe->cx+pe->x2, pe->cy);
                      Docs.current()->setChanged(true);
                      break;
      default:        Docs.current()->insertComponent((Component*)pe);
                      ((Component*)pe)->entireBounds(x1,y1,x2,y2);
                      enlargeView(x1, y1, x2, y2);
                      break;
    }
    pe->isSelected = false;
  }

  pasteElements();

  MouseMoveAction = &QucsView::MMovePaste;
  MousePressAction = &QucsView::MouseDoNothing;
  MouseReleaseAction = &QucsView::MouseDoNothing;
  MouseDoubleClickAction = &QucsView::MouseDoNothing;

  drawn = false;
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
  if(focusElement == 0) {  // if no component was double clicked, try the wires
    MPressLabel(Event);
    return;
  }
  
  if(focusElement->Type == isComponent) {
    Component *c = (Component*)focusElement;
    if(c->Sign == "GND") return;
    ComponentDialog *d = new ComponentDialog(c, this);
    if(d->exec() == 1)
      Docs.current()->setChanged(true);
    viewport()->repaint();
    return;
  }

  if(focusElement->Type == isDiagram) {
    Diagram *dia = (Diagram*)focusElement;
    DiagramDialog *ddia = new DiagramDialog(dia, Docs.current()->DataSet, this);
    if(ddia->exec() == 1)
      Docs.current()->setChanged(true);
    viewport()->repaint();
    return;
  }
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
