/***************************************************************************
                          qucsactions.cpp  -  description
                             -------------------
    begin                : Sat May 1 2004
    copyright            : (C) 2004 by Michael Margraf
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

#include "qucsactions.h"

#include "qucs.h"
#include "components/ground.h"
#include "components/subcirport.h"
#include "components/equation.h"


QucsActions::QucsActions()
{
}

QucsActions::~QucsActions()
{
}

// ########################################################################
void QucsActions::init(QucsApp *p_)
{
  App  = p_;
  view = App->view;
}

// ########################################################################
// This function is called from all toggle actions.
void QucsActions::performToggleAction(bool on, QAction *Action,
	pToggleFunc Function, pMouseFunc MouseMove, pMouseFunc MousePress)
{
  if(!on) {
    view->MouseMoveAction = &QucsView::MouseDoNothing;
    view->MousePressAction = &QucsView::MouseDoNothing;
    view->MouseReleaseAction = &QucsView::MouseDoNothing;
    view->MouseDoubleClickAction = &QucsView::MouseDoNothing;
    App->activeAction = 0;   // no action active
    return;
  }

  if((view->Docs.current()->*Function)()) {
    Action->blockSignals(true);
    Action->setOn(false);  // release toolbar button
    Action->blockSignals(false);
  }
  else {
    if(App->activeAction) {
      App->activeAction->blockSignals(true); // do not call toggle slot
      App->activeAction->setOn(false);       // set last toolbar button off
      App->activeAction->blockSignals(false);
    }
    App->activeAction = Action;

    view->MouseMoveAction = MouseMove;
    view->MousePressAction = MousePress;
    view->MouseReleaseAction = &QucsView::MouseDoNothing;
    view->MouseDoubleClickAction = &QucsView::MouseDoNothing;
  }
  view->viewport()->repaint();
  view->drawn = false;
}

// ########################################################################
// Is called when the rotate toolbar button is pressed.
void QucsActions::slotEditRotate(bool on)
{
  performToggleAction(on, editRotate, &QucsDoc::rotateElements,
		&QucsView::MMoveRotate, &QucsView::MPressRotate);
}

// ######################################################################
// Is called when the mirror toolbar button is pressed.
void QucsActions::slotEditMirrorX(bool on)
{
  performToggleAction(on, editMirror, &QucsDoc::mirrorXComponents,
		&QucsView::MMoveMirrorX, &QucsView::MPressMirrorX);
}

// ######################################################################
// Is called when the mirror toolbar button is pressed.
void QucsActions::slotEditMirrorY(bool on)
{
  performToggleAction(on, editMirrorY, &QucsDoc::mirrorYComponents,
		&QucsView::MMoveMirrorY, &QucsView::MPressMirrorY);
}

// -----------------------------------------------------------------------
// Is called when the activate/deactivate toolbar button is pressed.
void QucsActions::slotEditActivate(bool on)
{
  performToggleAction(on, editActivate, &QucsDoc::activateComponents,
		&QucsView::MMoveActivate, &QucsView::MPressActivate);
}

// ------------------------------------------------------------------------
// Is called if "Delete"-Button is pressed.
void QucsActions::slotEditDelete(bool on)
{
  performToggleAction(on, editDelete, &QucsDoc::deleteElements,
		&QucsView::MMoveDelete, &QucsView::MPressDelete);
}

// -----------------------------------------------------------------------
// Is called if "Wire"-Button is pressed.
void QucsActions::slotSetWire(bool on)
{
  if(!on) {
    view->MouseMoveAction = &QucsView::MouseDoNothing;
    view->MousePressAction = &QucsView::MouseDoNothing;
    view->MouseReleaseAction = &QucsView::MouseDoNothing;
    view->MouseDoubleClickAction = &QucsView::MouseDoNothing;
    App->activeAction = 0;   // no action active
    if(view->drawn) view->viewport()->repaint();
    return;
  }
  if(App->activeAction) {
    App->activeAction->blockSignals(true); // do not call toggle slot
    App->activeAction->setOn(false);       // set last toolbar button off
    App->activeAction->blockSignals(false);
  }
  App->activeAction = insWire;

  if(view->drawn) view->viewport()->repaint();
  view->drawn = false;
  view->MouseMoveAction = &QucsView::MMoveWire1;
  view->MousePressAction = &QucsView::MPressWire1;
  view->MouseReleaseAction = &QucsView::MouseDoNothing;
  view->MouseDoubleClickAction = &QucsView::MouseDoNothing;
}

// -----------------------------------------------------------------------
void QucsActions::slotInsertLabel(bool on)
{
  if(!on) {
    view->MouseMoveAction = &QucsView::MouseDoNothing;
    view->MousePressAction = &QucsView::MouseDoNothing;
    view->MouseReleaseAction = &QucsView::MouseDoNothing;
    view->MouseDoubleClickAction = &QucsView::MouseDoNothing;
    App->activeAction = 0;   // no action active
    return;
  }
  if(App->activeAction) {
    App->activeAction->blockSignals(true); // do not call toggle slot
    App->activeAction->setOn(false);       // set last toolbar button off
    App->activeAction->blockSignals(false);
  }
  App->activeAction = insLabel;

  if(view->drawn) view->viewport()->repaint();
  view->drawn = false;
  view->MouseMoveAction = &QucsView::MMoveLabel;
  view->MousePressAction = &QucsView::MPressLabel;
  view->MouseReleaseAction = &QucsView::MouseDoNothing;
  view->MouseDoubleClickAction = &QucsView::MouseDoNothing;
}

// -----------------------------------------------------------------------
void QucsActions::slotSetMarker(bool on)
{
  if(!on) {
    view->MouseMoveAction = &QucsView::MouseDoNothing;
    view->MousePressAction = &QucsView::MouseDoNothing;
    view->MouseReleaseAction = &QucsView::MouseDoNothing;
    view->MouseDoubleClickAction = &QucsView::MouseDoNothing;
    App->activeAction = 0;   // no action active
    return;
  }
  if(App->activeAction) {
    App->activeAction->blockSignals(true); // do not call toggle slot
    App->activeAction->setOn(false);       // set last toolbar button off
    App->activeAction->blockSignals(false);
  }
  App->activeAction = setMarker;

  if(view->drawn) view->viewport()->repaint();
  view->drawn = false;
  view->MouseMoveAction = &QucsView::MMoveMarker;
  view->MousePressAction = &QucsView::MPressMarker;
  view->MouseReleaseAction = &QucsView::MouseDoNothing;
  view->MouseDoubleClickAction = &QucsView::MouseDoNothing;
}

// -----------------------------------------------------------------------
// Is called when the select toolbar button is pressed.
void QucsActions::slotSelect(bool on)
{
  if(!on) {
    view->MouseMoveAction = &QucsView::MouseDoNothing;
    view->MousePressAction = &QucsView::MouseDoNothing;
    view->MouseReleaseAction = &QucsView::MouseDoNothing;
    view->MouseDoubleClickAction = &QucsView::MouseDoNothing;
    App->activeAction = 0;   // no action active

    return;
  }
  if(App->activeAction) {
    App->activeAction->blockSignals(true); // do not call toggle slot
    App->activeAction->setOn(false);       // set last toolbar button off
    App->activeAction->blockSignals(false);
  }
  App->activeAction = select;

  if(view->drawn) view->viewport()->repaint();
  view->drawn = false;
  view->MouseMoveAction = &QucsView::MouseDoNothing;
  view->MousePressAction = &QucsView::MPressSelect;
  view->MouseReleaseAction = &QucsView::MReleaseSelect;
  view->MouseDoubleClickAction = &QucsView::MDoubleClickSelect;
}

// ########################################################################
// Is called when the mouse is clicked upon the equation toolbar button.
void QucsActions::slotInsertEquation(bool on)
{
  if(!on) {
    view->MouseMoveAction = &QucsView::MouseDoNothing;
    view->MousePressAction = &QucsView::MouseDoNothing;
    view->MouseReleaseAction = &QucsView::MouseDoNothing;
    view->MouseDoubleClickAction = &QucsView::MouseDoNothing;
    App->activeAction = 0;   // no action active
    return;
  }
  if(App->activeAction) {
    App->activeAction->blockSignals(true); // do not call toggle slot
    App->activeAction->setOn(false);       // set last toolbar button off
    App->activeAction->blockSignals(false);
  }
  App->activeAction = insEquation;

  if(view->selComp)
    delete view->selComp;  // delete previously selected component

  view->selComp = new Equation();

  if(view->drawn) view->viewport()->repaint();
  view->drawn = false;
  view->MouseMoveAction = &QucsView::MMoveComponent;
  view->MousePressAction = &QucsView::MPressComponent;
  view->MouseReleaseAction = &QucsView::MouseDoNothing;
  view->MouseDoubleClickAction = &QucsView::MouseDoNothing;
}

// #######################################################################
// Is called when the mouse is clicked upon the ground toolbar button.
void QucsActions::slotInsertGround(bool on)
{
  if(!on) {
    view->MouseMoveAction = &QucsView::MouseDoNothing;
    view->MousePressAction = &QucsView::MouseDoNothing;
    view->MouseReleaseAction = &QucsView::MouseDoNothing;
    view->MouseDoubleClickAction = &QucsView::MouseDoNothing;
    App->activeAction = 0;   // no action active
    return;
  }
  if(App->activeAction) {
    App->activeAction->blockSignals(true); // do not call toggle slot
    App->activeAction->setOn(false);       // set last toolbar button off
    App->activeAction->blockSignals(false);
  }
  App->activeAction = insGround;

  if(view->selComp)
    delete view->selComp;  // delete previously selected component

  view->selComp = new Ground();

  if(view->drawn) view->viewport()->repaint();
  view->drawn = false;
  view->MouseMoveAction = &QucsView::MMoveComponent;
  view->MousePressAction = &QucsView::MPressComponent;
  view->MouseReleaseAction = &QucsView::MouseDoNothing;
  view->MouseDoubleClickAction = &QucsView::MouseDoNothing;
}

// #######################################################################
// Is called when the mouse is clicked upon the port toolbar button.
void QucsActions::slotInsertPort(bool on)
{
  if(!on) {
    view->MouseMoveAction = &QucsView::MouseDoNothing;
    view->MousePressAction = &QucsView::MouseDoNothing;
    view->MouseReleaseAction = &QucsView::MouseDoNothing;
    view->MouseDoubleClickAction = &QucsView::MouseDoNothing;
    App->activeAction = 0;   // no action active
    return;
  }
  if(App->activeAction) {
    App->activeAction->blockSignals(true); // do not call toggle slot
    App->activeAction->setOn(false);       // set last toolbar button off
    App->activeAction->blockSignals(false);
  }
  App->activeAction = insPort;

  if(view->selComp)
    delete view->selComp;  // delete previously selected component

  view->selComp = new SubCirPort();

  if(view->drawn) view->viewport()->repaint();
  view->drawn = false;
  view->MouseMoveAction = &QucsView::MMoveComponent;
  view->MousePressAction = &QucsView::MPressComponent;
  view->MouseReleaseAction = &QucsView::MouseDoNothing;
  view->MouseDoubleClickAction = &QucsView::MouseDoNothing;
}

// ######################################################################
void QucsActions::slotEditPaste(bool on)
{
  if(!on) {
    view->MouseMoveAction = &QucsView::MouseDoNothing;
    view->MousePressAction = &QucsView::MouseDoNothing;
    view->MouseReleaseAction = &QucsView::MouseDoNothing;
    view->MouseDoubleClickAction = &QucsView::MouseDoNothing;
    App->activeAction = 0;   // no action active
    return;
  }

  if(!view->pasteElements()) {
    editPaste->blockSignals(true); // do not call toggle slot
    editPaste->setOn(false);       // set toolbar button off
    editPaste->blockSignals(false);
    return;   // if clipboard empty
  }

  if(App->activeAction) {
    App->activeAction->blockSignals(true); // do not call toggle slot
    App->activeAction->setOn(false);       // set last toolbar button off
    App->activeAction->blockSignals(false);
  }
  App->activeAction = editPaste;

  view->drawn = false;
  view->MouseMoveAction = &QucsView::MMovePaste;
  view->MousePressAction = &QucsView::MouseDoNothing;
  view->MouseReleaseAction = &QucsView::MouseDoNothing;
  view->MouseDoubleClickAction = &QucsView::MouseDoNothing;
}

// #######################################################################
// Is called, when "set on grid" action is activated.
void QucsActions::slotOnGrid(bool on)
{
  if(!on) {
    view->MouseMoveAction = &QucsView::MouseDoNothing;
    view->MousePressAction = &QucsView::MouseDoNothing;
    view->MouseReleaseAction = &QucsView::MouseDoNothing;
    view->MouseDoubleClickAction = &QucsView::MouseDoNothing;
    App->activeAction = 0;   // no action active
    return;
  }

  if(view->Docs.current()->elementsOnGrid() > 0) {
    view->viewport()->repaint();
    view->drawn = false;
    onGrid->blockSignals(true); // do not call toggle slot
    onGrid->setOn(false);       // set toolbar button off
    onGrid->blockSignals(false);
    return;   // if no element was selected
  }

  if(App->activeAction) {
    App->activeAction->blockSignals(true); // do not call toggle slot
    App->activeAction->setOn(false);       // set last toolbar button off
    App->activeAction->blockSignals(false);
  }
  App->activeAction = onGrid;

  if(view->drawn) view->viewport()->repaint();
  view->drawn = false;
  view->MouseMoveAction = &QucsView::MMoveOnGrid;
  view->MousePressAction = &QucsView::MPressOnGrid;
  view->MouseReleaseAction = &QucsView::MouseDoNothing;
  view->MouseDoubleClickAction = &QucsView::MouseDoNothing;
}

// #######################################################################
// Is called, when "move component text" action is activated.
void QucsActions::slotMoveText(bool on)
{
  if(!on) {
    view->MouseMoveAction = &QucsView::MouseDoNothing;
    view->MousePressAction = &QucsView::MouseDoNothing;
    view->MouseReleaseAction = &QucsView::MouseDoNothing;
    view->MouseDoubleClickAction = &QucsView::MouseDoNothing;
    App->activeAction = 0;   // no action active
    if(view->drawn) view->viewport()->repaint();
    return;
  }
  if(App->activeAction) {
    App->activeAction->blockSignals(true); // do not call toggle slot
    App->activeAction->setOn(false);       // set last toolbar button off
    App->activeAction->blockSignals(false);
  }
  App->activeAction = moveText;

  if(view->drawn) view->viewport()->repaint();
  view->drawn = false;
  view->MouseMoveAction = &QucsView::MMoveMoveTextB;
  view->MousePressAction = &QucsView::MPressMoveText;
  view->MouseReleaseAction = &QucsView::MouseDoNothing;
  view->MouseDoubleClickAction = &QucsView::MouseDoNothing;
}
