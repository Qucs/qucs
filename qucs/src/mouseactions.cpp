/***************************************************************************
    copyright            : (C) 2003 by Michael Margraf
                               2014 Guilherme Brondani Torri
                               2020 Felix Salfelder

 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include "qucs.h"
#include "node.h"
#include "schematic_doc.h"
#include "misc.h"
#include "mouseactions.h"
#include "module.h"
//#include "components/spicedialog.h"
//#include "components/spicefile.h"
//#include "components/optimizedialog.h"
#include "labeldialog.h"
#include "io_trace.h"
#include "schematic_scene.h"
#include "wirelabel.h"

#include <QTextStream>
#include "qt_compat.h"
#include <QMouseEvent>
#include <QClipboard>
#include <QApplication>
#include <QMessageBox>
#include <QMenu>
#include <QEvent>
#include <QAction>
#include <QLineEdit>
#include <QDebug>
#include <QUndoStack>

#include <limits.h>
#include <stdlib.h>


void MouseAction::sceneAddItem(ElementGraphics* x)
{
  doc().sceneAddItem(x);
}
void MouseAction::sceneRemoveItem(ElementGraphics* x)
{
  doc().sceneRemoveItem(x);
}


QAction *formerAction;   // remember action before drag n'drop etc.

// only mouse actions? how about undoable keyboard actions?
MouseActions::MouseActions(QucsDoc& d)
  : _doc(d)
{itested();
  //selElem  = 0;  // no component/diagram is selected
  isMoveEqual = false;  // mouse cursor move x and y the same way
  //focusElement.clear(); //element being interacted with mouse

  // ...............................................................
  // initialize menu appearing by right mouse button click on component
  ComponentMenu = new QMenu(QucsMain);
  // focusMEvent   = new QMouseEvent(QEvent::MouseButtonPress, QPoint(0,0),
  //                 Qt::NoButton, Qt::NoButton, Qt::NoModifier);

}

MouseActions::~MouseActions()
{itested();
  delete ComponentMenu;
//  delete focusMEvent;
}
void MouseActions::setActive(MouseAction* a)
{
  if(_maCurrent){itested();
    _maCurrent->uncheck();
  }else{itested();
  }
  _maCurrent = a;
}

// -----------------------------------------------------------
void MouseActions::setPainter(SchematicDoc *)
{
}

// -----------------------------------------------------------
bool MouseActions::pasteElements(SchematicDoc *)
{ untested();
  assert(false);
  QClipboard *cb = QApplication::clipboard();   // get system clipboard
  QString s = cb->text(QClipboard::Clipboard);
//  DocumentStream stream(&s, QIODevice::ReadOnly);
#if 0
  movingElements.clear();
  if(!Doc->paste(&stream, &movingElements)){
    // something went wrong during parse.
    // throw?!
    return false;
  }else{
  }

  int xmax, xmin, ymax, ymin;
  xmin = ymin = INT_MAX;
  xmax = ymax = INT_MIN;
  // First, get the max and min coordinates of all selected elements.
  for(auto pe : movingElements){ untested();
    if(pe->Type == isWire) { untested();
      if(pe->x1_() < xmin) xmin = pe->x1_();
      if(pe->x2_() > xmax) xmax = pe->x2_();
      if(pe->y1_() < ymin) ymin = pe->y1_();
      if(pe->y2_() > ymax) ymax = pe->y2_();
    }
    else { untested();
      if(pe->cx_() < xmin) xmin = pe->cx_();
      if(pe->cx_() > xmax) xmax = pe->cx_();
      if(pe->cy_() < ymin) ymin = pe->cy_();
      if(pe->cy_() > ymax) ymax = pe->cy_();
    }
  }

  xmin = -((xmax+xmin) >> 1);   // calculate midpoint
  ymin = -((ymax+ymin) >> 1);
  Doc->setOnGrid(xmin, ymin);

  // moving with mouse cursor in the midpoint
  for(auto pe : movingElements){ untested();
    if(pe->Type & isLabel) { untested();
      auto L=dynamic_cast<WireLabel*>(pe);
      //pe->cx += xmin;  pe->x1 += xmin;
      //pe->cy += ymin;  pe->y1 += ymin;
      // TODO.
      auto oldtype=L->Type;
      L->Type = isMovingLabel;
      L->setCenter(xmin, ymin, true /*relative*/);
      L->Type = oldtype;
    } else{
      pe->setPos(xmin, ymin, true);
    }
  }

#endif
  return true;
}

// -----------------------------------------------------------
// TODO: graphical input texts
void MouseActions::editLabel(SchematicDoc *, WireLabel *)
{ untested();
  unreachable();
#if 0
  LabelDialog *Dia = new LabelDialog(pl, Doc);
  int Result = Dia->exec();
  if(Result == 0) return;

  QString Name  = Dia->NodeName->text();
  QString Value = Dia->InitValue->text();
  delete Dia;

  if(Name.isEmpty() && Value.isEmpty()) { // if nothing entered, delete label
    pl->pOwner->Label = 0;   // delete name of wire
    delete pl;
  } else { untested();
/*    Name.replace(' ', '_');	// label must not contain spaces
    while(Name.at(0) == '_') Name.remove(0,1);  // must not start with '_'
    if(Name.isEmpty()) return;
    if(Name == pl->Name) return;*/
    if(Result == 1) return;  // nothing changed

    int old_x2 = pl->x2_();
    pl->setName(Name);   // set new name
    pl->initValue = Value;
    if(pl->cx() > (pl->x1_()+(pl->x2_()>>1))){ untested();

      // pl->x1_() -= pl->x2_() - old_x2; // don't change position due to text width
      pl->moveLeft(pl->x2_() - old_x2);
    }
  }

  Doc->sizeOfAll(Doc->UsedX1, Doc->UsedY1, Doc->UsedX2, Doc->UsedY2);
  updateViewport();
  setDrawn(false);
  Doc->setChanged(true, true);
#endif
}

// -----------------------------------------------------------
// Reinserts all elements (moved by the user) back into the schematic.
// but why?!
#if 0
void MouseActions::endElementMoving(SchematicDoc *Doc, EGPList *movElements)
{ untested();
  unreachable(); // obsolete
  for(auto pe : *movElements){ untested();
//    pe->setSelected(false);  // deselect first (maybe afterwards pe == NULL)
    if(wire(pe)){ untested();
        if(pe->x1_() == pe->x2_()){ untested();
          if(pe->y1_() == pe->y2_()) { untested();
            // Delete wires with zero length, but preserve label.
#if 0
            if(((Wire*)pe)->Label) { untested();
              Doc->insertNodeLabel((WireLabel*)((Wire*)pe)->Label);
              ((Wire*)pe)->Label = 0;
            }else{
	    }
#endif
            delete (Wire*)pe;
            break;
          }
	}else{
	}

	// Doc->insertWire((Wire*)pe); wtf?
	break; // why?
    }else if(diagram(pe)){ untested();
	//already there. d'uh
    }else if(painting(pe)){ untested();
	Doc->paintings().append((Painting*)pe);
    }else if(auto c=component(pe)){ untested();
      qDebug() << "type" << pe->Type << c->label().c_str();
	Doc->insertRawComponent(c, false);
//      Doc->connect(c); // TODO.
    }else if(auto w=wireLabel(pe)){ untested();
	Doc->insertNodeLabel(w);
    }else if(marker(pe)){ untested();
      //?
    }else{ untested();
    }
  }

  movElements->clear();
  if((MAx3 != 0) || (MAy3 != 0))  // moved or put at the same place ?
    Doc->setChanged(true, true);

  // enlarge viewarea if components lie outside the view
  Doc->sizeOfAll(Doc->UsedX1, Doc->UsedY1, Doc->UsedX2, Doc->UsedY2);
  Doc->enlargeView(Doc->UsedX1, Doc->UsedY1, Doc->UsedX2, Doc->UsedY2);
  updateViewport();
  setDrawn(false);
}
#endif

