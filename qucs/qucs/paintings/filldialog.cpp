/***************************************************************************
                          filldialog.cpp  -  description
                             -------------------
    begin                : Thu May 20 2004
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

#include "filldialog.h"

#include <qlayout.h>
#include <qlabel.h>
#include <qhbox.h>
#include <qvalidator.h>
#include <qcolordialog.h>


FillDialog::FillDialog(const QString& _Caption, QWidget *parent,
			const char *name) : QDialog(parent, name)
{
  setCaption(_Caption);

  QVBoxLayout *v = new QVBoxLayout(this);
  v->setSpacing(5);
  v->setMargin(5);

  QHBox *h1 = new QHBox(this);
  h1->setSpacing(5);
  v->addWidget(h1);

  QHBox *h2 = new QHBox(this);
  h2->setSpacing(5);
  v->addWidget(h2);

  QHBox *h3 = new QHBox(this);
  h3->setSpacing(5);
  v->addWidget(h3);

  QHBox *h4 = new QHBox(this);
  h4->setSpacing(5);
  v->addWidget(h4);

  QHBox *h5 = new QHBox(this);
  h5->setSpacing(5);
  v->addWidget(h5);

  // must be first to get RETURN pressed event
  QPushButton *ButtOK = new QPushButton(tr("OK"),h5);
  connect(ButtOK, SIGNAL(clicked()), SLOT(accept()));
  QPushButton *ButtCancel = new QPushButton(tr("Cancel"),h5);
  connect(ButtCancel, SIGNAL(clicked()), SLOT(reject()));

  new QLabel(tr("Line Color: "), h1);
  ColorButt = new QPushButton("        ",h1);
  ColorButt->setPaletteBackgroundColor(QColor(0,0,0));
  connect(ColorButt, SIGNAL(clicked()), SLOT(slotSetColor()));

  new QLabel(tr("      Line Width: "), h1);
  Expr.setPattern("[0-9]{1,2}");  // valid expression for property input
  QValidator *Validator = new QRegExpValidator(Expr, this);
  LineWidth = new QLineEdit(h1);
  LineWidth->setValidator(Validator);
  LineWidth->setMaximumWidth(35);
  LineWidth->setText("0");


  new QLabel(tr("Line Style: "), h2);
  StyleBox = new QComboBox(h2);
  StyleBox->insertItem(tr("solid line"));
  StyleBox->insertItem(tr("dash line"));
  StyleBox->insertItem(tr("dot line"));
  StyleBox->insertItem(tr("dash dot line"));
  StyleBox->insertItem(tr("dash dot dot line"));
  connect(StyleBox, SIGNAL(activated(int)), SLOT(slotSetStyle(int)));
  LineStyle = Qt::SolidLine;


  new QLabel(tr("Fill Color: "), h3);
  FillColorButt = new QPushButton("        ", h3);
  FillColorButt->setPaletteBackgroundColor(QColor(0,0,0));
  connect(FillColorButt, SIGNAL(clicked()), SLOT(slotSetFillColor()));

  new QLabel(tr("Fill Style: "), h4);
  FillStyleBox = new QComboBox(h4);
  FillStyleBox->insertItem(tr("no filling"));
  FillStyleBox->insertItem(tr("solid"));
  FillStyleBox->insertItem(tr("dense 1 (densest)"));
  FillStyleBox->insertItem(tr("dense 2 (densest)"));
  FillStyleBox->insertItem(tr("dense 3 (densest)"));
  FillStyleBox->insertItem(tr("dense 4 (densest)"));
  FillStyleBox->insertItem(tr("dense 5 (densest)"));
  FillStyleBox->insertItem(tr("dense 6 (densest)"));
  FillStyleBox->insertItem(tr("dense 7 (least dense)"));
  FillStyleBox->insertItem(tr("horizontal line"));
  FillStyleBox->insertItem(tr("vertical line"));
  FillStyleBox->insertItem(tr("crossed lines"));
  FillStyleBox->insertItem(tr("hatched backwards"));
  FillStyleBox->insertItem(tr("hatched forwards"));
  FillStyleBox->insertItem(tr("diagonal crossed"));
  connect(FillStyleBox, SIGNAL(activated(int)), SLOT(slotSetFillStyle(int)));
  FillStyle = Qt::SolidPattern;

  ButtOK->setFocus();
}

FillDialog::~FillDialog()
{
}

// --------------------------------------------------------------------------
void FillDialog::slotSetColor()
{
  QColor c = QColorDialog::getColor(ColorButt->paletteBackgroundColor(),this);
  ColorButt->setPaletteBackgroundColor(c);
}

// --------------------------------------------------------------------------
void FillDialog::slotSetFillColor()
{
  QColor c =
    QColorDialog::getColor(FillColorButt->paletteBackgroundColor(), this);
  FillColorButt->setPaletteBackgroundColor(c);
}

// --------------------------------------------------------------------------
void FillDialog::slotSetStyle(int index)
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
  }
}

// --------------------------------------------------------------------------
void FillDialog::SetComboBox(Qt::PenStyle _Style)
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
    default:                ;
  }
}

// --------------------------------------------------------------------------
void FillDialog::slotSetFillStyle(int index)
{
  switch(index) {
    case 0 :  FillStyle = Qt::NoBrush;
              break;
    case 1 :  FillStyle = Qt::SolidPattern;
              break;
    case 2 :  FillStyle = Qt::Dense1Pattern;
              break;
    case 3 :  FillStyle = Qt::Dense2Pattern;
              break;
    case 4 :  FillStyle = Qt::Dense3Pattern;
              break;
    case 5 :  FillStyle = Qt::Dense4Pattern;
              break;
    case 6 :  FillStyle = Qt::Dense5Pattern;
              break;
    case 7 :  FillStyle = Qt::Dense6Pattern;
              break;
    case 8 :  FillStyle = Qt::Dense7Pattern;
              break;
    case 9 :  FillStyle = Qt::HorPattern;
              break;
    case 10 : FillStyle = Qt::VerPattern;
              break;
    case 11 : FillStyle = Qt::CrossPattern;
              break;
    case 12 : FillStyle = Qt::BDiagPattern;
              break;
    case 13 : FillStyle = Qt::FDiagPattern;
              break;
    case 14 : FillStyle = Qt::DiagCrossPattern;
              break;
  }
}

// --------------------------------------------------------------------------
void FillDialog::SetFillComboBox(Qt::BrushStyle _Style)
{
  FillStyle = _Style;
  switch(_Style) {
    case Qt::NoBrush :          FillStyleBox->setCurrentItem(0);
				break;
    case Qt::SolidPattern :     FillStyleBox->setCurrentItem(1);
				break;
    case Qt::Dense1Pattern :    FillStyleBox->setCurrentItem(2);
				break;
    case Qt::Dense2Pattern :    FillStyleBox->setCurrentItem(3);
				break;
    case Qt::Dense3Pattern :    FillStyleBox->setCurrentItem(4);
				break;
    case Qt::Dense4Pattern :    FillStyleBox->setCurrentItem(5);
				break;
    case Qt::Dense5Pattern :    FillStyleBox->setCurrentItem(6);
				break;
    case Qt::Dense6Pattern :    FillStyleBox->setCurrentItem(7);
				break;
    case Qt::Dense7Pattern :    FillStyleBox->setCurrentItem(8);
				break;
    case Qt::HorPattern :       FillStyleBox->setCurrentItem(9);
				break;
    case Qt::VerPattern :       FillStyleBox->setCurrentItem(10);
				break;
    case Qt::CrossPattern :     FillStyleBox->setCurrentItem(11);
				break;
    case Qt::BDiagPattern :     FillStyleBox->setCurrentItem(12);
				break;
    case Qt::FDiagPattern :     FillStyleBox->setCurrentItem(13);
				break;
    case Qt::DiagCrossPattern : FillStyleBox->setCurrentItem(14);
				break;
    default:  ;
  }
}
