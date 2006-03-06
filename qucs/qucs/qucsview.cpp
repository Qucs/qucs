/***************************************************************************
                               qucsview.cpp
                              --------------
    begin                : Thu Aug 28 2003
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

#include "qucs.h"
#include "main.h"
#include "node.h"
#include "qucsview.h"
#include "components/spicedialog.h"
#include "components/componentdialog.h"
#include "diagrams/diagramdialog.h"
#include "diagrams/markerdialog.h"
#include "diagrams/tabdiagram.h"
#include "diagrams/timingdiagram.h"
#include "dialogs/labeldialog.h"
#include "dialogs/matchdialog.h"

#include <qinputdialog.h>
#include <qclipboard.h>
#include <qapplication.h>
#include <qmessagebox.h>
#include <qpopupmenu.h>

#include <limits.h>
#include <stdlib.h>


QRegExp  Expr_CompProp;
QRegExpValidator Val_CompProp(Expr_CompProp, 0);  // for edit component name on schematic


QucsView::QucsView(QWidget *parent) : QScrollView(parent)
{
  setVScrollBarMode(QScrollView::AlwaysOn);
  setHScrollBarMode(QScrollView::AlwaysOn);
  viewport()->setPaletteBackgroundColor(QucsSettings.BGColor);
  viewport()->setMouseTracking(true);
  viewport()->setAcceptDrops(true);  // enable drag'n drop

  MouseMoveAction = 0;
  MousePressAction = 0;
  MouseReleaseAction = 0;
  MouseDoubleClickAction = 0;

  selElem  = 0;  // no component/diagram is selected
  isMoveEqual = false;  // mouse cursor move x and y the same way
  focusElement = 0;

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
		viewport(), SLOT(update()));
  connect(this, SIGNAL(verticalSliderReleased()),
		viewport(), SLOT(update()));

  // to prevent user from editing something that he doesn't see
  connect(this, SIGNAL(horizontalSliderPressed()), SLOT(slotHideEdit()));
  connect(this, SIGNAL(verticalSliderPressed()), SLOT(slotHideEdit()));

  // .......................................................................
  // initialize menu appearing by right mouse button click on component
  ComponentMenu = new QPopupMenu(this);
  focusMEvent   = new QMouseEvent(QEvent::MouseButtonPress, QPoint(0,0),
				  Qt::NoButton, Qt::NoButton);

  // .......................................................................
  // valid expressions for component property editor
  editText  = new QLineEdit(viewport());
  editText->setFrame(false);
  editText->setHidden(true);
  editText->setPaletteBackgroundColor(QucsSettings.BGColor);
//  editText->setEraseColor(QucsSettings.BGColor);
  connect(editText, SIGNAL(returnPressed()), SLOT(slotApplyCompText()));
  connect(editText, SIGNAL(lostFocus()), SLOT(slotHideEdit()));
}


QucsView::~QucsView()
{
  delete ComponentMenu;
  delete focusMEvent;
}

// -----------------------------------------------------------
// Is called when the content (schematic or data display) has to be drawn.
void QucsView::drawContents(QPainter *p, int, int, int, int)
{
  QucsDoc *d = Docs.current();
  if (!d) return;

  Painter.init(p, d->Scale, -d->ViewX1, -d->ViewY1, contentsX(), contentsY());

  d->paintGrid(&Painter, contentsX(), contentsY(),
			visibleWidth(), visibleHeight());
  d->paint(&Painter);
//  drawn = false;
}

// -----------------------------------------------------------
float QucsView::Zoom(float s)
{
  QucsDoc *d = Docs.current();
  float Scale = d->Scale;
  Scale *= s;
  if(Scale > 10.0) Scale = 10.0f;
  if(Scale < 0.01) Scale = 0.01f;

  d->Scale = Scale;
  resizeContents(int(Scale*float(d->ViewX2 - d->ViewX1)),
		 int(Scale*float(d->ViewY2 - d->ViewY1)));
  return Scale;
}

// -----------------------------------------------------------
bool QucsView::pasteElements()
{
  QClipboard *cb = QApplication::clipboard();   // get system clipboard
  QString s = cb->text(QClipboard::Clipboard);
  QTextStream stream(&s, IO_ReadOnly);
  movingElements.clear();
  if(!Docs.current()->paste(&stream, &movingElements)) return false;

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
    if(pe->Type & isLabel) {
      pe->cx += xmin;  pe->x1 += xmin;
      pe->cy += ymin;  pe->y1 += ymin;
    }
    else
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

  if(x1 < d->ViewX1) {
    dx = int(d->Scale * float(d->ViewX1-x1+40));
    d->ViewX1 = x1-40;
  }
  if(y1 < d->ViewY1) {
    dy = int(d->Scale * float(d->ViewY1-y1+40));
    d->ViewY1 = y1-40;
  }
  if(x2 > d->ViewX2) d->ViewX2 = x2+40;
  if(y2 > d->ViewY2) d->ViewY2 = y2+40;

  resizeContents(int(d->Scale*float(d->ViewX2 - d->ViewX1)),
		 int(d->Scale*float(d->ViewY2 - d->ViewY1)));
//  resizeContents(d->ViewX2-d->ViewX1,d->ViewY2-d->ViewY1);
  scrollBy(dx,dy);
}

// -----------------------------------------------------------
void QucsView::setPainter(QPainter *p, QucsDoc *d)
{
  // contents to viewport transformation
  p->translate(-contentsX(), -contentsY());
  p->scale(d->Scale, d->Scale);
  p->translate(-d->ViewX1, -d->ViewY1);
  p->setPen(Qt::DotLine);
  p->setRasterOp(Qt::NotROP);  // background should not be erased
}

// -----------------------------------------------------------
void QucsView::slotCursorLeft()
{
  if(!editText->isHidden()) return;  // for edit of component property ?

  QucsDoc *d = Docs.current();
  if(d->MarkerLeftRight(true)) {
    viewport()->update();
    drawn = false;
    return;   // if marker selected, do not move other elements
  }
  movingElements.clear();
  d->copySelectedElements(&movingElements);
  if(movingElements.isEmpty()) {
    if(ScrollLeft(horizontalScrollBar()->lineStep()))
      scrollBy(-horizontalScrollBar()->lineStep(), 0);
    viewport()->update();
    drawn = false;
    return;
  }

  MAx1 = -(d->GridX);  MAy1 = 0;   // move "GridX" to left
  MovingElements();
  MAx3 = 1;  // sign for moved elements
  endElementMoving();
}

void QucsView::slotCursorRight()
{
  if(!editText->isHidden()) return;  // for edit of component property ?

  QucsDoc *d = Docs.current();
  if(d->MarkerLeftRight(false)) {
    viewport()->update();
    drawn = false;
    return;   // if marker selected, do not move other elements
  }
  movingElements.clear();
  d->copySelectedElements(&movingElements);
  if(movingElements.isEmpty()) {
    if(ScrollRight(-horizontalScrollBar()->lineStep()))
      scrollBy(horizontalScrollBar()->lineStep(), 0);
    viewport()->update();
    drawn = false;
    return;
  }

  MAx1 = d->GridX;  MAy1 = 0;   // move "GridX" to right
  MovingElements();
  MAx3 = 1;  // sign for moved elements
  endElementMoving();
}

void QucsView::slotCursorUp()
{
  if(!editText->isHidden()) {  // for edit of component property ?
    if(MAx3 == 0) return;  // edit component namen ?
    Component *pc = (Component*)focusElement;
    Property *pp = pc->Props.at(MAx3-1);  // current property
    int Begin = pp->Description.find('[');
    if(Begin < 0) return;  // no selection list ?
    int End = pp->Description.find(editText->text(), Begin); // current
    if(End < 0) return;  // should never happen
    End = pp->Description.findRev(',', End);
    if(End < Begin) return;  // was first item ?
    End--;
    int Pos = pp->Description.findRev(',', End);
    if(Pos < Begin) Pos = Begin;   // is first item ?
    Pos++;
    if(pp->Description.at(Pos) == ' ') Pos++; // remove leading space
    editText->setText(pp->Description.mid(Pos, End-Pos+1));
    editText->selectAll();
    return;
  }

  QucsDoc *d = Docs.current();
  if(d->MarkerUpDown(true)) {
    viewport()->update();
    drawn = false;
    return;   // if marker selected, do not move other elements
  }
  movingElements.clear();
  d->copySelectedElements(&movingElements);
  if(movingElements.isEmpty()) {
    if(ScrollUp(verticalScrollBar()->lineStep()))
      scrollBy(0, -verticalScrollBar()->lineStep());
    viewport()->update();
    drawn = false;
    return;
  }

  MAx1 = 0;  MAy1 = -(d->GridY);   // move "GridY" up
  MovingElements();
  MAx3 = 1;  // sign for moved elements
  endElementMoving();
}

void QucsView::slotCursorDown()
{
  if(!editText->isHidden()) {  // for edit of component property ?
    if(MAx3 == 0) return;  // edit component namen ?
    Component *pc = (Component*)focusElement;
    Property *pp = pc->Props.at(MAx3-1);  // current property
    int Pos = pp->Description.find('[');
    if(Pos < 0) return;  // no selection list ?
    Pos = pp->Description.find(editText->text(), Pos); // current list item
    if(Pos < 0) return;  // should never happen
    Pos = pp->Description.find(',', Pos);
    if(Pos < 0) return;  // was last item ?
    Pos++;
    if(pp->Description.at(Pos) == ' ') Pos++; // remove leading space
    int End = pp->Description.find(',', Pos);
    if(End < 0) {  // is last item ?
      End = pp->Description.find(']', Pos);
      if(End < 0) return;  // should never happen
    }
    editText->setText(pp->Description.mid(Pos, End-Pos));
    editText->selectAll();
    return;
  }

  QucsDoc *d = Docs.current();
  if(d->MarkerUpDown(false)) {
    viewport()->update();
    drawn = false;
    return;   // if marker selected, do not move other elements
  }
  movingElements.clear();
  d->copySelectedElements(&movingElements);
  if(movingElements.isEmpty()) {
    if(ScrollDown(-verticalScrollBar()->lineStep()))
      scrollBy(0, verticalScrollBar()->lineStep());
    viewport()->update();
    drawn = false;
    return;
  }

  MAx1 = 0;  MAy1 = d->GridY;   // move "GridY" down
  MovingElements();
  MAx3 = 1;  // sign for moved elements
  endElementMoving();
}

// -----------------------------------------------------------
void QucsView::editLabel(WireLabel *pl)
{
  LabelDialog *Dia = new LabelDialog(pl, this);
  int Result = Dia->exec();
  if(Result == 0) return;

  QString Name  = Dia->NodeName->text();
  QString Value = Dia->InitValue->text();
  delete Dia;

  if(Name.isEmpty() && Value.isEmpty()) { // if nothing entered, delete label
    pl->pOwner->Label = 0;   // delete name of wire
    delete pl;
  }
  else {
/*    Name.replace(' ', '_');	// label must not contain spaces
    while(Name.at(0) == '_') Name.remove(0,1);  // must not start with '_'
    if(Name.isEmpty()) return;
    if(Name == pl->Name) return;*/
    if(Result == 1) return;  // nothing changed

    int old_x2 = pl->x2;
    pl->setName(Name);   // set new name
    pl->initValue = Value;
    if(pl->cx > (pl->x1+(pl->x2>>1)))
      pl->x1 -= pl->x2 - old_x2; // don't change position due to text width
  }

  QucsDoc *d = Docs.current();
  d->sizeOfAll(d->UsedX1, d->UsedY1, d->UsedX2, d->UsedY2);
  viewport()->update();
  drawn = false;
  Docs.current()->setChanged(true, true);
}

