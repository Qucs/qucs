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
  importButton = new QPushButton(tr("Import"));
  exportButton = new QPushButton(tr("Export"));

  connect(sequenceInput, SIGNAL(textChanged(QString)), SLOT(slotCheckKey()));
  connect(this, SIGNAL(signalValidKey()), SLOT(slotCheckUnique()));
  connect(actionList, SIGNAL(itemSelectionChanged()), SLOT(slotSetFocus()));

  connect(setButton, SIGNAL(clicked()), SLOT(slotSetShortcut()));
  connect(removeButton, SIGNAL(clicked()), SLOT(slotRemoveShortcut()));
  connect(defaultButton, SIGNAL(clicked()), SLOT(slotDefaultShortcut()));
  connect(okButton, SIGNAL(clicked()), SLOT(slotOK()));
  connect(importButton, SIGNAL(clicked()), SLOT(slotImport()));
  connect(exportButton, SIGNAL(clicked()), SLOT(slotExport()));

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
  bottomLayout->addWidget(importButton);
  bottomLayout->addWidget(exportButton);
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
QucsShortcutDialog::slotImport()
{
  QString filename;

  filename = QFileDialog::getOpenFileName(this, tr("Import Shortcut Map"),
      ".", "Mapfiles (*.shortcutmap)");
  if (!filename.isEmpty()) {

    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly)) {
      QMessageBox::critical(this, tr("Unable to open file"),
          file.errorString());
      return;
    }
    QTextStream stream(&file);
    QString line, action, keySequence;

    do {
      if (stream.atEnd()) {
        file.close();
        return;
      }

      line = stream.readLine();
    } while(line.isEmpty());

    //check header
    if (line != "<Qucs Shortcut>") {
      file.close();
      QMessageBox::critical(0, tr("Error"),
        tr("Wrong document type: "+filename));
      return;
    }

    QVector<QPair<QString, QMap<QString, QString>* > >::iterator menu_it = QucsSettings.Shortcut.begin();
    QVector<QPair<QString, QMap<QString, QString>* > >::iterator end = QucsSettings.Shortcut.end();
    //we assume shortcut map file save all shortcut
    //clean all pre-exist map
    while (menu_it != end) {
      menu_it->second->clear();
      menu_it++;
    }

    int i, j;
    menu_it = QucsSettings.Shortcut.begin();
    while (!stream.atEnd()) {
      line = stream.readLine();
      line = line.stripWhiteSpace();
      if (line.isEmpty()) { continue; }

      if (line.startsWith("Menu")) {
        //get iterator
        i = line.indexOf("<", 5);
        j = line.indexOf(">", 5);
        line = line.mid(i+1, j-i-1);
        //find the related map
        qDebug() << line;
        menu_it = QucsSettings.Shortcut.begin();
        while(menu_it != end) {
          if (menu_it->first == line) {
            qDebug() << "Menu: " << line;
            break;
          }
          menu_it++;
        }
      } else if (line.startsWith("Action")) {
        //read action
        i = line.indexOf("<", 7);
        j = line.indexOf(">", 7);
        action = line.mid(i+1, j-i-1);
        i = line.indexOf("<", j);
        j = line.lastIndexOf(">");
        keySequence = line.mid(i+1, j-i-1);
        if (menu_it != end) {
          qDebug() << "Action: " << action << " : " << keySequence;
          menu_it->second->insert(action, keySequence);
        }
      }
    }

    file.close();
  }
  slotChooseMenu();
}

void
QucsShortcutDialog::slotExport()
{
  QString filename;

  QFileDialog *sd = new QFileDialog;
  sd->setDefaultSuffix("shortcutmap");
  sd->setDirectory(".");
  sd->setFilter("Mapfiles (*.shortcutmap)");
  sd->setAcceptMode(QFileDialog::AcceptSave);
  int result = sd->exec();
  if (result == QDialog::Accepted) {
    QString filename = sd->selectedFiles()[0];
    qDebug() << filename;

    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly)) {
      QMessageBox::critical(this, tr("Unable to open file"),
          file.errorString());
      return;
    }
    QTextStream stream(&file);
    stream << "<Qucs Shortcut>\n";

    QVector<QPair<QString, QMap<QString, QString>* > >::const_iterator menu_it = QucsSettings.Shortcut.constBegin();
    while (menu_it != QucsSettings.Shortcut.constEnd()) {
      stream << "Menu <" << menu_it->first << ">\n";

      QMap<QString, QString>::const_iterator action_it = menu_it->second->constBegin();
      while (action_it != menu_it->second->constEnd()) {
        stream << "Action <" << action_it.key() << "> <" << action_it.value() << ">\n"; 
        ++action_it;
      }

      ++menu_it;
    }

    file.close();
  } else {
    qDebug("Cannot get file");
  }
  delete sd;
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
QucsShortcutDialog::slotSetFocus()
{
  sequenceInput->setFocus();
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
