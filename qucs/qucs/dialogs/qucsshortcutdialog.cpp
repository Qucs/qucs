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
  actionList->horizontalHeader()->setStretchLastSection(true);
  actionList->verticalHeader()->hide();
  actionList->setColumnCount(2);
  actionList->setHorizontalHeaderLabels(
      QStringList() << tr("Action") << tr("Shortcut Key"));
  actionList->setSelectionBehavior(QAbstractItemView::SelectRows);

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
  QVector<QPair<QString, QMap<QString, QString>* > >::const_iterator menu_it = QucsSettings.Shortcut.constBegin();

  while(menu_it != QucsSettings.Shortcut.constEnd()) {
    QListWidgetItem *item = new QListWidgetItem(menu_it->first);
    menuList->addItem(item);

    menu_it++;
  }

  menuList->setCurrentRow(0);
}