// -----------------------------------------------------------
// Reinserts all elements (moved by the user) back into the schematic.
void QucsView::endElementMoving()
{
  Element *pe;
  QucsDoc *d = Docs.current();

  // First the wires with length zero are removed. This is important
  // if they are labeled. These labels must be put in the schematic
  // before all other elements.
  for(pe = movingElements.first(); pe != 0; ) {
    if(pe->Type == isWire)
      if(pe->x1 == pe->x2) if(pe->y1 == pe->y2) {
	if(((Wire*)pe)->Label) {
	  d->insertNodeLabel((WireLabel*)((Wire*)pe)->Label);
	  ((Wire*)pe)->Label = 0;
	}
	movingElements.removeRef(pe);
	delete (Wire*)pe;
	pe = movingElements.current();
	continue;
      }
    pe = movingElements.next();
  }


  for(pe = movingElements.first(); pe!=0; pe = movingElements.next()) {
//    pe->isSelected = false;  // deselect first (maybe afterwards pe == NULL)
    switch(pe->Type) {
      case isWire:
	d->insertWire((Wire*)pe);
	break;
      case isDiagram:
	d->Diags->append((Diagram*)pe);
	break;
      case isPainting:
	d->Paints->append((Painting*)pe);
	break;
      case isComponent:
      case isAnalogComponent:
      case isDigitalComponent:
	d->insertRawComponent((Component*)pe);
	break;
      case isMovingLabel:
      case isHMovingLabel:
      case isVMovingLabel:
	d->insertNodeLabel((WireLabel*)pe);
	break;
      case isMarker:
	((Marker*)pe)->pGraph->Markers.append((Marker*)pe);
	break;
    }
  }

  movingElements.clear();
  if((MAx3 != 0) || (MAy3 != 0))  // moved or put at the same place ?
    d->setChanged(true, true);

  // enlarge viewarea if components lie outside the view
  d->sizeOfAll(d->UsedX1, d->UsedY1, d->UsedX2, d->UsedY2);
  enlargeView(d->UsedX1, d->UsedY1, d->UsedX2, d->UsedY2);
  viewport()->update();
  drawn = false;
}

// -----------------------------------------------------------
// Moves elements in "movingElements" by MAx1/MAy1
void QucsView::MovingElements()
{
  Wire *pw;
  Element *pe;
  for(pe = movingElements.first(); pe != 0; pe = movingElements.next()) {
    if(pe->Type == isWire) {
      pw = (Wire*)pe;   // connecting wires are not moved completely

      if(((unsigned long)pw->Port1) > 3) {
	pw->x1 += MAx1;  pw->y1 += MAy1;
	if(pw->Label) { pw->Label->cx += MAx1;  pw->Label->cy += MAy1; }
      }
      else {  if(long(pw->Port1) & 1) { pw->x1 += MAx1; }
              if(long(pw->Port1) & 2) { pw->y1 += MAy1; } }

      if(((unsigned long)pw->Port2) > 3) { pw->x2 += MAx1;  pw->y2 += MAy1; }
      else {  if(long(pw->Port2) & 1) pw->x2 += MAx1;
              if(long(pw->Port2) & 2) pw->y2 += MAy1; }

      if(pw->Label) {      // root of node label must lie on wire
        if(pw->Label->cx < pw->x1) pw->Label->cx = pw->x1;
        if(pw->Label->cy < pw->y1) pw->Label->cy = pw->y1;
        if(pw->Label->cx > pw->x2) pw->Label->cx = pw->x2;
        if(pw->Label->cy > pw->y2) pw->Label->cy = pw->y2;
      }

    }
    else pe->setCenter(MAx1, MAy1, true);
  }
}


// ***********************************************************************
// **********                                                   **********
// **********       Functions for serving mouse moving          **********
// **********                                                   **********
// ***********************************************************************
void QucsView::contentsMouseMoveEvent(QMouseEvent *Event)
{
  if(MouseMoveAction)
    (this->*MouseMoveAction)(Event);
}

// -----------------------------------------------------------
void QucsView::MMoveElement(QMouseEvent *Event)
{
  if(selElem == 0) return;

  QucsDoc *d = Docs.current();
  int x  = Event->pos().x();
  int y  = Event->pos().y();
  int fx = int(float(x)/d->Scale) + d->ViewX1;
  int fy = int(float(y)/d->Scale) + d->ViewY1;
  int gx = fx;
  int gy = fy;
  d->setOnGrid(gx, gy);
  

  QPainter painter(viewport());
  setPainter(&painter, d);

  if(selElem->Type == isPainting) {
    QPainter paintUnscaled(viewport());
    paintUnscaled.setRasterOp(Qt::NotROP); // not erasing background

    x -= contentsX();
    y -= contentsY();
    ((Painting*)selElem)->MouseMoving(&painter, fx, fy, gx, gy,
                                       &paintUnscaled, x, y, drawn);
    drawn = true;
    return;
  }  // of "if(isPainting)"


  // ********** it is a component or diagram
  if(drawn) selElem->paintScheme(&painter); // erase old scheme
  drawn = true;
  selElem->setCenter(gx, gy);
  selElem->paintScheme(&painter); // paint scheme at new position
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

  MAx2  = int(float(Event->pos().x())/d->Scale) + d->ViewX1;
  MAy2  = int(float(Event->pos().y())/d->Scale) + d->ViewY1;
  d->setOnGrid(MAx2, MAy2);

  if(MAx1 == 0) {
    painter.drawLine(MAx3, MAy3, MAx3, MAy2); // paint
    painter.drawLine(MAx3, MAy2, MAx2, MAy2); // paint
  }
  else {
    painter.drawLine(MAx3, MAy3, MAx2, MAy3); // paint
    painter.drawLine(MAx2, MAy3, MAx2, MAy2); // paint
  }

  MouseDoubleClickAction = &QucsView::MDoubleClickWire2;
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

  MAx3  = int(float(Event->pos().x())/d->Scale) + d->ViewX1;
  MAy3  = int(float(Event->pos().y())/d->Scale) + d->ViewY1;
  d->setOnGrid(MAx3, MAy3);

  MAx1  = contentsX()+d->ViewX1;
  MAy1  = contentsY()+d->ViewY1;
  MAx2  = MAx1 + visibleWidth();
  MAy2  = MAy1 + visibleHeight();

  painter.drawLine(MAx1, MAy3, MAx2, MAy3); // paint
  painter.drawLine(MAx3, MAy1, MAx3, MAy2);
}

// -----------------------------------------------------------
// Paints a rectangle to select elements within it.
void QucsView::MMoveSelect(QMouseEvent *Event)
{
  QucsDoc *d = Docs.current();
  QPainter painter(viewport());
  setPainter(&painter, d);

  if(drawn) painter.drawRect(MAx1, MAy1, MAx2, MAy2); // erase old rectangle
  drawn = true;
  MAx2 = int(float(Event->pos().x())/d->Scale) + d->ViewX1 - MAx1;
  MAy2 = int(float(Event->pos().y())/d->Scale) + d->ViewY1 - MAy1;
  if(isMoveEqual)     // x and y size must be equal ?
    if(abs(MAx2) > abs(MAy2)) {
      if(MAx2<0) MAx2 = -abs(MAy2); else MAx2 = abs(MAy2);
    }
    else { if(MAy2<0) MAy2 = -abs(MAx2); else MAy2 = abs(MAx2); }
  painter.drawRect(MAx1, MAy1, MAx2, MAy2); // paint new rectangle
}