// -----------------------------------------------------------
// Moves elements in "movElements" by x/y
#if 0
void MouseActions::moveElements(EGPList& , int, int)
{ itested();
  unreachable(); // obsolete
  auto movElements=&what;
  Wire *pw;
  (void)pw;
  for(auto pe : *movElements) { itested();
    if(pe->Type == isWire) { untested();
      pw = (Wire*)pe;   // connected wires are not moved completely

      incomplete();
      if(((uintptr_t)pw->portValue(0)) > 3) { // wtf?
	pw->move1(x, y);
#if 0
	if(pw->Label) { untested();
	  pw->Label->moveCenter(x, y);
	}else{ untested();
	}
#endif
      }else{ untested();
      	if((uintptr_t)(pw->portValue(0)) & 1) { pw->move1(x,0); }
	if((uintptr_t)(pw->portValue(0)) & 2) { pw->move1(0,y); }
      }

      if(((uintptr_t)pw->portValue(1)) > 3) { pw->move2(x, y);
      }else{ untested();
	if((uintptr_t)(pw->portValue(1)) & 1) { pw->move2(x,0); }
	if((uintptr_t)(pw->portValue(1)) & 2) { pw->move2(0,y); }
      }

#if 0
      if(pw->Label) { untested();
	// root of node label must lie on wire
        if(pw->Label->cx_() < pw->x1_()) pw->Label->moveTo(pw->x1_(), pw->Label->cy_());
        if(pw->Label->cy_() < pw->y1_()) pw->Label->moveTo(pw->Label->cx_(), pw->y1_());
        if(pw->Label->cx_() > pw->x2_()) pw->Label->moveTo(pw->x2_(), pw->Label->cy_());
        if(pw->Label->cy_() > pw->y2_()) pw->Label->moveTo(pw->Label->cx_(), pw->y2_());
      }else{ untested();
      }
#endif

    } else{
      pe->setCenter(x, y, true);
    }
  }
}
#endif


// ***********************************************************************
// **********                                                   **********
// **********       Functions for serving mouse moving          **********
// **********                                                   **********
// ***********************************************************************
#if 0 // obsolete?
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
void MouseActions::MMoveElement(SchematicDoc *Doc, QMouseEvent *Event)
{ untested();
  if(selElem == 0) return;

//  qDebug() << "MMoveElement got selElem";

  int x  = Event->pos().x();
  int y  = Event->pos().y();
  (void) (x+y);

  QPointF pos=Doc->mapToScene(Event->pos());
  int fx = pos.x();
  int fy = pos.y();
  int gx = fx;
  int gy = fy;
  Doc->setOnGrid(gx, gy);


  //QPainter painter(Doc->viewport());
  if(auto P=painting(selElem)) { untested();
    Doc->PostPaintEvent (_NotRop, 0,0,0,0);
    incomplete();
#if 0
    x -= Doc->contentsX();
    y -= Doc->contentsY();
#endif
    incomplete();
    (void)P;
//    P->MouseMoving(Doc, x, y, gx, gy, Doc, x, y, drawn);
    setDrawn();
    updateViewport();
  }else{

    // ********** it is a component or diagram
    if(_drawn){
      // erase old scheme
      selElem->paintScheme(Doc);
    }else{
      setDrawn();
    }

  //  Component *comp = (Component*)selElem;
    //qDebug() << "desc" << comp->Description << "gx" << gx << "gy" << gy;

    selElem->setCenter(gx, gy);
    selElem->paintScheme(Doc); // paint scheme at new position
    Doc->viewport()->update();
  }
}
#endif


/**
 * @brief draws wire aiming cross on Document view
 * @param Doc - pointer to Schematics object
 * @param fx  - document x-coordinate of center
 * @param fy  - document x-coordinate of center
 *
 *  \todo Move aim (crosshair) to separate class
 *
static void paintAim(SchematicDoc *Doc, int fx,int fy){ untested();
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
static void paintGhostLineH(SchematicDoc *Doc, int fx,int fy, int fxx){ untested();
	Doc->PostPaintEvent (_Line, fx, fy-1, fxx, fy-1);
	Doc->PostPaintEvent (_Line, fx, fy+1, fxx, fy+1);
}

//paint ghost line - vertical
static void paintGhostLineV(SchematicDoc *Doc, int fx, int fy, int fyy){ untested();
	Doc->PostPaintEvent (_Line, fx-1, fy, fx-1, fyy);
	Doc->PostPaintEvent (_Line, fx+1, fy, fx+1, fyy);
}
*/


// -----------------------------------------------------------
#if 0
/**
 * @brief MouseActions::MMoveWire2 Paint wire as it is being drawn with mouse.
 * @param Doc
 * @param Event
 */
void MouseActions::MMoveWire2(SchematicDoc *, QMouseEvent *)
{ untested();
  Set2(Event, Doc);
  Doc->setOnGrid(MAx2, MAy2);
  /// \todo paint aim
  /*paintAim(Doc,MAx2,MAy2); //let we paint aim cross

  //because cross slightly masks a wire, let we make wire thicker
  //better to make it by increasing of pen, but here we cannot access
  //pen
  if(MAx1 == 0) { untested();
	paintGhostLineV(Doc,MAx3,MAy3,MAy2);
	paintGhostLineH(Doc,MAx3,MAy2,MAx2);
  }
  else { untested();
	paintGhostLineH(Doc,MAx3,MAy3,MAx2);
	paintGhostLineV(Doc,MAx2,MAy3,MAy2);
  }
  */
  QucsMain->MouseDoubleClickAction = &MouseActions::MDoubleClickWire2;
  Doc->viewport()->update();
}

void MouseActions::Set1(QMouseEvent* Event, SchematicDoc*)
{
  SchematicDoc* s = prechecked_cast<SchematicDoc*>(&_doc);
  assert(s);
  QPointF pos=s->mapToScene(Event->pos());
  MAx1 = pos.x();
  MAy1 = pos.y();
}

void MouseActions::Set2(QMouseEvent* Event, SchematicDoc*)
{
  SchematicDoc* s = prechecked_cast<SchematicDoc*>(&_doc);
  assert(s);
  QPointF pos=s->mapToScene(Event->pos());
  MAx2 = pos.x();
  MAy2 = pos.y();
}

void MouseActions::Set3(QMouseEvent* Event, SchematicDoc*)
{
  SchematicDoc* s = prechecked_cast<SchematicDoc*>(&_doc);
  assert(s);
  QPointF pos=s->mapToScene(Event->pos());
  MAx3 = pos.x();
  MAy3 = pos.y();
}
#endif

#if 0
/**
 * @brief MouseActions::MMoveWire1 Paint hair cross for "insert wire" mode
 * @param Doc
 * @param Event
 */
void MouseActions::MMoveWire1(SchematicDoc *Doc, QMouseEvent *Event)
{ untested();
  Set3(Event);
  Doc->setOnGrid(MAx3, MAy3);
  /// \todo paint aim
  //paintAim(Doc,MAx3,MAy3);
  //MAx2 = DOC_X_POS(Doc->contentsX()+Doc->viewport()->width()-1-2);
  //MAx2 = DOC_Y_POS(Doc->contentsY()+Doc->viewport()->height()-1-2);
  Doc->viewport()->update();
}
#endif



// -----------------------------------------------------------
void MouseActions::MMoveResizePainting(SchematicDoc *Doc, QMouseEvent *)
{
  setPainter(Doc);
#if 0

  Set1(Event, Doc);
  Doc->setOnGrid(MAx1, MAy1);

  if(auto p=painting(focusElement)){ untested();
    p->MouseResizeMoving(MAx1, MAy1, Doc);
  }else{ untested();
    // why not always?
    // e->MouseResizeMoving(MAx1, MAy1, Doc);
  }
#endif
}

