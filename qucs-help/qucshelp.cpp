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
#include <qlayout.h>
#include <qhbox.h>

QucsHelp::QucsHelp(const QString& page)
{
  // set application icon
  setIcon (QPixmap(QucsSettings.BitmapDir + "big.qucs.xpm"));
  setCaption(tr("Qucs Help System"));

  QVBoxLayout *v = new QVBoxLayout(this);
  v->setSpacing(5);

  text = new QTextBrowser(this);
  text->setMinimumSize(400,200);
  v->addWidget(text);

  QHBox *h = new QHBox(this);
  h->setSpacing(5);
  v->addWidget(h);
  QWidget *st = new QWidget(h); // stretchable placeholder
  h->setStretchFactor(st,5);

  QPushButton *ButtIndex = new QPushButton(tr("Help Index"),h);
  connect(ButtIndex, SIGNAL(clicked()), SLOT(slotGotoIndex()));
  QPushButton *ButtClose = new QPushButton(tr("Close"),h);
  connect(ButtClose, SIGNAL(clicked()), SLOT(slotClose()));
  ButtClose->setFocus();

  // .......................................
  if(page.isEmpty()) slotGotoIndex();
  else text->setSource(QucsHelpDir.filePath(page));
}

QucsHelp::~QucsHelp()
{
}

//-----------------------------------------------------------------
void QucsHelp::slotGotoIndex()
{
  text->setSource(QucsHelpDir.filePath("index.html"));
}

//-----------------------------------------------------------------
void QucsHelp::slotClose()
{
  int tmp;
  tmp = x();		// call size and position function in order to ...
  tmp = y();		// ... set them correctly before closing the ...
  tmp = width();	// dialog !!!  Otherwise the frame of the window ...
  tmp = height();	// will not be recognized (a X11 problem).

  accept();
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
