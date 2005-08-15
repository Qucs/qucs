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
  DoubleVal = new QDoubleValidator(this);

  // ...........................................................
  all = new QGridLayout(this, 5,5,3,3);

  all->addWidget(new QLabel(tr("Reference Impedance:"), this), 0,0);
  ReferenceEdit = new QLineEdit(this);
  ReferenceEdit->setValidator(DoubleVal);
  ReferenceEdit->setText(QString::number(Z0_));
  all->addWidget(ReferenceEdit, 0,1);
  all->addMultiCellWidget(new QLabel(tr("Ohm"), this), 0,0,2,3);
  connect(ReferenceEdit, SIGNAL(textChanged(const QString&)),
	    SLOT(slotReflexionChanged(const QString&)));

  all->addWidget(new QLabel(tr("Frequency:"), this), 1,0);
  FrequencyEdit = new QLineEdit(this);
  FrequencyEdit->setValidator(DoubleVal);
  all->addWidget(FrequencyEdit, 1,1);
  UnitCombo = new QComboBox(this);
  UnitCombo->insertItem("Hz");
  UnitCombo->insertItem("kHz");
  UnitCombo->insertItem("MHz");
  UnitCombo->insertItem("GHz");
  all->addMultiCellWidget(UnitCombo, 1,1,2,3);

  int Expo = int(log10(Freq_) / 3.0);
  if(Expo < 0) Expo = 0;
  else if(Expo > 3) Expo = 3;
  UnitCombo->setCurrentItem(Expo);
  Freq_ /= pow(10.0, double(3*Expo));
  FrequencyEdit->setText(QString::number(Freq_));

  all->addWidget(new QLabel(tr("Impedance:"), this), 2,0);
  ImpedanceRealEdit = new QLineEdit(this);
  ImpedanceRealEdit->setValidator(DoubleVal);
  all->addWidget(ImpedanceRealEdit, 2,1);
  all->addWidget(new QLabel("+j", this), 2,2);
  ImpedanceImagEdit = new QLineEdit(this);
  ImpedanceImagEdit->setValidator(DoubleVal);
  all->addWidget(ImpedanceImagEdit, 2,3);
  all->addWidget(new QLabel(tr("Ohm"), this), 2,4);
  connect(ImpedanceRealEdit, SIGNAL(textChanged(const QString&)),
	    SLOT(slotImpedanceChanged(const QString&)));
  connect(ImpedanceImagEdit, SIGNAL(textChanged(const QString&)),
	    SLOT(slotImpedanceChanged(const QString&)));

  all->addWidget(new QLabel(tr("Reflexion Coefficient:"), this), 3,0);
  ReflexionRealEdit = new QLineEdit(this);
  ReflexionRealEdit->setValidator(DoubleVal);
  ReflexionRealEdit->setText(QString::number(Rreal_));
  all->addWidget(ReflexionRealEdit, 3,1);
  all->addWidget(new QLabel("+j", this), 3,2);
  ReflexionImagEdit = new QLineEdit(this);
  ReflexionImagEdit->setValidator(DoubleVal);
  ReflexionImagEdit->setText(QString::number(Rimag_));
  all->addWidget(ReflexionImagEdit, 3,3);
  connect(ReflexionRealEdit, SIGNAL(textChanged(const QString&)),
	    SLOT(slotReflexionChanged(const QString&)));
  connect(ReflexionImagEdit, SIGNAL(textChanged(const QString&)),
	    SLOT(slotReflexionChanged(const QString&)));
  slotReflexionChanged("");  // calculate impedance

  // ...........................................................
  QHBox *h0 = new QHBox(this);
  h0->setSpacing(5);
  all->addMultiCellWidget(h0, 4,4, 0,4);
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

  if(!calcMatchingCircuit(Real, Imag, Z0, Freq))
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
// Calculate matching circuit. Returns string like "sp:1nH:5pF"
QString MatchDialog::calcMatching(double r_real, double r_imag,
				double Z0, double Freq)
{
  double Zreal = r_real, Zimag = r_imag;
  r2z(Zreal, Zimag, Z0);

  if (Zreal < 0.0) {
    if (Zreal < -1e-13) {
      QMessageBox::critical(0, tr("Error"),
	tr("Real part of impedance must be greater zero,\nbut is %1 !").arg(Zreal));
      return QString("");  // matching not possible
    }

    // In high-Q circuits, Zreal often becomes somewhat about 1e-16
    // because of numerical inaccuracy.
    Zreal = 0.0;
  }


  double X1, X2, Omega = 2.0 * M_PI * Freq;
  QString Str;
  if(r_real < 0) {
    // ...................................................
    // first serial than parallel component (possible if Zreal <= Z0)
    Str = "sp";
    X1 = sqrt(Zreal * (Z0 - Zreal));
    if (Zimag < 0.0)  X1 *= -1.0;   // always use shortest matching path
    X1 -= Zimag;

    // parallel component
    X2 = (Zimag + X1) / (Zreal*Zreal + (Zimag + X1)*(Zimag + X1));
  }
  else {

    Str = "ps";
    X1 = Zreal + Zimag*Zimag / Zreal - Z0;
    // ...................................................
    // first parallel than serial component (possible if X >= 0.0)
    X1 = sqrt(Z0 * X1);
    if (Zimag > 0.0)  X1 *= -1.0;   // always use shortest matching path

    // parallel component
    X2 = Zimag / (Zreal*Zreal + Zimag*Zimag) + X1 / (Z0*Z0 + X1*X1);
  }


  // serial component
  if (X1 < 0.0)   // capacitance ?
    Str += ':' + num2str(-1.0 / Omega / X1) + 'F';
  else   // inductance
    Str += ':' + num2str(X1 / Omega) + 'H';

  // parallel component
  if (X2 < 0.0)   // inductance ?
    Str += ':' + num2str(-1.0 / Omega / X2) + 'H';
  else   // capacitance
    Str += ':' + num2str(X2 / Omega) + 'F';

  return Str;
}

