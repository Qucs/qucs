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
  invalidKeys << "Esc";

  conflictAt = -1;
  conflictKey = QString();

  menuList = new QListWidget();
  actionList = new QTableWidget();
  actionList->horizontalHeader()->setStretchLastSection(true);
  actionList->verticalHeader()->hide();
  actionList->setColumnCount(2);
  actionList->setHorizontalHeaderLabels(
      QStringList() << tr("Action") << tr("Shortcut Key"));
  actionList->setSelectionBehavior(QAbstractItemView::SelectRows);

  sequenceInput = new KeySequenceEdit;
  messageLabel = new QLabel();
  setButton = new QPushButton(tr("set shortcut"));
  removeButton = new QPushButton(tr("remove shortcut"));
  defaultButton = new QPushButton(tr("default"));
  okButton = new QPushButton(tr("OK"));

  connect(sequenceInput, SIGNAL(textChanged(QString)), SLOT(slotCheckKey()));
  connect(this, SIGNAL(signalValidKey()), SLOT(slotCheckUnique()));
  connect(setButton, SIGNAL(clicked()), SLOT(slotSetShortcut()));
  connect(removeButton, SIGNAL(clicked()), SLOT(slotRemoveShortcut()));
  connect(defaultButton, SIGNAL(clicked()), SLOT(slotDefaultShortcut()));
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

  connect(menuList, SIGNAL(itemClicked(QListWidgetItem*))
      , this, SLOT(slotChooseMenu()));
  
  //fill initial value
  fillMenu();
  slotChooseMenu();
}

QucsShortcutDialog::~QucsShortcutDialog()
{
}

void
QucsShortcutDialog::slotChooseMenu()
{
  int menurow = menuList->currentRow();
  actionList->clearContents();

  QMap<QString, QString> *map = QucsSettings.Shortcut.at(menurow).second;
  QMap<QString, QString>::const_iterator action_it = map->constBegin();

  int row = 0;
  while(action_it != map->constEnd()) {
    actionList->setRowCount(map->size());

    QTableWidgetItem *item_0 = new QTableWidgetItem(action_it.key());
    QTableWidgetItem *item_1 = new QTableWidgetItem(action_it.value());

    item_0->setFlags(item_0->flags() & ~Qt::ItemIsEditable);
    item_1->setFlags(item_1->flags() & ~Qt::ItemIsEditable);
    actionList->setItem(row, 0, item_0);
    actionList->setItem(row, 1, item_1);

    row++;
    action_it++;
  }
  actionList->setCurrentCell(0, 0);
}

void 
QucsShortcutDialog::slotSetShortcut() 
{
  if (sequenceInput->text() != QString())
  {
    setShortcut(sequenceInput->text());
  }
}

void 
QucsShortcutDialog::slotRemoveShortcut() 
{
  setShortcut("");
}

void
QucsShortcutDialog::setShortcut(QString keySequence)
{
  int menurow = menuList->currentRow();
  int row = actionList->currentRow();

  //solve conflict
  if (conflictAt != -1) {
    if (menurow == conflictAt) {
      QList<QTableWidgetItem *> conflictItems = actionList->findItems(conflictKey, Qt::MatchExactly);
      if (conflictItems.size() != 0) {
        int idx = actionList->row(conflictItems.at(0));
        actionList->item(idx, 1)->setText(QString());
      }
    }
    QucsSettings.Shortcut.at(conflictAt).second->insert(conflictKey, QString());
    conflictAt = -1;
    conflictKey.clear();
    messageLabel->clear();
  }

  QString key = actionList->item(row, 0)->text();
  QucsSettings.Shortcut.at(menurow).second->insert(key, keySequence);
  actionList->item(row, 1)->setText(keySequence);
  sequenceInput->clear();
}

void 
QucsShortcutDialog::slotDefaultShortcut() 
{
  clearShortcutMap();
  setDefaultShortcut();
  slotChooseMenu();
}

void 
QucsShortcutDialog::slotOK() 
{
  App->setAllShortcut();
  accept();
}

void
QucsShortcutDialog::slotCheckKey()
{
  QString keysequence = sequenceInput->text();
  foreach(QString invalidKey, invalidKeys) {
    if (keysequence == invalidKey) {
      sequenceInput->clear();
      QString msg = QString("Unable to set shortcut of: ") + invalidKey;
      messageLabel->setText(msg);
      return;
    }
  }

  emit signalValidKey();
}

void
QucsShortcutDialog::slotCheckUnique()
{
  QString keysequence = sequenceInput->text();
  if (keysequence == QString()) {
    return;
  }

  conflictAt = -1;
  conflictKey = "";
  messageLabel->clear();

  QVector<QPair<QString, QMap<QString, QString>* > >::const_iterator menu_it = QucsSettings.Shortcut.constBegin();

  while(menu_it != QucsSettings.Shortcut.constEnd()) {
    QMap<QString, QString>::const_iterator action_it = menu_it->second->constBegin();
    while(action_it != menu_it->second->constEnd()) {
      if (keysequence == action_it.value()) {
        conflictAt = menu_it - QucsSettings.Shortcut.constBegin();
        conflictKey = action_it.key();
        QString msg = QString("Conflict With: ") + menu_it->first 
            + "->" + conflictKey;
        messageLabel->setText(msg);
        return;
      }
      action_it++;
    }
    menu_it++;
  }
}

void
QucsShortcutDialog::fillMenu()
{
  QVector<QPair<QString, QMap<QString, QString>* > >::const_iterator menu_it = QucsSettings.Shortcut.constBegin();

  while(menu_it != QucsSettings.Shortcut.constEnd()) {
    QListWidgetItem *item = new QListWidgetItem(menu_it->first);
    menuList->addItem(item);

    menu_it++;
  }

  menuList->setCurrentRow(0);
}
