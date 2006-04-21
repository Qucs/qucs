/***************************************************************************
                              searchdialog.cpp
                             ------------------
    begin                : Sat Apr 01 2006
    copyright            : (C) 2006 by Michael Margraf
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

#include "searchdialog.h"
#include "textdoc.h"
#include "qucs.h"

#include <qhbox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qcheckbox.h>
#include <qlineedit.h>
#include <qtabwidget.h>
#include <qpushbutton.h>
#include <qmessagebox.h>


SearchDialog::SearchDialog(QucsApp *App_) : QDialog(App_, 0, true)
{
  App = App_;
  setCaption(tr("Search Text"));

  QVBoxLayout *all = new QVBoxLayout(this);
  all->setMargin(5);

  QLabel *Label1 = new QLabel(tr("Text to search for:"), this);
  all->addWidget(Label1);

  SearchEdit = new QLineEdit(this);
  all->addWidget(SearchEdit);

  PositionBox = new QCheckBox(tr("From cursor position"), this);
  all->addWidget(PositionBox);
  CaseBox = new QCheckBox(tr("Case sensitive"), this);
  all->addWidget(CaseBox);
  WordBox = new QCheckBox(tr("Whole words only"), this);
  all->addWidget(WordBox);
  BackwardBox = new QCheckBox(tr("Search backwards"), this);
  all->addWidget(BackwardBox);

  QHBox *Buttons = new QHBox(this);
  all->addWidget(Buttons);
  QPushButton *ButtonSearch = new QPushButton(tr("Search"), Buttons);
  connect(ButtonSearch, SIGNAL(clicked()), SLOT(slotSearch()));
  connect(new QPushButton(tr("Cancel"), Buttons),
          SIGNAL(clicked()), SLOT(reject()));

  ButtonSearch->setDefault(true);
}

SearchDialog::~SearchDialog()
{
}

// ---------------------------------------------------------------------
void SearchDialog::initSearch()
{
  TextDoc *Doc = (TextDoc*)App->DocumentTab->currentPage();
  SearchEdit->setText(Doc->selectedText());
  SearchEdit->selectAll();

  setFocusProxy(SearchEdit);
  exec();
}

// ---------------------------------------------------------------------
void SearchDialog::searchText(bool fromCursor, int Offset)
{
  TextDoc *Doc = (TextDoc*)App->DocumentTab->currentPage();

  int Line=0, Column=0;
  if(fromCursor)
    Doc->getCursorPosition(&Line, &Column);
  else if(BackwardBox->isChecked())
    Line = Doc->paragraphs();

  if(!BackwardBox->isChecked())
    Column += Offset;

  if(SearchEdit->text().isEmpty())
    return;
  if(!Doc->find(SearchEdit->text(), CaseBox->isChecked(),
         WordBox->isChecked(), !BackwardBox->isChecked(), &Line, &Column)) {
    QMessageBox::information(Doc, tr("Search..."),
                 tr("Could not find search string!"));
  }
}

// ---------------------------------------------------------------------
void SearchDialog::slotSearch()
{
  searchText(PositionBox->isChecked(), 0);
  accept();
}
