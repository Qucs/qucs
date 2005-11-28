/***************************************************************************
                               qucsactions.h
                              ---------------
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


typedef bool (QucsDoc::*pToggleFunc) (); // pointer to toggle action
typedef void (QucsView::*pMouseFunc) (QMouseEvent*);
typedef void (QucsView::*pMouseFunc2) (QMouseEvent*, QucsDoc*, int, int);


class QucsActions : public QObject {
  Q_OBJECT
public:
  QucsActions();
 ~QucsActions();

  void init(QucsApp *p_);
  void editFile(const QString&);

  QAction *insWire, *insLabel, *insGround, *insPort, *insEquation, *magPlus,
          *editRotate, *editMirror, *editMirrorY, *editPaste, *select,
          *editActivate, *wire, *editDelete, *setMarker, *onGrid, *moveText,
          *helpIndex, *helpGetStart, *callEditor, *callFilter, *callLine,
          *showMsg, *showNet, *alignTop, *alignBottom, *alignLeft, *alignRight,
          *distrHor, *distrVert, *selectAll, *callLib, *callMatch, *changeProps,
          *addToProj;

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
  void slotZoomIn(bool);
  void slotEditUndo();    // makes the last operation undone
  void slotEditRedo();    // makes the last undo undone
  void slotAlignTop();    // align selected elements with respect to top
  void slotAlignBottom(); // align selected elements with respect to bottom
  void slotAlignLeft();   // align selected elements with respect to left
  void slotAlignRight();  // align selected elements with respect to right
  void slotDistribHoriz();// distribute horizontally selected elements
  void slotDistribVert(); // distribute vertically selected elements
  void slotSelectAll();
  void slotShowLastMsg();
  void slotShowLastNetlist();
  void slotCallEditor();
  void slotCallFilter();
  void slotCallLine();
  void slotCallLibrary();
  void slotCallMatch();
  void slotHelpIndex();       // shows a HTML docu: Help Index
  void slotGettingStarted();  // shows a HTML docu: Getting started
  void slotChangeProps();
  void slotAddToProject();

private:
  void showHTML(const QString&);
  bool performToggleAction(bool, QAction*, pToggleFunc, pMouseFunc, pMouseFunc2);

  // copies of variables in QucsApps
  QucsApp  *App;
  QucsView *view;
};

#endif
