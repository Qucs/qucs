/***************************************************************************
                          arrowdialog.cpp  -  description
                             -------------------
    begin                : Fri Nov 28 2003
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

#include "arrowdialog.h"

#include <qlayout.h>
#include <qlabel.h>
#include <qhbox.h>
#include <qvalidator.h>
#include <qcolordialog.h>


ArrowDialog::ArrowDialog(QWidget *parent, const char *name)
                                  : QDialog(parent, name, Qt::WDestructiveClose)
{
  setCaption(tr("Edit Arrow Properties"));
  Expr.setPattern("[0-9]{1,2}");  // valid expression for property input
  QValidator *Validator = new QRegExpValidator(Expr, this);

  QVBoxLayout *v = new QVBoxLayout(this);
  v->setSpacing(5);
  v->setMargin(5);

  QHBox *h0 = new QHBox(this);
  h0->setSpacing(5);
  v->addWidget(h0);

  new QLabel(tr("Head Length: "), h0);
  HeadLength = new QLineEdit(h0);
  HeadLength->setValidator(Validator);
  HeadLength->setMaximumWidth(35);
  HeadLength->setText("10");

  new QLabel(tr("      Head Width: "), h0);
  HeadWidth = new QLineEdit(h0);
  HeadWidth->setValidator(Validator);
  HeadWidth->setMaximumWidth(35);
  HeadWidth->setText("10");


  QHBox *h1 = new QHBox(this);
  h1->setSpacing(5);
  v->addWidget(h1);

  new QLabel(tr("Line color: "), h1);
  ColorButt = new QPushButton("        ",h1);
  ColorButt->setPaletteBackgroundColor(QColor(0,0,0));
  connect(ColorButt, SIGNAL(clicked()), SLOT(slotSetColor()));

  new QLabel(tr("      Line Width: "), h1);
  LineWidth = new QLineEdit(h1);
  LineWidth->setValidator(Validator);
  LineWidth->setMaximumWidth(35);
  LineWidth->setText("0");


  QHBox *h2 = new QHBox(this);
  h2->setSpacing(5);
  v->addWidget(h2);

  new QLabel(tr("Line style: "), h2);
  StyleBox = new QComboBox(h2);
  StyleBox->insertItem(tr("solid line"));
  StyleBox->insertItem(tr("dash line"));
  StyleBox->insertItem(tr("dot line"));
  StyleBox->insertItem(tr("dash dot line"));
  StyleBox->insertItem(tr("dash dot dot line"));
  connect(StyleBox, SIGNAL(activated(int)), SLOT(slotSetStyle(int)));
  LineStyle = Qt::SolidLine;

  QHBox *h3 = new QHBox(this);
  h3->setSpacing(5);
  v->addWidget(h3);

  QPushButton *ButtOK = new QPushButton(tr("OK"),h3);
  connect(ButtOK, SIGNAL(clicked()), SLOT(accept()));
  QPushButton *ButtCancel = new QPushButton(tr("Cancel"),h3);
  connect(ButtCancel, SIGNAL(clicked()), SLOT(reject()));

  ButtOK->setFocus();
}

ArrowDialog::~ArrowDialog()
{
}

// --------------------------------------------------------------------------
void ArrowDialog::slotSetColor()
{
  QColor c = QColorDialog::getColor(ColorButt->paletteBackgroundColor(),this);
  ColorButt->setPaletteBackgroundColor(c);
}

// --------------------------------------------------------------------------
void ArrowDialog::slotSetStyle(int index)
{
  switch(index) {
    case 0 : LineStyle = Qt::SolidLine;
             break;
    case 1 : LineStyle = Qt::DashLine;
             break;
    case 2 : LineStyle = Qt::DotLine;
             break;
    case 3 : LineStyle = Qt::DashDotLine;
             break;
    case 4 : LineStyle = Qt::DashDotDotLine;
             break;
    default: ;
  }
}

// --------------------------------------------------------------------------
void ArrowDialog::SetComboBox(Qt::PenStyle _Style)
{
  LineStyle = _Style;
  switch(_Style) {
    case Qt::SolidLine      : StyleBox->setCurrentItem(0);
                              break;
    case Qt::DashLine       : StyleBox->setCurrentItem(1);
                              break;
    case Qt::DotLine        : StyleBox->setCurrentItem(2);
                              break;
    case Qt::DashDotLine    : StyleBox->setCurrentItem(3);
                              break;
    case Qt::DashDotDotLine : StyleBox->setCurrentItem(4);
                              break;
    default: ;
  }
}
