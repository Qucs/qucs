/***************************************************************************
                          fileshowdialog.cpp  -  description
                             -------------------
    begin                : Mon Nov 17 2003
    copyright            : (C) 2003 by Michael Margraf
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

#include "fileshowdialog.h"

#include <qlayout.h>
#include <qtextedit.h>
#include <qpushbutton.h>
#include <qfile.h>
#include <qtextstream.h>
#include <qmessagebox.h>


FileShowDialog::FileShowDialog(const QString& FileName, QWidget *parent, const char *name )
                                  : QDialog(parent, name, Qt::WDestructiveClose)
{
  setCaption("File: "+FileName);

  QVBoxLayout *v = new QVBoxLayout(this);
  v->setSpacing(5);

  QTextEdit *text = new QTextEdit(this);
  text->setTextFormat(Qt::PlainText);
  text->setReadOnly(true);
  text->setWordWrap(QTextEdit::NoWrap);
  text->setMinimumSize(300,380);
  v->addWidget(text);

  QPushButton *ButtOK = new QPushButton("OK",this);
  v->addWidget(ButtOK);
  connect(ButtOK, SIGNAL(clicked()), SLOT(accept()));
  ButtOK->setFocus();

  // .................................................
  QFile file(FileName);
  if(!file.open(IO_ReadOnly)) {
    text->setText("ERROR: Cannot read file: "+FileName+" !!!");
    return;
  }

  QTextStream stream(&file);
  while(!stream.atEnd())
    text->append(stream.readLine());
  file.close();
}

FileShowDialog::~FileShowDialog()
{
}
