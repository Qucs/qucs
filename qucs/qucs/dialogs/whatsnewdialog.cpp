/*
 * whatsnewdialog.h - dialog showing textual info about the current release
 *
 * Copyright (C) 2015, Qucs team (see AUTHORS file)
 *
 * This file is part of Qucs
 *
 * Qucs is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Qucs.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

/*!
 * \file whatsnewdialog.cpp
 * \brief Implementation of the What's new dialog
 */

#include <string>

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif
#include "main.h"
#include "whatsnewdialog.h"

#include <QObject>
#include <QApplication>
#include <QLabel>
#include <QVBoxLayout>
#include <QTextBrowser>
#include <QPushButton>
#include <QCheckBox>
#include <QDir>
#include <QDebug>


WhatsNewDialog::WhatsNewDialog(QWidget *parent)
    : QDialog(parent)
{
  resize(600, 400);
  setWindowTitle(tr("What's new"));

  mainBrowser = new QTextBrowser;
  // open external HTML links in the default browser
  mainBrowser ->setOpenExternalLinks(true);
  // the Ctrl-Wheel event we would like to filter is handled by the viewport
  mainBrowser->viewport()->installEventFilter(this);

  QString WhatsNewFile = "whatsnew.html";

  // this is taken from qucs-help
  QString locale = QucsSettings.Language;
  if(locale.isEmpty())
    locale = QString(QLocale::system().name());

  QDir QucsHelpDir = QucsSettings.DocDir + locale;
  if (!QucsHelpDir.exists(WhatsNewFile)) {
    int p = locale.indexOf ('_');
    if (p != -1) {
      QucsHelpDir = QucsSettings.DocDir + locale.left(p);
      if (!QucsHelpDir.exists(WhatsNewFile)) {
	QucsHelpDir = QucsSettings.DocDir + "en";
      }
    } else {
      QucsHelpDir = QucsSettings.DocDir + "en";
    }
  }
  mainBrowser->setSearchPaths(QStringList() 
			      << QucsHelpDir.absolutePath()
			      << ":/bitmaps");

  mainBrowser->setSource(QUrl::fromLocalFile(WhatsNewFile));

  all = new QVBoxLayout(this);
  all->addWidget(mainBrowser);

  QWidget *hbChk = new QWidget();
  QHBoxLayout *hlChk = new QHBoxLayout(hbChk);
  //hlChk->setContentsMargins(0,0,0,0);
  all->addWidget(hbChk);

  dnsaChk = new QCheckBox(tr("don't show this again"), parent);
  hlChk->addWidget(dnsaChk);
  hlChk->addStretch();

  QWidget *hbBtn = new QWidget();
  QHBoxLayout *hlBtn = new QHBoxLayout(hbBtn);
  hlBtn->setContentsMargins(0,0,0,0);
  all->addWidget(hbBtn);

  QPushButton *okButton = new QPushButton(tr("&OK"), parent);
  okButton->setFocus();
  connect(okButton, SIGNAL(clicked()), this, SLOT(close()));
  hlBtn->addStretch();
  hlBtn->addWidget(okButton);
}

bool WhatsNewDialog::getCheckBoxState() {
  return dnsaChk->isChecked();
}

void WhatsNewDialog::setCheckBoxState(bool state) {
  dnsaChk->setChecked(state);
}

// event filter to remove the Ctrl-Wheel (text zoom) event
bool WhatsNewDialog::eventFilter(QObject *obj, QEvent *event) {
  if ((event->type() == QEvent::Wheel) &&
      (QApplication::keyboardModifiers() & Qt::ControlModifier )) {    
    return true; // eat Ctrl-Wheel event
  } else {
    // pass the event on to the parent class
    return QDialog::eventFilter(obj, event);
  }
}