// -----------------------------------------------------------
#if 0 // obsolete.
// Moves components by keeping the mouse button pressed.
void MouseActions::MMoveMoving(SchematicDoc *Doc, QMouseEvent*)
{
  unreachable(); // obsolete
  setPainter(Doc);

  Set2(Event, Doc);

  Doc->setOnGrid(MAx2, MAy2);
  MAx3 = MAx1 = MAx2 - MAx1;
  MAy3 = MAy1 = MAy2 - MAy1;

  // assert(movingElements.empty); //?
  movingElements.clear();
  movingElements=Doc->cropSelectedElements(); // obsolete.
  Doc->viewport()->repaint();

//  currenttaskElement = new MoveElementsCommand(selectedItems, startposition);

  // Changes the position of all moving elements by dx/dy
  for(auto pe : movingElements) { untested();
    if(auto pw=wire(pe)){
      // connecting wires are not moved completely
	incomplete();

#if 0
      if(((uintptr_t)pw->portValue(0)) > 3) { pw->x1__() += MAx1;  pw->y1__() += MAy1; }
      else {  if((uintptr_t)(pw->portValue(0)) & 1) { pw->x1__() += MAx1; }
              if((uintptr_t)(pw->portValue(0)) & 2) { pw->y1__() += MAy1; } }

      if(((uintptr_t)pw->portValue(1)) > 3) { pw->x2__() += MAx1;  pw->y2__() += MAy1; }
      else {  if((uintptr_t)(pw->portValue(1)) & 1) pw->x2__() += MAx1;
              if((uintptr_t)(pw->portValue(1)) & 2) pw->y2__() += MAy1; }

      if(pw->Label) {  
	// root of node label must lie on wire
        if(pw->Label->cx_() < pw->x1_()) pw->Label->cx__() = pw->x1_();
        if(pw->Label->cy_() < pw->y1_()) pw->Label->cy__() = pw->y1_();
        if(pw->Label->cx_() > pw->x2_()) pw->Label->cx__() = pw->x2_();
        if(pw->Label->cy_() > pw->y2_()) pw->Label->cy__() = pw->y2_();
      }else{ untested();
      }
#endif

    }else{ untested();
      assert(!pw);
      pe->setCenter(MAx1, MAy1, true);
    }

  pe->paintScheme(Doc);
  }

  setDrawn();
  MAx1 = MAx2;
  MAy1 = MAy2;
  QucsMain->MouseMoveAction = &MouseActions::MMoveMoving2;
  QucsMain->MouseReleaseAction = &MouseActions::MReleaseMoving;

}

// -----------------------------------------------------------
// Moves components by keeping the mouse button pressed.
void MouseActions::MMoveMoving2(SchematicDoc *Doc, QMouseEvent *Event)
{
  setPainter(Doc);

  Set2(Event, Doc);

#if 0
  if(drawn){
    // erase old scheme
    for(auto pe : movingElements) {
      pe->paintScheme(Doc);
//      if(pe->Type == isWire)  if(((Wire*)pe)->Label)
//        if(!((Wire*)pe)->Label->isSelected)
//          ((Wire*)pe)->Label->paintScheme(&painter);
    }
  }
#endif

  setDrawn();
  if (!Event->modifiers().testFlag(Qt::ControlModifier))
    Doc->setOnGrid(MAx2, MAy2);  // use grid only if CTRL key not pressed
  MAx1 = MAx2 - MAx1;
  MAy1 = MAy2 - MAy1;
  MAx3 += MAx1;  MAy3 += MAy1;   // keep track of the complete movement

  moveElements(movingElements, MAx1, MAy1);  // moves elements by MAx1/MAy1

  // paint afterwards to avoid conflict between wire and label painting
  for(auto pe : movingElements) {
    pe->paintScheme(Doc);
//    if(pe->Type == isWire)  if(((Wire*)pe)->Label)
//      if(!((Wire*)pe)->Label->isSelected)
//        ((Wire*)pe)->Label->paintScheme(&painter);
  }

  MAx1 = MAx2;
  MAy1 = MAy2;
}
#endif


/**
 * @brief MouseActions::MMoveLabel Paints a label above the mouse cursor for "set wire label".
 * @param Doc
 * @param Event
 */
void MouseActions::MMoveLabel(SchematicDoc *, QMouseEvent *)
{
#if 0
  Set3(Event, Doc);

  // paint marker
  Doc->PostPaintEvent (_Line, MAx3, MAy3, MAx3+10, MAy3-10);
  Doc->PostPaintEvent (_Line, MAx3+10, MAy3-10, MAx3+20, MAy3-10);
  Doc->PostPaintEvent (_Line, MAx3+10, MAy3-10, MAx3+10, MAy3-17);

  // paint A
  Doc->PostPaintEvent (_Line, MAx3+12, MAy3-12, MAx3+15, MAy3-23);
  Doc->PostPaintEvent (_Line, MAx3+14, MAy3-17, MAx3+17, MAy3-17);
  Doc->PostPaintEvent (_Line, MAx3+19, MAy3-12, MAx3+16, MAy3-23);
#endif
}


/**
 * @brief MouseActions::MMoveMarker Paints a triangle above the mouse for "set marker on graph"
 * @param Doc
 * @param Event
 */
void MouseActions::MMoveMarker(SchematicDoc *, QMouseEvent *)
{
#if 0
  Set3(Event, Doc);

  Doc->PostPaintEvent (_Line, MAx3, MAy3-2, MAx3-8, MAy3-10);
  Doc->PostPaintEvent (_Line, MAx3+1, MAy3-3, MAx3+8, MAy3-10);
  Doc->PostPaintEvent (_Line, MAx3-7, MAy3-10, MAx3+7, MAy3-10);
#endif
}


/**
 * @brief MouseActions::MMoveMirrorX Paints rounded "mirror about y axis" mouse cursor
 * @param Doc
 * @param Event
 */
void MouseActions::MMoveMirrorY(SchematicDoc *, QMouseEvent *)
{
#if 0
  Set3(Event, Doc);

  Doc->PostPaintEvent (_Line, MAx3-11, MAy3-4, MAx3-9, MAy3-9);
  Doc->PostPaintEvent (_Line, MAx3-11, MAy3-3, MAx3-6, MAy3-3);
  Doc->PostPaintEvent (_Line, MAx3+11, MAy3-4, MAx3+9, MAy3-9);
  Doc->PostPaintEvent (_Line, MAx3+11, MAy3-3, MAx3+6, MAy3-3);
  Doc->PostPaintEvent (_Arc, MAx3-10, MAy3-8, 21, 10, 16*20, 16*140,false);
#endif
}


/**
 * @brief MouseActions::MMoveMirrorX Paints rounded "mirror about x axis" mouse cursor
 * @param Doc
 * @param Event
 */
void MouseActions::MMoveMirrorX(SchematicDoc *, QMouseEvent *)
{
#if 0
  Set3(Event, Doc);

  Doc->PostPaintEvent (_Line, MAx3-4, MAy3-11, MAx3-9, MAy3-9);
  Doc->PostPaintEvent (_Line, MAx3-3, MAy3-11, MAx3-3, MAy3-6);
  Doc->PostPaintEvent (_Line, MAx3-4, MAy3+11, MAx3-9, MAy3+9);
  Doc->PostPaintEvent (_Line, MAx3-3, MAy3+11, MAx3-3, MAy3+6);
  Doc->PostPaintEvent (_Arc, MAx3-8, MAy3-10, 10, 21, 16*110, 16*140,false);
#endif
}

/**
 * @brief MouseActions::MMoveMirrorX Paints "rotate" mouse cursor
 * @param Doc
 * @param Event
 */
