/***************************************************************************
                          graphictextdialog.cpp  -  description
                             -------------------
    begin                : Wed Nov 26 2003
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

#include "graphictextdialog.h"

#include <qlayout.h>
#include <qlabel.h>
#include <qhbox.h>
#include <qvalidator.h>
#include <qcolordialog.h>


GraphicTextDialog::GraphicTextDialog(QWidget *parent, const char *name)
                                  : QDialog(parent, name, Qt::WDestructiveClose)
{
  setCaption("Edit Text Properties");

  QVBoxLayout *v = new QVBoxLayout(this);
  v->setSpacing(5);

  text = new QTextEdit(this);
  text->setTextFormat(Qt::PlainText);
  text->setWordWrap(QTextEdit::NoWrap);
  text->setMinimumSize(350,150);
  v->addWidget(text);

  QHBox *h1 = new QHBox(this);
  h1->setSpacing(5);
  v->addWidget(h1);

  new QLabel("Text color: ", h1);
  ColorButt = new QPushButton("        ",h1);
  ColorButt->setPaletteBackgroundColor(QColor(0,0,0));
  connect(ColorButt, SIGNAL(clicked()), SLOT(slotSetColor()));

  new QLabel("          Text size: ", h1);
  Expr.setPattern("[0-9]{1,2}");  // valid expression for property input
  QValidator *Validator = new QRegExpValidator(Expr, this);
  TextSize = new QLineEdit(h1);
  TextSize->setValidator(Validator);
  TextSize->setMaximumWidth(35);
  TextSize->setText("12");


  QHBox *h2 = new QHBox(this);
  h2->setSpacing(5);
  v->addWidget(h2);

  QPushButton *ButtOK = new QPushButton("OK",h2);
  connect(ButtOK, SIGNAL(clicked()), SLOT(accept()));
  QPushButton *ButtCancel = new QPushButton("Cancel",h2);
  connect(ButtCancel, SIGNAL(clicked()), SLOT(reject()));
  
  text->setFocus();
}

GraphicTextDialog::~GraphicTextDialog()
{
}

// --------------------------------------------------------------------------
void GraphicTextDialog::slotSetColor()
{
  QColor c = QColorDialog::getColor(ColorButt->paletteBackgroundColor(),this);
  ColorButt->setPaletteBackgroundColor(c);
}
