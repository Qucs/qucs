/***************************************************************************
                          helpdialog.cpp  -  description
                             -------------------
    begin                : Sun Jan 11 2004
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

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include "helpdialog.h"

#include <qpushbutton.h>
#include <qlayout.h>
#include <qhbox.h>
/* FIXME: #include <qtextcodec.h> */

HelpDialog::HelpDialog(const QString& page, QWidget *parent, const char *name )
                                  : QDialog(parent, name, Qt::WDestructiveClose)
{
  setCaption(tr("Qucs Help System"));

  QVBoxLayout *v = new QVBoxLayout(this);
  v->setSpacing(5);

  text = new QTextBrowser(this);
  text->setMinimumSize(600,300);
  v->addWidget(text);
  text->setSource(DOCDIR +page);
  /* FIXME: text->setSource(QString(DOCDIR) + QTextCodec::locale() + page); */
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
}

HelpDialog::~HelpDialog()
{
}

void HelpDialog::slotGotoIndex()
{
  text->setSource(DOCDIR "index.html");
}

void HelpDialog::slotClose()
{
  done(0);  // close and delete (because of WDestructiveClose)
}
