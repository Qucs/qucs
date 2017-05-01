/***************************************************************************
                          markerdialog.cpp  -  description
                             -------------------
    begin                : Wed April 21 2004
    copyright            : (C) 2003 by Michael Margraf
    email                : michael.margraf@alumni.tu-berlin.de
 ***************************************************************************
    Update               : Added delta markers feature. March 2017
    Author		 : Andres Martinez-Mera
    email                : andresmartinezmera@gmail.com

****************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include "markerdialog.h"
#include "diagram.h"

#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QValidator>
#include <QGridLayout>
#include <QComboBox>
#include <QCheckBox>


MarkerDialog::MarkerDialog(Marker *pm_, QWidget *parent)
                     : QDialog(parent, Qt::WDestructiveClose)
{
  setWindowTitle(tr("Edit Marker Properties"));
  pMarker = pm_;
  MarkerColor = pMarker->getColor();
  ActiveMarkers = pm_->getMarkersMap();//The map of markers is needed so as to create a list for choosing a reference marker at the menu

  QGridLayout *g = new QGridLayout;

  Precision = new QLineEdit();
  Precision->setText(QString::number(pMarker->Precision));
  Precision->setValidator(new QIntValidator(0, 12, this));

  g->addWidget(new QLabel(tr("Precision: ")), 0, 0);
  g->addWidget(Precision, 0, 1);

  NumberBox = new QComboBox();
  NumberBox->insertItem(0,tr("real/imaginary"));
  NumberBox->insertItem(1, tr("magnitude/angle (degree)"));
  NumberBox->insertItem(2, tr("magnitude/angle (radian)"));
  NumberBox->setCurrentIndex(pMarker->numMode);

  g->addWidget(new QLabel(tr("Number Notation: ")), 1,0);
  g->addWidget(NumberBox, 1, 1);

  assert(pMarker->diag());
  if(pMarker->diag()->Name=="Smith") // BUG
  {
    //S parameter also displayed as Z, need Z0 here
		SourceImpedance = new QLineEdit();
  	SourceImpedance->setText(QString::number(pMarker->Z0));

		g->addWidget(new QLabel(tr("Z0: ")), 2,0);
		g->addWidget(SourceImpedance,2,1);
	}
  
  TransBox = new QCheckBox(tr("Transparent"));
  TransBox->setChecked(pMarker->transparent);
  g->addWidget(TransBox,3,0);

  g->addWidget(new QLabel(tr("Marker ID ")), 4,0);
  MarkerID = new QLineEdit();
  MarkerID->setText(pMarker->getID());
  g->addWidget(MarkerID, 4, 1);

  ColorLabel = new QLabel(tr("Color "));
  g->addWidget(ColorLabel, 5,0);
  ColorButton = new QPushButton("");
  QColor c = pMarker->getColor();
  QString stylesheet = QString("QPushButton {background-color: %1};").arg(c.name());
  ColorButton->setStyleSheet(stylesheet);
  connect(ColorButton, SIGNAL(clicked()), SLOT(slotSetColor()));
  g->addWidget(ColorButton, 5, 1);
  if (pMarker->getMarkerMode())  
  {
    ColorButton->setVisible(false);
    ColorLabel->setVisible(false);
  }

  g->addWidget(new QLabel(tr("Border width ")), 6,0);
  MarkerLineWidth = new QLineEdit();
  MarkerLineWidth->setText(QString("%1").arg(pMarker->getLineWidth()));
  g->addWidget(MarkerLineWidth, 6, 1);

  DeltaModeCheckBox = new QCheckBox(tr("Delta mode"));
  DeltaModeCheckBox->setChecked(pMarker->getMarkerMode());
  g->addWidget(DeltaModeCheckBox,7,0);
  connect(DeltaModeCheckBox, SIGNAL(clicked()), SLOT(slotDeltaMode()));

  g->addWidget(new QLabel(tr("Reference marker ")), 8,0);
  RefMarkerComboBox = new QComboBox();

  //Remove the current marker from the list of available markers (it makes no sense to use it as reference)
   std::vector<double> data;
   QMutableMapIterator<QString, std::vector<double>> map_it (ActiveMarkers);
   while (map_it.hasNext()) 
   {
    map_it.next();
    data = map_it.value();
    if ((map_it.key() == pm_->getID())||(data[5] == 1)) map_it.remove();
   }

  QStringList AvailableMarkers = ActiveMarkers.keys();
  if (AvailableMarkers.count() == 0 )
  { 
   RefMarkerComboBox->addItem("<Empty>");
   DeltaModeCheckBox->setEnabled(false);
  }
  else
  { 
   RefMarkerComboBox->addItems(AvailableMarkers);
   DeltaModeCheckBox->setEnabled(true);
  }
  RefMarkerComboBox->setEnabled(false);
  g->addWidget(RefMarkerComboBox, 8, 1);

  // first => activated by pressing RETURN
  QPushButton *ButtOK = new QPushButton(tr("OK"));
  connect(ButtOK, SIGNAL(clicked()), SLOT(slotAcceptValues()));
  ButtOK->setDefault(true);

  QPushButton *ButtCancel = new QPushButton(tr("Cancel"));
  connect(ButtCancel, SIGNAL(clicked()), SLOT(reject()));

  QHBoxLayout *b = new QHBoxLayout();
  b->setSpacing(9);
  b->addWidget(ButtOK);
  b->addWidget(ButtCancel);
  g->addLayout(b,9,0);

  this->setLayout(g);
}

MarkerDialog::~MarkerDialog()
{
}


//This function opens a color picker window to choose the color of the marker
void MarkerDialog::slotSetColor()
{
  QColor c = QColorDialog::getColor(QColor(ColorButton->styleSheet().section(' ',2,2).mid(0,7)),this);
  QString stylesheet = QString("QPushButton {background-color: %1};").arg(c.name());
  ColorButton->setStyleSheet(stylesheet);
  MarkerColor = c;
}

//Enables/Disables delta mode. The delta markers are black and it color cannot be changed
void MarkerDialog::slotDeltaMode()
{
  if (DeltaModeCheckBox->isChecked()) 
  {
    RefMarkerComboBox->setEnabled(true);
    ColorButton->setVisible(false);
    ColorLabel->setVisible(false);
  }
  else
  {
    RefMarkerComboBox->setEnabled(false);
    ColorButton->setVisible(true);
    ColorLabel->setVisible(true);
  }
}


// ----------------------------------------------------------
void MarkerDialog::slotAcceptValues()
{
  QString ID = MarkerID->text();
  if (!ID.isEmpty())pMarker->setID(ID);//Marker identifier
  pMarker->setLineWidth(MarkerLineWidth->text().toInt());//Marker line width property
  // Mode of operation: Normal (0) or Delta (1)


  if (DeltaModeCheckBox->isChecked())//Delta marker
  { 
    pMarker->setMarkerMode(1);
    pMarker->setReferenceMarkerID(RefMarkerComboBox->currentText());
    pMarker->setColor(Qt::darkMagenta);
  }
  else//Conventional marker
  {
    pMarker->setMarkerMode(0);
    pMarker->setReferenceMarkerID("None");
    pMarker->setColor(MarkerColor);
  }
  
  int tmp = Precision->text().toInt();
  if(tmp != pMarker->Precision) {
    pMarker->Precision = tmp;
  }
  assert(pMarker->diag());
  
   if(pMarker->diag()->Name=="Smith") // BUG: need generic MarkerDialog.
   {
      double SrcImp = SourceImpedance->text().toDouble();
      if(SrcImp != pMarker->Z0)
      {
	pMarker->Z0 = SrcImp;
      }
   }

  if(NumberBox->currentIndex() != pMarker->numMode) {
    pMarker->numMode = NumberBox->currentIndex();
  }

  pMarker->transparent = TransBox->isChecked();
  pMarker->createText();
    done(2);
}

// vim:ts=8:sw=2:noet