// -----------------------------------------------------------
void QucsView::MMoveResizePainting(QMouseEvent *Event)
{
  QucsDoc *d = Docs.current();
  QPainter painter(viewport());
  setPainter(&painter, d);

  MAx1 = int(float(Event->pos().x())/d->Scale) + d->ViewX1;
  MAy1 = int(float(Event->pos().y())/d->Scale) + d->ViewY1;
  Docs.current()->setOnGrid(MAx1, MAy1);
  ((Painting*)focusElement)->MouseResizeMoving(MAx1, MAy1, &painter);
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
  MAx3 = MAx1 = MAx2 - MAx1;
  MAy3 = MAy1 = MAy2 - MAy1;

  movingElements.clear();
  d->copySelectedElements(&movingElements);
  viewport()->repaint();

  Wire *pw;
  // Changes the position of all moving elements by dx/dy
  for(Element *pe=movingElements.first(); pe!=0; pe=movingElements.next()) {
    if(pe->Type == isWire) {
      pw = (Wire*)pe;   // connecting wires are not moved completely

      if(((unsigned long)pw->Port1) > 3) { pw->x1 += MAx1;  pw->y1 += MAy1; }
      else {  if(long(pw->Port1) & 1) { pw->x1 += MAx1; }
              if(long(pw->Port1) & 2) { pw->y1 += MAy1; } }

      if(((unsigned long)pw->Port2) > 3) { pw->x2 += MAx1;  pw->y2 += MAy1; }
      else {  if(long(pw->Port2) & 1) pw->x2 += MAx1;
              if(long(pw->Port2) & 2) pw->y2 += MAy1; }

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

  MAx2 = int(float(Event->pos().x())/d->Scale) + d->ViewX1;
  MAy2 = int(float(Event->pos().y())/d->Scale) + d->ViewY1;

  Element *pe;
  if(drawn) // erase old scheme
    for(pe = movingElements.first(); pe != 0; pe = movingElements.next())
      pe->paintScheme(&painter);
//      if(pe->Type == isWire)  if(((Wire*)pe)->Label)
//        if(!((Wire*)pe)->Label->isSelected)
//          ((Wire*)pe)->Label->paintScheme(&painter);

  drawn = true;
  if((Event->state() & Qt::ControlButton) == 0)
    d->setOnGrid(MAx2, MAy2);  // use grid only if CTRL key not pressed
  MAx1 = MAx2 - MAx1;
  MAy1 = MAy2 - MAy1;
  MAx3 += MAx1;  MAy3 += MAy1;   // keep track of the complete movement

  MovingElements();   // moves elements by MAx1/MAy1

  // paint afterwards to avoid conflict between wire and label painting
  for(pe = movingElements.first(); pe != 0; pe = movingElements.next())
    pe->paintScheme(&painter);
//    if(pe->Type == isWire)  if(((Wire*)pe)->Label)
//      if(!((Wire*)pe)->Label->isSelected)
//        ((Wire*)pe)->Label->paintScheme(&painter);

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

  MAx1 = int(float(Event->pos().x())/d->Scale) + d->ViewX1;
  MAy1 = int(float(Event->pos().y())/d->Scale) + d->ViewY1;
  d->setOnGrid(MAx1, MAy1);

  for(Element *pe=movingElements.first(); pe!=0; pe=movingElements.next()) {
    if(pe->Type & isLabel) {
      pe->cx += MAx1;  pe->x1 += MAx1;
      pe->cy += MAy1;  pe->y1 += MAy1;
    }
    else
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
  QPainter painter(viewport());
  painter.setRasterOp(Qt::NotROP);  // background should not be erased

  if(drawn) {
    painter.drawLine(MAx3-15, MAy3-15, MAx3+15, MAy3+15); // erase old
    painter.drawLine(MAx3-15, MAy3+15, MAx3+15, MAy3-15);
  }
  drawn = true;

  MAx3  = Event->pos().x() - contentsX();
  MAy3  = Event->pos().y() - contentsY();

  painter.drawLine(MAx3-15, MAy3-15, MAx3+15, MAy3+15); // paint
  painter.drawLine(MAx3-15, MAy3+15, MAx3+15, MAy3-15);
}

// -----------------------------------------------------------
// Paints a label above the mouse cursor to show the set wire label modus.
void QucsView::MMoveLabel(QMouseEvent *Event)
{
  QPainter painter(viewport());
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

  MAx3  = Event->pos().x() - contentsX();
  MAy3  = Event->pos().y() - contentsY();

  painter.drawLine(MAx3, MAy3, MAx3+10, MAy3-10); // paint new
  painter.drawLine(MAx3+10, MAy3-10, MAx3+20, MAy3-10);
  painter.drawLine(MAx3+10, MAy3-10, MAx3+10, MAy3-17);

  painter.drawLine(MAx3+12, MAy3-12, MAx3+15, MAy3-23);   // "A"
  painter.drawLine(MAx3+14, MAy3-17, MAx3+17, MAy3-17);
  painter.drawLine(MAx3+19, MAy3-12, MAx3+16, MAy3-23);
}

// -----------------------------------------------------------
// Paints a triangle above the mouse cursor to show the set marker modus.
void QucsView::MMoveMarker(QMouseEvent *Event)
{
  QPainter painter(viewport());
  painter.setRasterOp(Qt::NotROP);  // background should not be erased

  if(drawn) {
    painter.drawLine(MAx3, MAy3-2, MAx3-8, MAy3-10); // erase old
    painter.drawLine(MAx3+1, MAy3-3, MAx3+8, MAy3-10);
    painter.drawLine(MAx3-7, MAy3-10, MAx3+7, MAy3-10);
  }
  drawn = true;

  MAx3  = Event->pos().x() - contentsX();
  MAy3  = Event->pos().y() - contentsY();

  painter.drawLine(MAx3, MAy3-2, MAx3-8, MAy3-10); // paint new
  painter.drawLine(MAx3+1, MAy3-3, MAx3+8, MAy3-10);
  painter.drawLine(MAx3-7, MAy3-10, MAx3+7, MAy3-10);
}

// -----------------------------------------------------------
// Paints rounded arrows above the mouse cursor to show the
// "mirror about y axis" modus.
void QucsView::MMoveMirrorY(QMouseEvent *Event)
{
  QPainter painter(viewport());
  painter.setRasterOp(Qt::NotROP);  // background should not be erased

  if(drawn) {
    painter.drawLine(MAx3-11, MAy3-4, MAx3-9, MAy3-9); // erase old
    painter.drawLine(MAx3-11, MAy3-3, MAx3-6, MAy3-3);
    painter.drawLine(MAx3+11, MAy3-4, MAx3+9, MAy3-9);
    painter.drawLine(MAx3+11, MAy3-3, MAx3+6, MAy3-3);
    painter.drawArc(MAx3-10, MAy3-8, 21, 10, 16*20, 16*140);
  }
  drawn = true;

  MAx3  = Event->pos().x() - contentsX();
  MAy3  = Event->pos().y() - contentsY();

  painter.drawLine(MAx3-11, MAy3-4, MAx3-9, MAy3-9); // paint new
  painter.drawLine(MAx3-11, MAy3-3, MAx3-6, MAy3-3);
  painter.drawLine(MAx3+11, MAy3-4, MAx3+9, MAy3-9);
  painter.drawLine(MAx3+11, MAy3-3, MAx3+6, MAy3-3);
  painter.drawArc(MAx3-10, MAy3-8, 21, 10, 16*20, 16*140);
}

// -----------------------------------------------------------
// Paints rounded arrows beside the mouse cursor to show the
// "mirror about x axis" modus.
void QucsView::MMoveMirrorX(QMouseEvent *Event)
{
  QPainter painter(viewport());
  painter.setRasterOp(Qt::NotROP);  // background should not be erased

  if(drawn) {
    painter.drawLine(MAx3-4, MAy3-11, MAx3-9, MAy3-9); // erase old
    painter.drawLine(MAx3-3, MAy3-11, MAx3-3, MAy3-6);
    painter.drawLine(MAx3-4, MAy3+11, MAx3-9, MAy3+9);
    painter.drawLine(MAx3-3, MAy3+11, MAx3-3, MAy3+6);
    painter.drawArc(MAx3-8, MAy3-10, 10, 21, 16*110, 16*140);
  }
  drawn = true;

  MAx3  = Event->pos().x() - contentsX();
  MAy3  = Event->pos().y() - contentsY();

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
  QPainter painter(viewport());
  painter.setRasterOp(Qt::NotROP);  // background should not be erased

  if(drawn) {
    painter.drawLine(MAx3-6, MAy3+8, MAx3-6, MAy3+1); // erase old
    painter.drawLine(MAx3-7, MAy3+8, MAx3-12, MAy3+8);
    painter.drawArc(MAx3-10, MAy3-10, 21, 21, -16*20, 16*240);
  }
  drawn = true;

  MAx3  = Event->pos().x() - contentsX();
  MAy3  = Event->pos().y() - contentsY();

  painter.drawLine(MAx3-6, MAy3+8, MAx3-6, MAy3+1); // paint new
  painter.drawLine(MAx3-7, MAy3+8, MAx3-12, MAy3+8);
  painter.drawArc(MAx3-10, MAy3-10, 21, 21, -16*20, 16*240);
}

// -----------------------------------------------------------
// Paints a rectangle beside the mouse cursor to show the "activate" modus.
void QucsView::MMoveActivate(QMouseEvent *Event)
{
  QPainter painter(viewport());
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

  MAx3  = Event->pos().x() - contentsX();
  MAy3  = Event->pos().y() - contentsY();

  painter.drawLine(MAx3, MAy3-9, MAx3+13, MAy3-9); // paint new
  painter.drawLine(MAx3, MAy3, MAx3+13, MAy3);
  painter.drawLine(MAx3, MAy3-9, MAx3, MAy3);
  painter.drawLine(MAx3+13, MAy3-9, MAx3+13, MAy3);
  painter.drawLine(MAx3, MAy3-9, MAx3+13, MAy3);
  painter.drawLine(MAx3, MAy3, MAx3+13, MAy3-9);
}

// -----------------------------------------------------------
// Paints a grid beside the mouse cursor to show the "on grid" modus.
void QucsView::MMoveOnGrid(QMouseEvent *Event)
{
  QPainter painter(viewport());
  painter.setRasterOp(Qt::NotROP);  // background should not be erased

  if(drawn) {
    painter.drawLine(MAx3+10, MAy3+ 3, MAx3+25, MAy3+3); // erase old
    painter.drawLine(MAx3+10, MAy3+ 7, MAx3+25, MAy3+7);
    painter.drawLine(MAx3+10, MAy3+11, MAx3+25, MAy3+11);
    painter.drawLine(MAx3+13, MAy3, MAx3+13, MAy3+15);
    painter.drawLine(MAx3+17, MAy3, MAx3+17, MAy3+15);
    painter.drawLine(MAx3+21, MAy3, MAx3+21, MAy3+15);
  }
  drawn = true;

  MAx3  = Event->pos().x() - contentsX();
  MAy3  = Event->pos().y() - contentsY();

  painter.drawLine(MAx3+10, MAy3+ 3, MAx3+25, MAy3+3); // paint new
  painter.drawLine(MAx3+10, MAy3+ 7, MAx3+25, MAy3+7);
  painter.drawLine(MAx3+10, MAy3+11, MAx3+25, MAy3+11);
  painter.drawLine(MAx3+13, MAy3, MAx3+13, MAy3+15);
  painter.drawLine(MAx3+17, MAy3, MAx3+17, MAy3+15);
  painter.drawLine(MAx3+21, MAy3, MAx3+21, MAy3+15);
}

// -----------------------------------------------------------
// Paints symbol beside the mouse to show the "move component text" modus.
void QucsView::MMoveMoveTextB(QMouseEvent *Event)
{
  QPainter painter(viewport());
  painter.setRasterOp(Qt::NotROP);  // background should not be erased

  if(drawn) {
    painter.drawLine(MAx3+14, MAy3   , MAx3+16, MAy3); // erase old
    painter.drawLine(MAx3+23, MAy3   , MAx3+25, MAy3);
    painter.drawLine(MAx3+13, MAy3   , MAx3+13, MAy3+ 3);
    painter.drawLine(MAx3+13, MAy3+ 7, MAx3+13, MAy3+10);
    painter.drawLine(MAx3+14, MAy3+10, MAx3+16, MAy3+10);
    painter.drawLine(MAx3+23, MAy3+10, MAx3+25, MAy3+10);
    painter.drawLine(MAx3+26, MAy3   , MAx3+26, MAy3+ 3);
    painter.drawLine(MAx3+26, MAy3+ 7, MAx3+26, MAy3+10);
  }
  drawn = true;

  MAx3 = Event->pos().x() - contentsX();
  MAy3 = Event->pos().y() - contentsY();

  painter.drawLine(MAx3+14, MAy3   , MAx3+16, MAy3); // paint new
  painter.drawLine(MAx3+23, MAy3   , MAx3+25, MAy3);
  painter.drawLine(MAx3+13, MAy3   , MAx3+13, MAy3+ 3);
  painter.drawLine(MAx3+13, MAy3+ 7, MAx3+13, MAy3+10);
  painter.drawLine(MAx3+14, MAy3+10, MAx3+16, MAy3+10);
  painter.drawLine(MAx3+23, MAy3+10, MAx3+25, MAy3+10);
  painter.drawLine(MAx3+26, MAy3   , MAx3+26, MAy3+ 3);
  painter.drawLine(MAx3+26, MAy3+ 7, MAx3+26, MAy3+10);
}

// -----------------------------------------------------------
void QucsView::MMoveMoveText(QMouseEvent *Event)
{
  QucsDoc *d = Docs.current();
  QPainter painter(viewport());
  setPainter(&painter, d);

  if(drawn)
    painter.drawRect(MAx1, MAy1, MAx2, MAy2); // erase old
  drawn = true;

  int newX = int(float(Event->pos().x())/d->Scale) + d->ViewX1;
  int newY = int(float(Event->pos().y())/d->Scale) + d->ViewY1;
  MAx1 += newX - MAx3;
  MAy1 += newY - MAy3;
  MAx3  = newX;
  MAy3  = newY;

  painter.drawRect(MAx1, MAy1, MAx2, MAy2); // paint new
}

// -----------------------------------------------------------
// Paints symbol beside the mouse to show the "Zoom in" modus.
void QucsView::MMoveZoomIn(QMouseEvent *Event)
{
  QPainter painter(viewport());
  painter.setRasterOp(Qt::NotROP);  // background should not be erased

  if(drawn) {
    painter.drawLine(MAx3+14, MAy3   , MAx3+22, MAy3); // erase old
    painter.drawLine(MAx3+18, MAy3-4 , MAx3+18, MAy3+4);
    painter.drawEllipse(MAx3+12, MAy3-6, 13, 13);
  }
  drawn = true;

  MAx3 = Event->pos().x() - contentsX();
  MAy3 = Event->pos().y() - contentsY();

  painter.drawLine(MAx3+14, MAy3   , MAx3+22, MAy3);  // paint new
  painter.drawLine(MAx3+18, MAy3-4 , MAx3+18, MAy3+4);
  painter.drawEllipse(MAx3+12, MAy3-6, 13, 13);
}


// ************************************************************************
// **********                                                    **********
// **********    Functions for serving mouse button clicking     **********
// **********                                                    **********
// ************************************************************************
void QucsView::contentsMousePressEvent(QMouseEvent *Event)
{
  editText->setHidden(true); // disable text edit of component property
  if(MouseReleaseAction == &QucsView::MReleasePaste)
    return;
  
  QucsDoc *d = Docs.current();
  int x = int(float(Event->pos().x())/d->Scale) + d->ViewX1;
  int y = int(float(Event->pos().y())/d->Scale) + d->ViewY1;

  if(Event->button() != Qt::LeftButton) {
    if(MousePressAction == &QucsView::MPressElement)
      MPressElement(Event, d, x, y);
    else if(MousePressAction == &QucsView::MPressWire2)
      MPressWire2(Event, d, x, y);
    else {
      rightPressMenu(Event, d, x, y);   // show menu on right mouse button
      if(MouseReleaseAction)
        (this->*MouseReleaseAction)(Event, d); // not called (menu has focus)
    }
    return;
  }

  if(MousePressAction)
    (this->*MousePressAction)(Event, d, x, y);
}

// -----------------------------------------------------------
// Is called from several MousePress functions to show right button menu.
void QucsView::rightPressMenu(QMouseEvent *Event, QucsDoc *d, int x, int y)
{
  MAx1 = x;
  MAy1 = y;
  focusElement = d->selectElement(x, y, false);

  if(focusElement)  // remove special function (4 least significant bits)
    focusElement->Type &= isSpecialMask;


  // define menu
  ComponentMenu->clear();
  while(true) {
    if(focusElement) {
      focusElement->isSelected = true;
      ComponentMenu->insertItem(
               tr("Edit Properties"), this, SLOT(slotEditElement()));

      if((focusElement->Type & isComponent) == 0) break;
    }
    else {
      QucsMain->symEdit->addTo(ComponentMenu);
      QucsMain->fileSettings->addTo(ComponentMenu);
    }
    if(!QucsMain->Acts.moveText->isOn())
      QucsMain->Acts.moveText->addTo(ComponentMenu);
    break;
  }
  while(true) {
    if(focusElement)
      if(focusElement->Type == isGraph) break;
    if(!QucsMain->Acts.onGrid->isOn())
      QucsMain->Acts.onGrid->addTo(ComponentMenu);
    QucsMain->editCopy->addTo(ComponentMenu);
    if(!QucsMain->Acts.editPaste->isOn())
      QucsMain->Acts.editPaste->addTo(ComponentMenu);
    break;
  }
  if(!QucsMain->Acts.editDelete->isOn())
    QucsMain->Acts.editDelete->addTo(ComponentMenu);
  if(focusElement) if(focusElement->Type == isMarker) {
    ComponentMenu->insertSeparator();
    QString s = tr("power matching");
    if( ((Marker*)focusElement)->pGraph->Var == "Sopt" )
      s = tr("noise matching");
    ComponentMenu->insertItem(s, this, SLOT(slotPowerMatching()));
    if( ((Marker*)focusElement)->pGraph->Var.left(2) == "S[" )
      ComponentMenu->insertItem(tr("2-port matching"), this,
                                SLOT(slot2PortMatching()));
  }
  while(true) {
    if(focusElement) {
      if(focusElement->Type == isDiagram) break;
      if(focusElement->Type == isGraph) break;
    }
    ComponentMenu->insertSeparator();
    if(focusElement) if(focusElement->Type & isComponent)
      if(!QucsMain->Acts.editActivate->isOn())
        QucsMain->Acts.editActivate->addTo(ComponentMenu);
    if(!QucsMain->Acts.editRotate->isOn())
      QucsMain->Acts.editRotate->addTo(ComponentMenu);
    if(!QucsMain->Acts.editMirror->isOn())
      QucsMain->Acts.editMirror->addTo(ComponentMenu);
    if(!QucsMain->Acts.editMirrorY->isOn())
      QucsMain->Acts.editMirrorY->addTo(ComponentMenu);
    break;
  }


  *focusMEvent = *Event;  // remember event for "edit component" action
  ComponentMenu->popup(Event->globalPos());
  viewport()->update();
  drawn = false;
}

// -----------------------------------------------------------
void QucsView::MPressLabel(QMouseEvent*, QucsDoc *d, int x, int y)
{
  Wire *pw = 0;
  WireLabel *pl=0;
  Node *pn = d->selectedNode(x, y);
  if(!pn) {
    pw = d->selectedWire(x, y);
    if(!pw) return;
  }

  QString Name, Value;
  Element *pe=0;
  // is wire line already labeled ?
  if(pw) pe = d->getWireLabel(pw->Port1);
  else pe = d->getWireLabel(pn);
  if(pe) {
    if(pe->Type & isComponent) {
      QMessageBox::information(0, tr("Info"),
			tr("The ground potential cannot be labeled!"));
      return;
    }
    pl = ((Conductor*)pe)->Label;
  }

  LabelDialog *Dia = new LabelDialog(pl, this);
  if(Dia->exec() == 0) return;

  Name  = Dia->NodeName->text();
  Value = Dia->InitValue->text();
  delete Dia;

  if(Name.isEmpty() && Value.isEmpty() ) { // if nothing entered, delete name
    if(pe) {
      if(((Conductor*)pe)->Label)
        delete ((Conductor*)pe)->Label; // delete old name
      ((Conductor*)pe)->Label = 0;
    }
    else {
      if(pw) pw->setName("", "");   // delete name of wire
      else pn->setName("", "");
    }
  }
  else {
/*    Name.replace(' ', '_');	// label must not contain spaces
    while(Name.at(0) == '_') Name.remove(0,1);  // must not start with '_'
    if(Name.isEmpty()) return;
*/
    if(pe) {
      if(((Conductor*)pe)->Label)
        delete ((Conductor*)pe)->Label; // delete old name
      ((Conductor*)pe)->Label = 0;
    }

    int xl = x+30;
    int yl = y-30;
    d->setOnGrid(xl, yl);
    // set new name
    if(pw) pw->setName(Name, Value, x-pw->x1 + y-pw->y1, xl, yl);
    else pn->setName(Name, Value, xl, yl);
  }

  d->sizeOfAll(d->UsedX1, d->UsedY1, d->UsedX2, d->UsedY2);
  viewport()->update();
  drawn = false;
  d->setChanged(true, true);
}

// -----------------------------------------------------------
void QucsView::MPressSelect(QMouseEvent *Event, QucsDoc *d, int x, int y)
{
  bool Ctrl;
  if(Event->state() & ControlButton) Ctrl = true;
  else Ctrl = false;

  int No=0;
  MAx1 = x;
  MAy1 = y;
  focusElement = d->selectElement(x, y, Ctrl, &No);
  isMoveEqual = false;   // moving not neccessarily square


  if(focusElement)
  switch(focusElement->Type) {
    case isPaintingResize:  // resize painting ?
	focusElement->Type = isPainting;
	MouseReleaseAction = &QucsView::MReleaseResizePainting;
	MouseMoveAction = &QucsView::MMoveResizePainting;
	MousePressAction = 0;
	MouseDoubleClickAction = 0;
	this->grabKeyboard();  // no keyboard inputs during move actions
	return;

    case isDiagramResize:  // resize diagram ?
	if(((Diagram*)focusElement)->Name.left(4) != "Rect")
	  if(((Diagram*)focusElement)->Name.at(0) != 'T')
	    if(((Diagram*)focusElement)->Name != "Curve")
	      isMoveEqual = true;  // diagram must be square

	focusElement->Type = isDiagram;
	MAx1 = focusElement->cx;
	MAx2 = focusElement->x2;
	if(((Diagram*)focusElement)->State & 1) {
	  MAx1 += MAx2;
	  MAx2 *= -1;
	}
	MAy1 =  focusElement->cy;
	MAy2 = -focusElement->y2;
	if(((Diagram*)focusElement)->State & 2) {
	  MAy1 += MAy2;
	  MAy2 *= -1;
	}

//	Docs.current()->setOnGrid(MAx1, MAy1);
	MouseReleaseAction = &QucsView::MReleaseResizeDiagram;
	MouseMoveAction = &QucsView::MMoveSelect;
	MousePressAction = 0;
	MouseDoubleClickAction = 0;
	this->grabKeyboard(); // no keyboard inputs during move actions
	return;

    case isDiagramScroll:  // scroll in tabular ?
	focusElement->Type = isDiagram;

	if(((Diagram*)focusElement)->Name == "Time") {
	  if(((TimingDiagram*)focusElement)->scroll(MAx1))
	    d->setChanged(true, true, 'm'); // 'm' = only the first time
	}
	else {
	  if(((TabDiagram*)focusElement)->scroll(MAy1))
	    d->setChanged(true, true, 'm'); // 'm' = only the first time
	}
	viewport()->update();
	drawn = false;
//	focusElement = 0;  // avoid double-click on diagram scrollbar
	return;

    case isComponentText:  // property text of component ?
	focusElement->Type &= (~isComponentText) | isComponent;

	MAx3 = No;
	slotApplyCompText();
	return;
  }



  MousePressAction = 0;
  MouseDoubleClickAction = 0;
  this->grabKeyboard();  // no keyboard inputs during move actions
  viewport()->update();
  drawn = false;

  if(focusElement == 0) {
    MAx2 = 0;  // if not clicking on an element => open a rectangle
    MAy2 = 0;
    MouseReleaseAction = &QucsView::MReleaseSelect2;
    MouseMoveAction = &QucsView::MMoveSelect;
  }
  else {  // element could be moved
    if(!Ctrl) {
      if(!focusElement->isSelected)// Don't move selected elements if clicked
        d->deselectElements(focusElement); // element was not selected.
      focusElement->isSelected = true;
    }
    d->setOnGrid(MAx1, MAy1);
    MouseMoveAction = &QucsView::MMoveMoving;
  }
}

// -----------------------------------------------------------
void QucsView::MPressDelete(QMouseEvent*, QucsDoc *d, int x, int y)
{
  Element *pe = d->selectElement(x, y, false);
  if(pe) {
    pe->isSelected = true;
    d->deleteElements();

    d->sizeOfAll(d->UsedX1, d->UsedY1, d->UsedX2, d->UsedY2);
    viewport()->update();
    drawn = false;
  }
}

// -----------------------------------------------------------
void QucsView::MPressActivate(QMouseEvent*, QucsDoc *d, int x, int y)
{
  MAx1 = x;
  MAy1 = y;
  if(!d->activateComponent(x, y)) {
//    if(Event->button() != Qt::LeftButton) return;
    MAx2 = 0;  // if not clicking on a component => open a rectangle
    MAy2 = 0;
    MousePressAction = 0;
    MouseReleaseAction = &QucsView::MReleaseActivate;
    MouseMoveAction = &QucsView::MMoveSelect;
  }
  viewport()->update();
  drawn = false;
}

// -----------------------------------------------------------
void QucsView::MPressMirrorX(QMouseEvent*, QucsDoc *d, int x, int y)
{
  // no use in mirroring wires or diagrams
  Component *c = d->selectedComponent(x, y);
  if(c) {
    if(c->Ports.count() < 1) return;  // only mirror components with ports
    c->mirrorX();
    d->setCompPorts(c);
  }
  else {
    Painting *p = d->selectedPainting(x, y);
    if(p == 0) return;
    p->mirrorX();
  }

  viewport()->update();
  drawn = false;
  d->setChanged(true, true);
}

// -----------------------------------------------------------
void QucsView::MPressMirrorY(QMouseEvent*, QucsDoc *d, int x, int y)
{
  // no use in mirroring wires or diagrams
  Component *c = d->selectedComponent(x, y);
  if(c) {
    if(c->Ports.count() < 1) return;  // only mirror components with ports
    c->mirrorY();
    d->setCompPorts(c);
  }
  else {
    Painting *p = d->selectedPainting(x, y);
    if(p == 0) return;
    p->mirrorY();
  }

  viewport()->update();
  drawn = false;
  d->setChanged(true, true);
}

// -----------------------------------------------------------
void QucsView::MPressRotate(QMouseEvent*, QucsDoc *d, int x, int y)
{
  Element *e = d->selectElement(x, y, false);
  if(e == 0) return;
  e->Type &= isSpecialMask;  // remove special functions

  
  WireLabel *pl;
  int x1, y1, x2, y2;
//  e->isSelected = false;
  switch(e->Type) {
    case isComponent:
    case isAnalogComponent:
    case isDigitalComponent:
      if(((Component*)e)->Ports.count() < 1)
        break;  // do not rotate components without ports
      ((Component*)e)->rotate();
      d->setCompPorts((Component*)e);
      // enlarge viewarea if component lies outside the view
      ((Component*)e)->entireBounds(x1,y1,x2,y2, d->textCorr());
      enlargeView(x1, y1, x2, y2);
      break;

    case isWire:
      pl = ((Wire*)e)->Label;
      ((Wire*)e)->Label = 0;    // prevent label to be deleted
      d->Wires->setAutoDelete(false);
      d->deleteWire((Wire*)e);
      ((Wire*)e)->Label = pl;
      ((Wire*)e)->rotate();
      d->setOnGrid(e->x1, e->y1);
      d->setOnGrid(e->x2, e->y2);
      if(pl) d->setOnGrid(pl->cx, pl->cy);
      d->insertWire((Wire*)e);
      d->Wires->setAutoDelete(true);
      if (d->Wires->containsRef ((Wire*)e))
        enlargeView(e->x1, e->y1, e->x2, e->y2);
      break;

    case isPainting:
      ((Painting*)e)->rotate();
      // enlarge viewarea if component lies outside the view
      ((Painting*)e)->Bounding(x1,y1,x2,y2);
      enlargeView(x1, y1, x2, y2);
      break;

    default:
      return;
  }
  viewport()->update();
  drawn = false;
  Docs.current()->setChanged(true, true);
}

// -----------------------------------------------------------
void QucsView::MPressElement(QMouseEvent *Event, QucsDoc *d, int, int)
{
  if(selElem == 0) return;
  QPainter painter(viewport());
  setPainter(&painter, d);

  
  int x1, y1, x2, y2;
  if(selElem->Type & isComponent) {
    Component *Comp = (Component*)selElem;
    switch(Event->button()) {
      case Qt::LeftButton :
	// left mouse button inserts component into the schematic
	Comp->TextSize(x1, y1);
	d->insertComponent(Comp);
	Comp->TextSize(x2, y2);
	if(Comp->tx < Comp->x1) Comp->tx -= x2 - x1;

	// enlarge viewarea if component lies outside the view
	Comp->entireBounds(x1,y1,x2,y2, d->textCorr());
	enlargeView(x1, y1, x2, y2);

	drawn = false;
	viewport()->update();
	d->setChanged(true, true);
	Comp = Comp->newOne(); // component is used, so create a new one
//	Comp->paintScheme(&painter);
	break;

      case Qt::RightButton :  // right mouse button rotates the component
	if(Comp->Ports.count() == 0)
	  break;  // do not rotate components without ports
	Comp->paintScheme(&painter); // erase old component scheme
	Comp->rotate();
	Comp->paintScheme(&painter); // paint new component scheme
	break;

      default: ;   // avoids compiler warnings
    }
    selElem = Comp;
    return;

  }  // of "if(isComponent)"
  else if(selElem->Type == isDiagram) {
    if(Event->button() != Qt::LeftButton) return;

    Diagram *Diag = (Diagram*)selElem;
    QFileInfo Info(d->DocName);
    // dialog is Qt::WDestructiveClose !!!
    DiagramDialog *dia =
       new DiagramDialog(Diag,
           Info.dirPath() + QDir::separator() + d->DataSet, this);
    if(dia->exec() == QDialog::Rejected) {  // don't insert if dialog canceled
      viewport()->update();
      drawn = false;
      return;
    }

    d->Diags->append(Diag);
    enlargeView(Diag->cx, Diag->cy-Diag->y2, Diag->cx+Diag->x2, Diag->cy);
    d->setChanged(true, true);   // document has been changed

    viewport()->repaint();
    Diag = Diag->newOne(); // the component is used, so create a new one
    Diag->paintScheme(&painter);
    selElem = Diag;
    return;
  }  // of "if(isDiagram)"


  // ***********  it is a painting !!!
  if(((Painting*)selElem)->MousePressing()) {
    d->Paints->append((Painting*)selElem);
    ((Painting*)selElem)->Bounding(x1,y1,x2,y2);
    enlargeView(x1, y1, x2, y2);
    selElem = ((Painting*)selElem)->newOne();

    drawn = false;
    viewport()->update();
    d->setChanged(true, true);
  }
}

// -----------------------------------------------------------
// Is called if starting point of wire is pressed
void QucsView::MPressWire1(QMouseEvent*, QucsDoc *d, int x, int y)
{
  QPainter painter(viewport());
  setPainter(&painter, Docs.current());

  if(drawn) {
    painter.drawLine(MAx1, MAy3, MAx2, MAy3); // erase old mouse cross
    painter.drawLine(MAx3, MAy1, MAx3, MAy2);
  }
  drawn = false;

  MAx1 = 0;   // paint wire corner first up, then left/right
  MAx3 = x;
  MAy3 = y;
  d->setOnGrid(MAx3, MAy3);

  MouseMoveAction = &QucsView::MMoveWire2;
  MousePressAction = &QucsView::MPressWire2;
  // Double-click action is set in "MMoveWire2" to not initiate it
  // during "Wire1" actions.
}

// -----------------------------------------------------------
// Is called if ending point of wire is pressed
void QucsView::MPressWire2(QMouseEvent *Event, QucsDoc *d, int x, int y)
{
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
	  MouseDoubleClickAction = 0;
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
	  MouseDoubleClickAction = 0;
	} }
    }
    else {
      if(MAy2 != MAy3) {
	set = d->insertWire(new Wire(MAx2, MAy3, MAx2, MAy2));
	if(set & 2) {
	  // if last port is connected, then start a new wire
	  MouseMoveAction = &QucsView::MMoveWire1;
	  MousePressAction = &QucsView::MPressWire1;
	  MouseDoubleClickAction = 0;
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
	  MouseDoubleClickAction = 0;
	} }
    }
    viewport()->update();
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

    MAx2  = x;
    MAy2  = y;
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
void QucsView::MPressMarker(QMouseEvent*, QucsDoc *d, int x, int y)
{
  MAx1 = x;
  MAy1 = y;
  Marker *pm = d->setMarker(MAx1, MAy1);

  if(pm) {
    int x0 = pm->Diag->cx;
    int y0 = pm->Diag->cy;
    enlargeView(x0+pm->x1, y0-pm->y1-pm->y2, x0+pm->x1+pm->x2, y0-pm->y1);
  }
  viewport()->update();
  drawn = false;
}

// -----------------------------------------------------------
void QucsView::MPressOnGrid(QMouseEvent*, QucsDoc *d, int x, int y)
{
  Element *pe = d->selectElement(x, y, false);
  if(pe) {
    pe->Type &= isSpecialMask;  // remove special functions (4 lowest bits)
    pe->isSelected = true;
    Docs.current()->elementsOnGrid();

    d->sizeOfAll(d->UsedX1, d->UsedY1, d->UsedX2, d->UsedY2);
    viewport()->update();
    drawn = false;
  }
}

// -----------------------------------------------------------
void QucsView::MPressMoveText(QMouseEvent*, QucsDoc *d, int x, int y)
{
  MAx1 = x;
  MAy1 = y;
  focusElement = d->selectCompText(MAx1, MAy1, MAx2, MAy2);

  if(focusElement) {
    MAx3 = MAx1;
    MAy3 = MAy1;
    MAx1 = ((Component*)focusElement)->cx + ((Component*)focusElement)->tx;
    MAy1 = ((Component*)focusElement)->cy + ((Component*)focusElement)->ty;
    viewport()->update();
    drawn = false;
    MouseMoveAction = &QucsView::MMoveMoveText;
    MouseReleaseAction = &QucsView::MReleaseMoveText;
    this->grabKeyboard();  // no keyboard inputs during move actions
  }
}

// -----------------------------------------------------------
void QucsView::MPressZoomIn(QMouseEvent*, QucsDoc*, int x, int y)
{
  MAx1 = x;
  MAy1 = y;
  MAx2 = 0;  // rectangle size
  MAy2 = 0;

  MouseMoveAction = &QucsView::MMoveSelect;
  MouseReleaseAction = &QucsView::MReleaseZoomIn;
  this->grabKeyboard();  // no keyboard inputs during move actions
  viewport()->update();
  drawn = false;
}


// ***********************************************************************
// **********                                                   **********
// **********    Functions for serving mouse button releasing   **********
// **********                                                   **********
// ***********************************************************************
void QucsView::contentsMouseReleaseEvent(QMouseEvent *Event)
{
  if(MouseReleaseAction) {
    QucsDoc *d = Docs.current();
    (this->*MouseReleaseAction)(Event, d);
  }
}

// -----------------------------------------------------------
void QucsView::MReleaseSelect(QMouseEvent *Event, QucsDoc *d)
{
  bool ctrl;
  if(Event->state() & ControlButton) ctrl = true;
  else ctrl = false;

  if(!ctrl)  d->deselectElements(focusElement);

  if(focusElement)  if(Event->button() == LeftButton)
    if(focusElement->Type == isWire) {
      d->selectWireLine(focusElement, ((Wire*)focusElement)->Port1, ctrl);
      d->selectWireLine(focusElement, ((Wire*)focusElement)->Port2, ctrl);
    }

  this->releaseKeyboard();  // allow keyboard inputs again
  MousePressAction = &QucsView::MPressSelect;
  MouseReleaseAction = &QucsView::MReleaseSelect;
  MouseDoubleClickAction = &QucsView::MDoubleClickSelect;
  MouseMoveAction = 0;   // no element moving
  viewport()->update();
  drawn = false;
}

// -----------------------------------------------------------
// Is called after the rectangle for selection is released.
void QucsView::MReleaseSelect2(QMouseEvent *Event, QucsDoc *d)
{
  if(Event->button() != Qt::LeftButton) return;

  bool Ctrl;
  if(Event->state() & ControlButton) Ctrl = true;
  else Ctrl = false;

  // selects all elements within the rectangle
  d->selectElements(MAx1, MAy1, MAx1+MAx2, MAy1+MAy2, Ctrl);

  this->releaseKeyboard();  // allow keyboard inputs again
  MouseMoveAction = 0;
  MousePressAction = &QucsView::MPressSelect;
  MouseReleaseAction = &QucsView::MReleaseSelect;
  MouseDoubleClickAction = &QucsView::MDoubleClickSelect;
  viewport()->update();
  drawn = false;
}

// -----------------------------------------------------------
void QucsView::MReleaseActivate(QMouseEvent *Event, QucsDoc *d)
{
  if(Event->button() != Qt::LeftButton) return;

  // activates all components within the rectangle
  d->activateComps(MAx1, MAy1, MAx1+MAx2, MAy1+MAy2);

  MouseMoveAction = &QucsView::MMoveActivate;
  MousePressAction = &QucsView::MPressActivate;
  MouseReleaseAction = 0;
  MouseDoubleClickAction = 0;
  viewport()->update();
  drawn = false;
}

// -----------------------------------------------------------
// Is called after moving selected elements.
void QucsView::MReleaseMoving(QMouseEvent *Event, QucsDoc*)
{
  if(Event->button() != Qt::LeftButton) return;
  endElementMoving();
  this->releaseKeyboard();  // allow keyboard inputs again

  MouseMoveAction = 0;
  MousePressAction = &QucsView::MPressSelect;
  MouseReleaseAction = &QucsView::MReleaseSelect;
  MouseDoubleClickAction = &QucsView::MDoubleClickSelect;
}

// -----------------------------------------------------------
void QucsView::MReleaseResizeDiagram(QMouseEvent *Event, QucsDoc *d)
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
  for(Graph *pg = pd->Graphs.first(); pg != 0; pg = pd->Graphs.next())
    for(Marker *pm = pg->Markers.first(); pm!=0; pm = pg->Markers.next()) {
      pm->x1 += MAx3;      // correct changes due to move of diagram corner
      pm->y1 += MAy3;
    }

  int x1, x2, y1, y2;
  pd->Bounding(x1, x2, y1, y2);
  enlargeView(x1, x2, y1, y2);

  MouseMoveAction = 0;
  MousePressAction = &QucsView::MPressSelect;
  MouseReleaseAction = &QucsView::MReleaseSelect;
  MouseDoubleClickAction = &QucsView::MDoubleClickSelect;
  this->releaseKeyboard();  // allow keyboard inputs again

  viewport()->update();
  drawn = false;
  d->setChanged(true, true);
}

