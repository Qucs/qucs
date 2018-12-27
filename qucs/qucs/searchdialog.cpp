/*
 * searchdialog.cpp - implementation of search dialog
 *
 * Copyright (C) 2006, Michael Margraf, michael.margraf@alumni.tu-berlin.de
 * Copyright (C) 2014, Yodalee, lc85301@gmail.com
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

#include "searchdialog.h"
#include "dialogs/ui_searchdialog.h"

SearchDialog::SearchDialog(QWidget *parent) : 
    QDialog(parent),
    ui(new Ui::SearchDialog)
{
  ui->setupUi(this);

  connect(ui->ButtonSearch, SIGNAL(clicked()), SLOT(slotSearch()));
  connect(this, SIGNAL(finished(int)), SLOT(slotDisconnect()));
}

SearchDialog::~SearchDialog()
{
  delete ui;
}

// ---------------------------------------------------------------------
void SearchDialog::initSearch(QWidget *_doc, const QString &text, bool replace)
{
  doc = _doc;

  if(replace) {
    setWindowTitle(tr("Replace Text"));
    ui->AskBox->setHidden(false);
    ui->ReplaceGroup->setHidden(false);
    connect(this, SIGNAL(replace(const QString &, const QString &, bool, bool, bool, bool)),
        doc, SLOT(replace(const QString &, const QString &, bool, bool, bool, bool)));
  }
  else {
    setWindowTitle(tr("Search Text"));
    ui->AskBox->setHidden(true);
    ui->ReplaceGroup->setHidden(true);
    connect(this, SIGNAL(search(const QString &, bool, bool, bool)),
        doc, SLOT(search(const QString &, bool, bool, bool)));
  }

  ui->ReplaceEdit->clear();
  ui->SearchEdit->setText(text);
  ui->SearchEdit->selectAll();

  ui->SearchEdit->setFocus();
  layout()->setSizeConstraint(QLayout::SetFixedSize);
  show();
  raise();
  activateWindow();
}

// ---------------------------------------------------------------------
void SearchDialog::slotSearch()
{
  if(ui->SearchEdit->text().isEmpty()) {
    return;
  }

  if(ui->AskBox->isHidden()) { //search
    emit search(
        ui->SearchEdit->text(), ui->CaseBox->isChecked(), ui->WordBox->isChecked(),
        ui->BackwardBox->isChecked());
  } else {
    emit replace(
        ui->SearchEdit->text(), ui->ReplaceEdit->text(),
        ui->AskBox->isChecked(), ui->CaseBox->isChecked(),
        ui->WordBox->isChecked(), ui->BackwardBox->isChecked());
  }
}

// ---------------------------------------------------------------------
void SearchDialog::slotDisconnect()
{
  //hidden -> search
  if (ui->AskBox->isHidden()) {
    disconnect(this, SIGNAL(search(const QString &, bool, bool, bool)),
        doc, SLOT(search(const QString &, bool, bool, bool)));
  } else {
    disconnect(this, SIGNAL(replace(const QString &, const QString &, bool, bool, bool, bool)),
        doc, SLOT(replace(const QString &, const QString &, bool, bool, bool, bool)));
  }
}
