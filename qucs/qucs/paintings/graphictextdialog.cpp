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
                                  : QDialog(parent, name)
{
  setCaption(tr("Edit Text Properties"));

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


  QHBox *h2 = new QHBox(this);
  h2->setSpacing(5);
  v->addWidget(h2);

  QHBox *h3 = new QHBox(this);
  h2->setSpacing(5);
  v->addWidget(h3);

  // first => activated by pressing RETURN
  QPushButton *ButtOK = new QPushButton(tr("OK"),h3);
  connect(ButtOK, SIGNAL(clicked()), SLOT(accept()));
  QPushButton *ButtCancel = new QPushButton(tr("Cancel"),h3);
  connect(ButtCancel, SIGNAL(clicked()), SLOT(reject()));

  new QLabel(tr("Text color: "), h1);
  ColorButt = new QPushButton("        ",h1);
  ColorButt->setPaletteBackgroundColor(QColor(0,0,0));
  connect(ColorButt, SIGNAL(clicked()), SLOT(slotSetColor()));

  QWidget *place1 = new QWidget(h1); // stretchable placeholder
  h1->setStretchFactor(place1,5);

  new QLabel(tr("Text size: "), h1);
  TextSize = new QLineEdit(h1);
  TextSize->setValidator(new QIntValidator(1, 50, this));
  TextSize->setMaximumWidth(35);
  TextSize->setText("12");

  QWidget *place2 = new QWidget(h2); // stretchable placeholder
  h2->setStretchFactor(place2,5);

  new QLabel(tr("Rotation angle: "), h2);
  Angle = new QLineEdit(h2);
  Angle->setValidator(new QIntValidator(0, 359, this));
  Angle->setMaximumWidth(35);
  Angle->setText("0");

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
