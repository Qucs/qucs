/***************************************************************************
                          qucsactions.h  -  description
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

#ifndef QUCSACTIONS_H
#define QUCSACTIONS_H

#include <qaction.h>


class QucsApp;
class QucsDoc;
class QucsView;


/**
  *@author Michael Margraf
  */

typedef bool (QucsDoc::*pToggleFunc) (); // pointer to toggle action
typedef void (QucsView::*pMouseFunc) (QMouseEvent*);


class QucsActions : public QObject {
  Q_OBJECT
public:
  QucsActions();
  ~QucsActions();

  void init(QucsApp *p_);

  QAction *insWire, *insLabel, *insGround, *insPort, *insEquation;
  QAction *editRotate, *editMirror, *editMirrorY, *editPaste, *select;
  QAction *editActivate, *wire, *editDelete, *setMarker, *onGrid, *moveText;

public slots:
  void slotEditRotate(bool);  // rotate the selected items
  void slotEditMirrorX(bool); // mirror the selected items about X axis
  void slotEditMirrorY(bool); // mirror the selected items about Y axis
  void slotEditPaste(bool);   // paste the clipboard into the document
  void slotEditDelete(bool);  // delete the selected items
  void slotInsertEquation(bool);
  void slotInsertGround(bool);
  void slotInsertPort(bool);
  void slotSetWire(bool);
  void slotSelect(bool);
  void slotEditActivate(bool);
  void slotInsertLabel(bool);
  void slotSetMarker(bool);
  void slotOnGrid(bool);      // set selected elements on grid
  void slotMoveText(bool);    // move property text of components

private:
  void QucsActions::performToggleAction(bool, QAction*, pToggleFunc,
					pMouseFunc, pMouseFunc);

  // copies of variables in QucsApps
  QucsApp  *App;
  QucsView *view;
};

#endif