void MouseActions::MMoveRotate(SchematicDoc *, QMouseEvent *)
{
#if 0
  Set3(Event, Doc);

  Doc->PostPaintEvent (_Line, MAx3-6, MAy3+8, MAx3-6, MAy3+1);
  Doc->PostPaintEvent (_Line, MAx3-7, MAy3+8, MAx3-12, MAy3+8);
  Doc->PostPaintEvent (_Arc, MAx3-10, MAy3-10, 21, 21, -16*20, 16*240,false);
#endif
}

// not sure why this is needed
bool MouseActions::eventFilter(QObject *, QEvent *e)
{
  if(e->isAccepted()){itested();
  }else{itested();
    // drop event?
  }
  assert(e);
  e->ignore(); // but why??
  return false; // don't filter
}

/**
 * @brief MouseActions::MMoveActivate Paints a crossed box mouse cursor to "(de)activate" components.
 * @param Doc
 * @param Event
 */
void MouseActions::MMoveActivate(SchematicDoc *, QMouseEvent *)
{
#if 0
  Set3(Event, Doc);

  Doc->PostPaintEvent (_Rect, MAx3, MAy3-9, 14, 10);
  Doc->PostPaintEvent (_Line, MAx3, MAy3-9, MAx3+13, MAy3);
  Doc->PostPaintEvent (_Line, MAx3, MAy3, MAx3+13, MAy3-9);
#endif
}


/**
 * @brief MouseActions::MMoveOnGrid Paints a grid beside the mouse cursor, put "on grid" mode.
 * @param Event
 * @param Doc
 */
void MouseActions::MMoveOnGrid(SchematicDoc *, QMouseEvent *)
{
#if 0
  Set3(Event, Doc);

  Doc->PostPaintEvent (_Line, MAx3+10, MAy3+ 3, MAx3+25, MAy3+3);
  Doc->PostPaintEvent (_Line, MAx3+10, MAy3+ 7, MAx3+25, MAy3+7);
  Doc->PostPaintEvent (_Line, MAx3+10, MAy3+11, MAx3+25, MAy3+11);
  Doc->PostPaintEvent (_Line, MAx3+13, MAy3, MAx3+13, MAy3+15);
  Doc->PostPaintEvent (_Line, MAx3+17, MAy3, MAx3+17, MAy3+15);
  Doc->PostPaintEvent (_Line, MAx3+21, MAy3, MAx3+21, MAy3+15);
#endif
}


/**
 * @brief MouseActions::MMoveMoveTextB Paints mouse symbol for "move component text" mode.
 * @param Doc
 * @param Event
 */
void MouseActions::MMoveMoveTextB(SchematicDoc *, QMouseEvent *)
{
#if 0
  Set3(Event, Doc);

  Doc->PostPaintEvent (_Line, MAx3+14, MAy3   , MAx3+16, MAy3);
  Doc->PostPaintEvent (_Line, MAx3+23, MAy3   , MAx3+25, MAy3);
  Doc->PostPaintEvent (_Line, MAx3+13, MAy3   , MAx3+13, MAy3+ 3);
  Doc->PostPaintEvent (_Line, MAx3+13, MAy3+ 7, MAx3+13, MAy3+10);
  Doc->PostPaintEvent (_Line, MAx3+14, MAy3+10, MAx3+16, MAy3+10);
  Doc->PostPaintEvent (_Line, MAx3+23, MAy3+10, MAx3+25, MAy3+10);
  Doc->PostPaintEvent (_Line, MAx3+26, MAy3   , MAx3+26, MAy3+ 3);
  Doc->PostPaintEvent (_Line, MAx3+26, MAy3+ 7, MAx3+26, MAy3+10);
#endif
}


/**
 * @brief MouseActions::MMoveMoveText Paint rectangle around component text being mouse moved
 * @param Doc
 * @param Event
 */
void MouseActions::MMoveMoveText(SchematicDoc *, QMouseEvent *)
{
#if 0
  QPointF pos=Doc->mapToScene(Event->pos());
  int newX=pos.x();
  int newY=pos.y();

  MAx1 += newX - MAx3;
  MAy1 += newY - MAy3;
  MAx3  = newX;
  MAy3  = newY;

  Doc->PostPaintEvent (_Rect, MAx1, MAy1, MAx2, MAy2);
#endif
}


/**
 * @brief MouseActions::MMoveZoomIn Paints symbol beside the mouse to show the "Zoom in" modus.
 * @param Doc
 * @param Event
 */
void MouseActions::MMoveZoomIn(SchematicDoc *, QMouseEvent *)
{
#if 0 // TODO
  Set3(Event, Doc);

  Doc->PostPaintEvent (_Line, MAx3+14, MAy3   , MAx3+22, MAy3);
  Doc->PostPaintEvent (_Line, MAx3+18, MAy3-4 , MAx3+18, MAy3+4);
  Doc->PostPaintEvent (_Ellipse, MAx3+12, MAy3-6, 13, 13,0,0,false);
  Doc->viewport()->update();
#endif
}


// ************************************************************************
// **********                                                    **********
// **********    Functions for serving mouse button clicking     **********
// **********                                                    **********
// ************************************************************************

#if 0
// Is called from several MousePress functions to show right button menu.
void MouseActions::rightPressMenu(QMouseEvent *Event)
{ untested();
  Set1(Event);

  SchematicDoc* Doc = prechecked_cast<SchematicDoc*>(&_doc);
  assert(Doc);
  focusElement = selectElement(Event->pos(), false);

  if(focusElement){  // remove special function (4 least significant bits)
    incomplete();
    // what's this??
    // focusElement->Type &= isSpecialMask;
  }else{ untested();
  }


  // define menu
  ComponentMenu->clear();
  { untested();
    if(focusElement) { untested();
      focusElement.setSelected(true);
      QAction *editProp = new QAction(QObject::tr("Edit Properties"), QucsMain);
      QucsMain->connect(editProp, SIGNAL(triggered()), SLOT(slotEditElement()));
      ComponentMenu->addAction(editProp);

      if(!component(focusElement)){ untested();
      }else if(!QucsMain->moveText->isChecked()){ untested();
	ComponentMenu->addAction(QucsMain->moveText);
      }else{ untested();
      }
    }else{ untested();
      ComponentMenu->addAction(QucsMain->symEdit);
      ComponentMenu->addAction(QucsMain->fileSettings);

      if(!QucsMain->moveText->isChecked()){ untested();
	ComponentMenu->addAction(QucsMain->moveText);
      }else{ untested();
      }
    }
  }

  // possibly, we just want to call focuselement->somethign_menu() ?!
  while(true) { untested();
    if(focusElement){ untested();
      if(graph(focusElement)) break;
    }else{ untested();
    }
    if(!QucsMain->onGrid->isChecked())
      ComponentMenu->addAction(QucsMain->onGrid);
    ComponentMenu->addAction(QucsMain->editCopy);
    if(!QucsMain->editPaste->isChecked())
      ComponentMenu->addAction(QucsMain->editPaste);
    break;
  }

  // possibly, we just want to call focuselement->rightMenuAction() ?!
  { untested();
    if (focusElement) { untested();
      if (diagram(focusElement)) { untested();
        QAction *imgExport = new QAction(QObject::tr("Export as image"), QucsMain);
        QucsMain->connect(imgExport, SIGNAL(triggered()), SLOT(slotSaveDiagramToGraphicsFile()));
        ComponentMenu->addAction(imgExport);
      }else{ untested();
      }
    }
  }

  if(!QucsMain->editDelete->isChecked()){ untested();
    // still ElementGraphics->rightMenuAction?!
    ComponentMenu->addAction(QucsMain->editDelete);
  }else{
  }

  if(!focusElement){ untested();
  }else if(auto m=marker(focusElement)) { untested();
    ComponentMenu->addSeparator();
    QString s = QObject::tr("power matching");
    if(m->pGraph->Var == "Sopt" ){ // BUG
      s = QObject::tr("noise matching");
    }

    QAction *powerMatch = new QAction(s, QucsMain);
    QucsMain->connect(powerMatch, SIGNAL(triggered()), SLOT(slotPowerMatching()));
    ComponentMenu->addAction(powerMatch);
    if(m->pGraph->Var.left(2) == "S[" ) { // BUG
      QAction *power2Match = new QAction(QObject::tr("2-port matching"), QucsMain);
      QucsMain->connect(power2Match, SIGNAL(triggered()), SLOT(slot2PortMatching()));
      ComponentMenu->addAction(power2Match);
    }else{ untested();
    }
  }
  do { untested();
    if(!focusElement) { untested();
    }else if(diagram(focusElement)){ untested();
      break;
    }else if(graph(focusElement)){ untested();
        ComponentMenu->addAction(QucsMain->graph2csv);
        break;
    }else{ untested();
    }
    ComponentMenu->addSeparator();
    if(focusElement) if(component(focusElement))
      if(!QucsMain->editActivate->isChecked())
        ComponentMenu->addAction(QucsMain->editActivate);
    if(!QucsMain->editRotate->isChecked())
      ComponentMenu->addAction(QucsMain->editRotate);
    if(!QucsMain->editMirror->isChecked())
      ComponentMenu->addAction(QucsMain->editMirror);
    if(!QucsMain->editMirrorY->isChecked())
      ComponentMenu->addAction(QucsMain->editMirrorY);

    // right-click menu to go into hierarchy
    if(focusElement) { untested();
      if(auto c=component(focusElement)){ untested();
      if(c->obsolete_model_hack() != "Sub"){ untested();
      }else if(!QucsMain->intoH->isChecked())
	// BUG: c->addAction.. later.
	ComponentMenu->addAction(QucsMain->intoH);
      }
    }
    // right-click menu to pop out of hierarchy
    if(!focusElement)
      if(!QucsMain->popH->isChecked())
        ComponentMenu->addAction(QucsMain->popH);
  } while(false);

  *focusMEvent = *Event;  // remember event for "edit component" action
  ComponentMenu->popup(Event->globalPos());
  updateViewport();
  setDrawn(false);
}
#endif

