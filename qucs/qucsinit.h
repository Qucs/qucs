/***************************************************************************
                          qucsinit.h  -  description
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

#ifndef QUCSINIT_H
#define QUCSINIT_H

#include <qaction.h>
#include <qaccel.h>
#include <qobject.h>
#include <qtoolbar.h>
#include <qpopupmenu.h>

class QucsApp;
class QucsActions;


/**
  *@author Michael Margraf
  */

class QucsInit : public QObject {
  Q_OBJECT
public:
  QucsInit();
  ~QucsInit();

  void perform(QucsApp *p_);

public slots:
  void slotViewToolBar(bool toggle);    // toggle the toolbar
  void slotViewStatusBar(bool toggle);  // toggle the statusbar

  void slotHelpAbout();       // shows an about dialog
  void slotHelpAboutQt();     // shows the standard about dialog for Qt

private:
  void initActions();    // initializes all QActions of the application
  void initMenuBar();    // creates the menu_bar and inserts the menuitems
  void initToolBar();    // creates the toolbars
  void initStatusBar();  // setup the statusbar

  QucsApp *App;   // the application that called this instance
  QucsActions *Acts;

  QAction *helpAboutApp, *helpAboutQt, *viewToolBar, *viewStatusBar;

  // menus contain the items of their menubar
  QPopupMenu *fileMenu, *editMenu, *insMenu, *projMenu, *simMenu, *viewMenu;
  QPopupMenu *helpMenu;

  QToolBar *fileToolbar, *editToolbar, *viewToolbar, *workToolbar;

  QAccel *mainAccel;     // to set more than one key to one action
};

#endif
