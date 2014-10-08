/***************************************************************************
                           qucssettingsdialog.h
                          ----------------------
    begin                : Fri Oct 3 2014
    copyright            : (C) 2014 by Yodalee
    email                : lc85301@gmail.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtGui>

#include "qucsshortcutdialog.h"
#include "keysequenceedit.h"
#include "main.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>
#include <QLabel>

#include <QDebug>

QucsShortcutDialog::QucsShortcutDialog(QucsApp *parent, const char *name)
  : QDialog(parent, name)
{
  qDebug() << "open shortcut dialog";
  App = parent;
  setWindowTitle(tr("Edit Qucs Shortcuts"));

  menuList = new QListWidget();
  actionList = new QTableWidget();

  sequenceInput = new KeySequenceEdit;
  messageLabel = new QLabel("test message");
  setButton = new QPushButton(tr("set shortcut"));
  removeButton = new QPushButton(tr("remove shortcut"));
  defaultButton = new QPushButton(tr("default"));
  okButton = new QPushButton(tr("OK"));

  connect(okButton, SIGNAL(clicked()), SLOT(slotOK()));

  QHBoxLayout *topLayout = new QHBoxLayout;
  topLayout->addWidget(menuList);
  topLayout->addWidget(actionList);

  QVBoxLayout *btnLayout = new QVBoxLayout;
  btnLayout->addWidget(setButton);
  btnLayout->addWidget(removeButton);
  btnLayout->addWidget(defaultButton);

  QVBoxLayout *textLayout = new QVBoxLayout;
  textLayout->addWidget(sequenceInput);
  textLayout->addWidget(messageLabel);
  textLayout->setAlignment(Qt::AlignVCenter);

  QHBoxLayout *midLayout = new QHBoxLayout;
  midLayout->addLayout(textLayout);
  midLayout->addLayout(btnLayout);

  QHBoxLayout *bottomLayout = new QHBoxLayout;
  bottomLayout->addStretch();
  bottomLayout->addWidget(okButton);

  QVBoxLayout *all = new QVBoxLayout;
  all->addLayout(topLayout);
  all->addLayout(midLayout);
  all->addLayout(bottomLayout);

  this->setLayout(all);
}

QucsShortcutDialog::~QucsShortcutDialog()
{
}

void
QucsShortcutDialog::slotChooseMenu()
{
}

void 
QucsShortcutDialog::slotSetShortcut() 
{
}

void 
QucsShortcutDialog::slotRemoveShortcut() 
{
}

void 
QucsShortcutDialog::slotDefaultShortcut() 
{
}

void 
QucsShortcutDialog::slotOK() 
{
  accept();
}

void
QucsShortcutDialog::fillMenu()
{
}