// -----------------------------------------------------------------------
bool MatchDialog::calcMatchingCircuit(double r_real, double r_imag,
					double Z0, double Freq)
{
  QString Schematic =
      "<Qucs Schematic " PACKAGE_VERSION ">\n"
      "<Components>\n";


  QString Str = calcMatching(r_real, r_imag, Z0, Freq);
  if(Str.isEmpty())  return false;

  if(Str.section(':', 0,0) == "sp") {
    // ...................................................
    // first serial than parallel component

    if(Str.section(':', 1,1).right(1) == "F")
      Schematic += "<C C1";
    else
      Schematic += "<L L1";
    Schematic += " 1 100 10 -26 10 0 0 \"" + Str.section(':', 1,1) + "\" 1>\n";

    if(Str.section(':', 2,2).right(1) == "F")
      Schematic += "<C C2";
    else
      Schematic += "<L L2";
    Schematic += " 1 30 80 17 -26 0 1 \"" + Str.section(':', 2,2) + "\" 1>\n";

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

    if(Str.section(':', 1,1).right(1) == "F")
      Schematic += "<C C1";
    else
      Schematic += "<L L1";
    Schematic += " 1 10 10 -26 10 0 0 \"" + Str.section(':', 1,1) + "\" 1>\n";

    if(Str.section(':', 2,2).right(1) == "F")
      Schematic += "<C C2";
    else
      Schematic += "<L L2";
    Schematic += " 1 90 70 17 -26 0 1 \"" + Str.section(':', 2,2) + "\" 1>\n";

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
  return true;
}

// -----------------------------------------------------------------------
// Fundamental calculations for concurrent 2-port matching.
QString MatchDialog::calcBiMatch(double S11real, double S11imag,
            double S22real, double S22imag, double DetReal, double DetImag,
            double Z0, double Freq)
{
  double B = 1.0 + S11real*S11real + S11imag*S11imag
                 - S22real*S22real - S22imag*S22imag
                 - DetReal*DetReal - DetImag*DetImag;
  double Creal = S11real - S22real*DetReal - S22imag*DetImag;
  double Cimag = S22real*DetImag - S11imag - S22imag*DetReal;
  double Cmag  = 2.0 * (Creal*Creal + Cimag*Cimag);
  Creal /= Cmag;
  Cimag /= Cmag;

  double Rreal = B*B - 2.0*Cmag;
  double Rimag;
  if(Rreal < 0.0) {
    Rimag = Cimag * B - Creal * sqrt(-Rreal);
    Rreal = Creal * B + Cimag * sqrt(-Rreal);
  }
  else {
    Rreal  = B - sqrt(Rreal);
    Rimag  = Cimag * Rreal;
    Rreal *= Creal;
  }

  return calcMatching(Rreal, -Rimag, Z0, Freq);
}

// -----------------------------------------------------------------------
bool MatchDialog::calc2PortMatch(double S11real, double S11imag,
            double S22real, double S22imag, double DetReal, double DetImag,
            double Z0, double Freq)
{
  QString Input = calcBiMatch(S11real, S11imag, S22real, S22imag,
                              DetReal, DetImag, Z0, Freq);
  if(Input.isEmpty()) return false;

  QString Output = calcBiMatch(S22real, S22imag, S11real, S11imag,
                              DetReal, DetImag, Z0, Freq);
  if(Output.isEmpty()) return false;

  QString Schematic =
      "<Qucs Schematic " PACKAGE_VERSION ">\n"
      "<Components>\n";


  // ...................................................
  // create input circuit
  if(Input.section(':', 0,0) == "sp") {
    // first serial than parallel component

    if(Input.section(':', 1,1).right(1) == "F")
      Schematic += "<C C1";
    else
      Schematic += "<L L1";
    Schematic += " 1 -50 10 -26 10 0 0 \"" + Input.section(':', 1,1) + "\" 1>\n";

    if(Input.section(':', 2,2).right(1) == "F")
      Schematic += "<C C2";
    else
      Schematic += "<L L2";
    Schematic += " 1 -120 80 17 -26 0 1 \"" + Input.section(':', 2,2) + "\" 1>\n";

    Schematic += "<GND * 1 -120 110 0 0 0 0>\n";
  }
  else {
    // first parallel than serial component

    if(Input.section(':', 1,1).right(1) == "F")
      Schematic += "<C C1";
    else
      Schematic += "<L L1";
    Schematic += " 1 -140 10 -26 10 0 0 \"" + Input.section(':', 1,1) + "\" 1>\n";

    if(Input.section(':', 2,2).right(1) == "F")
      Schematic += "<C C2";
    else
      Schematic += "<L L2";
    Schematic += " 1 -60 70 17 -26 0 1 \"" + Input.section(':', 2,2) + "\" 1>\n";

    Schematic += "<GND * 1 -60 100 0 0 0 0>\n";
  }


  // ...................................................
  // create output circuit
  if(Output.section(':', 0,0) == "sp") {
    // first serial than parallel component

    if(Output.section(':', 1,1).right(1) == "F")
      Schematic += "<C C1";
    else
      Schematic += "<L L1";
    Schematic += " 1 50 10 -26 10 0 0 \"" + Output.section(':', 1,1) + "\" 1>\n";

    if(Output.section(':', 2,2).right(1) == "F")
      Schematic += "<C C2";
    else
      Schematic += "<L L2";
    Schematic += " 1 120 80 17 -26 0 1 \"" + Output.section(':', 2,2) + "\" 1>\n";

    Schematic += "<GND * 1 120 110 0 0 0 0>\n";
  }
  else {
    // ...................................................
    // first parallel than serial component

    if(Output.section(':', 1,1).right(1) == "F")
      Schematic += "<C C1";
    else
      Schematic += "<L L1";
    Schematic += " 1 140 10 -26 10 0 0 \"" + Output.section(':', 1,1) + "\" 1>\n";

    if(Output.section(':', 2,2).right(1) == "F")
      Schematic += "<C C2";
    else
      Schematic += "<L L2";
    Schematic += " 1 60 70 17 -26 0 1 \"" + Output.section(':', 2,2) + "\" 1>\n";

    Schematic += "<GND * 1 60 100 0 0 0 0>\n";
  }
  Schematic += "</Components>\n"
               "<Wires>\n";


  // ...................................................
  // create wires for input circuit
  if(Input.section(':', 0,0) == "sp") // first serial than parallel component
    Schematic +=
       "<-140 10 -120 10 \"\" 0 0 0 \"\">\n"
       "<-120 10 -80 10 \"\" 0 0 0 \"\">\n"
       "<-120 10 -120 50 \"\" 0 0 0 \"\">\n";
  else    // first parallel than serial component
    Schematic +=
       "<-60 10 -60 40 \"\" 0 0 0 \"\">\n"
       "<-60 10 -40 10 \"\" 0 0 0 \"\">\n"
       "<-110 10 -60 10 \"\" 0 0 0 \"\">\n";


  // ...................................................
  // create wires for output circuit
  if(Output.section(':', 0,0) == "sp") // first serial than parallel component
    Schematic +=
       "<140 10 120 10 \"\" 0 0 0 \"\">\n"
       "<120 10 80 10 \"\" 0 0 0 \"\">\n"
       "<120 10 120 50 \"\" 0 0 0 \"\">\n";
  else    // first parallel than serial component
    Schematic +=
       "<60 10 60 40 \"\" 0 0 0 \"\">\n"
       "<60 10 40 10 \"\" 0 0 0 \"\">\n"
       "<110 10 60 10 \"\" 0 0 0 \"\">\n";

  Schematic +=
     "</Wires>\n"
     "<Diagrams>\n"
     "</Diagrams>\n"
     "<Paintings>\n"
     "</Paintings>\n";

  QApplication::clipboard()->setText(Schematic, QClipboard::Clipboard);
  return true;
}
