/***************************************************************************
                              mouseactions.cpp
                             ------------------
    begin                : Thu Aug 28 2003
    copyright            : (C) 2003 by Michael Margraf
    email                : michael.margraf@alumni.tu-berlin.de
 ***************************************************************************/

/* Copyright (C) 2014 Guilherme Brondani Torri <guitorri@gmail.com>        */

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include "qucs.h"
#include "node.h"
#include "schematic.h"
#include "misc.h"
#include "mouseactions.h"
#include "module.h"
#include "components/component.h"
#include "components/spicedialog.h"
#include "components/spicefile.h"
#include "components/optimizedialog.h"
#include "components/componentdialog.h"
#include "components/vacomponent.h"
#include "diagrams/diagramdialog.h"
#include "diagrams/markerdialog.h"
#include "diagrams/tabdiagram.h"
#include "diagrams/timingdiagram.h"
#include "dialogs/labeldialog.h"

#include <QTextStream>
#include <Q3PtrList>
#include <QMouseEvent>
#include <QClipboard>
#include <QApplication>
#include <QMessageBox>
#include <QMenu>
#include <QEvent>
#include <QAction>
#include <QLineEdit>
#include <QDebug>

#include <limits.h>
#include <stdlib.h>


QAction *formerAction;   // remember action before drag n'drop etc.


MouseActions::MouseActions(QucsApp* App_)
{
  App = App_; // pointer to main app
  selElem  = 0;  // no component/diagram is selected
  isMoveEqual = false;  // mouse cursor move x and y the same way
  focusElement = 0; //element being interacted with mouse

  // ...............................................................
  // initialize menu appearing by right mouse button click on component
  ComponentMenu = new QMenu(QucsMain);
  focusMEvent   = new QMouseEvent(QEvent::MouseButtonPress, QPoint(0,0),
                  Qt::NoButton, Qt::NoButton, Qt::NoModifier);
}


MouseActions::~MouseActions()
{
  delete ComponentMenu;
  delete focusMEvent;
}

