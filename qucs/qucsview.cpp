/***************************************************************************
                          qucsview.cpp  -  description
                             -------------------
    begin                : Thu Aug 28 18:17:41 CEST 2003
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

#include "main.h"
#include "qucsview.h"
#include "components/componentdialog.h"
#include "diagrams/diagramdialog.h"
#include "diagrams/markerdialog.h"

#include <qinputdialog.h>
#include <qclipboard.h>
#include <qapplication.h>
#include <qmessagebox.h>

#include <limits.h>
#include <stdlib.h>


QucsView::QucsView(QWidget *parent) : QScrollView(parent)
{
  setVScrollBarMode(QScrollView::AlwaysOn);
  setHScrollBarMode(QScrollView::AlwaysOn);
  viewport()->setPaletteBackgroundColor(QucsSettings.BGColor);
  viewport()->setMouseTracking(true);

  MouseMoveAction = &QucsView::MouseDoNothing;
  MousePressAction = &QucsView::MouseDoNothing;
  MouseReleaseAction = &QucsView::MouseDoNothing;
  MouseDoubleClickAction = &QucsView::MouseDoNothing;

  selComp  = 0;  // no component is selected
  selDiag  = 0;  // no diagram is selected
  selPaint = 0;  // no painting is selected
  isMoveEqual = false;  // mouse cursor move x and y the same way

  Docs.setAutoDelete(true);

  connect(horizontalScrollBar(), SIGNAL(prevLine()),
                                 SLOT(slotScrollLeft()));
  connect(horizontalScrollBar(), SIGNAL(nextLine()),
                                 SLOT(slotScrollRight()));
  connect(verticalScrollBar(), SIGNAL(prevLine()), SLOT(slotScrollUp()));
  connect(verticalScrollBar(), SIGNAL(nextLine()), SLOT(slotScrollDown()));

  // .......................................................................
  // to repair some strange  scrolling artefacts
  connect(this, SIGNAL(horizontalSliderReleased()),
          viewport(), SLOT(repaint()));
  connect(this, SIGNAL(verticalSliderReleased()),
          viewport(), SLOT(repaint()));
}


QucsView::~QucsView()
{
}

// -----------------------------------------------------------
// this function is called when the content (schematic or data display) has to be draw
void QucsView::drawContents(QPainter *p, int, int, int, int)
{
  Docs.current()->paintGrid(p, contentsX(), contentsY(),
                               visibleWidth(), visibleHeight());

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
// Enlarge the viewport area if the coordinates x1-x2/y1-y2 exceed the
// visible area.
void QucsView::enlargeView(int x1, int y1, int x2, int y2)
{
  int dx=0, dy=0;
  QucsDoc *d = Docs.current();

  if(x1 < d->UsedX1) d->UsedX1 = x1;
  if(y1 < d->UsedY1) d->UsedY1 = y1;
  if(x2 > d->UsedX2) d->UsedX2 = x2;
  if(y2 > d->UsedY2) d->UsedY2 = y2;

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
  p->setPen(Qt::DotLine);
  p->setRasterOp(Qt::NotROP);  // background should not be erased
}

// -----------------------------------------------------------
void QucsView::slotMarkerLeft()
{
  if(Docs.current()->MarkerLeftRight(true)) {
    viewport()->repaint();
    drawn = false;
  }
}

void QucsView::slotMarkerRight()
{
  if(Docs.current()->MarkerLeftRight(false)) {
    viewport()->repaint();
    drawn = false;
  }
}

void QucsView::slotMarkerUp()
{
  if(Docs.current()->MarkerUpDown(true)) {
    viewport()->repaint();
    drawn = false;
  }
}

void QucsView::slotMarkerDown()
{
  if(Docs.current()->MarkerUpDown(false)) {
    viewport()->repaint();
    drawn = false;
  }
}

// -----------------------------------------------------------
void QucsView::editLabel(WireLabel *pl)
{
  bool OK;
  QString Name = QInputDialog::getText(tr("Insert Nodename"),
	tr("Enter the label:"), QLineEdit::Normal, pl->Name, &OK, this);
  if(!OK) return;

  if(Name.isEmpty()) { // if no name was entered (empty string), delete label
    if(pl->pWire) pl->pWire->setName("");   // delete name of wire
    else pl->pNode->setName("");
  }
  else {
    Name.replace(' ', '_');	// label must not contain spaces
    while(Name.at(0) == '_') Name.remove(0,1);  // must not start with '_'
    if(Name.isEmpty()) return;
    if(Name == pl->Name) return;

    int old_x2 = pl->x2;
    pl->setName(Name);   // set new name
    if(pl->cx > (pl->x1+(pl->x2>>1)))
      pl->x1 -= pl->x2 - old_x2; // don't change position due to text width
  }

  QucsDoc *d = Docs.current();
  d->sizeOfAll(d->UsedX1, d->UsedY1, d->UsedX2, d->UsedY2);
  viewport()->repaint();
  drawn = false;
  Docs.current()->setChanged(true, true);
}

// -----------------------------------------------------------
// Reinserts all elements (moved by the user) back into the schematic.
void QucsView::endElementMoving()
{
  Element *pe;
  QucsDoc *d = Docs.current();

  for(pe = movingElements.first(); pe!=0; pe = movingElements.next()) {
    pe->isSelected = false;  // deselect first (maybe afterwards pe == NULL)
    switch(pe->Type) {
      case isWire:
	if(pe->x1 == pe->x2) if(pe->y1 == pe->y2) break;
	d->insertWire((Wire*)pe);
	break;
      case isDiagram:
	d->Diags.append((Diagram*)pe);
	break;
      case isPainting:
	d->Paints.append((Painting*)pe);
	break;
      case isComponent:
	d->insertRawComponent((Component*)pe);
	break;
      case isMovingLabel:
	d->insertNodeLabel((WireLabel*)pe);
	break;
      case isMarker:
	((Marker*)pe)->Diag->Markers.append((Marker*)pe);
	break;
      default: ;
    }
  }

  movingElements.clear();
  d->setChanged(true, true);

  // enlarge viewarea if components lie outside the view
  d->sizeOfAll(d->UsedX1, d->UsedY1, d->UsedX2, d->UsedY2);
  enlargeView(d->UsedX1, d->UsedY1, d->UsedX2, d->UsedY2);
  viewport()->repaint();
  drawn = false;
}

// -----------------------------------------------------------
void QucsView::MouseDoNothing(QMouseEvent*)
{
}

// ***********************************************************************
// **********                                                   **********
// **********       Functions for serving mouse moving          **********
// **********                                                   **********
// ***********************************************************************
void QucsView::contentsMouseMoveEvent(QMouseEvent *Event)
{
  (this->*MouseMoveAction)(Event);
}

// -----------------------------------------------------------
void QucsView::MMovePainting(QMouseEvent *Event)
{
  QucsDoc *d = Docs.current();
  QPainter painter(viewport());
  setPainter(&painter, d);

  int x  = int(Event->pos().x()/d->Scale) + d->ViewX1;
  int y  = int(Event->pos().y()/d->Scale) + d->ViewY1;
  int gx = x;
  int gy = y;
  d->setOnGrid(gx, gy);

  selPaint->MouseMoving(x, y, gx, gy, &painter, drawn);
  drawn = true;
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
// Paints a rectangle to select elements within it.
void QucsView::MMoveSelect(QMouseEvent *Event)
{
  QucsDoc *d = Docs.current();
  QPainter painter(viewport());
  setPainter(&painter, d);

  if(drawn) painter.drawRect(MAx1, MAy1, MAx2, MAy2);   // erase old rectangle
  drawn = true;
  MAx2 = int(Event->pos().x()/d->Scale) + d->ViewX1 - MAx1;
  MAy2 = int(Event->pos().y()/d->Scale) + d->ViewY1 - MAy1;
  if(isMoveEqual)     // x and y size must be equal ?
    if(abs(MAx2) > abs(MAy2)) { if(MAx2<0) MAx2 = -abs(MAy2); else MAx2 = abs(MAy2); }
    else { if(MAy2<0) MAy2 = -abs(MAx2); else MAy2 = abs(MAx2); }
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
  this->grabKeyboard();      // hinders keyboard inputs during moving
  d->copySelectedElements(&movingElements);
  viewport()->repaint();

  Wire *pw;
  // Changes the position of all moving elements by dx/dy
  for(Element *pe = movingElements.first(); pe != 0; pe = movingElements.next()) {
    if(pe->Type == isWire) {
      pw = (Wire*)pe;   // connecting wires are not moved completely

      if(int(pw->Port1) > 3) { pw->x1 += MAx1;  pw->y1 += MAy1; }
      else {  if(int(pw->Port1) & 1) { pw->x1 += MAx1; }
              if(int(pw->Port1) & 2) { pw->y1 += MAy1; } }

      if(int(pw->Port2) > 3) { pw->x2 += MAx1;  pw->y2 += MAy1; }
      else {  if(int(pw->Port2) & 1) pw->x2 += MAx1;
              if(int(pw->Port2) & 2) pw->y2 += MAy1; }

      if(pw->Label) {      // root of node label must lie on wire
        if(pw->Label->cx < pw->x1) pw->Label->cx = pw->x1;
        if(pw->Label->cy < pw->y1) pw->Label->cy = pw->y1;
        if(pw->Label->cx > pw->x2) pw->Label->cx = pw->x2;
        if(pw->Label->cy > pw->y2) pw->Label->cy = pw->y2;
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

  Element *pe;
  if(drawn) // erase old scheme
    for(pe = movingElements.first(); pe != 0; pe = movingElements.next())
      pe->paintScheme(&painter);

  drawn = true;
  d->setOnGrid(MAx2, MAy2);
  MAx1 = MAx2 - MAx1;
  MAy1 = MAy2 - MAy1;

  Wire *pw;
  for(pe = movingElements.first(); pe != 0; pe = movingElements.next()) {
    if(pe->Type == isWire) {
      pw = (Wire*)pe;   // connecting wires are not moved completely

      if(int(pw->Port1) > 3) {
	pw->x1 += MAx1;  pw->y1 += MAy1;
	if(pw->Label) { pw->Label->cx += MAx1;  pw->Label->cy += MAy1; }
      }
      else {  if(int(pw->Port1) & 1) { pw->x1 += MAx1; }
              if(int(pw->Port1) & 2) { pw->y1 += MAy1; } }

      if(int(pw->Port2) > 3) { pw->x2 += MAx1;  pw->y2 += MAy1; }
      else {  if(int(pw->Port2) & 1) pw->x2 += MAx1;
              if(int(pw->Port2) & 2) pw->y2 += MAy1; }

      if(pw->Label) {      // root of node label must lie on wire
        if(pw->Label->cx < pw->x1) pw->Label->cx = pw->x1;
        if(pw->Label->cy < pw->y1) pw->Label->cy = pw->y1;
        if(pw->Label->cx > pw->x2) pw->Label->cx = pw->x2;
        if(pw->Label->cy > pw->y2) pw->Label->cy = pw->y2;
      }

    }
    else pe->setCenter(MAx1, MAy1, true);
  }

  // paint afterwards to avoid conflict between wire and label painting
  for(pe = movingElements.first(); pe != 0; pe = movingElements.next())
    pe->paintScheme(&painter);

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

// -----------------------------------------------------------
// Paints a cross under the mouse cursor to show the delete modus.
void QucsView::MMoveDelete(QMouseEvent *Event)
{
  QucsDoc *d = Docs.current();
  QPainter painter(viewport());
  painter.translate(-contentsX(), -contentsY());  // contents to viewport transformation
  painter.scale(d->Scale, d->Scale);
  painter.translate(-d->ViewX1, -d->ViewY1);
  painter.setRasterOp(Qt::NotROP);  // background should not be erased

  if(drawn) {
    painter.drawLine(MAx3-15, MAy3-15, MAx3+15, MAy3+15); // erase old
    painter.drawLine(MAx3-15, MAy3+15, MAx3+15, MAy3-15);
  }
  drawn = true;

  MAx3  = int(Event->pos().x()/d->Scale) + d->ViewX1;
  MAy3  = int(Event->pos().y()/d->Scale) + d->ViewY1;

  painter.drawLine(MAx3-15, MAy3-15, MAx3+15, MAy3+15); // paint
  painter.drawLine(MAx3-15, MAy3+15, MAx3+15, MAy3-15);
}

// -----------------------------------------------------------
// Paints a label above the mouse cursor to show the set wire label modus.
void QucsView::MMoveLabel(QMouseEvent *Event)
{
  QucsDoc *d = Docs.current();
  QPainter painter(viewport());
  painter.translate(-contentsX(), -contentsY());  // contents to viewport transformation
  painter.scale(d->Scale, d->Scale);
  painter.translate(-d->ViewX1, -d->ViewY1);
  painter.setRasterOp(Qt::NotROP);  // background should not be erased

  if(drawn) {
    painter.drawLine(MAx3, MAy3, MAx3+10, MAy3-10); // erase old
    painter.drawLine(MAx3+10, MAy3-10, MAx3+20, MAy3-10);
    painter.drawLine(MAx3+10, MAy3-10, MAx3+10, MAy3-17);

    painter.drawLine(MAx3+12, MAy3-12, MAx3+15, MAy3-23);   // "A"
    painter.drawLine(MAx3+14, MAy3-17, MAx3+17, MAy3-17);
    painter.drawLine(MAx3+19, MAy3-12, MAx3+16, MAy3-23);
  }
  drawn = true;

  MAx3  = int(Event->pos().x()/d->Scale) + d->ViewX1;
  MAy3  = int(Event->pos().y()/d->Scale) + d->ViewY1;

  painter.drawLine(MAx3, MAy3, MAx3+10, MAy3-10); // paint new
  painter.drawLine(MAx3+10, MAy3-10, MAx3+20, MAy3-10);
  painter.drawLine(MAx3+10, MAy3-10, MAx3+10, MAy3-17);

  painter.drawLine(MAx3+12, MAy3-12, MAx3+15, MAy3-23);   // "A"
  painter.drawLine(MAx3+14, MAy3-17, MAx3+17, MAy3-17);
  painter.drawLine(MAx3+19, MAy3-12, MAx3+16, MAy3-23);
}

// -----------------------------------------------------------
// Paints a little triangle above the mouse cursor to show the set marker modus.
void QucsView::MMoveMarker(QMouseEvent *Event)
{
  QucsDoc *d = Docs.current();
  QPainter painter(viewport());
  painter.translate(-contentsX(), -contentsY());  // contents to viewport transformation
  painter.scale(d->Scale, d->Scale);
  painter.translate(-d->ViewX1, -d->ViewY1);
  painter.setRasterOp(Qt::NotROP);  // background should not be erased

  if(drawn) {
    painter.drawLine(MAx3, MAy3-2, MAx3-8, MAy3-10); // erase old
    painter.drawLine(MAx3+1, MAy3-3, MAx3+8, MAy3-10);
    painter.drawLine(MAx3-7, MAy3-10, MAx3+7, MAy3-10);
  }
  drawn = true;

  MAx3  = int(Event->pos().x()/d->Scale) + d->ViewX1;
  MAy3  = int(Event->pos().y()/d->Scale) + d->ViewY1;

  painter.drawLine(MAx3, MAy3-2, MAx3-8, MAy3-10); // paint new
  painter.drawLine(MAx3+1, MAy3-3, MAx3+8, MAy3-10);
  painter.drawLine(MAx3-7, MAy3-10, MAx3+7, MAy3-10);
}

// -----------------------------------------------------------
// Paints rounded arrows above the mouse cursor to show the "mirror about y axis" modus.
void QucsView::MMoveMirrorY(QMouseEvent *Event)
{
  QucsDoc *d = Docs.current();
  QPainter painter(viewport());
  painter.translate(-contentsX(), -contentsY());  // contents to viewport transformation
  painter.scale(d->Scale, d->Scale);
  painter.translate(-d->ViewX1, -d->ViewY1);
  painter.setRasterOp(Qt::NotROP);  // background should not be erased

  if(drawn) {
    painter.drawLine(MAx3-11, MAy3-4, MAx3-9, MAy3-9); // erase old
    painter.drawLine(MAx3-11, MAy3-3, MAx3-6, MAy3-3);
    painter.drawLine(MAx3+11, MAy3-4, MAx3+9, MAy3-9);
    painter.drawLine(MAx3+11, MAy3-3, MAx3+6, MAy3-3);
    painter.drawArc(MAx3-10, MAy3-8, 21, 10, 16*20, 16*140);
  }
  drawn = true;

  MAx3  = int(Event->pos().x()/d->Scale) + d->ViewX1;
  MAy3  = int(Event->pos().y()/d->Scale) + d->ViewY1;

  painter.drawLine(MAx3-11, MAy3-4, MAx3-9, MAy3-9); // paint new
  painter.drawLine(MAx3-11, MAy3-3, MAx3-6, MAy3-3);
  painter.drawLine(MAx3+11, MAy3-4, MAx3+9, MAy3-9);
  painter.drawLine(MAx3+11, MAy3-3, MAx3+6, MAy3-3);
  painter.drawArc(MAx3-10, MAy3-8, 21, 10, 16*20, 16*140);
}

// -----------------------------------------------------------
// Paints rounded arrows beside the mouse cursor to show the "mirror about x axis" modus.
void QucsView::MMoveMirrorX(QMouseEvent *Event)
{
  QucsDoc *d = Docs.current();
  QPainter painter(viewport());
  painter.translate(-contentsX(), -contentsY());  // contents to viewport transformation
  painter.scale(d->Scale, d->Scale);
  painter.translate(-d->ViewX1, -d->ViewY1);
  painter.setRasterOp(Qt::NotROP);  // background should not be erased

  if(drawn) {
    painter.drawLine(MAx3-4, MAy3-11, MAx3-9, MAy3-9); // erase old
    painter.drawLine(MAx3-3, MAy3-11, MAx3-3, MAy3-6);
    painter.drawLine(MAx3-4, MAy3+11, MAx3-9, MAy3+9);
    painter.drawLine(MAx3-3, MAy3+11, MAx3-3, MAy3+6);
    painter.drawArc(MAx3-8, MAy3-10, 10, 21, 16*110, 16*140);
  }
  drawn = true;

  MAx3  = int(Event->pos().x()/d->Scale) + d->ViewX1;
  MAy3  = int(Event->pos().y()/d->Scale) + d->ViewY1;

  painter.drawLine(MAx3-4, MAy3-11, MAx3-9, MAy3-9); // paint new
  painter.drawLine(MAx3-3, MAy3-11, MAx3-3, MAy3-6);
  painter.drawLine(MAx3-4, MAy3+11, MAx3-9, MAy3+9);
  painter.drawLine(MAx3-3, MAy3+11, MAx3-3, MAy3+6);
  painter.drawArc(MAx3-8, MAy3-10, 10, 21, 16*110, 16*140);
}

// -----------------------------------------------------------
// Paints a rounded arrow above the mouse cursor to show the "rotate" modus.
void QucsView::MMoveRotate(QMouseEvent *Event)
{
  QucsDoc *d = Docs.current();
  QPainter painter(viewport());
  painter.translate(-contentsX(), -contentsY());  // contents to viewport transformation
  painter.scale(d->Scale, d->Scale);
  painter.translate(-d->ViewX1, -d->ViewY1);
  painter.setRasterOp(Qt::NotROP);  // background should not be erased

  if(drawn) {
    painter.drawLine(MAx3-6, MAy3+8, MAx3-6, MAy3+1); // erase old
    painter.drawLine(MAx3-7, MAy3+8, MAx3-12, MAy3+8);
    painter.drawArc(MAx3-10, MAy3-10, 21, 21, -16*20, 16*240);
  }
  drawn = true;

  MAx3  = int(Event->pos().x()/d->Scale) + d->ViewX1;
  MAy3  = int(Event->pos().y()/d->Scale) + d->ViewY1;

  painter.drawLine(MAx3-6, MAy3+8, MAx3-6, MAy3+1); // paint new
  painter.drawLine(MAx3-7, MAy3+8, MAx3-12, MAy3+8);
  painter.drawArc(MAx3-10, MAy3-10, 21, 21, -16*20, 16*240);
}

// -----------------------------------------------------------
// Paints a a rectangle beside the mouse cursor to show the "activate" modus.
void QucsView::MMoveActivate(QMouseEvent *Event)
{
  QucsDoc *d = Docs.current();
  QPainter painter(viewport());
  painter.translate(-contentsX(), -contentsY());  // contents to viewport transformation
  painter.scale(d->Scale, d->Scale);
  painter.translate(-d->ViewX1, -d->ViewY1);
  painter.setRasterOp(Qt::NotROP);  // background should not be erased

  if(drawn) {
    painter.drawLine(MAx3, MAy3-9, MAx3+13, MAy3-9); // erase old
    painter.drawLine(MAx3, MAy3, MAx3+13, MAy3);
    painter.drawLine(MAx3, MAy3-9, MAx3, MAy3);
    painter.drawLine(MAx3+13, MAy3-9, MAx3+13, MAy3);
    painter.drawLine(MAx3, MAy3-9, MAx3+13, MAy3);
    painter.drawLine(MAx3, MAy3, MAx3+13, MAy3-9);
  }
  drawn = true;

  MAx3  = int(Event->pos().x()/d->Scale) + d->ViewX1;
  MAy3  = int(Event->pos().y()/d->Scale) + d->ViewY1;

    painter.drawLine(MAx3, MAy3-9, MAx3+13, MAy3-9); // erase old
    painter.drawLine(MAx3, MAy3, MAx3+13, MAy3);
    painter.drawLine(MAx3, MAy3-9, MAx3, MAy3);
    painter.drawLine(MAx3+13, MAy3-9, MAx3+13, MAy3);
    painter.drawLine(MAx3, MAy3-9, MAx3+13, MAy3);
    painter.drawLine(MAx3, MAy3, MAx3+13, MAy3-9);
}

// ************************************************************************
// **********                                                    **********
// **********    Functions for serving mouse button clicking     **********
// **********                                                    **********
// ************************************************************************
void QucsView::contentsMousePressEvent(QMouseEvent *Event)
{
  (this->*MousePressAction)(Event);
}

// -----------------------------------------------------------
void QucsView::MPressPainting(QMouseEvent *)
{
  if(selPaint->MousePressing()) {
    Docs.current()->Paints.append(selPaint);
    selPaint = selPaint->newOne();

    drawn = false;
    viewport()->repaint();
    Docs.current()->setChanged(true, true);
  }
}

// -----------------------------------------------------------
void QucsView::MPressLabel(QMouseEvent *Event)
{
  QucsDoc *d = Docs.current();
  int x = int(Event->pos().x()/d->Scale) + d->ViewX1;
  int y = int(Event->pos().y()/d->Scale) + d->ViewY1;
  Wire *pw = 0;
  Node *pn = d->selectedNode(x, y);
  if(!pn) {
    pw = d->selectedWire(x, y);
    if(!pw) return;
  }

  QString Name;
  Element *pe=0;
  // is wire line already labeled ?
  if(pw) pe = d->getWireLabel(pw->Port1);
  else pe = d->getWireLabel(pn);
  if(pe) {
    if(pe->Type == isComponent) {
      QMessageBox::information(0, tr("Info"),
			tr("The ground potential cannot be labeled!"));
      return;
    }
    if(pe->Type == isNode) Name = ((Node*)pe)->Label->Name;
    else Name = ((Wire*)pe)->Label->Name;
  }

  bool OK;
  Name = QInputDialog::getText(tr("Insert Nodename"),
		tr("Enter the label:"), QLineEdit::Normal, Name, &OK, this);
  if(!OK) return;

  if(!Name.isEmpty()) {
    Name.replace(' ', '_');	// label must not contain spaces
    while(Name.at(0) == '_') Name.remove(0,1);  // must not start with '_'
    if(Name.isEmpty()) return;

    if(pe) {
      if(pe->Type == isWire) ((Wire*)pe)->setName("");  // delete old name
      else ((Node*)pe)->setName("");
    }

    // set new name
    if(pw) pw->setName(Name, x-pw->x1 + y-pw->y1, x+30, y-30);
    else pn->setName(Name, x+30, y-30);
  }
  else {    // if no name was entered (empty string), delete name
    if(pe) {
      if(pe->Type == isWire) ((Wire*)pe)->setName("");  // delete old name
      else ((Node*)pe)->setName("");
    }
    else {
      if(pw) pw->setName("");   // delete name of wire
      else pn->setName("");
    }
  }

  d->sizeOfAll(d->UsedX1, d->UsedY1, d->UsedX2, d->UsedY2);
  viewport()->repaint();
  drawn = false;
  Docs.current()->setChanged(true, true);
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
  viewport()->repaint();
  if(Event->button() != Qt::LeftButton) return;
  if(focusElement == 0) {
    MAx2 = 0;  // if not clicking on an element => open a rectangle
    MAy2 = 0;
    MouseReleaseAction = &QucsView::MReleaseSelect2;
    MouseMoveAction = &QucsView::MMoveSelect;
    MousePressAction = &QucsView::MouseDoNothing;
    MouseDoubleClickAction = &QucsView::MouseDoNothing;
  }
  else {  // element could be moved

    if(focusElement->Type == isDiagram)   // resize diagram ?
      if(((Diagram*)focusElement)->ResizeTouched(MAx1, MAy1, MAx2, MAy2)) {
	if(((Diagram*)focusElement)->Name == "Polar")
	  isMoveEqual = true;  // diagram must be square
	else  if(((Diagram*)focusElement)->Name == "Smith")
		isMoveEqual = true;  // diagram must be square

	Docs.current()->setOnGrid(MAx1, MAy1);
	MouseReleaseAction = &QucsView::MReleaseResizeDiagram;
	MouseMoveAction = &QucsView::MMoveSelect;
	MousePressAction = &QucsView::MouseDoNothing;
	MouseDoubleClickAction = &QucsView::MouseDoNothing;
	return;
      }

    Docs.current()->setOnGrid(MAx1, MAy1);
    MouseMoveAction = &QucsView::MMoveMoving;
    MousePressAction = &QucsView::MouseDoNothing;
    MouseDoubleClickAction = &QucsView::MouseDoNothing;
  }
}

// -----------------------------------------------------------
void QucsView::MPressDelete(QMouseEvent *Event)
{
  QucsDoc *d = Docs.current();

  Element *pe = d->selectElement(int(Event->pos().x()/d->Scale)+d->ViewX1,
				 int(Event->pos().y()/d->Scale)+d->ViewY1,
				 false);
  if(pe) {
    pe->isSelected = true;
    Docs.current()->deleteElements();

    d->sizeOfAll(d->UsedX1, d->UsedY1, d->UsedX2, d->UsedY2);
    viewport()->repaint();
    drawn = false;
  }
}

// -----------------------------------------------------------
void QucsView::MPressActivate(QMouseEvent *Event)
{
  MAx1 = int(Event->pos().x()/Docs.current()->Scale) + Docs.current()->ViewX1;
  MAy1 = int(Event->pos().y()/Docs.current()->Scale) + Docs.current()->ViewY1;
  if(!Docs.current()->activateComponent(MAx1, MAy1)) {
    if(Event->button() != Qt::LeftButton) return;
    MAx2 = 0;  // if not clicking on a component => open a rectangle
    MAy2 = 0;
    MousePressAction = &QucsView::MouseDoNothing;
    MouseReleaseAction = &QucsView::MReleaseActivate;
    MouseMoveAction = &QucsView::MMoveSelect;
  }
  viewport()->repaint();
  drawn = false;
}

// -----------------------------------------------------------
void QucsView::MPressMirrorX(QMouseEvent *Event)
{
  int x = int(Event->pos().x()/Docs.current()->Scale)+Docs.current()->ViewX1;
  int y = int(Event->pos().y()/Docs.current()->Scale)+Docs.current()->ViewY1;

  // no use in mirroring wires or diagrams
  Component *c = Docs.current()->selectedComponent(x, y);
  if(c) {
    if(c->Ports.count() < 1) return;  // do not mirror components without ports
    c->mirrorX();
    Docs.current()->setCompPorts(c);
  }
  else {
    Painting *p = Docs.current()->selectedPainting(x, y);
    if(p == 0) return;
    p->mirrorX();
  }

  viewport()->repaint();
  drawn = false;
  Docs.current()->setChanged(true, true);
}

// -----------------------------------------------------------
void QucsView::MPressMirrorY(QMouseEvent *Event)
{
  int x = int(Event->pos().x()/Docs.current()->Scale)+Docs.current()->ViewX1;
  int y = int(Event->pos().y()/Docs.current()->Scale)+Docs.current()->ViewY1;

  // no use in mirroring wires or diagrams
  Component *c = Docs.current()->selectedComponent(x, y);
  if(c) {
    if(c->Ports.count() < 1) return;  // do not mirror components without ports
    c->mirrorY();
    Docs.current()->setCompPorts(c);
  }
  else {
    Painting *p = Docs.current()->selectedPainting(x, y);
    if(p == 0) return;
    p->mirrorY();
  }

  viewport()->repaint();
  drawn = false;
  Docs.current()->setChanged(true, true);
}

// -----------------------------------------------------------
void QucsView::MPressRotate(QMouseEvent *Event)
{
  QucsDoc *d = Docs.current();

  Element *e = d->selectElement(
	int(Event->pos().x()/Docs.current()->Scale)+Docs.current()->ViewX1,
	int(Event->pos().y()/Docs.current()->Scale)+Docs.current()->ViewY1,
	false);
  if(e == 0) return;

  WireLabel *pl;
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
    case isWire:      pl = ((Wire*)e)->Label;
                      ((Wire*)e)->Label = 0;    // prevent label to be deleted
                      d->Wires.setAutoDelete(false);
                      d->deleteWire((Wire*)e);
                      ((Wire*)e)->Label = pl;
                      ((Wire*)e)->rotate();
                      d->setOnGrid(e->x1, e->y1);
                      d->setOnGrid(e->x2, e->y2);
                      if(pl) d->setOnGrid(pl->cx, pl->cy);
                      d->insertWire((Wire*)e);
                      d->Wires.setAutoDelete(true);
                      if (d->Wires.containsRef ((Wire*)e))
			enlargeView(e->x1, e->y1, e->x2, e->y2);
                      break;
    case isPainting:  ((Painting*)e)->rotate();
                      // enlarge viewarea if component lies outside the view
                      ((Painting*)e)->Bounding(x1,y1,x2,y2);
                      enlargeView(x1, y1, x2, y2);
                      break;
    default:          return;
  }
  viewport()->repaint();
  drawn = false;
  Docs.current()->setChanged(true, true);
}

// -----------------------------------------------------------
void QucsView::MPressComponent(QMouseEvent *Event)
{
  QucsDoc *d = Docs.current();
  QPainter painter(viewport());
  setPainter(&painter, d);

  int x1, y1, x2, y2;
  switch(Event->button()) {
  case Qt::LeftButton :
    // left mouse button inserts component into the schematic
    if(selComp == 0) break;
    d->insertComponent(selComp);

    // enlarge viewarea if component lies outside the view
    selComp->entireBounds(x1,y1,x2,y2);
    enlargeView(x1, y1, x2, y2);

    viewport()->repaint();
    d->setChanged(true, true);
    selComp = selComp->newOne(); // component is used, so create a new one
    selComp->paintScheme(&painter);
    break;

  case Qt::RightButton :  // right mouse button rotates the component
    if(selComp == 0) break;
    if(selComp->Ports.count() == 0)
      break;  // do not rotate components without ports
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
  if(Event->button() != Qt::LeftButton) return;
  
  // dialog is Qt::WDestructiveClose !!!
  DiagramDialog *dia = new DiagramDialog(selDiag, d->DataSet, this);
  if(dia->exec() == QDialog::Rejected) {  // don't insert if dialog canceled
    viewport()->repaint();
    drawn = false;
    return;
  }

  d->Diags.append(selDiag);
  enlargeView(selDiag->cx, selDiag->cy-selDiag->y2,
	      selDiag->cx+selDiag->x2, selDiag->cy);
  d->setChanged(true, true);   // document has been changed

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

  int set = 0;
  switch(Event->button()) {
  case Qt::LeftButton :
    if(MAx1 == 0) { // which wire direction first ?
      if(MAx2 != MAx3) {
	set = d->insertWire(new Wire(MAx3, MAy2, MAx2, MAy2));
	if(set & 2) {
	  // if last port is connected, then start a new wire
	  MouseMoveAction = &QucsView::MMoveWire1;
	  MousePressAction = &QucsView::MPressWire1;
	  MouseDoubleClickAction = &QucsView::MouseDoNothing;
	}
	if(MAy2 != MAy3)
	  set |= d->insertWire(new Wire(MAx3, MAy3, MAx3, MAy2));
      }
      else if(MAy2 != MAy3) {
	set = d->insertWire(new Wire(MAx3, MAy3, MAx3, MAy2));
	if(set & 2) {
	  // if last port is connected, then start a new wire
	  MouseMoveAction = &QucsView::MMoveWire1;
	  MousePressAction = &QucsView::MPressWire1;
	  MouseDoubleClickAction = &QucsView::MouseDoNothing;
	} }
    }
    else {
      if(MAy2 != MAy3) {
	set = d->insertWire(new Wire(MAx2, MAy3, MAx2, MAy2));
	if(set & 2) {
	  // if last port is connected, then start a new wire
	  MouseMoveAction = &QucsView::MMoveWire1;
	  MousePressAction = &QucsView::MPressWire1;
	  MouseDoubleClickAction = &QucsView::MouseDoNothing;
	}
	if(MAx2 != MAx3)
	  set |= d->insertWire(new Wire(MAx3, MAy3, MAx2, MAy3));
      }
      else if(MAx2 != MAx3) {
	set = d->insertWire(new Wire(MAx3, MAy3, MAx2, MAy3));
	if(set & 2) {
	  // if last port is connected, then start a new wire
	  MouseMoveAction = &QucsView::MMoveWire1;
	  MousePressAction = &QucsView::MPressWire1;
	  MouseDoubleClickAction = &QucsView::MouseDoNothing;
	} }
    }
    viewport()->repaint();
    drawn = false;
    if(set) d->setChanged(true, true);
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

// -----------------------------------------------------------
// Is called for setting a marker on a diagram's graph
void QucsView::MPressMarker(QMouseEvent *Event)
{
  MAx1 = int(double(Event->pos().x())/Docs.current()->Scale)
		+Docs.current()->ViewX1;
  MAy1 = int(double(Event->pos().y())/Docs.current()->Scale)
		+Docs.current()->ViewY1;
  Marker *pm = Docs.current()->setMarker(MAx1, MAy1);

  if(pm) {
    int x0 = pm->Diag->cx;
    int y0 = pm->Diag->cy;
    enlargeView(x0+pm->x1, y0-pm->y1-pm->y2, x0+pm->x1+pm->x2, y0-pm->y1);
  }
  viewport()->repaint();
  drawn = false;
}


// ***********************************************************************
// **********                                                   **********
// **********    Functions for serving mouse button releasing   **********
// **********                                                   **********
// ***********************************************************************
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
  drawn = false;
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
  
   // selects all elements within the rectangle
  d->selectElements(MAx1, MAy1, MAx1+MAx2, MAy1+MAy2, Ctrl);

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

  MouseMoveAction = &QucsView::MMoveActivate;
  MousePressAction = &QucsView::MPressActivate;
  MouseReleaseAction = &QucsView::MouseDoNothing;
  MouseDoubleClickAction = &QucsView::MouseDoNothing;
  viewport()->repaint();
}

// -----------------------------------------------------------
void QucsView::MReleaseMoving(QMouseEvent *Event)
{
  if(Event->button() != Qt::LeftButton) return;
  endElementMoving();
  this->releaseKeyboard();      // allow keyboard inputs again

  MouseMoveAction = &QucsView::MouseDoNothing;
  MousePressAction = &QucsView::MPressSelect;
  MouseReleaseAction = &QucsView::MReleaseSelect;
  MouseDoubleClickAction = &QucsView::MDoubleClickSelect;
}

// -----------------------------------------------------------
void QucsView::MReleaseResizeDiagram(QMouseEvent *Event)
{
  if(Event->button() != Qt::LeftButton) return;

  MAx3  = focusElement->cx;
  MAy3  = focusElement->cy;
  if(MAx2 < 0) {    // resize diagram
    if(MAx2 > -10) MAx2 = -10;   // not smaller than 10 pixels
    focusElement->x2 = -MAx2;
    focusElement->cx = MAx1+MAx2;
  }
  else {
    if(MAx2 < 10) MAx2 = 10;
    focusElement->x2 = MAx2;
    focusElement->cx = MAx1;
  }
  if(MAy2 < 0) {
    if(MAy2 > -10) MAy2 = -10;
    focusElement->y2 = -MAy2;
    focusElement->cy = MAy1;
  }
  else {
    if(MAy2 < 10) MAy2 = 10;
    focusElement->y2 = MAy2;
    focusElement->cy = MAy1+MAy2;
  }
  MAx3 -= focusElement->cx;
  MAy3 -= focusElement->cy;

  Diagram *pd = (Diagram*)focusElement;
  pd->updateGraphData();
  for(Marker *pm = pd->Markers.first(); pm!=0; pm = pd->Markers.next()) {
    pm->x1 += MAx3;      // correct changes due to move of diagram corner
    pm->y1 -= MAy3;
  }
  isMoveEqual = false;

  MouseMoveAction = &QucsView::MouseDoNothing;
  MousePressAction = &QucsView::MPressSelect;
  MouseReleaseAction = &QucsView::MReleaseSelect;
  MouseDoubleClickAction = &QucsView::MDoubleClickSelect;

  viewport()->repaint();
  drawn = false;
  Docs.current()->setChanged(true, true);
}

// -----------------------------------------------------------
void QucsView::MReleasePaste(QMouseEvent *Event)
{
  int x1, y1, x2, y2, tmp;
  QucsDoc *d = Docs.current();
  QPainter painter(viewport());

  Element *pe;
  switch(Event->button()) {
  case Qt::LeftButton :
    // insert all moved elements into document
    for(pe = movingElements.first(); pe!=0; pe = movingElements.next()) {
      pe->isSelected = false;
      switch(pe->Type) {
	case isWire:
	  if(pe->x1 == pe->x2) if(pe->y1 == pe->y2) break;
	  d->insertWire((Wire*)pe);
	  if (d->Wires.containsRef ((Wire*)pe))
	    enlargeView(pe->x1, pe->y1, pe->x2, pe->y2);
	  else pe = NULL;
	  break;
	case isDiagram:
	  d->Diags.append((Diagram*)pe);
	  ((Diagram*)pe)->loadGraphData(d->DataSet);
	  enlargeView(pe->cx, pe->cy-pe->y2, pe->cx+pe->x2, pe->cy);
	  break;
	case isPainting:
	  d->Paints.append((Painting*)pe);
	  ((Painting*)pe)->Bounding(x1,y1,x2,y2);
	  enlargeView(x1, y1, x2, y2);
	  break;
	default:
	  d->insertComponent((Component*)pe);
	  ((Component*)pe)->entireBounds(x1,y1,x2,y2);
	  enlargeView(x1, y1, x2, y2);
	  break;
      }
    }

    pasteElements();
    MouseMoveAction = &QucsView::MMovePaste;
    MousePressAction = &QucsView::MouseDoNothing;
    MouseReleaseAction = &QucsView::MouseDoNothing;
    MouseDoubleClickAction = &QucsView::MouseDoNothing;

    drawn = false;
    viewport()->repaint();
    d->setChanged(true, true);
    break;

  // ............................................................
  case Qt::RightButton :  // right button rotates the elements
    setPainter(&painter, d);

    if(drawn) // erase old scheme
      for(pe = movingElements.first(); pe != 0; pe = movingElements.next())
        pe->paintScheme(&painter);
    drawn = true;

    x1  = int(Event->pos().x()/d->Scale) + d->ViewX1;
    y1  = int(Event->pos().y()/d->Scale) + d->ViewY1;
    d->setOnGrid(x1, y1);

    for(pe = movingElements.first(); pe != 0; pe = movingElements.next()) {
      switch(pe->Type) {
        case isComponent:
	  ((Component*)pe)->rotate(); // rotate !before! rotating the center
          x2 = x1 - pe->cx;
          pe->setCenter(pe->cy - y1 + x1, x2 + y1);
          break;
        case isWire:
	  x2    = pe->x1;
          pe->x1 = pe->y1 - y1 + x1;
          pe->y1 = x1 - x2 + y1;
          x2    = pe->x2;
          pe->x2 = pe->y2 - y1 + x1;
          pe->y2 = x1 - x2 + y1;
          break;
        case isPainting:
	  ((Painting*)pe)->rotate();  // rotate !before! rotating the center
          ((Painting*)pe)->getCenter(x2, y2);
          tmp = x1 - x2;
          pe->setCenter(y2 - y1 + x1, tmp + y1);
          break;
        default:
	  x2 = x1 - pe->cx;   // if diagram -> only rotate cx/cy
          pe->setCenter(pe->cy - y1 + x1, x2 + y1);
          break;
      }
      pe->paintScheme(&painter);
    }
    break;

  default: ;    // avoids compiler warnings
  }
}

// ***********************************************************************
// **********                                                   **********
// **********    Functions for mouse button double clicking     **********
// **********                                                   **********
// ***********************************************************************
void QucsView::contentsMouseDoubleClickEvent(QMouseEvent *Event)
{
  (this->*MouseDoubleClickAction)(Event);
}

// -----------------------------------------------------------
void QucsView::MDoubleClickSelect(QMouseEvent *Event)
{
  if(focusElement == 0) return;

  Component *c;
  Diagram *dia;
  ComponentDialog *d;
  DiagramDialog *ddia;
  MarkerDialog *mdia;

  switch(focusElement->Type) {
    case isComponent:
         c = (Component*)focusElement;
         if(c->Sign == "GND") return;
         d = new ComponentDialog(c, this);  // is WDestructiveClose
         if(d->exec() == 1) {
           int x1, y1, x2, y2;
           x2 = Docs.current()->Comps.findRef(c);
           Docs.current()->Comps.take();
           Docs.current()->setComponentNumber(c); // for ports/power sources
           Docs.current()->Comps.append(c);

           Docs.current()->setChanged(true, true);
           c->entireBounds(x1,y1,x2,y2);
           enlargeView(x1,y1,x2,y2);
         }
         break;

    case isDiagram :
         dia = (Diagram*)focusElement;
         ddia = new DiagramDialog(dia, Docs.current()->DataSet, this);
         if(ddia->exec()  != QDialog::Rejected)   // is WDestructiveClose
           Docs.current()->setChanged(true, true);
         break;

    case isWire:
         MPressLabel(Event);
         break;

    case isNodeLabel:
    case isHWireLabel:
    case isVWireLabel:
	 editLabel((WireLabel*)focusElement);
	 break;

    case isPainting:
         if( ((Painting*)focusElement)->Dialog() )
           Docs.current()->setChanged(true, true);
         break;

    case isMarker:
         mdia = new MarkerDialog((Marker*)focusElement, this);
         if(mdia->exec() > 1) Docs.current()->setChanged(true, true);
         break;
    default:  ;
  }

  viewport()->repaint();
  drawn = false;
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

// *************************************************************************
// **********                                                     **********
// **********        Function for serving mouse wheel moving      **********
// **********                                                     **********
// *************************************************************************
void QucsView::contentsWheelEvent(QWheelEvent *Event)
{
  int delta = Event->delta() >> 1;     // use smaller steps

  // .....................................................................
  if((Event->state() & Qt::ShiftButton) || (Event->orientation() == Horizontal)) { // scroll horizontally ?
      if(delta > 0) { if(ScrollLeft(delta)) scrollBy(-delta, 0); }
      else { if(ScrollRight(delta)) scrollBy(-delta, 0); }
      viewport()->repaint();    // because QScrollView thinks nothing has changed
      drawn = false;
  }
  // .....................................................................
  else if(Event->state() & Qt::ControlButton) {  // use mouse wheel to zoom ?
      double Scaling;
      if(delta < 0) Scaling = double(-delta)/80;
      else Scaling = 80/double(delta);
      Zoom(Scaling);
      center(int(Event->x()*Scaling), int(Event->y()*Scaling));
      viewport()->repaint();
      drawn = false;
  }
  // .....................................................................
  else {     // scroll vertically !
      if(delta > 0) { if(ScrollUp(delta)) scrollBy(0, -delta); }
      else { if(ScrollDown(delta)) scrollBy(0, -delta); }
      viewport()->repaint();    // because QScrollView thinks nothing has changed
      drawn = false;
  }

  Event->accept();   // QScrollView must not handle this event
}

// -----------------------------------------------------------
// Scrolls the visible area upwards and enlarges or reduces the view
// area accordingly.
bool QucsView::ScrollUp(int step)
{
  int diff;
  QucsDoc *d = Docs.current();

  diff = contentsY() - step;
  if(diff < 0) {     // scroll outside the active area ?  (upwards)
    resizeContents(contentsWidth(), contentsHeight()-diff);
    d->ViewY1 += diff;
    scrollBy(0, diff);
    return false;
  }

  diff = d->ViewY2 - d->UsedY2 - 20;    // keep border of 20
  if(diff > 0) {      // make active area smaller ?
    if(step < diff) diff = step;
    resizeContents(contentsWidth(), contentsHeight()-diff);
    d->ViewY2 -= diff;
  }

  return true;
}

// -----------------------------------------------------------
// Scrolls the visible area downwards and enlarges or reduces the view
// area accordingly. ("step" must be negative!)
bool QucsView::ScrollDown(int step)
{
  int diff;
  QucsDoc *d = Docs.current();

  diff = contentsHeight() - contentsY()-visibleHeight() + step;
  if(diff < 0) {     // scroll outside the active area ?  (downwards)
    resizeContents(contentsWidth(), contentsHeight()-diff);
    d->ViewY2 -= diff;
    scrollBy(0, -step);
    return false;
  }

  diff = d->ViewY1 - d->UsedY1 + 20;    // keep border of 20
  if(diff < 0) {      // make active area smaller ?
    if(step > diff) diff = step;
    resizeContents(contentsWidth(), contentsHeight()+diff);
    d->ViewY1 -= diff;
    return false;
  }

  return true;
}

// -----------------------------------------------------------
// Scrolls the visible area to the left and enlarges or reduces the view
// area accordingly.
bool QucsView::ScrollLeft(int step)
{
  int diff;
  QucsDoc *d = Docs.current();

  diff = contentsX() - step;
  if(diff < 0) {     // scroll outside the active area ?  (to the left)
    resizeContents(contentsWidth()-diff, contentsHeight());
    d->ViewX1 += diff;
    scrollBy(diff, 0);
    return false;
  }

  diff = d->ViewX2 - d->UsedX2 - 20;    // keep border of 20
  if(diff > 0) {      // make active area smaller ?
    if(step < diff) diff = step;
    resizeContents(contentsWidth()-diff, contentsHeight());
    d->ViewX2 -= diff;
  }

  return true;
}

// -----------------------------------------------------------
// Scrolls the visible area to the right and enlarges or reduces the
// view area accordingly. ("step" must be negative!)
bool QucsView::ScrollRight(int step)
{
  int diff;
  QucsDoc *d = Docs.current();

  diff = contentsWidth() - contentsX()-visibleWidth() + step;
  if(diff < 0) {     // scroll outside the active area ?  (to the right)
    resizeContents(contentsWidth()-diff, contentsHeight());
    d->ViewX2 -= diff;
    scrollBy(-step, 0);
    return false;
  }

  diff = d->ViewX1 - d->UsedX1 + 20;    // keep border of 20
  if(diff < 0) {      // make active area smaller ?
    if(step > diff) diff = step;
    resizeContents(contentsWidth()+diff, contentsHeight());
    d->ViewX1 -= diff;
    return false;
  }

  return true;
}

// -----------------------------------------------------------
// Is called if the scroll arrow of the ScrollBar is pressed.
void QucsView::slotScrollUp()
{
  ScrollUp(verticalScrollBar()->lineStep());
  viewport()->repaint();   // because QScrollView thinks nothing has changed
  drawn = false;
}

// -----------------------------------------------------------
// Is called if the scroll arrow of the ScrollBar is pressed.
void QucsView::slotScrollDown()
{
  ScrollDown(-verticalScrollBar()->lineStep());
  viewport()->repaint();   // because QScrollView thinks nothing has changed
  drawn = false;
}

// -----------------------------------------------------------
// Is called if the scroll arrow of the ScrollBar is pressed.
void QucsView::slotScrollLeft()
{
  ScrollLeft(horizontalScrollBar()->lineStep());
  viewport()->repaint();   // because QScrollView thinks nothing has changed
  drawn = false;
}

// -----------------------------------------------------------
// Is called if the scroll arrow of the ScrollBar is pressed.
void QucsView::slotScrollRight()
{
  ScrollRight(-horizontalScrollBar()->lineStep());
  viewport()->repaint();   // because QScrollView thinks nothing has changed
  drawn = false;
}
