/***************************************************************************
                          arrowdialog.cpp  -  description
                             -------------------
    begin                : Fri Nov 28 2003
    copyright            : (C) 2003 by Michael Margraf
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

#include "arrowdialog.h"

#include <qlayout.h>
#include <qlabel.h>
#include <qhbox.h>
#include <qvalidator.h>
#include <qcolordialog.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qcombobox.h>


ArrowDialog::ArrowDialog(QWidget *parent, const char *name)
                                  : QDialog(parent, name)
{
  setCaption(tr("Edit Arrow Properties"));
  val100 = new QIntValidator(0, 100, this);

  vert = new QVBoxLayout(this);
  vert->setSpacing(5);
  vert->setMargin(5);

  QHBox *h0 = new QHBox(this);
  h0->setSpacing(5);
  vert->addWidget(h0);

  new QLabel(tr("Head Length: "), h0);
  HeadLength = new QLineEdit(h0);
  HeadLength->setValidator(val100);
  HeadLength->setMaximumWidth(35);
  HeadLength->setText("10");

  new QLabel(tr("      Head Width: "), h0);
  HeadWidth = new QLineEdit(h0);
  HeadWidth->setValidator(val100);
  HeadWidth->setMaximumWidth(35);
  HeadWidth->setText("10");


  QHBox *h1 = new QHBox(this);
  h1->setSpacing(5);
  vert->addWidget(h1);

  QHBox *h2 = new QHBox(this);
  h2->setSpacing(5);
  vert->addWidget(h2);

  QHBox *h3 = new QHBox(this);
  h3->setSpacing(5);
  vert->addWidget(h3);

  // must be first button => press RETURN
  QPushButton *ButtOK = new QPushButton(tr("OK"),h3);
  connect(ButtOK, SIGNAL(clicked()), SLOT(accept()));
  QPushButton *ButtCancel = new QPushButton(tr("Cancel"),h3);
  connect(ButtCancel, SIGNAL(clicked()), SLOT(reject()));


  new QLabel(tr("Line color: "), h1);
  ColorButt = new QPushButton("        ",h1);
  ColorButt->setPaletteBackgroundColor(QColor(0,0,0));
  connect(ColorButt, SIGNAL(clicked()), SLOT(slotSetColor()));

  new QLabel(tr("      Line Width: "), h1);
  LineWidth = new QLineEdit(h1);
  LineWidth->setValidator(val100);
  LineWidth->setMaximumWidth(35);
  LineWidth->setText("0");


  new QLabel(tr("Line style: "), h2);
  StyleBox = new QComboBox(h2);
  StyleBox->insertItem(tr("solid line"));
  StyleBox->insertItem(tr("dash line"));
  StyleBox->insertItem(tr("dot line"));
  StyleBox->insertItem(tr("dash dot line"));
  StyleBox->insertItem(tr("dash dot dot line"));
  connect(StyleBox, SIGNAL(activated(int)), SLOT(slotSetStyle(int)));
  LineStyle = Qt::SolidLine;

  ButtOK->setFocus();
}

ArrowDialog::~ArrowDialog()
{
  delete vert;
  delete val100;
}

// --------------------------------------------------------------------------
void ArrowDialog::slotSetColor()
{
  QColor c = QColorDialog::getColor(ColorButt->paletteBackgroundColor(),this);
  if(c.isValid()) ColorButt->setPaletteBackgroundColor(c);
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