// -----------------------------------------------------------
bool MouseActions::pasteElements(Schematic *Doc)
{
  QClipboard *cb = QApplication::clipboard();   // get system clipboard
  QString s = cb->text(QClipboard::Clipboard);
  QTextStream stream(&s, QIODevice::ReadOnly);
  movingElements.clear();
  if(!Doc->paste(&stream, &movingElements)) return false;

  Element *pe;
  int xmax, xmin, ymax, ymin;
  xmin = ymin = INT_MAX;
  xmax = ymax = INT_MIN;
  // First, get the max and min coordinates of all selected elements.
  for(pe = movingElements.first(); pe != 0; pe = movingElements.next()) {
    if(pe->ElemType == isWire) {
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
  Doc->setOnGrid(xmin, ymin);

  // moving with mouse cursor in the midpoint
  for(pe = movingElements.first(); pe != 0; pe = movingElements.next())
    if(pe->ElemType & isLabel) {
      pe->cx += xmin;  pe->x1 += xmin;
      pe->cy += ymin;  pe->y1 += ymin;
    }
    else
      pe->setCenter(xmin, ymin, true);

  return true;
}

// -----------------------------------------------------------
void MouseActions::editLabel(Schematic *Doc, WireLabel *pl)
{
  LabelDialog *Dia = new LabelDialog(pl, Doc);
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

  Doc->sizeOfAll(Doc->UsedX1, Doc->UsedY1, Doc->UsedX2, Doc->UsedY2);
  Doc->viewport()->update();
  drawn = false;
  Doc->setChanged(true, true);
}

// -----------------------------------------------------------
// Reinserts all elements (moved by the user) back into the schematic.
void MouseActions::endElementMoving(Schematic *Doc, Q3PtrList<Element> *movElements)
{
  Element *pe;
  for(pe = movElements->first(); pe!=0; pe = movElements->next()) {
//    pe->isSelected = false;  // deselect first (maybe afterwards pe == NULL)
    switch(pe->ElemType) { // FIXME: use casts.
      case isWire:
        if(pe->x1 == pe->x2)
          if(pe->y1 == pe->y2) {
            // Delete wires with zero length, but preserve label.
            if(((Wire*)pe)->Label) {
              Doc->insertNodeLabel((WireLabel*)((Wire*)pe)->Label);
              ((Wire*)pe)->Label = 0;
            }
            delete (Wire*)pe;
            break;
          }

	Doc->insertWire((Wire*)pe);
	break;
      case isDiagram:
	Doc->Diagrams->append((Diagram*)pe);
	break;
      case isPainting:
	Doc->Paintings->append((Painting*)pe);
	break;
      case isComponent:
      case isAnalogComponent:
      case isDigitalComponent:
	Doc->insertRawComponent((Component*)pe, false);
	break;
      case isMovingLabel:
      case isHMovingLabel:
      case isVMovingLabel:
	Doc->insertNodeLabel((WireLabel*)pe);
	break;
      case isMarker:
	assert(dynamic_cast<Marker*>(pe));
	break;
    }
  }

  movElements->clear();
  if((MAx3 != 0) || (MAy3 != 0))  // moved or put at the same place ?
    Doc->setChanged(true, true);

  // enlarge viewarea if components lie outside the view
  Doc->sizeOfAll(Doc->UsedX1, Doc->UsedY1, Doc->UsedX2, Doc->UsedY2);
  Doc->enlargeView(Doc->UsedX1, Doc->UsedY1, Doc->UsedX2, Doc->UsedY2);
  Doc->viewport()->update();
  drawn = false;
}

// -----------------------------------------------------------
// Moves elements in "movElements" by x/y
void MouseActions::moveElements(Q3PtrList<Element> *movElements, int x, int y)
{
  Wire *pw;
  Element *pe;
  for(pe = movElements->first(); pe != 0; pe = movElements->next()) {
    if(pe->ElemType == isWire) {
      pw = (Wire*)pe;   // connected wires are not moved completely

      if(((uintptr_t)pw->Port1) > 3) {
	pw->x1 += x;  pw->y1 += y;
	if(pw->Label) { pw->Label->cx += x;  pw->Label->cy += y; }
      }
      else {  if((uintptr_t)(pw->Port1) & 1) { pw->x1 += x; }
              if((uintptr_t)(pw->Port1) & 2) { pw->y1 += y; } }

      if(((uintptr_t)pw->Port2) > 3) { pw->x2 += x;  pw->y2 += y; }
      else {  if((uintptr_t)(pw->Port2) & 1) pw->x2 += x;
              if((uintptr_t)(pw->Port2) & 2) pw->y2 += y; }

      if(pw->Label) {      // root of node label must lie on wire
        if(pw->Label->cx < pw->x1) pw->Label->cx = pw->x1;
        if(pw->Label->cy < pw->y1) pw->Label->cy = pw->y1;
        if(pw->Label->cx > pw->x2) pw->Label->cx = pw->x2;
        if(pw->Label->cy > pw->y2) pw->Label->cy = pw->y2;
      }

    }
    else pe->setCenter(x, y, true);
  }
}


// ***********************************************************************
// **********                                                   **********
// **********       Functions for serving mouse moving          **********
// **********                                                   **********
// ***********************************************************************

/*!
 * \brief MouseActions::MMoveElement
 * \param Doc
 * \param Event
 *
 * Event handler, an Element selected for insertion is moved
 * on the View.
 *
 * Set the mouse decoration/cursor to represent the selElem.
 * Motion snaps to grid points.
 *
 * selElem is an Element object, see QucsApp::slotSelectComponent and
 * other similar slots.
 *
 * When mouse moves over the scene, the Element must be added and
 * updated acordingly.
 */
void MouseActions::MMoveElement(Schematic *Doc, QMouseEvent *Event)
{
  if(selElem == 0) return;

  QPointF pos = Doc->mapToScene(Event->pos());
  int x = pos.x();
  int y = pos.y();
  int gx = x;
  int gy = y;
  Doc->setOnGrid(gx, gy);

  // while moving, add selElem only once to scene
  if(!drawn) {
    Doc->scene->addItem(selElem);
    drawn = true;
    selElem->drawScheme = true;
  }

  // Painting handle the tracking of the mouse movement
  if(selElem->ElemType == isPainting) {
    // propagate mouse move event
    // it takes care of setCenter
    ((Painting*)selElem)->MouseMoving(Doc, x, y, gx, gy,
                                       Doc, x, y, drawn);
    return;
  }

  // Default to Component or Diagram
  // move Element with the cursor
  selElem->setCenter(gx, gy);

}


/**
 * @brief draws wire aiming cross on Document view
 * @param Doc - pointer to Schematics object
 * @param fx  - document x-coordinate of center
 * @param fy  - document x-coordinate of center
 *
 *  \todo Move aim (crosshair) to separate class
 *
static void paintAim(Schematic *Doc, int fx,int fy){
	//let we reserve couple of points at the edges of lines for some aesthetics,
	//and visual check that our calculations has fit the widget window.
	const int ldelta = 2;

	//left and upper edges of our lines
	int lx0 = DOC_X_POS(Doc->contentsX()+ldelta);
	int ly0 = DOC_Y_POS(Doc->contentsY()+ldelta);

	//right and bottom edges
	int lx1 = DOC_X_POS(Doc->contentsX()+Doc->viewport()->width()-1-ldelta);
	int ly1 = DOC_Y_POS(Doc->contentsY()+Doc->viewport()->height()-1-ldelta);

	//post line paint events
	Doc->PostPaintEvent (_Line, lx0, fy, lx1, fy);
	Doc->PostPaintEvent (_Line, fx, ly0, fx, ly1);
}

//paint ghost line - horizontal
static void paintGhostLineH(Schematic *Doc, int fx,int fy, int fxx){
	Doc->PostPaintEvent (_Line, fx, fy-1, fxx, fy-1);
	Doc->PostPaintEvent (_Line, fx, fy+1, fxx, fy+1);
}

//paint ghost line - vertical
static void paintGhostLineV(Schematic *Doc, int fx, int fy, int fyy){
	Doc->PostPaintEvent (_Line, fx-1, fy, fx-1, fyy);
	Doc->PostPaintEvent (_Line, fx+1, fy, fx+1, fyy);
}
*/


// -----------------------------------------------------------
/**
 * @brief MouseActions::MMoveWire2 Paint wire as it is being drawn with mouse.
 * @param Doc
 * @param Event
 */
void MouseActions::MMoveWire2(Schematic *Doc, QMouseEvent *Event)
{
  QPointF pos = Doc->mapToScene(Event->pos());
  MAx2  = pos.x();
  MAy2  = pos.y();
  Doc->setOnGrid(MAx2, MAy2);
  /// \todo paint aim
  /*paintAim(Doc,MAx2,MAy2); //let we paint aim cross

  //because cross slightly masks a wire, let we make wire thicker
  //better to make it by increasing of pen, but here we cannot access
  //pen
  if(MAx1 == 0) {
	paintGhostLineV(Doc,MAx3,MAy3,MAy2);
	paintGhostLineH(Doc,MAx3,MAy2,MAx2);
  }
  else {
	paintGhostLineH(Doc,MAx3,MAy3,MAx2);
	paintGhostLineV(Doc,MAx2,MAy3,MAy2);
  }
  */
  QucsMain->MouseDoubleClickAction = &MouseActions::MDoubleClickWire2;
  Doc->viewport()->update();
}


/**
 * @brief MouseActions::MMoveWire1 Paint hair cross for "insert wire" mode
 * @param Doc
 * @param Event
 */
void MouseActions::MMoveWire1(Schematic *Doc, QMouseEvent *Event)
{
  QPointF pos = Doc->mapToScene(Event->pos());
  MAx3 = pos.x();
  MAy3 = pos.y();
  Doc->setOnGrid(MAx3, MAy3);
  /// \todo paint aim
  //paintAim(Doc,MAx3,MAy3);
  //MAx2 = DOC_X_POS(Doc->contentsX()+Doc->viewport()->width()-1-2);
  //MAx2 = DOC_Y_POS(Doc->contentsY()+Doc->viewport()->height()-1-2);
  Doc->viewport()->update();
}


/**
 * @brief MouseActions::MMoveSelect Paints a rectangle for select area.
 * @param Doc
 * @param Event
 * Set size of selection box.
 * Relative to first mouse press location stored in MAx1, MAy1.
 * Box size is set on MAx2, MAy2.
 */
void MouseActions::MMoveSelect(Schematic *Doc, QMouseEvent *Event)
{
  //qDebug() << "MMoveSelect " << "select area";
  QPointF pos = Doc->mapToScene(Event->pos());
  MAx2 = pos.x() - MAx1;
  MAy2 = pos.y() - MAy1;
  if(isMoveEqual) {    // x and y size must be equal ?
    if(abs(MAx2) > abs(MAy2)) {
      if(MAx2<0) MAx2 = -abs(MAy2); else MAx2 = abs(MAy2);
    }
    else { if(MAy2<0) MAy2 = -abs(MAx2); else MAy2 = abs(MAx2); }
  }

  TODO("draw selection ruberband");
}

// -----------------------------------------------------------
void MouseActions::MMoveResizePainting(Schematic *Doc, QMouseEvent *Event)
{
  QPointF pos = Doc->mapToScene(Event->pos());
  MAx1 = pos.x();
  MAy1 = pos.y();
  Doc->setOnGrid(MAx1, MAy1);
  ((Painting*)focusElement)->MouseResizeMoving(MAx1, MAy1, Doc);
}

/*!
 * \brief MouseActions::MMoveMoving
 *  Moves components by keeping the mouse button pressed.
 *
 * \param Doc is a pointer to a Schematic
 * \param Event is a QMouseEvent
 *
 *  Set Event to grid.
 *  Copy elements being moved into a movingElements list.
 *  Handle dx/dy of Wires (Nodes and Labels) in movingElements.
 *  Handle center of other Elements in movingElements.
 *  Set next Move and Release actions.
 */
void MouseActions::MMoveMoving(Schematic *Doc, QMouseEvent *Event)
{
  QPointF pos = Doc->mapToScene(Event->pos());
  MAx2 = pos.x();
  MAy2 = pos.y();

  Doc->setOnGrid(MAx2, MAy2);
  /// MAx1, MAy1 set on MPressSelect (?)
  MAx3 = MAx1 = MAx2 - MAx1;
  MAy3 = MAy1 = MAy2 - MAy1;

  movingElements.clear();
  Doc->copySelectedElements(&movingElements);
  Doc->viewport()->repaint();

  Wire *pw;
  // Changes the position of all moving elements by dx/dy
  for(Element *pe=movingElements.first(); pe!=0; pe=movingElements.next()) {
    if(pe->ElemType == isWire) {
      pw = (Wire*)pe;   // connecting wires are not moved completely

      if(((uintptr_t)pw->Port1) > 3) { pw->x1 += MAx1;  pw->y1 += MAy1; }
      else {  if((uintptr_t)(pw->Port1) & 1) { pw->x1 += MAx1; }
              if((uintptr_t)(pw->Port1) & 2) { pw->y1 += MAy1; } }

      if(((uintptr_t)pw->Port2) > 3) { pw->x2 += MAx1;  pw->y2 += MAy1; }
      else {  if((uintptr_t)(pw->Port2) & 1) pw->x2 += MAx1;
              if((uintptr_t)(pw->Port2) & 2) pw->y2 += MAy1; }

      if(pw->Label) {      // root of node label must lie on wire
        if(pw->Label->cx < pw->x1) pw->Label->cx = pw->x1;
        if(pw->Label->cy < pw->y1) pw->Label->cy = pw->y1;
        if(pw->Label->cx > pw->x2) pw->Label->cx = pw->x2;
        if(pw->Label->cy > pw->y2) pw->Label->cy = pw->y2;
      }

    }
    else pe->setCenter(MAx1, MAy1, true);

  /// \todo pe->paintScheme(Doc);
  }

  drawn = true;
  MAx1 = MAx2;
  MAy1 = MAy2;
  QucsMain->MouseMoveAction = &MouseActions::MMoveMoving2;
  QucsMain->MouseReleaseAction = &MouseActions::MReleaseMoving;

}

// -----------------------------------------------------------
// Moves components by keeping the mouse button pressed.
void MouseActions::MMoveMoving2(Schematic *Doc, QMouseEvent *Event)
{
  QPointF pos = Doc->mapToScene(Event->pos());
  MAx2 = pos.x();
  MAy2 = pos.y();

  //Element *pe;
  //if(drawn) // erase old scheme
    //for(pe = movingElements.first(); pe != 0; pe = movingElements.next())
      ///\todo pe->paintScheme(Doc);
//      if(pe->Type == isWire)  if(((Wire*)pe)->Label)
//        if(!((Wire*)pe)->Label->isSelected)
//          ((Wire*)pe)->Label->paintScheme(&painter);

  drawn = true;
  if (!Event->modifiers().testFlag(Qt::ControlModifier))
    Doc->setOnGrid(MAx2, MAy2);  // use grid only if CTRL key not pressed
  MAx1 = MAx2 - MAx1;
  MAy1 = MAy2 - MAy1;
  MAx3 += MAx1;  MAy3 += MAy1;   // keep track of the complete movement

  moveElements(&movingElements, MAx1, MAy1);  // moves elements by MAx1/MAy1

  // paint afterwards to avoid conflict between wire and label painting
  //for(pe = movingElements.first(); pe != 0; pe = movingElements.next())
    ///\ todo pe->paintScheme(Doc);
//    if(pe->Type == isWire)  if(((Wire*)pe)->Label)
//      if(!((Wire*)pe)->Label->isSelected)
//        ((Wire*)pe)->Label->paintScheme(&painter);

  MAx1 = MAx2;
  MAy1 = MAy2;
}


/**
 * @brief MouseActions::MMovePaste Moves components after paste from clipboard.
 * @param Doc
 * @param Event
 */
void MouseActions::MMovePaste(Schematic *Doc, QMouseEvent *Event)
{
  QPointF pos = Doc->mapToScene(Event->pos());
  MAx1 = pos.x();
  MAy1 = pos.y();
  moveElements(Doc,MAx1,MAy1);
  paintElementsScheme(Doc);

  drawn = true;
  QucsMain->MouseMoveAction = &MouseActions::MMoveMoving2;
  QucsMain->MouseReleaseAction = &MouseActions::MReleasePaste;
}

// -----------------------------------------------------------
// Moves scroll bar of diagram (e.g. tabular) according the mouse cursor.
void MouseActions::MMoveScrollBar(Schematic *Doc, QMouseEvent *Event)
{
  TabDiagram *d = (TabDiagram*)focusElement;
  QPointF pos = Doc->mapToScene(Event->pos());
  int x = pos.x();
  int y = pos.y();

  if(d->scrollTo(MAx2, x - MAx1, y - MAy1)) {
    Doc->setChanged(true, true, 'm'); // 'm' = only the first time

// FIXME #warning QPainter p(Doc->viewport());
    // FIXME #warning ViewPainter Painter;
    // FIXME #warning Painter.init(&p, Doc->Scale, -Doc->ViewX1, -Doc->ViewY1,
// FIXME #warning                  Doc->contentsX(), Doc->contentsY());
// FIXME #warning     Painter.fillRect(d->cx-d->x1, d->cy-d->y2, d->x2+d->x1, d->y2+d->y1,
// FIXME #warning                      QucsSettings.BGColor);
// FIXME #warning     d->paint(&Painter);
  }
}


/**
* @brief MouseActions::MMoveDelete
*   Paints a cross under the mouse cursor to show the delete mode.
* @param Doc Schematic document
* @param Event
*/
void MouseActions::MMoveDelete(Schematic *Doc, QMouseEvent *Event)
{
  QPointF pos = Doc->mapToScene(Event->pos());
  MAx3 = pos.x();
  MAy3 = pos.y();

  Doc->mouseCursor->setCursorType(MouseCursor::CursorDelete);
  Doc->mouseCursor->setPos(Event->pos());
}


/**
 * @brief MouseActions::MMoveLabel Paints a label above the mouse cursor for "set wire label".
 * @param Doc
 * @param Event
 */
void MouseActions::MMoveLabel(Schematic *Doc, QMouseEvent *Event)
{
  QPointF pos = Doc->mapToScene(Event->pos());
  MAx3 = pos.x();
  MAy3 = pos.y();

  Doc->mouseCursor->setCursorType(MouseCursor::CursorLabel);
  Doc->mouseCursor->setPos(Event->pos());
}


/**
 * @brief MouseActions::MMoveMarker Paints a triangle above the mouse for "set marker on graph"
 * @param Doc
 * @param Event
 */
void MouseActions::MMoveMarker(Schematic *Doc, QMouseEvent *Event)
{
  QPointF pos = Doc->mapToScene(Event->pos());
  MAx3 = pos.x();
  MAy3 = pos.y();

  Doc->mouseCursor->setCursorType(MouseCursor::CursorMarker);
  Doc->mouseCursor->setPos(Event->pos());
}


/**
 * @brief MouseActions::MMoveMirrorX Paints rounded "mirror about y axis" mouse cursor
 * @param Doc
 * @param Event
 */
void MouseActions::MMoveMirrorY(Schematic *Doc, QMouseEvent *Event)
{
  QPointF pos = Doc->mapToScene(Event->pos());
  MAx3 = pos.x();
  MAy3 = pos.y();

  Doc->mouseCursor->setCursorType(MouseCursor::CursorMirrorX);
  Doc->mouseCursor->setPos(Event->pos());
}


/**
 * @brief MouseActions::MMoveMirrorX Paints rounded "mirror about x axis" mouse cursor
 * @param Doc
 * @param Event
 */
void MouseActions::MMoveMirrorX(Schematic *Doc, QMouseEvent *Event)
{
  QPointF pos = Doc->mapToScene(Event->pos());
  MAx3 = pos.x();
  MAy3 = pos.y();

  Doc->mouseCursor->setCursorType(MouseCursor::CursorMirrorY);
  Doc->mouseCursor->setPos(Event->pos());
}

/**
 * @brief MouseActions::MMoveRotate Paints "rotate" mouse cursor
 * @param Doc
 * @param Event
 */
void MouseActions::MMoveRotate(Schematic *Doc, QMouseEvent *Event)
{
  QPointF pos = Doc->mapToScene(Event->pos());
  MAx3 = pos.x();
  MAy3 = pos.y();

  Doc->mouseCursor->setCursorType(MouseCursor::CursorRotate);
  Doc->mouseCursor->setPos(Event->pos());
}


/**
 * @brief MouseActions::MMoveActivate Paints a crossed box mouse cursor to "(de)activate" components.
 * @param Doc
 * @param Event
 */
void MouseActions::MMoveActivate(Schematic *Doc, QMouseEvent *Event)
{
  QPointF pos = Doc->mapToScene(Event->pos());
  MAx3 = pos.x();
  MAy3 = pos.y();

  Doc->mouseCursor->setCursorType(MouseCursor::CursorActivate);
  Doc->mouseCursor->setPos(Event->pos());
}


/**
 * @brief MouseActions::MMoveOnGrid Paints a grid beside the mouse cursor, put "on grid" mode.
 * @param Doc
 * @param Event
 */
void MouseActions::MMoveOnGrid(Schematic *Doc, QMouseEvent *Event)
{
  QPointF pos = Doc->mapToScene(Event->pos());
  MAx3 = pos.x();
  MAy3 = pos.y();

  Doc->mouseCursor->setCursorType(MouseCursor::CursorOnGrid);
  Doc->mouseCursor->setPos(Event->pos());
}


/**
 * @brief MouseActions::MMoveMoveTextB Paints mouse symbol for "move component text" mode.
 * @param Doc
 * @param Event
 */
void MouseActions::MMoveMoveTextB(Schematic *Doc, QMouseEvent *Event)
{
  QPointF pos = Doc->mapToScene(Event->pos());
  MAx3 = pos.x();
  MAy3 = pos.y();

  Doc->mouseCursor->setCursorType(MouseCursor::CursorMoveTextB);
  Doc->mouseCursor->setPos(Event->pos());
}


/**
 * @brief MouseActions::MMoveMoveText Paint rectangle around component text being mouse moved
 * @param Doc
 * @param Event
 */
void MouseActions::MMoveMoveText(Schematic *Doc, QMouseEvent *Event)
{
  QPointF pos = Doc->mapToScene(Event->pos());
  int newX = pos.x();
  int newY = pos.y();
  MAx1 += newX - MAx3;
  MAy1 += newY - MAy3;
  MAx3  = newX;
  MAy3  = newY;

  /// \todo Doc->PostPaintEvent (_Rect, MAx1, MAy1, MAx2, MAy2);
  /* MPressMoveText sets the MAxy cache vars, this was supposed to draw a rectangle
   * This is not really a cursor, isn't it more down to the paint() of the text?
  */
  Doc->mouseCursor->setCursorType(MouseCursor::CursorMoveText);
  Doc->mouseCursor->setPos(Event->pos());
}


/**
 * @brief MouseActions::MMoveZoomIn Paints symbol beside the mouse to show the "Zoom in" modus.
 * @param Doc
 * @param Event
 */
void MouseActions::MMoveZoomIn(Schematic *Doc, QMouseEvent *Event)
{
  QPointF pos = Doc->mapToScene(Event->pos());
  MAx3  = pos.x();
  MAy3  = pos.y();

  Doc->mouseCursor->setCursorType(MouseCursor::CursorZoomIn);
  Doc->mouseCursor->setPos(pos);
}


// ************************************************************************
// **********                                                    **********
// **********    Functions for serving mouse button clicking     **********
// **********                                                    **********
// ************************************************************************

// Is called from several MousePress functions to show right button menu.
void MouseActions::rightPressMenu(Schematic *Doc, QMouseEvent *Event, float fX, float fY)
{
  MAx1 = int(fX);
  MAy1 = int(fY);
  focusElement = Doc->selectElement(fX, fY, false);

  if(focusElement)  // remove special function (4 least significant bits)
    focusElement->ElemType &= isSpecialMask;


  // define menu
  ComponentMenu->clear();
  while(true) {
    if(focusElement) {
      focusElement->ElemSelected = true;
      QAction *editProp = new QAction(QObject::tr("Edit Properties"), QucsMain);
      QucsMain->connect(editProp, SIGNAL(triggered()), SLOT(slotEditElement()));
      ComponentMenu->addAction(editProp);

      if((focusElement->ElemType & isComponent) == 0) break;
    }
    else {
      ComponentMenu->addAction(QucsMain->symEdit);
      ComponentMenu->addAction(QucsMain->fileSettings);
    }
    if(!QucsMain->moveText->isChecked())
      ComponentMenu->addAction(QucsMain->moveText);
    break;
  }
  while(true) {
    if(focusElement)
      if(focusElement->ElemType == isGraph) break;
    if(!QucsMain->onGrid->isChecked())
      ComponentMenu->addAction(QucsMain->onGrid);
    ComponentMenu->addAction(QucsMain->editCopy);
    if(!QucsMain->editPaste->isChecked())
      ComponentMenu->addAction(QucsMain->editPaste);
    break;
  }

  while (true) {
    if (focusElement) {
      if (focusElement->ElemType == isDiagram) {
        QAction *imgExport = new QAction(QObject::tr("Export as image"), QucsMain);
        QucsMain->connect(imgExport, SIGNAL(triggered()), SLOT(slotSaveDiagramToGraphicsFile()));
        ComponentMenu->addAction(imgExport);
      }
    }
    break;
  }

  if(!QucsMain->editDelete->isChecked())
    ComponentMenu->addAction(QucsMain->editDelete);
  if(focusElement) if(focusElement->ElemType == isMarker) {
    ComponentMenu->addSeparator();
    QString s = QObject::tr("power matching");
    if( ((Marker*)focusElement)->pGraph->Var == "Sopt" )
      s = QObject::tr("noise matching");

    QAction *powerMatch = new QAction(s, QucsMain);
    QucsMain->connect(powerMatch, SIGNAL(triggered()), SLOT(slotPowerMatching()));
    ComponentMenu->addAction(powerMatch);
    if( ((Marker*)focusElement)->pGraph->Var.left(2) == "S[" ) {
      QAction *power2Match = new QAction(QObject::tr("2-port matching"), QucsMain);
      QucsMain->connect(power2Match, SIGNAL(triggered()), SLOT(slot2PortMatching()));
      ComponentMenu->addAction(power2Match);
    }
  }
  do {
    if(focusElement) {
      if(focusElement->ElemType == isDiagram) break;
      if(focusElement->ElemType == isGraph) {
        ComponentMenu->addAction(QucsMain->graph2csv);
        break;
      }
    }
    ComponentMenu->addSeparator();
    if(focusElement) if(focusElement->ElemType & isComponent)
      if(!QucsMain->editActivate->isChecked())
        ComponentMenu->addAction(QucsMain->editActivate);
    if(!QucsMain->editRotate->isChecked())
      ComponentMenu->addAction(QucsMain->editRotate);
    if(!QucsMain->editMirror->isChecked())
      ComponentMenu->addAction(QucsMain->editMirror);
    if(!QucsMain->editMirrorY->isChecked())
      ComponentMenu->addAction(QucsMain->editMirrorY);

    // right-click menu to go into hierarchy
    if(focusElement) {
      if(focusElement->elemType() & isComponent)
	if(((Component*)focusElement)->obsolete_model_hack() == "Sub")
      if(!QucsMain->intoH->isChecked())
        ComponentMenu->addAction(QucsMain->intoH);
    }
    // right-click menu to pop out of hierarchy
    if(!focusElement)
      if(!QucsMain->popH->isChecked())
        ComponentMenu->addAction(QucsMain->popH);
  } while(false);

  *focusMEvent = *Event;  // remember event for "edit component" action
  ComponentMenu->popup(Event->globalPos());
  Doc->viewport()->update();
  drawn = false;
}

// -----------------------------------------------------------
void MouseActions::MPressLabel(Schematic *Doc, QMouseEvent*, float fX, float fY)
{
  int x = int(fX), y = int(fY);
  Wire *pw = 0;
  WireLabel *pl=0;
  Node *pn = Doc->selectedNode(x, y);
  if(!pn) {
    pw = Doc->selectedWire(x, y);
    if(!pw) return;
  }

  QString Name, Value;
  Element *pe=0;
  // is wire line already labeled ?
  if(pw) pe = Doc->getWireLabel(pw->Port1);
  else pe = Doc->getWireLabel(pn);
  if(pe) {
    if(pe->ElemType & isComponent) {
      QMessageBox::information(0, QObject::tr("Info"),
                 QObject::tr("The ground potential cannot be labeled!"));
      return;
    }
    pl = ((Conductor*)pe)->Label;
  }

  LabelDialog *Dia = new LabelDialog(pl, Doc);
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
    Doc->setOnGrid(xl, yl);
    // set new name
    if(pw) pw->setName(Name, Value, x-pw->x1 + y-pw->y1, xl, yl);
    else pn->setName(Name, Value, xl, yl);
  }

  Doc->sizeOfAll(Doc->UsedX1, Doc->UsedY1, Doc->UsedX2, Doc->UsedY2);
  Doc->viewport()->update();
  drawn = false;
  Doc->setChanged(true, true);
}

// -----------------------------------------------------------
/*!
 * \brief MouseActions::MPressSelect
 * \param Doc
 * \param Event
 * \param fX
 * \param fY
 *
 * Event handler, in Select mode use presses on the View.
 *
 * Detect if Control modifier is in use.
 *
 * Get the selected element (if any) into focusElement.
 *
 * Set next actions (Release, Move, Press, DoubleClick, ...) for ElemType
 *   - isPaintingResize
 *   - isDiagramResize
 *   - isDiagramHScroll
 *   - isDiagramVScroll
 *   - isComponentText
 *   - isNode
 * and return
 *
 * For all other ElemType.
 * If no focusElement, set next actions to create selection box.
 * If focusElement, set next actions to move selected element.
 */
void MouseActions::MPressSelect(Schematic *Doc, QMouseEvent *Event, float fX, float fY)
{
  bool Ctrl = Event->modifiers().testFlag(Qt::ControlModifier);

  int No=0;
  MAx1 = int(fX);
  MAy1 = int(fY);
  focusElement = Doc->selectElement(fX, fY, Ctrl, &No);
  isMoveEqual = false;   // moving not neccessarily square

  if(focusElement)
    // print define value in hex, see element.h
    qDebug() << "MPressSelect: focusElement->Type" <<  QString("0x%1").arg(focusElement->ElemType, 0, 16);
  else
    qDebug() << "MPressSelect";

  if(focusElement)
  switch(focusElement->ElemType)
  {
    case isPaintingResize:  // resize painting ?
      focusElement->ElemType = isPainting;
      QucsMain->MouseReleaseAction = &MouseActions::MReleaseResizePainting;
      QucsMain->MouseMoveAction = &MouseActions::MMoveResizePainting;
      QucsMain->MousePressAction = 0;
      QucsMain->MouseDoubleClickAction = 0;
      Doc->grabKeyboard();  // no keyboard inputs during move actions
      // Update matching wire label highlighting
      Doc->highlightWireLabels ();
      return;

    case isDiagramResize:  // resize diagram ?
      if(((Diagram*)focusElement)->Name.left(4) != "Rect")
        if(((Diagram*)focusElement)->Name.at(0) != 'T')
          if(((Diagram*)focusElement)->Name != "Curve")
           /* if(((Diagram*)focusElement)->Name != "Waveac")
          if(((Diagram*)focusElement)->Name != "Phasor")*/
            isMoveEqual = true;  // diagram must be square

      focusElement->ElemType = isDiagram;
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

      QucsMain->MouseReleaseAction = &MouseActions::MReleaseResizeDiagram;
      QucsMain->MouseMoveAction = &MouseActions::MMoveSelect;
      QucsMain->MousePressAction = 0;
      QucsMain->MouseDoubleClickAction = 0;
      Doc->grabKeyboard(); // no keyboard inputs during move actions
      // Update matching wire label highlighting
      Doc->highlightWireLabels ();
      return;

    case isDiagramHScroll:  // scroll in tabular ?
      MAy1 = MAx1;

    case isDiagramVScroll:
      focusElement->ElemType = isDiagram;

      No = ((TabDiagram*)focusElement)->scroll(MAy1);

      switch(No)
      {
        case 1:
          Doc->setChanged(true, true, 'm'); // 'm' = only the first time
          break;
        case 2:  // move scroll bar with mouse cursor
          QucsMain->MouseMoveAction = &MouseActions::MMoveScrollBar;
          QucsMain->MousePressAction = 0;
          QucsMain->MouseDoubleClickAction = 0;
          Doc->grabKeyboard();  // no keyboard inputs during move actions

          // Remember inital scroll bar position.
          MAx2 = int(((TabDiagram*)focusElement)->xAxis.limit_min);
          // Update matching wire label highlighting
          Doc->highlightWireLabels ();
          return;
      }
      // Update matching wire label highlighting
      Doc->highlightWireLabels ();
      Doc->viewport()->update();
      drawn = false;
      return;

    case isComponentText:  // property text of component ?
      focusElement->ElemType &= (~isComponentText) | isComponent;

      MAx3 = No;
      QucsMain->slotApplyCompText();
      // Update matching wire label highlighting
      Doc->highlightWireLabels ();
      return;

    case isNode:
      if (QucsSettings.NodeWiring)
      {
        MAx1 = 0;   // paint wire corner first up, then left/right
        MAx3 = focusElement->cx;  // works even if node is not on grid
        MAy3 = focusElement->cy;
        QucsMain->MouseMoveAction = &MouseActions::MMoveWire2;
        QucsMain->MousePressAction = &MouseActions::MPressWire2;
        QucsMain->MouseReleaseAction = 0; // if function is called from elsewhere
        QucsMain->MouseDoubleClickAction = 0;

        formerAction = QucsMain->select; // to restore action afterwards
        QucsMain->activeAction = QucsMain->insWire;

        QucsMain->select->blockSignals(true);
        QucsMain->select->setChecked(false);
        QucsMain->select->blockSignals(false);

        QucsMain->insWire->blockSignals(true);
        QucsMain->insWire->setChecked(true);
        QucsMain->insWire->blockSignals(false);
        // Update matching wire label highlighting
        Doc->highlightWireLabels ();
        return;
      }
  }

  QucsMain->MousePressAction = 0;
  QucsMain->MouseDoubleClickAction = 0;
  Doc->grabKeyboard();  // no keyboard inputs during move actions
  Doc->viewport()->update();
  drawn = false;

  if(focusElement == 0) {
    MAx2 = 0;  // if not clicking on an element => open a rectangle
    MAy2 = 0;
    QucsMain->MouseReleaseAction = &MouseActions::MReleaseSelect2;
    QucsMain->MouseMoveAction = &MouseActions::MMoveSelect;
  }
  else
  {
    // element could be moved
    if(!Ctrl)
    {
      if(!focusElement->ElemSelected)// Don't move selected elements if clicked
        Doc->deselectElements(focusElement); // element was not selected.
      focusElement->ElemSelected = true;
    }
    Doc->setOnGrid(MAx1, MAy1);
    QucsMain->MouseMoveAction = &MouseActions::MMoveMoving;
  }
  // Update matching wire label highlighting
  Doc->highlightWireLabels ();
}

// -----------------------------------------------------------
void MouseActions::MPressDelete(Schematic *Doc, QMouseEvent*, float fX, float fY)
{
  Element *pe = Doc->selectElement(fX, fY, false);
  if(pe)
  {
    pe->ElemSelected = true;
    Doc->deleteElements();

    Doc->sizeOfAll(Doc->UsedX1, Doc->UsedY1, Doc->UsedX2, Doc->UsedY2);
    Doc->viewport()->update();
    drawn = false;
  }
}

// -----------------------------------------------------------
void MouseActions::MPressActivate(Schematic *Doc, QMouseEvent*, float fX, float fY)
{
  MAx1 = int(fX);
  MAy1 = int(fY);
  if(!Doc->activateSpecifiedComponent(MAx1, MAy1)) {
//    if(Event->button() != Qt::LeftButton) return;
    MAx2 = 0;  // if not clicking on a component => open a rectangle
    MAy2 = 0;
    QucsMain->MousePressAction = 0;
    QucsMain->MouseReleaseAction = &MouseActions::MReleaseActivate;
    QucsMain->MouseMoveAction = &MouseActions::MMoveSelect;
  }
  Doc->viewport()->update();
  drawn = false;
}

// -----------------------------------------------------------
void MouseActions::MPressMirrorX(Schematic *Doc, QMouseEvent*, float fX, float fY)
{
  // no use in mirroring wires or diagrams
  Component *c = Doc->selectedComponent(int(fX), int(fY));
  if(c) {
    if(c->Ports.count() < 1) return;  // only mirror components with ports
    c->mirrorX();
    Doc->setCompPorts(c);
  }
  else {
    Painting *p = Doc->selectedPainting(fX, fY);
    if(p == 0) return;
    p->mirrorX();
  }

  Doc->viewport()->update();
  drawn = false;
  Doc->setChanged(true, true);
}

// -----------------------------------------------------------
void MouseActions::MPressMirrorY(Schematic *Doc, QMouseEvent*, float fX, float fY)
{
  // no use in mirroring wires or diagrams
  Component *c = Doc->selectedComponent(int(fX), int(fY));
  if(c) {
    if(c->Ports.count() < 1) return;  // only mirror components with ports
    c->mirrorY();
    Doc->setCompPorts(c);
  }
  else {
    Painting *p = Doc->selectedPainting(fX, fY);
    if(p == 0) return;
    p->mirrorY();
  }

  Doc->viewport()->update();
  drawn = false;
  Doc->setChanged(true, true);
}

// -----------------------------------------------------------
void MouseActions::MPressRotate(Schematic *Doc, QMouseEvent*, float fX, float fY)
{
  Element *e = Doc->selectElement(int(fX), int(fY), false);
  if(e == 0) return;
  e->ElemType &= isSpecialMask;  // remove special functions


  WireLabel *pl;
  int x1, y1, x2, y2;
//  e->isSelected = false;
  switch(e->ElemType) {
    case isComponent:
    case isAnalogComponent:
    case isDigitalComponent:
      if(((Component*)e)->Ports.count() < 1)
        break;  // do not rotate components without ports
      ((Component*)e)->rotate();
      Doc->setCompPorts((Component*)e);
      // enlarge viewarea if component lies outside the view
      ((Component*)e)->entireBounds(x1,y1,x2,y2, Doc->textCorr());
      Doc->enlargeView(x1, y1, x2, y2);
      break;

    case isWire:
      pl = ((Wire*)e)->Label;
      ((Wire*)e)->Label = 0;    // prevent label to be deleted
      Doc->Wires->setAutoDelete(false);
      Doc->deleteWire((Wire*)e);
      ((Wire*)e)->Label = pl;
      ((Wire*)e)->rotate();
      Doc->setOnGrid(e->x1, e->y1);
      Doc->setOnGrid(e->x2, e->y2);
      if(pl)  Doc->setOnGrid(pl->cx, pl->cy);
      Doc->insertWire((Wire*)e);
      Doc->Wires->setAutoDelete(true);
      if (Doc->Wires->containsRef ((Wire*)e))
        Doc->enlargeView(e->x1, e->y1, e->x2, e->y2);
      break;

    case isPainting:
      ((Painting*)e)->rotate();
      // enlarge viewarea if component lies outside the view
      ((Painting*)e)->Bounding(x1,y1,x2,y2);
      Doc->enlargeView(x1, y1, x2, y2);
      break;

    default:
      return;
  }
  Doc->viewport()->update();
  drawn = false;
  Doc->setChanged(true, true);
}

/*!
 * \brief MouseActions::MPressElement
 * \param Doc
 * \param Event
 * \param x
 * \param y
 *
 * Event handler, an Element is selected for insersion and a mouse
 * press takes place on the View.
 *
 * Handle the insertion of Elements, component, diagram and painting
 * into the schematic.
 */
void MouseActions::MPressElement(Schematic *Doc, QMouseEvent *Event, float x, float y)
{
  if(selElem == 0) return;

  int x1, y1, x2, y2, rot;
  int gx, gy;
  if(selElem->ElemType & isComponent) {
    Component *Comp = (Component*)selElem;
//    qDebug() << "+-+ got to switch:" << Comp->Name;
    QString entryName = Comp->name();

    switch(Event->button()) {
      case Qt::LeftButton :
	// left mouse button inserts component into the schematic
	// give the component a pointer to the schematic it's a
	// part of
	Comp->setSchematic (Doc);
	Comp->textSize(x1, y1);

	/// insert Component into scene
	/// snap to grid points
	/// set center position before nodes are inserted (?)
	gx = x;
	gy = y;
	Doc->setOnGrid(gx, gy);
	Comp->setPos(gx,gy);
	Doc->scene->addItem(Comp);
	/// clear flag, paint whole Element, not just outline
	Comp->drawScheme = false;

	// Note: insertCopmponents does increment  name1 -> name2
	Doc->insertComponent(Comp);
	Comp->textSize(x2, y2);
	if(Comp->tx < Comp->x1) Comp->tx -= x2 - x1;

	// enlarge viewarea if component lies outside the view
	Comp->entireBounds(x1,y1,x2,y2, Doc->textCorr());
	Doc->enlargeView(x1, y1, x2, y2);

	Doc->viewport()->update();
	Doc->setChanged(true, true);
	rot = Comp->rotated;

	// handle static and dynamic components
	if (Module::vaComponents.contains(entryName)){
	  QString filename = Module::vaComponents[entryName];
	  Comp = dynamic_cast<vacomponent*>(Comp)->newOne(filename); //va component
	  qDebug() << "   => recast = Comp;" << Comp->name() << "filename: " << filename;
	}
	else {
	  Comp = Comp->newOne(); // static component is used, so create a new one
	}
	rot -= Comp->rotated;
	rot &= 3;
	while(rot--) Comp->rotate(); // keep last rotation for single component
	break;

      case Qt::RightButton :  // right mouse button rotates the component
	if(Comp->Ports.count() == 0)
	  break;  // do not rotate components without ports
	Comp->rotate();
	Doc->viewport()->update();
	break;

      default: ;   // avoids compiler warnings
    }

    // keep current Element selected
    selElem = Comp;
    // get ready to paint selElem scheme on next move event
    drawn = false;
    return;
  } // isComponent

  else if(selElem->ElemType == isDiagram) {
    if(Event->button() != Qt::LeftButton) return;

    Diagram *Diag = (Diagram*)selElem;
    // dialog is Qt::WDestructiveClose !!!
    DiagramDialog *dia =
       new DiagramDialog(Diag, Doc);
    // don't insert if dialog canceled
    if(dia->exec() == QDialog::Rejected) {
      Doc->viewport()->update();
      drawn = false;
      return;
    }
    // add to list
    Doc->Diagrams->append(Diag);
    // clear flag, draw whole Diagram, not only scheme
    selElem->drawScheme = false;
    /// \todo Doc->enlargeView(Diag->cx, Diag->cy-Diag->y2, Diag->cx+Diag->x2, Diag->cy);
    Doc->setChanged(true, true);   // document has been changed
    Doc->viewport()->update();
    // the selEleme is used, so create a new one;
    Diag = Diag->newOne();
    selElem = Diag;
    // clear draw flag to trigger new insert on MMoveElement
    drawn = false;
    return;
  } // isDiagram

  else if(selElem->ElemType == isPainting) {
    // propagate press event
    bool finalPress = ((Painting*)selElem)->MousePressing();

    // wait till all press events took place
    // 1 press: graphictext
    // 2 press: line, arrow, ellipse, rectangle
    // 4 press: ellipsearc
    // Note that MMoveElement keeps snapping to grid and updatig cx,cy
    if( finalPress )  {
      // add Element to list
      Doc->Paintings->append((Painting*)selElem);
      // clear scheme flag
      selElem->drawScheme = false;
      // mark document as changed
      Doc->setChanged(true, true);
      // keep a new Element selected
      selElem = ((Painting*)selElem)->newOne();
      // clear draw flag to trigger new insert on MMoveElement
      drawn = false;

      Doc->viewport()->update();
      /// \todo fit view
      //Doc->enlargeView(x1, y1, x2, y2);
    }
  } // isPainting
}


/**
 * @brief MouseActions::MPressWire1 Is called if starting point of wire is pressed
 * @param Doc
 * @param fX
 * @param fY
 */
void MouseActions::MPressWire1(Schematic *Doc, QMouseEvent*, float fX, float fY)
{
  //Doc->PostPaintEvent (_DotLine);
  //Doc->PostPaintEvent (_NotRop);
  //if(drawn) {
#if 0  //ALYS - it draws some garbage, not deleted because of possible questions
	Doc->PostPaintEvent (_Line, 0, MAy3, MAx2, MAy3); // erase old mouse cross
    Doc->PostPaintEvent (_Line, MAx3, 0, MAx3, MAy2);
#endif
  //}
  //drawn = false;

  MAx1 = 0;   // paint wire corner first up, then left/right
  MAx3 = int(fX);
  MAy3 = int(fY);
  Doc->setOnGrid(MAx3, MAy3);

//ALYS - draw aiming cross
  /// \todo paintAim(Doc,MAx3, MAy3);
//#######################

  formerAction = 0; // keep wire action active after first wire finished
  QucsMain->MouseMoveAction = &MouseActions::MMoveWire2;
  QucsMain->MousePressAction = &MouseActions::MPressWire2;
  // Double-click action is set in "MMoveWire2" to not initiate it
  // during "Wire1" actions.
  Doc->viewport()->update();
}


/**
 * @brief MouseActions::MPressWire2 Is called if ending point of wire is pressed
 * @param Doc
 * @param Event
 * @param fX
 * @param fY
 */
void MouseActions::MPressWire2(Schematic *Doc, QMouseEvent *Event, float fX, float fY)
{

  int set1 = 0, set2 = 0;
  switch(Event->button()) {
  case Qt::LeftButton :
    if(MAx1 == 0) { // which wire direction first ?
      if(MAy2 != MAy3)
        set1 = Doc->insertWire(new Wire(MAx3, MAy3, MAx3, MAy2));
      if(MAx2 != MAx3) {
        set2 = set1;
        set1 = Doc->insertWire(new Wire(MAx3, MAy2, MAx2, MAy2));
      }
    }
    else {
      if(MAx2 != MAx3)
        set1 = Doc->insertWire(new Wire(MAx3, MAy3, MAx2, MAy3));
      if(MAy2 != MAy3) {
        set2 = set1;
        set1 = Doc->insertWire(new Wire(MAx2, MAy3, MAx2, MAy2));
      }
    }

    if(set1 & 2) {
      // if last port is connected, then...
      if(formerAction) {
        // ...restore old action
        QucsMain->select->setChecked(true);
      }
      else {
        // ...start a new wire
        QucsMain->MouseMoveAction = &MouseActions::MMoveWire1;
        QucsMain->MousePressAction = &MouseActions::MPressWire1;
        QucsMain->MouseDoubleClickAction = 0;
      }
    }

    //ALYS: excessive update. end of function does it.
	//Doc->viewport()->update();

	drawn = false;
    if(set1 | set2) Doc->setChanged(true, true);
    MAx3 = MAx2;
    MAy3 = MAy2;
    break;

   /// \todo document right mouse button changes the wire corner
  case Qt::RightButton :
      TODO("Sort out paintAim and GhostLine")

#if 0
	//ALYS - old code preserved because isn't clear - what it was???
	//looks like deletion via painting.
	//i'll delete it after possible clarification from team
	if(MAx1 == 0) {
      Doc->PostPaintEvent (_Line, MAx3, MAy3, MAx3, MAy2); // erase old
      Doc->PostPaintEvent (_Line, MAx3, MAy2, MAx2, MAy2); // erase old
    }
    else {
      Doc->PostPaintEvent (_Line, MAx3, MAy3, MAx2, MAy3); // erase old
      Doc->PostPaintEvent (_Line, MAx2, MAy3, MAx2, MAy2); // erase old
    }
#endif

    MAx2  = int(fX);
    MAy2  = int(fY);
    Doc->setOnGrid(MAx2, MAy2);

    MAx1 ^= 1;    // change the painting direction of wire corner
	if(MAx1 == 0) {
		/// \todo paintGhostLineV(Doc,MAx3,MAy3,MAy2);
		///paintGhostLineH(Doc,MAx3,MAy2,MAx2);
    }
    else {
                /// \todo paintGhostLineH(Doc,MAx3,MAy3,MAx2);
                //paintGhostLineV(Doc,MAx2,MAy3,MAy2);
    }
    break;

  default: ;    // avoids compiler warnings
  }

  /// \todo paintAim(Doc,MAx2,MAy2); //ALYS - added missed aiming cross
  Doc->viewport()->update();
}

// -----------------------------------------------------------
// Is called for setting a marker on a diagram's graph
void MouseActions::MPressMarker(Schematic *Doc, QMouseEvent*, float fX, float fY)
{
  MAx1 = int(fX);
  MAy1 = int(fY);
  Marker *pm = Doc->setMarker(MAx1, MAy1);

  if(pm) {
    assert(pm->diag());
    int x0 = pm->diag()->cx;
    int y0 = pm->diag()->cy;
    Doc->enlargeView(x0+pm->x1, y0-pm->y1-pm->y2, x0+pm->x1+pm->x2, y0-pm->y1);
  }
  Doc->viewport()->update();
  drawn = false;
}

// -----------------------------------------------------------
void MouseActions::MPressOnGrid(Schematic *Doc, QMouseEvent*, float fX, float fY)
{
  Element *pe = Doc->selectElement(fX, fY, false);
  if(pe)
  {
    pe->ElemType &= isSpecialMask;  // remove special functions (4 lowest bits)

    // onGrid is toggle action -> no other element can be selected
    pe->ElemSelected = true;
    Doc->elementsOnGrid();

    Doc->sizeOfAll(Doc->UsedX1, Doc->UsedY1, Doc->UsedX2, Doc->UsedY2);
    // Update matching wire label highlighting
    Doc->highlightWireLabels ();
    Doc->viewport()->update();
    drawn = false;
  }

}

// -----------------------------------------------------------
void MouseActions::MPressMoveText(Schematic *Doc, QMouseEvent*, float fX, float fY)
{
  MAx1 = int(fX);
  MAy1 = int(fY);
  focusElement = Doc->selectCompText(MAx1, MAy1, MAx2, MAy2);

  if(focusElement) {
    MAx3 = MAx1;
    MAy3 = MAy1;
    MAx1 = ((Component*)focusElement)->cx + ((Component*)focusElement)->tx;
    MAy1 = ((Component*)focusElement)->cy + ((Component*)focusElement)->ty;
    Doc->viewport()->update();
    drawn = false;
    QucsMain->MouseMoveAction = &MouseActions::MMoveMoveText;
    QucsMain->MouseReleaseAction = &MouseActions::MReleaseMoveText;
    Doc->grabKeyboard();  // no keyboard inputs during move actions
  }
}

// -----------------------------------------------------------
void MouseActions::MPressZoomIn(Schematic *Doc, QMouseEvent*, float fX, float fY)
{
  qDebug() << "zoom into box";
  MAx1 = int(fX);
  MAy1 = int(fY);
  MAx2 = 0;  // clear rectangle size
  MAy2 = 0;

  QucsMain->MouseMoveAction = &MouseActions::MMoveSelect;
  QucsMain->MouseReleaseAction = &MouseActions::MReleaseZoomIn;
  Doc->grabKeyboard();  // no keyboard inputs during move actions
  Doc->viewport()->update();
  drawn = false;
}


// ***********************************************************************
// **********                                                   **********
// **********    Functions for serving mouse button releasing   **********
// **********                                                   **********
// ***********************************************************************
void MouseActions::MReleaseSelect(Schematic *Doc, QMouseEvent *Event)
{
  bool ctrl = Event->modifiers().testFlag(Qt::ControlModifier);

  if(!ctrl) Doc->deselectElements(focusElement);

  if(focusElement)  if(Event->button() == Qt::LeftButton)
    if(focusElement->ElemType == isWire) {
      Doc->selectWireLine(focusElement, ((Wire*)focusElement)->Port1, ctrl);
      Doc->selectWireLine(focusElement, ((Wire*)focusElement)->Port2, ctrl);
    }

  Doc->releaseKeyboard();  // allow keyboard inputs again
  QucsMain->MousePressAction = &MouseActions::MPressSelect;
  QucsMain->MouseReleaseAction = &MouseActions::MReleaseSelect;
  QucsMain->MouseDoubleClickAction = &MouseActions::MDoubleClickSelect;
  QucsMain->MouseMoveAction = 0;   // no element moving
  Doc->highlightWireLabels ();
  Doc->viewport()->update();
  drawn = false;
}

// -----------------------------------------------------------
// Is called after the rectangle for selection is released.
void MouseActions::MReleaseSelect2(Schematic *Doc, QMouseEvent *Event)
{
  if(Event->button() != Qt::LeftButton) return;

  bool Ctrl = Event->modifiers().testFlag(Qt::ControlModifier);

  // selects all elements within the rectangle
  Doc->selectElements(MAx1, MAy1, MAx1+MAx2, MAy1+MAy2, Ctrl);

  Doc->releaseKeyboard();  // allow keyboard inputs again
  QucsMain->MouseMoveAction = 0;
  QucsMain->MousePressAction = &MouseActions::MPressSelect;
  QucsMain->MouseReleaseAction = &MouseActions::MReleaseSelect;
  QucsMain->MouseDoubleClickAction = &MouseActions::MDoubleClickSelect;
  Doc->highlightWireLabels ();
  Doc->viewport()->update();
  drawn = false;
}

// -----------------------------------------------------------
void MouseActions::MReleaseActivate(Schematic *Doc, QMouseEvent *Event)
{
  if(Event->button() != Qt::LeftButton) return;

  // activates all components within the rectangle
  Doc->activateCompsWithinRect(MAx1, MAy1, MAx1+MAx2, MAy1+MAy2);

  QucsMain->MouseMoveAction = &MouseActions::MMoveActivate;
  QucsMain->MousePressAction = &MouseActions::MPressActivate;
  QucsMain->MouseReleaseAction = 0;
  QucsMain->MouseDoubleClickAction = 0;
  Doc->highlightWireLabels ();
  Doc->viewport()->update();
  drawn = false;
}

// -----------------------------------------------------------
// Is called after moving selected elements.
void MouseActions::MReleaseMoving(Schematic *Doc, QMouseEvent*)
{
  // Allow all mouse buttons, because for others than the left one,
  // a menu has already created.
  endElementMoving(Doc, &movingElements);
  Doc->releaseKeyboard();  // allow keyboard inputs again

  QucsMain->MouseMoveAction = 0;
  QucsMain->MousePressAction = &MouseActions::MPressSelect;
  QucsMain->MouseReleaseAction = &MouseActions::MReleaseSelect;
  QucsMain->MouseDoubleClickAction = &MouseActions::MDoubleClickSelect;
}

// -----------------------------------------------------------
void MouseActions::MReleaseResizeDiagram(Schematic *Doc, QMouseEvent *Event)
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
  foreach(Graph *pg, pd->Graphs)
    foreach(Marker *pm, pg->Markers) {
      pm->x1 += MAx3;      // correct changes due to move of diagram corner
      pm->y1 += MAy3;
    }

  int x1, x2, y1, y2;
  pd->Bounding(x1, x2, y1, y2);
  Doc->enlargeView(x1, x2, y1, y2);

  QucsMain->MouseMoveAction = 0;
  QucsMain->MousePressAction = &MouseActions::MPressSelect;
  QucsMain->MouseReleaseAction = &MouseActions::MReleaseSelect;
  QucsMain->MouseDoubleClickAction = &MouseActions::MDoubleClickSelect;
  Doc->releaseKeyboard();  // allow keyboard inputs again

  Doc->viewport()->update();
  drawn = false;
  Doc->setChanged(true, true);
}