// -----------------------------------------------------------
void QucsView::MReleaseResizePainting(QMouseEvent *Event, QucsDoc *d)
{
  if(Event->button() != Qt::LeftButton) return;

  MouseMoveAction = 0;
  MousePressAction = &QucsView::MPressSelect;
  MouseReleaseAction = &QucsView::MReleaseSelect;
  MouseDoubleClickAction = &QucsView::MDoubleClickSelect;
  this->releaseKeyboard();  // allow keyboard inputs again

  viewport()->update();
  drawn = false;
  d->setChanged(true, true);
}

// -----------------------------------------------------------
void QucsView::MReleasePaste(QMouseEvent *Event, QucsDoc *d)
{
  int x1, y1, x2, y2;
  QFileInfo Info(d->DocName);
  QPainter painter(viewport());

  Element *pe;
  switch(Event->button()) {
  case Qt::LeftButton :
    // insert all moved elements into document
    for(pe = movingElements.first(); pe!=0; pe = movingElements.next()) {
      pe->isSelected = false;
      switch(pe->Type) {
	case isWire:
	  if(pe->x1 == pe->x2) if(pe->y1 == pe->y2)  break;
	  d->insertWire((Wire*)pe);
	  if (d->Wires->containsRef ((Wire*)pe))
	    enlargeView(pe->x1, pe->y1, pe->x2, pe->y2);
	  else pe = NULL;
	  break;
	case isDiagram:
	  d->Diags->append((Diagram*)pe);
	  ((Diagram*)pe)->loadGraphData(Info.dirPath() + QDir::separator() + 
					d->DataSet);
	  enlargeView(pe->cx, pe->cy-pe->y2, pe->cx+pe->x2, pe->cy);
	  break;
	case isPainting:
	  d->Paints->append((Painting*)pe);
	  ((Painting*)pe)->Bounding(x1,y1,x2,y2);
	  enlargeView(x1, y1, x2, y2);
	  break;
	case isMovingLabel:
	  pe->Type = isNodeLabel;
	  d->placeNodeLabel((WireLabel*)pe);
	  break;
	case isComponent:
	case isAnalogComponent:
	case isDigitalComponent:
	  d->insertComponent((Component*)pe);
	  ((Component*)pe)->entireBounds(x1,y1,x2,y2, d->textCorr());
	  enlargeView(x1, y1, x2, y2);
	  break;
      }
    }

    pasteElements();
    MouseMoveAction = &QucsView::MMovePaste;
    MousePressAction = 0;
    MouseReleaseAction = 0;
    MouseDoubleClickAction = 0;

    drawn = false;
    viewport()->update();
    d->setChanged(true, true);
    break;

  // ............................................................
  case Qt::RightButton :  // right button rotates the elements
    setPainter(&painter, d);

    if(drawn) // erase old scheme
      for(pe = movingElements.first(); pe != 0; pe = movingElements.next())
        pe->paintScheme(&painter);
    drawn = true;

    x1  = int(float(Event->pos().x())/d->Scale) + d->ViewX1;
    y1  = int(float(Event->pos().y())/d->Scale) + d->ViewY1;
    d->setOnGrid(x1, y1);

    for(pe = movingElements.first(); pe != 0; pe = movingElements.next()) {
      switch(pe->Type) {
        case isComponent:
        case isAnalogComponent:
        case isDigitalComponent:
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
	  ((Painting*)pe)->rotate(); // rotate !before! rotating the center
          ((Painting*)pe)->getCenter(x2, y2);
          pe->setCenter(y2 - y1 + x1, x1 - x2 + y1);
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

// -----------------------------------------------------------
void QucsView::MReleaseMoveText(QMouseEvent *Event, QucsDoc *d)
{
  if(Event->button() != Qt::LeftButton) return;

  MouseMoveAction = &QucsView::MMoveMoveTextB;
  MouseReleaseAction = 0;
  this->releaseKeyboard();  // allow keyboard inputs again

  ((Component*)focusElement)->tx = MAx1 - ((Component*)focusElement)->cx;
  ((Component*)focusElement)->ty = MAy1 - ((Component*)focusElement)->cy;
  viewport()->update();
  drawn = false;

  d->setChanged(true, true);
}

// -----------------------------------------------------------
void QucsView::MReleaseZoomIn(QMouseEvent *Event, QucsDoc *d)
{
  if(Event->button() != Qt::LeftButton) return;

  MAx1 = Event->pos().x();
  MAy1 = Event->pos().y();
  float DX = float(abs(MAx2));
  float DY = float(abs(MAy2));
  if((d->Scale * DX) < 6.0) {
    MAx1  = (MAx1<<1) - (visibleWidth()>>1)  - contentsX();
    MAy1  = (MAy1<<1) - (visibleHeight()>>1) - contentsY();
    Zoom(2.0);    // a simple click zooms by factor 2
  }
  else {
    float xScale = float(visibleWidth())  / DX;
    float yScale = float(visibleHeight()) / DY;
    if(xScale > yScale) xScale = yScale;
    yScale  = d->Scale;
    xScale /= yScale;
    Zoom(xScale);    

    if(MAx2 > 0)  MAx1 -= int(float(MAx2)*yScale);
    if(MAy2 > 0)  MAy1 -= int(float(MAy2)*yScale);
    MAx1 = int(float(MAx1) * xScale) - contentsX();
    MAy1 = int(float(MAy1) * xScale) - contentsY();
  }
  scrollBy(MAx1, MAy1);

  MouseMoveAction = &QucsView::MMoveZoomIn;
  MouseReleaseAction = 0;
  this->releaseKeyboard();  // allow keyboard inputs again

  viewport()->update();
  drawn = false;
}


// ***********************************************************************
// **********                                                   **********
// **********    Functions for mouse button double clicking     **********
// **********                                                   **********
// ***********************************************************************
void QucsView::contentsMouseDoubleClickEvent(QMouseEvent *Event)
{
  if(MouseDoubleClickAction)
    (this->*MouseDoubleClickAction)(Event);
}

// -----------------------------------------------------------
void QucsView::editElement(QMouseEvent *Event)
{
  if(focusElement == 0) return;

  Graph *pg;
  Component *c;
  Diagram *dia;
  DiagramDialog *ddia;
  MarkerDialog *mdia;
  int x1, y1, x2, y2;

  QucsDoc *d = Docs.current();
  QFileInfo Info(d->DocName);
  int x = int(Event->pos().x()/d->Scale) + d->ViewX1;
  int y = int(Event->pos().y()/d->Scale) + d->ViewY1;

  switch(focusElement->Type) {
    case isComponent:
    case isAnalogComponent:
    case isDigitalComponent:
         c = (Component*)focusElement;
         if(c->Model == "GND") return;

	 if(c->Model == "SPICE") {
	   SpiceDialog *sd = new SpiceDialog((SpiceFile*)c, d, this);
           if(sd->exec() != 1) break;   // dialog is WDestructiveClose
	 }
	 else {
	   ComponentDialog * cd = new ComponentDialog(c, d, this);
	   if(cd->exec() != 1) break;   // dialog is WDestructiveClose

           d->Comps->findRef(c);
           d->Comps->take();
           d->setComponentNumber(c); // for ports/power sources
           d->Comps->append(c);
	 }

         d->setChanged(true, true);
         c->entireBounds(x1,y1,x2,y2, d->textCorr());
         enlargeView(x1,y1,x2,y2);
         break;

    case isDiagram :
         dia = (Diagram*)focusElement;
         if(dia->Name.at(0) == 'T')  // don't open dialog on scrollbar
           if(dia->Name == "Time") {
             if(dia->cy < y) {
	       if(((TimingDiagram*)focusElement)->scroll(MAx1))
	         d->setChanged(true, true, 'm'); // 'm' = only the first time
	       break;
             }
	   }
           else {
             if(dia->cx > x) {
	       if(((TabDiagram*)focusElement)->scroll(MAy1))
	         d->setChanged(true, true, 'm'); // 'm' = only the first time
	       break;
             }
	   }

	 ddia = new DiagramDialog(dia,
		Info.dirPath() + QDir::separator() + d->DataSet, this);
         if(ddia->exec() != QDialog::Rejected)   // is WDestructiveClose
           d->setChanged(true, true);

	 dia->Bounding(x1, x2, y1, y2);
	 enlargeView(x1, x2, y1, y2);
	 break;

    case isGraph :
	 pg = (Graph*)focusElement;
	 // searching diagram for this graph
	 for(dia = d->Diags->last(); dia != 0; dia = d->Diags->prev())
	   if(dia->Graphs.findRef(pg) >= 0)
	     break;
	 if(!dia) break;

	 
	 ddia = new DiagramDialog(dia,
	 	Info.dirPath() + QDir::separator() + d->DataSet, this, pg);
	 if(ddia->exec() != QDialog::Rejected)   // is WDestructiveClose
	   d->setChanged(true, true);
         break;

    case isWire:
         MPressLabel(Event, d, x, y);
         break;

    case isNodeLabel:
    case isHWireLabel:
    case isVWireLabel:
	 editLabel((WireLabel*)focusElement);
	 break;

    case isPainting:
         if( ((Painting*)focusElement)->Dialog() )
           d->setChanged(true, true);
         break;

    case isMarker:
         mdia = new MarkerDialog((Marker*)focusElement, this);
         if(mdia->exec() > 1) d->setChanged(true, true);
         break;
  }

  viewport()->update();
  drawn = false;
}

// -----------------------------------------------------------
void QucsView::MDoubleClickSelect(QMouseEvent *Event)
{
  this->releaseKeyboard();  // allow keyboard inputs again
  editElement(Event);
}

// -----------------------------------------------------------
// Is called if the "edit" action is clicked on right mouse button menu.
void QucsView::slotEditElement()
{
  if(focusMEvent)
    editElement(focusMEvent);
}

// -----------------------------------------------------------
void QucsView::MDoubleClickWire2(QMouseEvent *Event)
{
  QucsDoc *d = Docs.current();
  int x = int(Event->pos().x()/d->Scale) + d->ViewX1;
  int y = int(Event->pos().y()/d->Scale) + d->ViewY1;

  MPressWire2(Event, d, x, y);

  MouseMoveAction = &QucsView::MMoveWire1;
  MousePressAction = &QucsView::MPressWire1;
  MouseDoubleClickAction = 0;
}

// *************************************************************************
// **********                                                     **********
// **********        Function for serving mouse wheel moving      **********
// **********                                                     **********
// *************************************************************************
void QucsView::contentsWheelEvent(QWheelEvent *Event)
{
  editText->setHidden(true);  // disable edit of component property
  int delta = Event->delta() >> 1;     // use smaller steps

  // .....................................................................
  if((Event->state() & Qt::ShiftButton) ||
     (Event->orientation() == Horizontal)) { // scroll horizontally ?
      if(delta > 0) { if(ScrollLeft(delta)) scrollBy(-delta, 0); }
      else { if(ScrollRight(delta)) scrollBy(-delta, 0); }
      viewport()->update(); // because QScrollView thinks nothing has changed
      drawn = false;
  }
  // .....................................................................
  else if(Event->state() & Qt::ControlButton) {  // use mouse wheel to zoom ?
      float Scaling;
      if(delta < 0) Scaling = float(-delta)/60.0/1.1;
      else Scaling = 1.1*60.0/float(delta);
      Scaling = Zoom(Scaling);
//      center(Event->x(), Event->y());
      viewport()->update();
      drawn = false;
  }
  // .....................................................................
  else {     // scroll vertically !
      if(delta > 0) { if(ScrollUp(delta)) scrollBy(0, -delta); }
      else { if(ScrollDown(delta)) scrollBy(0, -delta); }
      viewport()->update(); // because QScrollView thinks nothing has changed
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
  editText->setHidden(true);  // disable edit of component property
  ScrollUp(verticalScrollBar()->lineStep());
  viewport()->update();   // because QScrollView thinks nothing has changed
  drawn = false;
}

// -----------------------------------------------------------
// Is called if the scroll arrow of the ScrollBar is pressed.
void QucsView::slotScrollDown()
{
  editText->setHidden(true);  // disable edit of component property
  ScrollDown(-verticalScrollBar()->lineStep());
  viewport()->update();   // because QScrollView thinks nothing has changed
  drawn = false;
}

// -----------------------------------------------------------
// Is called if the scroll arrow of the ScrollBar is pressed.
void QucsView::slotScrollLeft()
{
  editText->setHidden(true);  // disable edit of component property
  ScrollLeft(horizontalScrollBar()->lineStep());
  viewport()->update();   // because QScrollView thinks nothing has changed
  drawn = false;
}

// -----------------------------------------------------------
// Is called if the scroll arrow of the ScrollBar is pressed.
void QucsView::slotScrollRight()
{
  editText->setHidden(true);  // disable edit of component property
  ScrollRight(-horizontalScrollBar()->lineStep());
  viewport()->update();   // because QScrollView thinks nothing has changed
  drawn = false;
}


// *************************************************************************
// **********                                                     **********
// **********          Function for serving drag'n drop           **********
// **********                                                     **********
// *************************************************************************

// Is called if an object is dropped (after drag'n drop).
void QucsView::contentsDropEvent(QDropEvent *Event)
{
  if(dragIsOkay) {
    QStrList List;
    QUriDrag::decode(Event, List);

    // URI:  file:/home/linuxuser/Desktop/example.sch
    for(unsigned int i=0; i < List.count(); i++)
      QucsMain->gotoPage(QString(List.at(i)).section(':',1));

    return;
  }


  QMouseEvent e(QEvent::MouseButtonPress, Event->pos(),
                Qt::LeftButton, Qt::NoButton);
  QucsDoc *d = Docs.current();
  int x = int(Event->pos().x()/d->Scale) + d->ViewX1;
  int y = int(Event->pos().y()/d->Scale) + d->ViewY1;

  MPressElement(&e, d, x, y);

  if(selElem) delete selElem;
  selElem = 0;  // no component selected

  if(labeledWire)
    ((QAction*)labeledWire)->setOn(true);  // restore old action
}

void QucsView::contentsDragEnterEvent(QDragEnterEvent *Event)
{
  dragIsOkay = false;
  if(Event->provides("text/uri-list"))  // file dragged in ?
    if(QUriDrag::canDecode(Event)) {
      dragIsOkay = true;
      Event->accept();
      return;
    }


  if(Event->format(1) == 0)   // only one MIME type ?
    if(Event->provides("application/x-qiconlist")) {
      QIconViewItem *Item = QucsMain->CompComps->currentItem();
      if(Item) {
	labeledWire = (Wire*)QucsMain->activeAction; // misuse variable
	QucsMain->slotSelectComponent(Item);  // also sets drawn=false
	MouseMoveAction = 0;
	MousePressAction = 0;

	Event->accept();
	return;
      }
    }

  Event->ignore();
}

void QucsView::contentsDragLeaveEvent(QDragLeaveEvent*)
{
  if(selElem)
    if(selElem->Type & isComponent)
      if(drawn) {

        QucsDoc *d = Docs.current();
        QPainter painter(viewport());
        setPainter(&painter, d);
        ((Component*)selElem)->paintScheme(&painter);
        drawn = false;
      }

  if(labeledWire)
    ((QAction*)labeledWire)->setOn(true);  // restore old action
}

void QucsView::contentsDragMoveEvent(QDragMoveEvent *Event)
{
  if(!dragIsOkay) {
    if(selElem == 0) {
      Event->ignore();
      return;
    }

    QMouseEvent e(QEvent::MouseMove, Event->pos(), Qt::NoButton, Qt::NoButton);
    MMoveElement(&e);
  }

  Event->accept();
}

// -----------------------------------------------------------
// Is called if user clicked on component text of if return is
// pressed in the component text QLineEdit.
void QucsView::slotApplyCompText()
{
  QString s;
  QucsDoc *d = Docs.current();
  QFont f = QucsSettings.font;
  f.setPointSizeFloat( d->Scale * float(f.pointSize()) );
  editText->setFont(f);

  Property  *pp = 0;
  Component *pc = (Component*)focusElement;
  if(!pc) return;  // should never happen
  MAx1 = pc->cx + pc->tx;
  MAy1 = pc->cy + pc->ty;

  int z, n=0;  // "n" is number of property on screen
  pp = pc->Props.first();
  for(z=MAx3; z>0; z--) {  // calculate "n"
    if(!pp) {  // should never happen
      editText->setHidden(true);
      return;
    }
    if(pp->display) n++;   // is visible ?
    pp = pc->Props.next();
  }
  
  pp = 0;
  if(MAx3 > 0)  pp = pc->Props.at(MAx3-1);   // current property
  else s = pc->Name;

  if(!editText->isHidden()) {   // is called the first time ?
    // no -> apply value to current property
    if(MAx3 == 0) {   // component name ?
      Component *pc2;
      if(!editText->text().isEmpty())
        if(pc->Name != editText->text()) {
          for(pc2 = d->Comps->first(); pc2!=0; pc2 = d->Comps->next())
            if(pc2->Name == editText->text())
              break;  // found component with the same name ?
          if(!pc2) {
            pc->Name = editText->text();
            d->setChanged(true, true);  // only one undo state
          }
        }
    }
    else if(pp) {  // property was applied
      if(pp->Value != editText->text()) {
        pp->Value = editText->text();
        d->recreateComponent(pc);  // because of "Num" and schematic symbol
        d->setChanged(true, true); // only one undo state
      }
    }

    n++;     // next row on screen
    MAx3++;  // next property
    pp = pc->Props.at(MAx3-1);  // search for next property

    if(!pp) {     // was already last property ?
      editText->setHidden(true);
      viewport()->update();  // maybe text is now longer
      drawn = false;
      return;
    }


    while(!pp->display) {  // search for next visible property
      MAx3++;  // next property
      pp = pc->Props.next();
      if(!pp) {     // was already last property ?
        editText->setHidden(true);
        viewport()->update();  // maybe text is now longer
        drawn = false;
        return;
      }
    }
  }


  contentsToViewport(int(d->Scale * float(MAx1 - d->ViewX1)),
			 int(d->Scale * float(MAy1 - d->ViewY1)),
			 MAx2, MAy2);
  editText->setReadOnly(false);
  if(pp) {  // is it a property ?
    s = pp->Value;
    MAx2 += editText->fontMetrics().width(pp->Name+"=");
    if(pp->Description.find('[') >= 0)  // is selection list ?
      editText->setReadOnly(true);
    Expr_CompProp.setPattern("[^\"=]+");
  }
  else   // it is the component name
    Expr_CompProp.setPattern("[\\w_]+");
  Val_CompProp.setRegExp(Expr_CompProp);
  editText->setValidator(&Val_CompProp);

  z = editText->fontMetrics().lineSpacing();
  MAy2 += n*z;
  editText->move(QPoint(MAx2, MAy2));
  editText->setText(s);
  editText->resize(editText->fontMetrics().width(s)+3*z, z);
  editText->setFocus();
  editText->selectAll();
  editText->setHidden(false);
}

// -----------------------------------------------------------
// Hides the edit for component property. Called e.g. if QLineEdit lost focus.
void QucsView::slotHideEdit()
{
  editText->setHidden(true);
}

// -----------------------------------------------------------
void QucsView::slotPowerMatching()
{
  if(!focusElement) return;
  if(focusElement->Type != isMarker) return;
  Marker *pm = (Marker*)focusElement;

//  double Z0 = 50.0;
  QString Var = pm->pGraph->Var;
  double Imag = pm->VarPos[pm->nVarPos+1];
  if(Var == "Sopt")  // noise matching ?
    Imag *= -1.0;

  MatchDialog *Dia = new MatchDialog(this);
  Dia->TwoCheck->setChecked(false);
  Dia->TwoCheck->setEnabled(false);
//  Dia->Ref1Edit->setText(QString::number(Z0));
  Dia->S11magEdit->setText(QString::number(pm->VarPos[pm->nVarPos]));
  Dia->S11degEdit->setText(QString::number(Imag));
  Dia->setFrequency(pm->VarPos[0]);

  QucsMain->slotToPage();
  if(Dia->exec() != QDialog::Accepted)
    return;
}

// -----------------------------------------------------------
void QucsView::slot2PortMatching()
{
  if(!focusElement) return;
  if(focusElement->Type != isMarker) return;
  Marker *pm = (Marker*)focusElement;

  QString DataSet;
  int z = pm->pGraph->Var.find(':');
  if(z <= 0)  DataSet = Docs.current()->DataSet;
  else  DataSet = pm->pGraph->Var.mid(z+1);
  double Freq = pm->VarPos[0];

  QFileInfo Info(Docs.current()->DocName);
  DataSet = Info.dirPath()+QDir::separator()+DataSet;

  Diagram *Diag = new Diagram();

  Graph *pg = new Graph("S[1,1]");
  Diag->Graphs.append(pg);
  if(!Diag->loadVarData(DataSet, pg)) {
    QMessageBox::critical(0, tr("Error"), tr("Could not load S[1,1]."));
    return;
  }

  pg = new Graph("S[1,2]");
  Diag->Graphs.append(pg);
  if(!Diag->loadVarData(DataSet, pg)) {
    QMessageBox::critical(0, tr("Error"), tr("Could not load S[1,2]."));
    return;
  }

  pg = new Graph("S[2,1]");
  Diag->Graphs.append(pg);
  if(!Diag->loadVarData(DataSet, pg)) {
    QMessageBox::critical(0, tr("Error"), tr("Could not load S[2,1]."));
    return;
  }

  pg = new Graph("S[2,2]");
  Diag->Graphs.append(pg);
  if(!Diag->loadVarData(DataSet, pg)) {
    QMessageBox::critical(0, tr("Error"), tr("Could not load S[2,2]."));
    return;
  }

  DataX *Data = Diag->Graphs.getFirst()->cPointsX.first();
  if(Data->Var != "frequency") {
    QMessageBox::critical(0, tr("Error"), tr("Wrong dependency!"));
    return;
  }

  double *Value = Data->Points;
  // search for values for chosen frequency
  for(z=0; z<Data->count; z++)
    if(*(Value++) == Freq) break;

  // get S-parameters
  double S11real = *(Diag->Graphs.first()->cPointsY + 2*z);
  double S11imag = *(Diag->Graphs.current()->cPointsY + 2*z + 1);
  double S12real = *(Diag->Graphs.next()->cPointsY + 2*z);
  double S12imag = *(Diag->Graphs.current()->cPointsY + 2*z + 1);
  double S21real = *(Diag->Graphs.next()->cPointsY + 2*z);
  double S21imag = *(Diag->Graphs.current()->cPointsY + 2*z + 1);
  double S22real = *(Diag->Graphs.next()->cPointsY + 2*z);
  double S22imag = *(Diag->Graphs.current()->cPointsY + 2*z + 1);

  delete Diag;

  MatchDialog *Dia = new MatchDialog(this);
  Dia->TwoCheck->setEnabled(false);
  Dia->setFrequency(Freq);
  Dia->S11magEdit->setText(QString::number(S11real));
  Dia->S11degEdit->setText(QString::number(S11imag));
  Dia->S12magEdit->setText(QString::number(S12real));
  Dia->S12degEdit->setText(QString::number(S12imag));
  Dia->S21magEdit->setText(QString::number(S21real));
  Dia->S21degEdit->setText(QString::number(S21imag));
  Dia->S22magEdit->setText(QString::number(S22real));
  Dia->S22degEdit->setText(QString::number(S22imag));

  QucsMain->slotToPage();
  if(Dia->exec() != QDialog::Accepted)
    return;
}