// -----------------------------------------------------------
void MouseActions::MPressLabel(SchematicDoc*, QMouseEvent*)
{ untested();
#if 0
  QPointF pos=Doc->mapToScene(Event->pos());
  float fX=pos.x();
  float fY=pos.y();

  int x = int(fX), y = int(fY);
  Wire *pw = 0;
  WireLabel *pl=0;
  Node *pn = Doc->selectedNode(x, y);
  if(!pn) { untested();
    pw = Doc->selectedWire(x, y);
    if(!pw) return;
  }

  QString Name, Value;
  Element *pe=0;
  // is wire line already labeled ?
#if 0
  if(pw){
    pe = Doc->getWireLabel(pw->portValue(0));
  }else{
    pe = Doc->getWireLabel(pn);
  }
#endif
  if(pe) { untested();
    if(pe->Type & isComponent) { untested();
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
    if(pe) { untested();
      if(((Conductor*)pe)->Label)
        delete ((Conductor*)pe)->Label; // delete old name
      ((Conductor*)pe)->Label = 0;
    }
    else { untested();
      if(pw) pw->setName("", "");   // delete name of wire
      else pn->setName("", "");
    }
  }
  else { untested();
/*    Name.replace(' ', '_');	// label must not contain spaces
    while(Name.at(0) == '_') Name.remove(0,1);  // must not start with '_'
    if(Name.isEmpty()) return;
*/
    if(pe) { untested();
      if(((Conductor*)pe)->Label)
        delete ((Conductor*)pe)->Label; // delete old name
      ((Conductor*)pe)->Label = 0;
    }

    int xl = x+30;
    int yl = y-30;
    Doc->setOnGrid(xl, yl);
    // set new name
    if(pw) pw->setName(Name, Value, x-pw->x1_() + y-pw->y1_(), xl, yl);
    else pn->setName(Name, Value, xl, yl);
  }

  Doc->sizeOfAll(Doc->UsedX1, Doc->UsedY1, Doc->UsedX2, Doc->UsedY2);
  updateViewport();
  setDrawn(false);
  Doc->setChanged(true, true);
#endif
}

// -----------------------------------------------------------
// yikes.

// -----------------------------------------------------------

// -----------------------------------------------------------
#if 0
void MouseActions::MPressActivate(SchematicDoc*, QMouseEvent*)
{ untested();
  QPointF pos=Doc->mapToScene(Event->pos());
  float fX=pos.x();
  float fY=pos.y();

  MAx1 = int(fX);
  MAy1 = int(fY);
  if(!Doc->activateSpecifiedComponent(MAx1, MAy1)) { untested();
//    if(Event->button() != Qt::LeftButton) return;
    MAx2 = 0;  // if not clicking on a component => open a rectangle
    MAy2 = 0;
    QucsMain->MousePressAction = 0;
    QucsMain->MouseReleaseAction = &MouseActions::MReleaseActivate;
    QucsMain->MouseMoveAction = &MouseActions::MMoveSelect;
  }
  updateViewport();
  setDrawn(false);
}
#endif

// -----------------------------------------------------------
#if 0
// insert component, diagram, painting into schematic ?!
void MouseActions::MPressElement(SchematicDoc *Doc, QMouseEvent *Event)
{
  // QPointF pos=Doc->mapToScene(Event->pos());

  if(selElem == 0) return;

  int x1, y1, x2, y2, rot;
  if(selElem->Type & isComponent) { untested();
    Component *Comp = (Component*)selElem;
//    qDebug() << "+-+ got to switch:" << Comp->name();
    QString entryName = Comp->name();

    const Component* cComp = Comp;
    switch(Event->button()) {
      case Qt::LeftButton :
	// left mouse button inserts component into the schematic
	// give the component a pointer to the schematic it's a
	// part of
	Comp->textSize(x1, y1);
	Doc->insertElement(Comp);
//	assert(&Doc->DocModel == &cComp->getScope());
	Comp->textSize(x2, y2);
	if(Comp->tx < Comp->x1_()) Comp->tx -= x2 - x1;

    // Note: insertCopmponents does increment  name1 -> name2
//    qDebug() << "  +-+ got to insert:" << Comp->name();

	// enlarge viewarea if component lies outside the view
	Comp->entireBounds(x1,y1,x2,y2, Doc->textCorr());
	Doc->enlargeView(x1, y1, x2, y2);

	setDrawn(false);
	updateViewport();
	Doc->setChanged(true, true);
	rot = Comp->rotated;

    // handle static and dynamic components
//    QucsApp::CompChoose;
    if (Module::vaComponents.contains(entryName)){ untested();
      QString filename = Module::vaComponents[entryName];
//      qDebug() << "   ===+ recast";
      incomplete(); // myust be Comp->pressElement (or so).
      //Comp = dynamic_cast<vacomponent*>(Comp)->newOne(filename); //va component
      qDebug() << "   => recast = Comp;" << Comp->name() << "filename: " << filename;
    }else{
	  // static component is used, so create a new one
	  Comp = prechecked_cast<Component*>(Comp->clone());
    }
	rot -= Comp->rotated;
	rot &= 3;
	while(rot--) Comp->rotate(); // keep last rotation for single component
	break;

      case Qt::RightButton :  // right mouse button rotates the component
	if(Comp->Ports.count() == 0)
	  break;  // do not rotate components without ports
	Comp->paintScheme(Doc); // erase old component scheme
	Comp->rotate();
	Comp->paintScheme(Doc); // paint new component scheme
	break;

      default: ;   // avoids compiler warnings
    }
//    qDebug() << "   => selElem = Comp;" << Comp->name();
    // comp it geting empty
    selElem = Comp;
    return;

  }  // of "if(isComponent)"
  else if(auto d=diagram(selElem)) { untested();

    d->pressElement(Doc, element(selElem), Event);

    Doc->paintings().append((Painting*)selElem);
    ((Painting*)selElem)->Bounding(x1,y1,x2,y2);
    //Doc->enlargeView(x1, y1, x2, y2);
    selElem = prechecked_cast<Element*>(selElem->clone());
    assert(selElem);

    Doc->viewport()->update();
    Doc->setChanged(true, true);

    MMoveElement(Doc, Event);  // needed before next mouse pressing
    setDrawn(false);
  }else{
  }
}
#endif



// -----------------------------------------------------------
// Is called for setting a marker on a diagram's graph
void MouseActions::MPressMarker(SchematicDoc *, QMouseEvent*)
{ untested();
#if 0
  QPointF pos=Doc->mapToScene(Event->pos());
  float fX=pos.x();
  float fY=pos.y();

  MAx1 = int(fX);
  MAy1 = int(fY);
  Marker *pm = Doc->setMarker(MAx1, MAy1);

  if(pm) { untested();
    assert(pm->diag());
    int x0 = pm->diag()->cx();
    int y0 = pm->diag()->cy();
    incomplete();
//    Doc->enlargeView(x0+pm->x1_(), y0-pm->y1_()-pm->y2_(), x0+pm->x1_()+pm->x2_(), y0-pm->y1_());
  }
  updateViewport();
  setDrawn(false);
#endif
}

// -----------------------------------------------------------
void MouseActions::MPressOnGrid(SchematicDoc *, QMouseEvent*)
{
  //QPointF pos=Doc->mapToScene(Event->pos());
#if 0

  auto pe = selectElement(Event->pos(), false);
  if(pe)
  { untested();
    pe->Type &= isSpecialMask;  // remove special functions (4 lowest bits)

    // onGrid is toggle action -> no other element can be selected
    pe->setSelected();
    Doc->elementsOnGrid();

    Doc->sizeOfAll(Doc->UsedX1, Doc->UsedY1, Doc->UsedX2, Doc->UsedY2);
    // Update matching wire label highlighting
    Doc->highlightWireLabels ();
    Doc->viewport()->update();
    setDrawn(false);
  }

#endif
}

// -----------------------------------------------------------
void MouseActions::MPressMoveText(SchematicDoc *, QMouseEvent*)
{ untested();
#if 0
  QPointF pos=Doc->mapToScene(Event->pos());
  float fX=pos.x();
  float fY=pos.y();

  MAx1 = int(fX);
  MAy1 = int(fY);

#ifndef USE_SCROLLVIEW
  incomplete();
#else
  Component* c=selectCompText(Doc, MAx1, MAy1, MAx2, MAy2);
  focusElement = ElementMouseAction(c);

  auto C=component(focusElement);

  if(focusElement) { untested();
    MAx3 = MAx1;
    MAy3 = MAy1;
    MAx1 = C->cx_() + C->tx;
    MAy1 = C->cy_() + C->ty;
    Doc->viewport()->update();
    setDrawn(false);
    QucsMain->MouseMoveAction = &MouseActions::MMoveMoveText;
    QucsMain->MouseReleaseAction = &MouseActions::MReleaseMoveText;
    Doc->grabKeyboard();  // no keyboard inputs during move actions
  }
#endif
#endif
}

// -----------------------------------------------------------
void MouseActions::MPressZoomIn(SchematicDoc *, QMouseEvent* )
{ untested();
  unreachable();
#if 0
  QPointF pos=Doc->mapToScene(Event->pos());
  float fX=pos.x();
  float fY=pos.y();

  qDebug() << "zoom into box";
  MAx1 = int(fX);
  MAy1 = int(fY);
  MAx2 = 0;  // rectangle size
  MAy2 = 0;

  QucsMain->MouseMoveAction = &MouseActions::MMoveSelect;
  QucsMain->MouseReleaseAction = &MouseActions::MReleaseZoomIn;
  Doc->grabKeyboard();  // no keyboard inputs during move actions
  Doc->viewport()->update();
  setDrawn(false);
#endif
}


// ***********************************************************************
// **********                                                   **********
// **********    Functions for serving mouse button releasing   **********
// **********                                                   **********
// ***********************************************************************
void MouseActions::MReleaseSelect(SchematicDoc *, QMouseEvent *)
{ untested();
  incomplete(); // obsolete
}

// -----------------------------------------------------------
QList<ElementGraphics*> MouseAction::selectedItems()
{
   // BUG. copies.
  unreachable();
  return doc().selectedItems();
}
#if 0
// Is called after the rectangle for selection is released.
void MouseActions::MReleaseSelect2(SchematicDoc *Doc, QMouseEvent *Event)
{
  unreachable();
  // obsolete. Qt Scene does rectangle select for us
  if(Event->button() != Qt::LeftButton){
    unreachable(); // yikes.
    return;
  }else{
  }

  bool Ctrl = Event->modifiers().testFlag(Qt::ControlModifier);

  // selects all elements within the rectangle
  Doc->selectElements(MAx1, MAy1, MAx1+MAx2, MAy1+MAy2, Ctrl);

  Doc->releaseKeyboard();  // allow keyboard inputs again
  QucsMain->MouseMoveAction = 0;
  QucsMain->MousePressAction = &MouseActions::MPressSelect;
  QucsMain->MouseReleaseAction = &MouseActions::MReleaseSelect;
  QucsMain->MouseDoubleClickAction = &MouseActions::MDoubleClickSelect;
  // MouseAction = actionSelect;
  Doc->highlightWireLabels ();
  Doc->viewport()->update();
  setDrawn(false);
}
#endif

// -----------------------------------------------------------
void MouseActions::MReleaseActivate(SchematicDoc *, QMouseEvent *)
{ untested();
#if 0
  if(Event->button() != Qt::LeftButton) return;

  // activates all components within the rectangle
  Doc->activateCompsWithinRect(MAx1, MAy1, MAx1+MAx2, MAy1+MAy2);

  QucsMain->MouseMoveAction = &MouseActions::MMoveActivate;
  QucsMain->MousePressAction = &MouseActions::MPressActivate;
  QucsMain->MouseReleaseAction = 0;
  QucsMain->MouseDoubleClickAction = 0;
  Doc->highlightWireLabels ();
  Doc->viewport()->update();
  setDrawn(false);
#endif
}

// -----------------------------------------------------------
// Is called after moving selected elements.
void MouseActions::MReleaseMoving(SchematicDoc *Doc, QMouseEvent*)
{ untested();
  // Allow all mouse buttons, because for others than the left one,
  // a menu has already created.
  // endElementMoving(Doc, &movingElements);
  Doc->releaseKeyboard();  // allow keyboard inputs again

#if 0
  QucsMain->MouseMoveAction = 0;
  QucsMain->MousePressAction = &MouseActions::MPressSelect;
  QucsMain->MouseReleaseAction = &MouseActions::MReleaseSelect;
  QucsMain->MouseDoubleClickAction = &MouseActions::MDoubleClickSelect;
#else
  // MouseAction = actionSelect;
#endif
}

// -----------------------------------------------------------
#if 0
void MouseActions::MReleaseResizeDiagram(SchematicDoc *Doc, QMouseEvent *Event)
{ untested();
  unreachable();
  if(Event->button() != Qt::LeftButton){ untested();
    return;
  }
  Diagram* d=diagram(focusElement);
  if(!d){ untested();
    assert(false && "we are in trouble here");
    return;
  }

  MAx3  = d->cx__();
  MAy3  = d->cy__();
  if(MAx2 < 0) {    // resize diagram
    if(MAx2 > -10) MAx2 = -10;   // not smaller than 10 pixels
    d->x2__() = -MAx2;
    d->cx__() = MAx1+MAx2;
  }
  else { untested();
    if(MAx2 < 10) MAx2 = 10;
    d->x2__() = MAx2;
    d->cx__() = MAx1;
  }
  if(MAy2 < 0) { untested();
    if(MAy2 > -10) MAy2 = -10;
    d->y2__() = -MAy2;
    d->cy__() = MAy1;
  }
  else { untested();
    if(MAy2 < 10) MAy2 = 10;
    d->y2__() = MAy2;
    d->cy__() = MAy1+MAy2;
  }
  MAx3 -= focusElement->cx_();
  MAy3 -= focusElement->cy_();

  Diagram *pd = d;
  pd->updateGraphData();
  foreach(Graph *pg, pd->Graphs)
    foreach(Marker *pm, pg->Markers) { untested();
      pm->x1__() += MAx3;      // correct changes due to move of diagram corner
      pm->y1__() += MAy3;
    }

  int x1, x2, y1, y2;
  pd->Bounding(x1, x2, y1, y2);
  Doc->enlargeView(x1, x2, y1, y2);

#if 0
  QucsMain->MouseMoveAction = 0;
  QucsMain->MousePressAction = &MouseActions::MPressSelect;
  QucsMain->MouseReleaseAction = &MouseActions::MReleaseSelect;
  QucsMain->MouseDoubleClickAction = &MouseActions::MDoubleClickSelect;
#else
  // MouseAction = actionSelect;
#endif
  Doc->releaseKeyboard();  // allow keyboard inputs again

  Doc->viewport()->update();
  setDrawn(false);
  Doc->setChanged(true, true);
}
#endif

// -----------------------------------------------------------
void MouseActions::MReleaseResizePainting(SchematicDoc *Doc, QMouseEvent *Event)
{ untested();
  if(Event->button() != Qt::LeftButton) return;

#if 0
  QucsMain->MouseMoveAction = 0;
  QucsMain->MousePressAction = &MouseActions::MPressSelect;
  QucsMain->MouseReleaseAction = &MouseActions::MReleaseSelect;
  QucsMain->MouseDoubleClickAction = &MouseActions::MDoubleClickSelect;
#else
  // MouseAction = actionSelect;
#endif


  Doc->releaseKeyboard();  // allow keyboard inputs again

  Doc->viewport()->update();
  setDrawn(false);
  Doc->setChanged(true, true);
}

// -----------------------------------------------------------
#if 0
void MouseActions::moveElements(SchematicDoc *Doc, int& x1, int& y1)
{ untested();
  unreachable(); // obsolete
  Doc->setOnGrid(x1, y1);

  for(auto pe : movingElements) {
    auto L=dynamic_cast<WireLabel*>(pe);
    if(pe->Type & isLabel) { untested();
      assert(L);
      L->cx__() += x1;  L->x1__() += x1;
      L->cy__() += y1;  L->y1__() += y1;
    }else{ untested();
      assert(!L);
      pe->setCenter(x1, y1, true);
    }
  }
}
#endif

// -----------------------------------------------------------
#if 0
void MouseActions::rotateElements(SchematicDoc *Doc, int& x1, int& y1)
{ untested();
  (void)Doc;
  incomplete();
  int x2, y2;
  Doc->setOnGrid(x1, y1);

  for(auto pe=movingElements.first(); pe != 0; pe = movingElements.next()) { untested();
    auto W=dynamic_cast<Wire*>(pe);
    switch(pe->Type) {
    case isComponent:
    case isAnalogComponent:
    case isDigitalComponent:
      ((Component*)pe)->rotate(); // rotate !before! rotating the center
      x2 = x1 - pe->cx_();
      pe->setCenter(pe->cy_() - y1 + x1, x2 + y1);
      break;
    case isWire:
      x2     = pe->x1_();
      W->x1__() = pe->y1_() - y1 + x1;
      W->y1__() = x1 - x2 + y1;
      x2     = pe->x2_();
      W->x2__() = pe->y2_() - y1 + x1;
      W->y2__() = x1 - x2 + y1;
      break;
    case isPainting:
      ((Painting*)pe)->rotate(); // rotate !before! rotating the center
      ((Painting*)pe)->getCenter(x2, y2);
      pe->setCenter(y2 - y1 + x1, x1 - x2 + y1);
          break;
    default:
      x2 = x1 - pe->cx_();   // if diagram -> only rotate cx/cy
      pe->setCenter(pe->cy_() - y1 + x1, x2 + y1);
      break;
    }
  }
}
#endif

// -----------------------------------------------------------
#if 1
void MouseActions::MReleasePaste(SchematicDoc *, QMouseEvent *)
{ untested();
#if 0
  int x1, y1, x2, y2, rot;
  QFileInfo Info(Doc->docName());
  //QPainter painter(Doc->viewport());
  QPointF pos=Doc->mapToScene(Event->pos());

  switch(Event->button()) {
  case Qt::LeftButton :
    // insert all moved elements into document
    for(auto pe : movingElements){
      pe->setSelected(false);
      switch(pe->Type) {
	case isWire:
	  if(pe->x1_() == pe->x2_()){ untested();
	    if(pe->y1_() == pe->y2_())  break;
	  }
	  Doc->insertWire((Wire*)pe);
	  if (Doc->wires().containsRef ((Wire*)pe))
	    Doc->enlargeView(pe->x1_(), pe->y1_(), pe->x2_(), pe->y2_());
	  else pe = NULL;
	  break;
	case isDiagram:
      Doc->pushBack((Diagram*)pe);
      ((Diagram*)pe)->loadGraphData(Info.path() + QDir::separator() +
					Doc->DataSet);
	  Doc->enlargeView(pe->cx_(), pe->cy_()-pe->y2_(), pe->cx_()+pe->x2_(), pe->cy_());
	  break;
	case isPainting:
	  Doc->paintings().append((Painting*)pe);
	  ((Painting*)pe)->Bounding(x1,y1,x2,y2);
	  Doc->enlargeView(x1, y1, x2, y2);
	  break;
	case isMovingLabel:
	  incomplete();
//	  pe->Type = isNodeLabel;
	  Doc->placeNodeLabel((WireLabel*)pe);
	  break;
//	case isComponent:
//	case isAnalogComponent:
//	case isDigitalComponent:
	default:
	  incomplete();
      }
      if(Component* C=dynamic_cast<Component*>(pe)){
	// legacy component
	Doc->insertElement(C);
	C->entireBounds(x1,y1,x2,y2, Doc->textCorr());
	Doc->enlargeView(x1, y1, x2, y2);
      }else if(Symbol* S=dynamic_cast<Symbol*>(pe)){ untested();
	Doc->insertElement(S);
	incomplete();
	// S->entireBounds(x1,y1,x2,y2, Doc->textCorr());
	Doc->enlargeView(x1, y1, x2, y2);
      }
    }

    pasteElements(Doc);
    // keep rotation sticky for pasted elements
    // rot = movingRotated;
    x1 = y1 = 0;
    while(rot--) { incomplete();
      // rotateElements(Doc,x1,y1);
    }

    QucsMain->MouseMoveAction = &MouseActions::MMovePaste;
    QucsMain->MousePressAction = 0;
    QucsMain->MouseReleaseAction = 0;
    QucsMain->MouseDoubleClickAction = 0;

    setDrawn(false);
    Doc->viewport()->update();
    Doc->setChanged(true, true);
    break;

  // ............................................................
  case Qt::RightButton :  // right button rotates the elements

    if(wasDrawn()){
      // erase old scheme
      // paintElementsScheme(Doc);
    }else{
    }
    setDrawn();

    x1 = pos.x();
    y1 = pos.y();
    // rotateElements(Doc,x1,y1);
    // paintElementsScheme(Doc);
    // save rotation
    movingRotated++;
    movingRotated &= 3;
    break;

  default: ;    // avoids compiler warnings
  }
#endif
}
#endif

// -----------------------------------------------------------
#if 0
void MouseActions::MReleaseMoveText(SchematicDoc *Doc, QMouseEvent *Event)
{ untested();
  if(Event->button() != Qt::LeftButton) return;

  QucsMain->MouseMoveAction = &MouseActions::MMoveMoveTextB;
  QucsMain->MouseReleaseAction = 0;
  Doc->releaseKeyboard();  // allow keyboard inputs again

  auto c=component(focusElement);
  assert(c);

  c->tx = MAx1 - c->cx();
  c->ty = MAy1 - c->cy();
  Doc->viewport()->update();
  setDrawn(false);
  Doc->setChanged(true, true);
}
#endif

// -----------------------------------------------------------
void MouseActions::MReleaseZoomIn(SchematicDoc *, QMouseEvent *)
{ untested();
  unreachable();
  return;
}


// ***********************************************************************
// **********                                                   **********
// **********    Functions for mouse button double clicking     **********
// **********                                                   **********
// ***********************************************************************
#if 0
void MouseActions::editElement(SchematicDoc *Doc, QMouseEvent *Event)
{ untested();
  assert(false); // obsolete;
//    qDebug() << "+double click, editElement";
//
// BUG: focusElement is a parameter
  if(focusElement == (Element*)nullptr){
    incomplete();
    return;
  }else{
  }

//  qDebug() << "+focusElement->Type" << focusElement->Type;

  int x1, y1, x2, y2;

  QFileInfo Info(Doc->docName());

  QPointF pos=Doc->mapToScene(Event->pos());
  float fX=pos.x();
  float fY=pos.y();

//  incomplete();
 // Element* E=element();
  ElementGraphics* EG=focusElement->element();
  Element* E=element(EG);
  E->schematicWidget(Doc);

  // BUG. move to respective classes. 1 at a time...
  if(auto c=component(focusElement)){ untested();
//         qDebug() << "cast focusElement into" << c->Name;
         bool done=false;
         if(c->obsolete_model_hack() == "GND") { // BUG
	   return;
	 }else if(c->obsolete_model_hack() == "SPICE") { // BUG. use cast
	   incomplete();
           // SpiceDialog *sd = new SpiceDialog(App, (SpiceFile*)c, Doc);
           // if(sd->exec() != 1) done=true;   // dialog is WDestructiveClose
         } else if(c->obsolete_model_hack() == ".Opt") { // BUG again...
	   incomplete();
           // OptimizeDialog *od = new OptimizeDialog((Optimize_Sim*)c, Doc);
           // if(od->exec() != 1) done=true;   // dialog is WDestructiveClose
         } else {
	   incomplete();
#if 0
           ComponentDialog * cd = new ComponentDialog(c, Doc);
           if(cd->exec() != 1){ untested();
	     done=true;   // dialog is WDestructiveClose
	   }else{ untested();
	     incomplete();
	     Doc->Components->findRef(c);
	     Doc->Components->take();
	     Doc->setComponentNumber(c); // for ports/power sources
	     Doc->Components->append(c);
	   }
#endif
         }

	 if(!done){ untested();
	   Doc->setChanged(true, true);
	   c->entireBounds(x1,y1,x2,y2, Doc->textCorr());
	   Doc->enlargeView(x1,y1,x2,y2);
	 }else{ untested();
	 }
  }else if(auto dia=diagram(focusElement)){ untested();
         bool done=false;
         if(dia->name().at(0) == 'T') { // don't open dialog on scrollbar
           if(dia->name() == "Time") {
             if(dia->cy() < int(fY)) {
	       if(dia->scroll(MAx1))
	         Doc->setChanged(true, true, 'm'); // 'm' = only the first time
	       done=true;
             }
	   }else if(dia->cx() > int(fX)) { untested();
	       if(dia->scroll(MAy1)){ untested();
	         Doc->setChanged(true, true, 'm'); // 'm' = only the first time
	       }
	       done=true;
	   }
	 }

	 if(!done){ untested();
	   incomplete();
#if 0
	   auto ddia=new DiagramDialog(dia, Doc);
	   if(ddia->exec() != QDialog::Rejected)   // is WDestructiveClose
	     Doc->setChanged(true, true);

	   dia->Bounding(x1, x2, y1, y2);
	   Doc->enlargeView(x1, x2, y1, y2);
#endif
	 }else{ untested();
	 }
  }else if(auto pg=graph(focusElement)){ untested();
	 // searching diagram for this graph
	 // BUG: a graph should know its parent.
	 for(auto d : Doc->diagrams()){
	   if(dia->Graphs.indexOf(pg) >= 0){
	     dia=d;
	     break;
	   }
	 }

	 if(dia){ untested();

	   incomplete();
#if 0
	   ddia = new DiagramDialog(dia, Doc, pg);
	   if(ddia->exec() != QDialog::Rejected)   // is WDestructiveClose
	     Doc->setChanged(true, true);
#endif
	 }else{ untested();
	 }

  }else if(auto w=wire(focusElement)){
    (void) w;
         MPressLabel(Doc, Event);

  }else if(auto l=wireLabel(focusElement)){ untested();
         editLabel(Doc, l);
         // update highlighting, labels may have changed
         Doc->highlightWireLabels ();
  }else if(auto p=painting(focusElement)){ untested();

         if( p->Dialog() )
           Doc->setChanged(true, true);

  }else if(auto m=marker(focusElement)){
    (void) m;
    incomplete();
        // mdia = new MarkerDialog(m, Doc);
        // if(mdia->exec() > 1)
        //   Doc->setChanged(true, true);
  }else{
  }

  // Very strange: Now an open VHDL editor gets all the keyboard input !?!
  // I don't know why it only happens here, nor am I sure whether it only
  // happens here. Anyway, I hope the best and give the focus back to the
  // current document.
  Doc->setFocus();

  Doc->viewport()->update();
  setDrawn(false);
}
#endif

// -----------------------------------------------------------
#if 0
void MouseActions::MDoubleClickSelect(SchematicDoc *Doc, QMouseEvent *Event)
{
  qDebug() << "doubleclick";
  Doc->releaseKeyboard();  // allow keyboard inputs again
  QucsMain->editText->setHidden(true);
  editElement(Doc, Event);
}
#endif


/**
 * @brief MouseActions::MDoubleClickWire2  Double click terminates wire insertion.
 * @param Doc
 * @param Event
 */
void MouseActions::MDoubleClickWire2(SchematicDoc *, QMouseEvent *)
{
  incomplete();
  // MPressWire2(Doc, Event);

  if(formerAction)
    QucsMain->select->setChecked(true);  // restore old action
  else { untested();
//    QucsMain->MouseMoveAction = &MouseActions::MMoveWire1;
//    QucsMain->MousePressAction = &MouseActions::MPressWire1;
    QucsMain->MouseDoubleClickAction = 0;
  }
}

#if 0
void ElementMouseAction::setSelected(bool x)
{ untested();
  if(_e){
    _e->setSelected(x);
  }else{
    unreachable();
  }
}

bool ElementMouseAction::isSelected() const
{
  assert(_e);
  return _e->isSelected();
}
#endif

QucsDoc& MouseActions::doc()
{
  return _doc;
}

// TODO: move to schematic_doc or _scene
// what does it return?
bool MouseActions::handle(QEvent*e)
{itested();
  QUndoCommand* c=nullptr;
  if(_maCurrent){itested();
    c = _maCurrent->handle(e);
  }else{
  }

  if(c){itested();
    // emit signalUndoState(true);
    executeCommand(c);
    return true;
  }else{
    return false;
  }

}

void MouseActions::executeCommand(QUndoCommand* c)
{
  assert(c);
  QUndoStack* u = _doc.undoStack();

  if(u){itested();
    u->push(c); // also calls redo

    // train wreck. must be part of push. fix later.
    assert(doc()._app);
    assert(doc()._app->undo);
    doc()._app->undo->setEnabled(true);
  }else{ untested();
    // forget about it.
    delete c;
  }
}

void MouseAction::setCursor(QCursor const& c)
{
  doc().setCursor(c);
}

// vim:ts=8:sw=2:noet