// -----------------------------------------------------------
void MouseActions::MReleaseResizePainting(Schematic *Doc, QMouseEvent *Event)
{
  if(Event->button() != Qt::LeftButton) return;

  QucsMain->MouseMoveAction = 0;
  QucsMain->MousePressAction = &MouseActions::MPressSelect;
  QucsMain->MouseReleaseAction = &MouseActions::MReleaseSelect;
  QucsMain->MouseDoubleClickAction = &MouseActions::MDoubleClickSelect;
  Doc->releaseKeyboard();  // allow keyboard inputs again

  Doc->viewport()->update();
  drawn = false;
  Doc->setChanged(true, true);
}

// -----------------------------------------------------------
void MouseActions::paintElementsScheme(Schematic *p)
{
  TODO("is paintElementsScheme needed?")
  //Element *pe;
  //for(pe = movingElements.first(); pe != 0; pe = movingElements.next())
    /// \todo drawScheme maybe? pe->paintScheme(p);

}

// -----------------------------------------------------------
void MouseActions::moveElements(Schematic *Doc, int& x1, int& y1)
{
  Element *pe;
  Doc->setOnGrid(x1, y1);

  for(pe=movingElements.first(); pe!=0; pe=movingElements.next()) {
    if(pe->ElemType & isLabel) {
      pe->cx += x1;  pe->x1 += x1;
      pe->cy += y1;  pe->y1 += y1;
    }
    else
      pe->setCenter(x1, y1, true);
  }
}

