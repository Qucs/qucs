/***************************************************************************
                          qucsactions.cpp  -  description
                             -------------------
    begin                : Sat May 1 2004
    copyright            : (C) 2004 by Michael Margraf
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

#include "qucsactions.h"

#include "qucs.h"



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
// Is called when the rotate toolbar button is pressed.
void QucsActions::slotEditRotate(bool on)
{
  if(!on) {
    view->MouseMoveAction = &QucsView::MouseDoNothing;
    view->MousePressAction = &QucsView::MouseDoNothing;
    view->MouseReleaseAction = &QucsView::MouseDoNothing;
    view->MouseDoubleClickAction = &QucsView::MouseDoNothing;
    App->activeAction = 0;   // no action active
    return;
  }

  if(view->Docs.current()->rotateElements()) {
    editRotate->blockSignals(true);
    editRotate->setOn(false);  // release toolbar button
    editRotate->blockSignals(false);
  }
  else {
    if(App->activeAction) {
      App->activeAction->blockSignals(true); // do not call toggle slot
      App->activeAction->setOn(false);       // set last toolbar button off
      App->activeAction->blockSignals(false);
    }
    App->activeAction = editRotate;

    view->MouseMoveAction = &QucsView::MMoveRotate;
    view->MousePressAction = &QucsView::MPressRotate;
    view->MouseReleaseAction = &QucsView::MouseDoNothing;
    view->MouseDoubleClickAction = &QucsView::MouseDoNothing;
  }
  view->viewport()->repaint();
  view->drawn = false;
}

// ######################################################################
// Is called when the mirror toolbar button is pressed.
void QucsActions::slotEditMirrorX(bool on)
{
  if(!on) {
    view->MouseMoveAction = &QucsView::MouseDoNothing;
    view->MousePressAction = &QucsView::MouseDoNothing;
    view->MouseReleaseAction = &QucsView::MouseDoNothing;
    view->MouseDoubleClickAction = &QucsView::MouseDoNothing;
    App->activeAction = 0;   // no action active
    return;
  }

  if(view->Docs.current()->mirrorXComponents()) {
    editMirror->blockSignals(true);
    editMirror->setOn(false);  // release toolbar button
    editMirror->blockSignals(false);
  }
  else {
    if(App->activeAction) {
      App->activeAction->blockSignals(true); // do not call toggle slot
      App->activeAction->setOn(false);       // set last toolbar button off
      App->activeAction->blockSignals(false);
    }
    App->activeAction = editMirror;

    view->MouseMoveAction = &QucsView::MMoveMirrorX;
    view->MousePressAction = &QucsView::MPressMirrorX;
    view->MouseReleaseAction = &QucsView::MouseDoNothing;
    view->MouseDoubleClickAction = &QucsView::MouseDoNothing;
  }
  view->viewport()->repaint();
  view->drawn = false;
}

// ######################################################################
// Is called when the mirror toolbar button is pressed.
void QucsActions::slotEditMirrorY(bool on)
{
  if(!on) {
    view->MouseMoveAction = &QucsView::MouseDoNothing;
    view->MousePressAction = &QucsView::MouseDoNothing;
    view->MouseReleaseAction = &QucsView::MouseDoNothing;
    view->MouseDoubleClickAction = &QucsView::MouseDoNothing;
    App->activeAction = 0;   // no action active
    return;
  }

  if(view->Docs.current()->mirrorYComponents()) {
    editMirrorY->blockSignals(true);
    editMirrorY->setOn(false);  // release toolbar button
    editMirrorY->blockSignals(false);
  }
  else {
    if(App->activeAction) {
      App->activeAction->blockSignals(true); // do not call toggle slot
      App->activeAction->setOn(false);       // set last toolbar button off
      App->activeAction->blockSignals(false);
    }
    App->activeAction = editMirrorY;

    view->MouseMoveAction = &QucsView::MMoveMirrorY;
    view->MousePressAction = &QucsView::MPressMirrorY;
    view->MouseReleaseAction = &QucsView::MouseDoNothing;
    view->MouseDoubleClickAction = &QucsView::MouseDoNothing;
  }
  view->viewport()->repaint();
  view->drawn = false;
}

// -----------------------------------------------------------------------
// Is called when the activate/deactivate toolbar button is pressed.
void QucsActions::slotEditActivate(bool on)
{
  if(!on) {
    view->MouseMoveAction = &QucsView::MouseDoNothing;
    view->MousePressAction = &QucsView::MouseDoNothing;
    view->MouseReleaseAction = &QucsView::MouseDoNothing;
    view->MouseDoubleClickAction = &QucsView::MouseDoNothing;
    App->activeAction = 0;   // no action active
    return;
  }

  if(view->Docs.current()->activateComponents()) {
    editActivate->blockSignals(true);
    editActivate->setOn(false);  // release toolbar button
    editActivate->blockSignals(false);
  }
  else {
    if(App->activeAction) {
      App->activeAction->blockSignals(true); // do not call toggle slot
      App->activeAction->setOn(false);       // set last toolbar button off
      App->activeAction->blockSignals(false);
    }
    App->activeAction = editActivate;

    // if no component is selected, activate the one that will be clicked on
    view->MouseMoveAction = &QucsView::MMoveActivate;
    view->MousePressAction = &QucsView::MPressActivate;
    view->MouseReleaseAction = &QucsView::MouseDoNothing;
    view->MouseDoubleClickAction = &QucsView::MouseDoNothing;
  }

  view->viewport()->repaint();
  view->drawn = false;
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

// ------------------------------------------------------------------------
// Is called if "Delete"-Button is pressed.
void QucsActions::slotEditDelete(bool on)
{
  if(!on) {
    view->MouseMoveAction = &QucsView::MouseDoNothing;
    view->MousePressAction = &QucsView::MouseDoNothing;
    view->MouseReleaseAction = &QucsView::MouseDoNothing;
    view->MouseDoubleClickAction = &QucsView::MouseDoNothing;
    App->activeAction = 0;   // no action active
    return;
  }

  if(view->Docs.current()->deleteElements()) {
    editDelete->blockSignals(true);
    editDelete->setOn(false);  // release toolbar button
    editDelete->blockSignals(false);
  }
  else {
    if(App->activeAction) {
      App->activeAction->blockSignals(true); // do not call toggle slot
      App->activeAction->setOn(false);       // set last toolbar button off
      App->activeAction->blockSignals(false);
    }
    App->activeAction = editDelete;

    // if no component is selected, delete the one that will be clicked on
    view->MouseMoveAction = &QucsView::MMoveDelete;
    view->MousePressAction = &QucsView::MPressDelete;
    view->MouseReleaseAction = &QucsView::MouseDoNothing;
    view->MouseDoubleClickAction = &QucsView::MouseDoNothing;
  }
  view->viewport()->repaint();
  view->drawn = false;
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
    // do not disturb diagram resize
    if(view->MouseMoveAction == &QucsView::MMoveSelect) {
      App->activeAction->blockSignals(true); // do not call toggle slot
      App->activeAction->setOn(true);        // set back select on
      App->activeAction->blockSignals(false);
      return;
    }

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

  if(view->selComp != 0)
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

  if(view->selComp != 0)
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

  if(view->selComp != 0)
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

  App->statusBar()->message(tr("Inserting clipboard contents..."));

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

  App->statusBar()->message(tr("Ready."));
}
