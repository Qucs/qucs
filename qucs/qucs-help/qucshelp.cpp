/***************************************************************************
                          qucshelp.cpp  -  description
                             -------------------
    begin                : Sun Jan 11 2004
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

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include "qucshelp.h"

#include <qpushbutton.h>
#include <qaction.h>
#include <qpixmap.h>
#include <qfile.h>
#include <qtextstream.h>
#include <qpopupmenu.h>
#include <qmenubar.h>
#include <qapplication.h>

QucsHelp::QucsHelp(const QString& page)
{
  // set application icon
  setIcon (QPixmap(QucsSettings.BitmapDir + "big.qucs.xpm"));
  setCaption(tr("Qucs Help System"));

  text = new QTextBrowser(this);
  text->setMinimumSize(400,200);
  setCentralWidget(text);
  setupActions();
  initList();

  text->setSource(QucsHelpDir.filePath(links[0]));

  // .......................................
  if(!page.isEmpty())
    text->setSource(QucsHelpDir.filePath(page));
}

QucsHelp::~QucsHelp()
{
}

//-----------------------------------------------------------------
// To get all close events.
void QucsHelp::closeEvent(QCloseEvent *Event)
{
  int tmp;
  tmp = x();		// call size and position function in order to ...
  tmp = y();		// ... set them correctly before closing the ...
  tmp = width();	// dialog !!!  Otherwise the frame of the window ...
  tmp = height();	// will not be recognized (a X11 problem).

  Event->accept();
}

void QucsHelp::setupActions()
{
  QToolBar *toolbar = new QToolBar(this,"main_toolbar");
  QMenuBar *bar = menuBar();
  statusBar();

  const QKeySequence ks = QKeySequence();

  QAction *quitAction = new QAction(QIconSet(QPixmap(QucsSettings.BitmapDir + "quit.png")),
                                    tr("&Quit"), CTRL+Key_Q, this);
  QAction *backAction = new QAction(QIconSet(QPixmap(QucsSettings.BitmapDir + "back.png")),
                                    tr("&Back"), ALT+Key_Left, this);
  QAction *forwardAction = new QAction(QIconSet(QPixmap(QucsSettings.BitmapDir + "forward.png")),
                                       tr("&Forward"), ALT+Key_Right, this);
  QAction *homeAction = new QAction(QIconSet(QPixmap(QucsSettings.BitmapDir + "home.png")),
                                    tr("&Home"),CTRL+Key_H,this);
  previousAction = new QAction(QIconSet(QPixmap(QucsSettings.BitmapDir + "previous.png")),tr("&Previous"),
                               ks, this);
  nextAction = new QAction(QIconSet(QPixmap(QucsSettings.BitmapDir + "next.png")),
                           tr("&Next"), ks, this);

  connect(quitAction,SIGNAL(activated()),qApp,SLOT(quit()));

  connect(backAction,SIGNAL(activated()),text,SLOT(backward()));
  connect(text,SIGNAL(backwardAvailable(bool)),backAction,SLOT(setEnabled(bool)));

  connect(forwardAction,SIGNAL(activated()),text,SLOT(forward()));
  connect(text,SIGNAL(forwardAvailable(bool)),forwardAction,SLOT(setEnabled(bool)));

  connect(homeAction,SIGNAL(activated()),text,SLOT(home()));

  connect(text,SIGNAL(sourceChanged(const QString &)),this,SLOT(slotSourceChanged(const QString&)));
  connect(previousAction,SIGNAL(activated()),this,SLOT(previousLink()));
  connect(nextAction,SIGNAL(activated()),this,SLOT(nextLink()));


  backAction->addTo(toolbar);
  forwardAction->addTo(toolbar);
  toolbar->addSeparator();
  homeAction->addTo(toolbar);
  previousAction->addTo(toolbar);
  nextAction->addTo(toolbar);
  toolbar->addSeparator();
  quitAction->addTo(toolbar);

  QPopupMenu *fileMenu = new QPopupMenu(this);
  quitAction->addTo(fileMenu);

  QPopupMenu *viewMenu = new QPopupMenu(this);
  backAction->addTo(viewMenu);
  forwardAction->addTo(viewMenu);
  homeAction->addTo(viewMenu);
  previousAction->addTo(viewMenu);
  nextAction->addTo(viewMenu);
  
  QPopupMenu *helpMenu = new QPopupMenu(this);
  helpMenu->insertItem("&About Qt",qApp,SLOT(aboutQt()));
  
  bar->insertItem( "&File", fileMenu );
  bar->insertItem("&View",viewMenu);
  bar->insertSeparator();
  bar->insertItem("&Help",helpMenu);
}

//This function finds all links dynamically from "index.html"
//This is used for next and back action
void QucsHelp::initList()
{
  links << QString("index.html");
  QFile file(QucsHelpDir.filePath("index.html"));
  if(!file.open(IO_ReadOnly))
    return;
  QTextStream str(&file);
  QString line,link;
  int index = -1;
  int end = -1;
  while ( !str.atEnd() )
  {
    line = str.readLine();
    index = line.find("href=\"");//find link to other file
    if(index != -1)
    {
      index += 6;
      end = line.find('"',index);
      if(end == -1)
      {
        qWarning("can't find end quote. May be HTML error");
        return;
      }
      link = line.mid(index,end-index);
      if(link.startsWith("http"))//discard links to site
        continue;
      links << link;
    }
  }
}

//This slot updates next and previous actions i.e enabling/disabling
void QucsHelp::slotSourceChanged(const QString& str)
{
  bool found = false;
  for(unsigned int i=0;i < links.count(); i++)
  {
    if(str.endsWith(links[i]))
    {
      currentIndex = i;
      previousAction->setEnabled(bool(i!=0));
      nextAction->setEnabled(bool(i+1 != links.count()));
      found = true;
      break;
    }
  }
  if(found == false) // some error
  {
    previousAction->setEnabled(bool(currentIndex!=0));
    nextAction->setEnabled(bool(currentIndex+1 != links.count()));
    text->blockSignals(true);
    text->setSource(QucsHelpDir.filePath(links[currentIndex]));
    text->blockSignals(false);
  }
}


void QucsHelp::previousLink()
{
  if(currentIndex > 0)
    --currentIndex;
  text->setSource(QucsHelpDir.filePath(links[currentIndex]));
}

void QucsHelp::nextLink()
{
  ++currentIndex;
  if(currentIndex >= links.count())
    currentIndex = links.count();
  text->setSource(QucsHelpDir.filePath(links[currentIndex]));
}