// -----------------------------------------------------------
void MouseActions::rotateElements(Schematic *Doc, int& x1, int& y1)
{
  int x2, y2;
  Element *pe;
  Doc->setOnGrid(x1, y1);

  for(pe = movingElements.first(); pe != 0; pe = movingElements.next()) {
    switch(pe->ElemType) {
    case isComponent:
    case isAnalogComponent:
    case isDigitalComponent:
      ((Component*)pe)->rotate(); // rotate !before! rotating the center
      x2 = x1 - pe->cx;
      pe->setCenter(pe->cy - y1 + x1, x2 + y1);
      break;
    case isWire:
      x2     = pe->x1;
      pe->x1 = pe->y1 - y1 + x1;
      pe->y1 = x1 - x2 + y1;
      x2     = pe->x2;
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
  }
}

// -----------------------------------------------------------
void MouseActions::MReleasePaste(Schematic *Doc, QMouseEvent *Event)
{
  int x1, y1, x2, y2, rot;
  QFileInfo Info(Doc->DocName);
  QPointF pos = Doc->mapToScene(Event->pos());

  Element *pe;
  switch(Event->button()) {
  case Qt::LeftButton :
    // insert all moved elements into document
    for(pe = movingElements.first(); pe!=0; pe = movingElements.next()) {
      pe->ElemSelected = false;
      switch(pe->ElemType) {
	case isWire:
	  if(pe->x1 == pe->x2) if(pe->y1 == pe->y2)  break;
	  Doc->insertWire((Wire*)pe);
	  if (Doc->Wires->containsRef ((Wire*)pe))
	    Doc->enlargeView(pe->x1, pe->y1, pe->x2, pe->y2);
	  else pe = NULL;
	  break;
	case isDiagram:
      Doc->Diagrams->append((Diagram*)pe);
      ((Diagram*)pe)->loadGraphData(Info.path() + QDir::separator() +
					Doc->DataSet);
	  Doc->enlargeView(pe->cx, pe->cy-pe->y2, pe->cx+pe->x2, pe->cy);
	  break;
	case isPainting:
	  Doc->Paintings->append((Painting*)pe);
	  ((Painting*)pe)->Bounding(x1,y1,x2,y2);
	  Doc->enlargeView(x1, y1, x2, y2);
	  break;
	case isMovingLabel:
	  pe->ElemType = isNodeLabel;
	  Doc->placeNodeLabel((WireLabel*)pe);
	  break;
	case isComponent:
	case isAnalogComponent:
	case isDigitalComponent:
	  Doc->insertComponent((Component*)pe);
	  ((Component*)pe)->entireBounds(x1,y1,x2,y2, Doc->textCorr());
	  Doc->enlargeView(x1, y1, x2, y2);
	  break;
      }
    }

    pasteElements(Doc);
    // keep rotation sticky for pasted elements
    rot = movingRotated;
    x1 = y1 = 0;
    while(rot--) rotateElements(Doc,x1,y1);

    QucsMain->MouseMoveAction = &MouseActions::MMovePaste;
    QucsMain->MousePressAction = 0;
    QucsMain->MouseReleaseAction = 0;
    QucsMain->MouseDoubleClickAction = 0;

    drawn = false;
    Doc->viewport()->update();
    Doc->setChanged(true, true);
    break;

  // ............................................................
  case Qt::RightButton :  // right button rotates the elements

    if(drawn) // erase old scheme
      paintElementsScheme(Doc);
    drawn = true;

    x1 = pos.x();
    y1 = pos.y();
    rotateElements(Doc,x1,y1);
    paintElementsScheme(Doc);
    // save rotation
    movingRotated++;
    movingRotated &= 3;
    break;

  default: ;    // avoids compiler warnings
  }
}

// -----------------------------------------------------------
void MouseActions::MReleaseMoveText(Schematic *Doc, QMouseEvent *Event)
{
  if(Event->button() != Qt::LeftButton) return;

  QucsMain->MouseMoveAction = &MouseActions::MMoveMoveTextB;
  QucsMain->MouseReleaseAction = 0;
  Doc->releaseKeyboard();  // allow keyboard inputs again

  ((Component*)focusElement)->tx = MAx1 - ((Component*)focusElement)->cx;
  ((Component*)focusElement)->ty = MAy1 - ((Component*)focusElement)->cy;
  Doc->viewport()->update();
  drawn = false;
  Doc->setChanged(true, true);
}

/*!
 * \brief MouseActions::MReleaseZoomIn
 * \param Doc
 * \param Event
 * Handle zoom in for mouse press or small selection box, center on event position.
 * Handle zoom fit to rectangle. Limited to 500% zoom in.
 */
void MouseActions::MReleaseZoomIn(Schematic *Doc, QMouseEvent *Event)
{
  if(Event->button() != Qt::LeftButton) return;

  // position of the curson when released
  QPointF pos = Doc->mapToScene(Event->pos());
  MAx1 = int(pos.x());
  MAy1 = int(pos.y());

  // size of selection rectangle, relative to release position
  // cleared by MPressZoomIn
  // set by MMoveSelect
  float DX = float(MAx2);
  float DY = float(MAy2);

  // legacy: why 6.0? if narrow X selection, handle as single click?
  //if((Doc->Scale * std::abs(DX)) < 6.0) {
  if (std::abs(DX) < 10 | std::abs(DY) < 10) {
    // if zoom box too small, handle as simple zoomIn
    Doc->zoomIn();
    // center to event position
    Doc->centerOn( Doc->mapToScene( Event->pos()) );
  } else {
    // zoom to selection rectangle

    // figure out position of top left corner of zoom box
    // zoom box size (DX, DY) it relative to release event (MAx1, MAx2)
    float xtop;
    float ytop;

    if (DX > 0) { // drag right
      xtop = MAx1 - std::abs( DX );
    } else { // drag left
      xtop = MAx1;
    }

    if (DY > 0) { // drag down
      ytop = MAy1 - std::abs( DY );
    } else  { // drag up
      ytop = MAy1;
    }

    // zoom box
    QRect rect = QRect(xtop, ytop, std::abs(DX), std::abs(DY));

    // try to fit rectangle to view
    float xRatio = Doc->rect().width() / std::abs(DX);
    float yRatio = Doc->rect().height() / std::abs(DY);
    float minScale = qMin(xRatio, yRatio)/Doc->Scale;

    // scale and center
    qDebug() << "minScale" << minScale;
    //! limit max zoom to 500% \sa Schematic::zoomIn
    if (minScale > 5)
      minScale = 5;
    Doc->scale(minScale, minScale);
    Doc->centerOn( rect.center() );

    // update
    Doc->Scale = Doc->matrix().m11();
  }

  QucsMain->MouseMoveAction = &MouseActions::MMoveZoomIn;
  QucsMain->MouseReleaseAction = 0;
  Doc->releaseKeyboard();  // allow keyboard inputs again
}


// ***********************************************************************
// **********                                                   **********
// **********    Functions for mouse button double clicking     **********
// **********                                                   **********
// ***********************************************************************
void MouseActions::editElement(Schematic *Doc, QMouseEvent *Event)
{
//    qDebug() << "+double click, editElement";

  if(focusElement == 0) return;

//  qDebug() << "+focusElement->Type" << focusElement->Type;

  Graph *pg;
  Component *c;
  Diagram *dia;
  DiagramDialog *ddia;
  MarkerDialog *mdia;
  int x1, y1, x2, y2;

  QFileInfo Info(Doc->DocName);

  QPointF pos = Doc->mapToScene(Event->pos());
  float fX = pos.x();
  float fY = pos.y();

  switch(focusElement->ElemType) {
    case isComponent:
    case isAnalogComponent:
    case isDigitalComponent:
         c = (Component*)focusElement;
//         qDebug() << "cast focusElement into" << c->Name;
         if(c->obsolete_model_hack() == "GND") { // BUG
	   return;
	 }else if(c->obsolete_model_hack() == "SPICE") { // BUG. use cast
           SpiceDialog *sd = new SpiceDialog(App, (SpiceFile*)c, Doc);
           if(sd->exec() != 1) break;   // dialog is WDestructiveClose
         } else if(c->obsolete_model_hack() == ".Opt") { // BUG again...
           OptimizeDialog *od = new OptimizeDialog((Optimize_Sim*)c, Doc);
           if(od->exec() != 1) break;   // dialog is WDestructiveClose
         } else {
           ComponentDialog * cd = new ComponentDialog(c, Doc);
           if(cd->exec() != 1) break;   // dialog is WDestructiveClose

           Doc->Components->findRef(c);
           Doc->Components->take();
           Doc->setComponentNumber(c); // for ports/power sources
           Doc->Components->append(c);
         }

         Doc->setChanged(true, true);
         c->entireBounds(x1,y1,x2,y2, Doc->textCorr());
         Doc->enlargeView(x1,y1,x2,y2);
         break;

    case isDiagram :
         dia = (Diagram*)focusElement;
         if(dia->Name.at(0) == 'T') { // don't open dialog on scrollbar
           if(dia->Name == "Time") {
             if(dia->cy < int(fY)) {
	       if(((TimingDiagram*)focusElement)->scroll(MAx1))
	         Doc->setChanged(true, true, 'm'); // 'm' = only the first time
	       break;
             }
	   }
           else {
             if(dia->cx > int(fX)) {
	       if(((TabDiagram*)focusElement)->scroll(MAy1))
	         Doc->setChanged(true, true, 'm'); // 'm' = only the first time
	       break;
             }
	   }
	 }

	 ddia = new DiagramDialog(dia, Doc);
         if(ddia->exec() != QDialog::Rejected)   // is WDestructiveClose
           Doc->setChanged(true, true);

	 dia->Bounding(x1, x2, y1, y2);
	 Doc->enlargeView(x1, x2, y1, y2);
	 break;

    case isGraph :
	 pg = (Graph*)focusElement;
	 // searching diagram for this graph
	 for(dia = Doc->Diagrams->last(); dia != 0; dia = Doc->Diagrams->prev())
	   if(dia->Graphs.indexOf(pg) >= 0)
	     break;
	 if(!dia) break;


	 ddia = new DiagramDialog(dia, Doc, pg);
	 if(ddia->exec() != QDialog::Rejected)   // is WDestructiveClose
	   Doc->setChanged(true, true);
         break;

    case isWire:
         MPressLabel(Doc, Event, fX, fY);
         break;

    case isNodeLabel:
    case isHWireLabel:
    case isVWireLabel:
         editLabel(Doc, (WireLabel*)focusElement);
         // update highlighting, labels may have changed
         Doc->highlightWireLabels ();
         break;

    case isPainting:
         if( ((Painting*)focusElement)->Dialog() )
           Doc->setChanged(true, true);
         break;

    case isMarker:
         mdia = new MarkerDialog((Marker*)focusElement, Doc);
         if(mdia->exec() > 1)
           Doc->setChanged(true, true);
         break;
  }

  // Very strange: Now an open VHDL editor gets all the keyboard input !?!
  // I don't know why it only happens here, nor am I sure whether it only
  // happens here. Anyway, I hope the best and give the focus back to the
  // current document.
  Doc->setFocus();

  Doc->viewport()->update();
  drawn = false;
}

// -----------------------------------------------------------
void MouseActions::MDoubleClickSelect(Schematic *Doc, QMouseEvent *Event)
{
  Doc->releaseKeyboard();  // allow keyboard inputs again
  QucsMain->editText->setHidden(true);
  editElement(Doc, Event);
}


/**
 * @brief MouseActions::MDoubleClickWire2  Double click terminates wire insertion.
 * @param Doc
 * @param Event
 */
void MouseActions::MDoubleClickWire2(Schematic *Doc, QMouseEvent *Event)
{
  QPointF pos = Doc->mapToScene(Event->pos());

  MPressWire2(Doc, Event, pos.x(), pos.y());

  if(formerAction)
    QucsMain->select->setChecked(true);  // restore old action
  else {
    QucsMain->MouseMoveAction = &MouseActions::MMoveWire1;
    QucsMain->MousePressAction = &MouseActions::MPressWire1;
    QucsMain->MouseDoubleClickAction = 0;
  }
}

// vim:ts=8:sw=2:noet
