/***************************************************************************
                          qucsactions.h  -  description
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

#ifndef QUCSACTIONS_H
#define QUCSACTIONS_H

#include <qaction.h>


class QucsApp;
class QucsView;


/**
  *@author Michael Margraf
  */

class QucsActions : public QObject {
  Q_OBJECT
public:
  QucsActions();
  ~QucsActions();

  void init(QucsApp *p_);

  QAction *insWire, *insLabel, *insGround, *insPort, *insEquation;
  QAction *editRotate, *editMirror, *editMirrorY, *editPaste, *select;
  QAction *editActivate, *wire, *editDelete, *setMarker;

public slots:
  void slotEditRotate(bool on);  // rotate the selected items
  void slotEditMirrorX(bool on); // mirror the selected items about X axis
  void slotEditMirrorY(bool on); // mirror the selected items about Y axis
  void slotEditPaste(bool on);   // paste the clipboard into the document
  void slotEditDelete(bool on);  // delete the selected items
  void slotInsertEquation(bool on);
  void slotInsertGround(bool on);
  void slotInsertPort(bool on);
  void slotSetWire(bool on);
  void slotSelect(bool on);
  void slotEditActivate(bool on);
  void slotInsertLabel(bool on);
  void slotSetMarker(bool on);

private:
  // copies of variables in QucsApps
  QucsApp  *App;
  QucsView *view;
};

#endif
