/***************************************************************************
                               matchdialog.cpp
                              -----------------
    begin                : Fri Jul 22 2005
    copyright            : (C) 2005 by Michael Margraf
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

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include "matchdialog.h"
#include "qucs.h"
#include "qucsview.h"
#include "element.h"
#include "../components/capacitor.h"
#include "../components/inductor.h"
#include "../components/ground.h"

#include <qlabel.h>
#include <qlayout.h>
#include <qhbox.h>
#include <qlineedit.h>
#include <qcombobox.h>
#include <qvalidator.h>
#include <qpushbutton.h>
#include <qmessagebox.h>
#include <qapplication.h>
#include <qclipboard.h>


MatchDialog::MatchDialog(QWidget *parent, double Rreal_, double Rimag_,
			double Freq_, double Z0_)
			: QDialog(parent, 0, TRUE, Qt::WDestructiveClose)
{
  setCaption(tr("Create Matching Circuit"));

  all = new QVBoxLayout(this); // to provide neccessary size
  QWidget *myParent = this;

  DoubleVal = new QDoubleValidator(this);


  // ...........................................................
  QGridLayout *topGrid = new QGridLayout(0, 4,5,3,3);
  all->addLayout(topGrid);

  topGrid->addWidget(new QLabel(tr("Reference Impedance:"), myParent), 0,0);
  ReferenceEdit = new QLineEdit(myParent);
  ReferenceEdit->setValidator(DoubleVal);
  ReferenceEdit->setText(QString::number(Z0_));
  topGrid->addWidget(ReferenceEdit, 0,1);
  topGrid->addMultiCellWidget(new QLabel(tr("Ohm"), myParent), 0,0,2,3);
  connect(ReferenceEdit, SIGNAL(textChanged(const QString&)),
	    SLOT(slotReflexionChanged(const QString&)));

  topGrid->addWidget(new QLabel(tr("Frequency:"), myParent), 1,0);
  FrequencyEdit = new QLineEdit(myParent);
  FrequencyEdit->setValidator(DoubleVal);
  topGrid->addWidget(FrequencyEdit, 1,1);
  UnitCombo = new QComboBox(myParent);
  UnitCombo->insertItem("Hz");
  UnitCombo->insertItem("kHz");
  UnitCombo->insertItem("MHz");
  UnitCombo->insertItem("GHz");
  topGrid->addMultiCellWidget(UnitCombo, 1,1,2,3);

  int Expo = int(log10(Freq_) / 3.0);
  if(Expo < 0) Expo = 0;
  else if(Expo > 3) Expo = 3;
  UnitCombo->setCurrentItem(Expo);
  Freq_ /= pow(10.0, double(3*Expo));
  FrequencyEdit->setText(QString::number(Freq_));

  topGrid->addWidget(new QLabel(tr("Impedance:"), myParent), 2,0);
  ImpedanceRealEdit = new QLineEdit(myParent);
  ImpedanceRealEdit->setValidator(DoubleVal);
  topGrid->addWidget(ImpedanceRealEdit, 2,1);
  topGrid->addWidget(new QLabel("+j", myParent), 2,2);
  ImpedanceImagEdit = new QLineEdit(myParent);
  ImpedanceImagEdit->setValidator(DoubleVal);
  topGrid->addWidget(ImpedanceImagEdit, 2,3);
  topGrid->addWidget(new QLabel(tr("Ohm"), myParent), 2,4);
  connect(ImpedanceRealEdit, SIGNAL(textChanged(const QString&)),
	    SLOT(slotImpedanceChanged(const QString&)));
  connect(ImpedanceImagEdit, SIGNAL(textChanged(const QString&)),
	    SLOT(slotImpedanceChanged(const QString&)));

  topGrid->addWidget(new QLabel(tr("Reflexion Coefficient:"), myParent), 3,0);
  ReflexionRealEdit = new QLineEdit(myParent);
  ReflexionRealEdit->setValidator(DoubleVal);
  ReflexionRealEdit->setText(QString::number(Rreal_));
  topGrid->addWidget(ReflexionRealEdit, 3,1);
  topGrid->addWidget(new QLabel("+j", myParent), 3,2);
  ReflexionImagEdit = new QLineEdit(myParent);
  ReflexionImagEdit->setValidator(DoubleVal);
  ReflexionImagEdit->setText(QString::number(Rimag_));
  topGrid->addWidget(ReflexionImagEdit, 3,3);
  connect(ReflexionRealEdit, SIGNAL(textChanged(const QString&)),
	    SLOT(slotReflexionChanged(const QString&)));
  connect(ReflexionImagEdit, SIGNAL(textChanged(const QString&)),
	    SLOT(slotReflexionChanged(const QString&)));
  slotReflexionChanged("");  // calculate impedance

  // ...........................................................
  QHBox *h0 = new QHBox(this);
  h0->setSpacing(5);
  all->addWidget(h0);
  connect(new QPushButton(tr("Create"),h0), SIGNAL(clicked()),
	  SLOT(slotButtCreate()));
  connect(new QPushButton(tr("Cancel"),h0), SIGNAL(clicked()),
	  SLOT(reject()));
}

MatchDialog::~MatchDialog()
{
  delete all;
  delete DoubleVal;
}

// -----------------------------------------------------------------------
// Is called if the user changed the impedance. -> The reflexion
// coefficient is calculated.
void MatchDialog::slotImpedanceChanged(const QString&)
{
  double Z0   = ReferenceEdit->text().toDouble();
  double Real = ImpedanceRealEdit->text().toDouble();
  double Imag = ImpedanceImagEdit->text().toDouble();
  z2r(Real, Imag, Z0);
  ReflexionRealEdit->blockSignals(true); // do not call "changed-slot"
  ReflexionRealEdit->setText(QString::number(Real));
  ReflexionRealEdit->blockSignals(false);
  ReflexionImagEdit->blockSignals(true); // do not call "changed-slot"
  ReflexionImagEdit->setText(QString::number(Imag));
  ReflexionImagEdit->blockSignals(false);
}

// -----------------------------------------------------------------------
// Is called if the user changed the reflexion coefficient. -> The impedance
// is calculated.
void MatchDialog::slotReflexionChanged(const QString&)
{
  double Z0   = ReferenceEdit->text().toDouble();
  double Real = ReflexionRealEdit->text().toDouble();
  double Imag = ReflexionImagEdit->text().toDouble();
  r2z(Real, Imag, Z0);
  ImpedanceRealEdit->blockSignals(true); // do not call "changed-slot"
  ImpedanceRealEdit->setText(QString::number(Real));
  ImpedanceRealEdit->blockSignals(false);
  ImpedanceImagEdit->blockSignals(true); // do not call "changed-slot"
  ImpedanceImagEdit->setText(QString::number(Imag));
  ImpedanceImagEdit->blockSignals(false);
}

// -----------------------------------------------------------------------
// Is called if the "Create"-button is pressed.
void MatchDialog::slotButtCreate()
{
  double Z0   = ReferenceEdit->text().toDouble();
  double Freq = FrequencyEdit->text().toDouble() *
                pow(10.0, 3.0*double(UnitCombo->currentItem()));
  double Real = ReflexionRealEdit->text().toDouble();
  double Imag = ReflexionImagEdit->text().toDouble();

  if(calcMatchingCircuit(Real, Imag, Z0, Freq) < 0)
    return;

  QucsMain->Acts.slotEditPaste(true);
  accept();
}


// -----------------------------------------------------------------------
// transform reflexion coefficient into impedance
void MatchDialog::r2z(double& Real, double& Imag, double Z0)
{
  double tmp  = Z0 / ((1.0-Real)*(1.0-Real) + Imag*Imag);
  Real  = (1.0 - Real*Real - Imag*Imag) * tmp;
  Imag *= 2.0 * tmp;
}

// -----------------------------------------------------------------------
// transform impedance into reflexion coefficient
void MatchDialog::z2r(double& Real, double& Imag, double Z0)
{
  double tmp  = (Real+Z0)*(Real+Z0) + Imag*Imag;
  Real  = (Real*Real + Imag*Imag - Z0*Z0) / tmp;
  Imag *= 2.0 * Z0 / tmp;
}

// -----------------------------------------------------------------------
int MatchDialog::calcMatchingCircuit(double r_real, double r_imag,
					double Z0, double Freq)
{
  double Zreal = r_real, Zimag = r_imag;
  r2z(Zreal, Zimag, Z0);

  if (Zreal < 1e-250) {
    QMessageBox::critical(0, tr("Error"),
	  tr("Real part of impedance must be greater zero."));
    return -1;  // matching not possible
  }

  QString Schematic =
      "<Qucs Schematic " PACKAGE_VERSION ">\n"
      "<Components>\n";


  char Unit = ' ';
  double Xs, Xp, Vs, Vp;
  if(r_real < 0) {
  // ...................................................
  // first serial than parallel component
//  if (Zreal <= Z0) {   // is this method possible ?
    Xs = sqrt(Zreal * (Z0 - Zreal));
    if (Zimag < 0.0)  Xs *= -1.0;   // always use shortest matching path
    Xs -= Zimag;

    // first serial component
    if (Xs < 0.0) {  // capacitance ?
      Vs = -0.5 / M_PI / Freq / Xs;
      Schematic += "<C C1";
      Unit = 'F';
    }
    else {  // inductance
      Vs = Xs / 2.0 / M_PI / Freq;
      Schematic += "<L L1";
      Unit = 'H';
    }
    Schematic += " 1 100 10 -26 10 0 0 \"" + num2str(Vs) + Unit + "\" 1>\n";


    // second parallel component
    Xp = (Zimag + Xs) / (Zreal*Zreal + (Zimag + Xs)*(Zimag + Xs));
    if (Xp < 0.0) {  // inductance ?
      Vp = -0.5 / M_PI / Freq / Xp;
      Schematic += "<L L2";
      Unit = 'H';
    }
    else {  // capacitance
      Vp = Xp / 2.0 / M_PI / Freq;
      Schematic += "<C C2";
      Unit = 'F';
    }
    Schematic += " 1 30 80 17 -26 0 1 \"" + num2str(Vp) + Unit + "\" 1>\n";


    Schematic +=
       "<GND * 1 30 110 0 0 0 0>\n"
       "</Components>\n"
       "<Wires>\n"
       "<10 10 30 10 \"\" 0 0 0 \"\">\n"
       "<30 10 70 10 \"\" 0 0 0 \"\">\n"
       "<30 10 30 50 \"\" 0 0 0 \"\">\n";
  }
  else {

    // ...................................................
    // first parallel than serial component
    Xs = Zreal + Zimag*Zimag / Zreal - Z0;
//    if(Xs >= 0.0) {   // is this method possible ?
    Xs = sqrt(Z0 * Xs);
    if (Zimag > 0.0)  Xs *= -1.0;   // always use shortest matching path

    // serial component
    if (Xs < 0.0) {  // capacitance ?
      Vs = -0.5 / M_PI / Freq / Xs;
      Schematic += "<C C1";
      Unit = 'F';
    }
    else {  // inductance
      Vs = Xs / 2.0 / M_PI / Freq;
      Schematic += "<L L1";
      Unit = 'H';
    }
    Schematic += " 1 10 10 -26 10 0 0 \"" + num2str(Vs) + Unit + "\" 1>\n";


    // parallel component
    Xp = Zimag / (Zreal*Zreal + Zimag*Zimag) + Xs / (Z0*Z0 + Xs*Xs);
    if (Xp < 0.0) {  // inductance ?
      Vp = -0.5 / M_PI / Freq / Xp;
      Schematic += "<L L2";
      Unit = 'H';
    }
    else {  // capacitance
      Vp = Xp / 2.0 / M_PI / Freq;
      Schematic += "<C C2";
      Unit = 'F';
   }
    Schematic += " 1 90 70 17 -26 0 1 \"" + num2str(Vp) + Unit + "\" 1>\n";


    Schematic +=
       "<GND * 1 90 100 0 0 0 0>\n"
       "</Components>\n"
       "<Wires>\n"
       "<90 10 90 40 \"\" 0 0 0 \"\">\n"
       "<90 10 110 10 \"\" 0 0 0 \"\">\n"
       "<40 10 90 10 \"\" 0 0 0 \"\">\n";
  }

  Schematic +=
     "</Wires>\n"
     "<Diagrams>\n"
     "</Diagrams>\n"
     "<Paintings>\n"
     "</Paintings>\n";

  QApplication::clipboard()->setText(Schematic, QClipboard::Clipboard);
  return 0;
}
