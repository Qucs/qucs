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
#include "main.h"
#include "qucs.h"
#include "element.h"
#include "../components/capacitor.h"
#include "../components/inductor.h"
#include "../components/ground.h"
#include "misc.h"

#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QComboBox>
#include <QCheckBox>
#include <QRadioButton>
#include <QValidator>
#include <QPushButton>
#include <QMessageBox>
#include <QApplication>
#include <QClipboard>
#include <QGroupBox>

#include <cmath>


MatchDialog::MatchDialog(QWidget *parent)
			: QDialog(parent)
{
  setWindowTitle(tr("Create Matching Circuit"));
  DoubleVal = new QDoubleValidator(this);

  all = new QVBoxLayout(this);

	QGroupBox *MethodBox = new QGroupBox(tr("Implementation"));
  all->addWidget(MethodBox);
  QVBoxLayout *MethodLayout = new QVBoxLayout();
  MethodBox->setLayout(MethodLayout);

  QHBoxLayout *h4 = new QHBoxLayout();
  h4->setSpacing(3);
  TopoLabel = new QLabel(tr("Method"));
  h4->addWidget(TopoLabel);
  TopoCombo = new QComboBox();
  TopoCombo->insertItem(tr("LC matching"));
  TopoCombo->insertItem(tr("Single stub"));
  TopoCombo->insertItem(tr("Double stub"));
	QString str = tr("Multistage binomial ") + QString(QChar(0xBB, 0x03)) + "/4";
  TopoCombo->insertItem((str));
  h4->addWidget(TopoCombo);
	connect(TopoCombo, SIGNAL(activated(int)), SLOT(slotChangeMode_TopoCombo(int)));
  h4->addStretch(5);
	MethodLayout->addLayout(h4);

	QHBoxLayout *h5 = new QHBoxLayout();
	OpenRadioButton = new QRadioButton(tr("Open stub"), this);
	h5->addWidget(OpenRadioButton);
	OpenRadioButton->setChecked(true);
	OpenRadioButton->setVisible(false);

	ShortRadioButton = new QRadioButton(tr("Short circuit stub"), this);
	h5->addWidget(ShortRadioButton);
	ShortRadioButton->setChecked(false);
	ShortRadioButton->setVisible(false);
	MethodLayout->addLayout(h5);

	QHBoxLayout *h6 = new QHBoxLayout();
	OrderLabel = new QLabel(tr("Number of sections"));
	OrderEdit  = new QLineEdit("3");
	h6->addWidget(OrderLabel);
	h6->addWidget(OrderEdit);
	OrderLabel->setVisible(false);
	OrderEdit->setVisible(false);
	OrderEdit->setFixedWidth(50);
	OrderEdit->setAlignment(Qt::AlignLeft);
	h6->setAlignment(Qt::AlignLeft);
	MethodLayout->addLayout(h6);

  TwoCheck = new QCheckBox(tr("Calculate two-port matching"));
  all->addWidget(TwoCheck);
  TwoCheck->setChecked(true);
  connect(TwoCheck, SIGNAL(toggled(bool)), SLOT(slotSetTwoPort(bool)));



  // ...........................................................
  QGroupBox *ImpBox = new QGroupBox(tr("Reference Impedance"));
  all->addWidget(ImpBox);
  QHBoxLayout *ImpLayout = new QHBoxLayout();
  Port1Label = new QLabel(tr("Port 1"));
  Ref1Edit = new QLineEdit("50");
  Ref1Edit->setValidator(DoubleVal);
  Ohm1Label = new QLabel(tr("ohms"));
  connect(Ref1Edit, SIGNAL(textChanged(const QString&)),
	  SLOT(slotImpedanceChanged(const QString&)));
  Port2Label = new QLabel(tr("Port 2"));
  Ref2Edit = new QLineEdit("50");
  Ref2Edit->setValidator(DoubleVal);
  Ohm2Label = new QLabel(tr("Ohms"));
  ImpLayout->addWidget(Port1Label);
  ImpLayout->addWidget(Ref1Edit);
  ImpLayout->addWidget(Ohm1Label);
  ImpLayout->addSpacing(50);
  ImpLayout->addWidget(Port2Label);
  ImpLayout->addWidget(Ref2Edit);
  ImpLayout->addWidget(Ohm2Label);
  ImpBox->setLayout(ImpLayout);

  // ...........................................................
  QGroupBox *SParBox = new QGroupBox(tr("S Parameter"));
  all->addWidget(SParBox);
  QVBoxLayout *SParLayout = new QVBoxLayout();
  SParBox->setLayout(SParLayout);

  QHBoxLayout *h1 = new QHBoxLayout();
  h1->setSpacing(3);
  FormatLabel = new QLabel(tr("Input format"));
  h1->addWidget(FormatLabel);
  FormatCombo = new QComboBox();
  h1->addWidget(FormatCombo);
  FormatCombo->insertItem(tr("real/imag"));
  FormatCombo->insertItem(tr("mag/deg"));
  connect(FormatCombo, SIGNAL(activated(int)), SLOT(slotChangeMode(int)));
  h1->addStretch(5);
  SParLayout->addLayout(h1);

  QHBoxLayout *h3 = new QHBoxLayout();
  h3->setSpacing(3);
  QVBoxLayout *VBox1 = new QVBoxLayout();
  h3->addLayout(VBox1);
    S11Label = new QLabel(tr("S11"));
    S21Label = new QLabel(tr("S21"));
    VBox1->addWidget(S11Label);
    VBox1->addWidget(S21Label);
  QVBoxLayout *VBox2 = new QVBoxLayout();
  h3->addLayout(VBox2);
    S11magEdit = new QLineEdit("0.5");
    S11magEdit->setValidator(DoubleVal);
    S21magEdit = new QLineEdit("0.5");
    S21magEdit->setValidator(DoubleVal);
    VBox2->addWidget(S11magEdit);
    VBox2->addWidget(S21magEdit);
  QVBoxLayout *VBox3 = new QVBoxLayout();
  h3->addLayout(VBox3);
    S11sLabel = new QLabel("+j");
    S21sLabel = new QLabel("+j");
    VBox3->addWidget(S11sLabel);
    VBox3->addWidget(S21sLabel);
  QVBoxLayout *VBox4 = new QVBoxLayout();
  h3->addLayout(VBox4);
    S11degEdit = new QLineEdit("0");
    S11degEdit->setValidator(DoubleVal);
    S21degEdit = new QLineEdit("0");
    S21degEdit->setValidator(DoubleVal);
    VBox4->addWidget(S11degEdit);
    VBox4->addWidget(S21degEdit);
  QVBoxLayout *VBox5 = new QVBoxLayout();
  h3->addLayout(VBox5);
    S11uLabel = new QLabel(" ");
    S21uLabel = new QLabel(" ");
    VBox5->addWidget(S11uLabel);
    VBox5->addWidget(S21uLabel);
  h3->addStretch(5);
  QVBoxLayout *VBox6 = new QVBoxLayout();
  h3->addLayout(VBox6);
    S12Label = new QLabel(tr("S12"));
    S22Label = new QLabel(tr("S22"));
    VBox6->addWidget(S12Label);
    VBox6->addWidget(S22Label);
  QVBoxLayout *VBox7 = new QVBoxLayout();
  h3->addLayout(VBox7);
    S12magEdit = new QLineEdit("0");
    S12magEdit->setValidator(DoubleVal);
    S22magEdit = new QLineEdit("0.5");
    S22magEdit->setValidator(DoubleVal);
    VBox7->addWidget(S12magEdit);
    VBox7->addWidget(S22magEdit);
  QVBoxLayout *VBox8 = new QVBoxLayout();
  h3->addLayout(VBox8);
    S12sLabel = new QLabel("+j");
    S22sLabel = new QLabel("+j");
    VBox8->addWidget(S12sLabel);
    VBox8->addWidget(S22sLabel);
  QVBoxLayout *VBox9 = new QVBoxLayout();
  h3->addLayout(VBox9);
    S12degEdit = new QLineEdit("0");
    S12degEdit->setValidator(DoubleVal);
    S22degEdit = new QLineEdit("0");
    S22degEdit->setValidator(DoubleVal);
    VBox9->addWidget(S12degEdit);
    VBox9->addWidget(S22degEdit);
  QVBoxLayout *VBox0 = new QVBoxLayout();
  h3->addLayout(VBox0);
    S12uLabel = new QLabel(" ");
    S22uLabel = new QLabel(" ");
    VBox0->addWidget(S12uLabel);
    VBox0->addWidget(S22uLabel);
  SParLayout->addLayout(h3);

  connect(S21magEdit, SIGNAL(textChanged(const QString&)),
	  SLOT(slotImpedanceChanged(const QString&)));
  connect(S21degEdit, SIGNAL(textChanged(const QString&)),
	  SLOT(slotImpedanceChanged(const QString&)));
  connect(S11magEdit, SIGNAL(textChanged(const QString&)),
	  SLOT(slotReflexionChanged(const QString&)));
  connect(S11degEdit, SIGNAL(textChanged(const QString&)),
	  SLOT(slotReflexionChanged(const QString&)));


  QHBoxLayout *h2 = new QHBoxLayout();
  h2->setSpacing(3);
  FrequencyLabel = new QLabel(tr("Frequency:"));
  FrequencyEdit = new QLineEdit();
  FrequencyEdit->setValidator(DoubleVal);
  h2->addWidget(FrequencyLabel);
  h2->addWidget(FrequencyEdit);
  UnitCombo = new QComboBox();
  UnitCombo->insertItem("Hz");
  UnitCombo->insertItem("kHz");
  UnitCombo->insertItem("MHz");
  UnitCombo->insertItem("GHz");
  h2->addWidget(UnitCombo);
  h2->addStretch(5);
  SParLayout->addLayout(h2);

  // ...........................................................
  QHBoxLayout *h0 = new QHBoxLayout();
  h0->setSpacing(5);
  all->addLayout(h0);
  h0->addStretch(5);
  QPushButton *buttCreate = new QPushButton(tr("Create"));
  QPushButton *buttCancel = new QPushButton(tr("Cancel"));
  h0->addWidget(buttCreate);
  h0->addWidget(buttCancel);
  connect(buttCreate, SIGNAL(clicked()), SLOT(slotButtCreate()));
  connect(buttCancel, SIGNAL(clicked()), SLOT(reject()));

  slotReflexionChanged("");  // calculate impedance
  setFrequency(1e9);  // set 1GHz
  resize(520, 100);
}

MatchDialog::~MatchDialog()
{
  delete all;
  delete DoubleVal;
}

// -----------------------------------------------------------------------
void MatchDialog::setFrequency(double Freq_)
{
  int Expo = int(log10(Freq_) / 3.0);
  if(Expo < 0) Expo = 0;
  else if(Expo > 3) Expo = 3;
  UnitCombo->setCurrentItem(Expo);
  Freq_ /= pow(10.0, double(3*Expo));
  FrequencyEdit->setText(QString::number(Freq_));
}

// -----------------------------------------------------------------------
// Is called when the checkbox for two-port matching changes.
void MatchDialog::slotSetTwoPort(bool on)
{
  if(on) { // two-port matching ?
    S11Label->setText(tr("S11"));
    S21Label->setText(tr("S21"));
    S12magEdit->setEnabled(true);
    S22magEdit->setEnabled(true);
    S12degEdit->setEnabled(true);
    S22degEdit->setEnabled(true);
    S12Label->setEnabled(true);
    S22Label->setEnabled(true);
    S12sLabel->setEnabled(true);
    S22sLabel->setEnabled(true);
    S12degEdit->setEnabled(true);
    S22degEdit->setEnabled(true);
    S12uLabel->setEnabled(true);
    S22uLabel->setEnabled(true);
    Port2Label->setEnabled(true);
    Ref2Edit->setEnabled(true);
    Ohm2Label->setEnabled(true);
  }
  else {
    S11Label->setText(tr("Reflexion Coefficient"));
    S21Label->setText(tr("Impedance (ohms)"));
    S12magEdit->setEnabled(false);
    S22magEdit->setEnabled(false);
    S12degEdit->setEnabled(false);
    S22degEdit->setEnabled(false);
    S12Label->setEnabled(false);
    S22Label->setEnabled(false);
    S12sLabel->setEnabled(false);
    S22sLabel->setEnabled(false);
    S12degEdit->setEnabled(false);
    S22degEdit->setEnabled(false);
    S12uLabel->setEnabled(false);
    S22uLabel->setEnabled(false);
    Port2Label->setEnabled(false);
    Ref2Edit->setEnabled(false);
    Ohm2Label->setEnabled(false);
  }
}
//------------------------------------------------------------------------
// It is called when the implementation combobox changes
void MatchDialog::slotChangeMode_TopoCombo(int Index)
{
	if ((TopoCombo->currentIndex() == 1)||(TopoCombo->currentIndex() == 2))
	{
		ShortRadioButton->setVisible(true);
		OpenRadioButton->setVisible(true);
	}
	else
	{
		ShortRadioButton->setVisible(false);
		OpenRadioButton->setVisible(false);
	}
	if (TopoCombo->currentIndex() == 3)
	{
		OrderLabel->setVisible(true);
		OrderEdit->setVisible(true);
	}
	else
	{
		OrderLabel->setVisible(false);
		OrderEdit->setVisible(false);
	}
}

// -----------------------------------------------------------------------
// Is called when the combobox changes between mag/deg and real/imag.
void MatchDialog::slotChangeMode(int Index)
{
  if(Index) { // polar ?
    S11sLabel->setText("/");
    S12sLabel->setText("/");
    S21sLabel->setText("/");
    S22sLabel->setText("/");
    S11uLabel->setText(QString::fromUtf8("°"));
    S12uLabel->setText(QString::fromUtf8("°"));
    S21uLabel->setText(QString::fromUtf8("°"));
    S22uLabel->setText(QString::fromUtf8("°"));

    double Real = S11magEdit->text().toDouble();
    double Imag = S11degEdit->text().toDouble();
    c2p(Real, Imag);
    S11magEdit->setText(QString::number(Real));
    S11degEdit->setText(QString::number(Imag));

    Real = S12magEdit->text().toDouble();
    Imag = S12degEdit->text().toDouble();
    c2p(Real, Imag);
    S12magEdit->setText(QString::number(Real));
    S12degEdit->setText(QString::number(Imag));

    Real = S21magEdit->text().toDouble();
    Imag = S21degEdit->text().toDouble();
    c2p(Real, Imag);
    S21magEdit->setText(QString::number(Real));
    S21degEdit->setText(QString::number(Imag));

    Real = S22magEdit->text().toDouble();
    Imag = S22degEdit->text().toDouble();
    c2p(Real, Imag);
    S22magEdit->setText(QString::number(Real));
    S22degEdit->setText(QString::number(Imag));
  }
  else {  // cartesian
    S11sLabel->setText("+j");
    S12sLabel->setText("+j");
    S21sLabel->setText("+j");
    S22sLabel->setText("+j");
    S11uLabel->setText(" ");
    S12uLabel->setText(" ");
    S21uLabel->setText(" ");
    S22uLabel->setText(" ");

    double Mag   = S11magEdit->text().toDouble();
    double Phase = S11degEdit->text().toDouble();
    p2c(Mag, Phase);
    S11magEdit->setText(QString::number(Mag));
    S11degEdit->setText(QString::number(Phase));

    Mag   = S12magEdit->text().toDouble();
    Phase = S12degEdit->text().toDouble();
    p2c(Mag, Phase);
    S12magEdit->setText(QString::number(Mag));
    S12degEdit->setText(QString::number(Phase));

    Mag   = S21magEdit->text().toDouble();
    Phase = S21degEdit->text().toDouble();
    p2c(Mag, Phase);
    S21magEdit->setText(QString::number(Mag));
    S21degEdit->setText(QString::number(Phase));

    Mag   = S22magEdit->text().toDouble();
    Phase = S22degEdit->text().toDouble();
    p2c(Mag, Phase);
    S22magEdit->setText(QString::number(Mag));
    S22degEdit->setText(QString::number(Phase));
  }
}

// -----------------------------------------------------------------------
// Is called if the user changed the impedance. -> The reflexion
// coefficient is calculated.
void MatchDialog::slotImpedanceChanged(const QString&)
{
  if(TwoCheck->isChecked())  return;

  double Z0   = Ref1Edit->text().toDouble();
  double Real = S21magEdit->text().toDouble();
  double Imag = S21degEdit->text().toDouble();
  z2r(Real, Imag, Z0);
  S11magEdit->blockSignals(true); // do not call "changed-slot"
  S11magEdit->setText(QString::number(Real));
  S11magEdit->blockSignals(false);
  S11degEdit->blockSignals(true); // do not call "changed-slot"
  S11degEdit->setText(QString::number(Imag));
  S11degEdit->blockSignals(false);
}

// -----------------------------------------------------------------------
// Is called if the user changed the reflexion coefficient. -> The impedance
// is calculated.
void MatchDialog::slotReflexionChanged(const QString&)
{
  if(TwoCheck->isChecked())  return;

  double Z0   = Ref1Edit->text().toDouble();
  double Real = S11magEdit->text().toDouble();
  double Imag = S11degEdit->text().toDouble();
  r2z(Real, Imag, Z0);
  S21magEdit->blockSignals(true); // do not call "changed-slot"
  S21magEdit->setText(QString::number(Real));
  S21magEdit->blockSignals(false);
  S21degEdit->blockSignals(true); // do not call "changed-slot"
  S21degEdit->setText(QString::number(Imag));
  S21degEdit->blockSignals(false);
}

// -----------------------------------------------------------------------
// Is called if the "Create"-button is pressed.
void MatchDialog::slotButtCreate()
{
	bool openStub;//When stub matching, it selects the type of stub (short circuit or open)
	int order;//Order of the lambda/4 matching
  double Z1   = Ref1Edit->text().toDouble();
  double Z2   = Ref2Edit->text().toDouble();
  double Freq = FrequencyEdit->text().toDouble() *
                pow(10.0, 3.0*double(UnitCombo->currentItem()));

  double S11real = S11magEdit->text().toDouble();
  double S11imag = S11degEdit->text().toDouble();
  double S12real = S12magEdit->text().toDouble();
  double S12imag = S12degEdit->text().toDouble();
  double S21real = S21magEdit->text().toDouble();
  double S21imag = S21degEdit->text().toDouble();
  double S22real = S22magEdit->text().toDouble();
  double S22imag = S22degEdit->text().toDouble();
  if(FormatCombo->currentItem()) {  // are they polar ?
    p2c(S11real, S11imag);
    p2c(S12real, S12imag);
    p2c(S21real, S21imag);
    p2c(S22real, S22imag);
  }
double Z0=50;
  if(TwoCheck->isChecked()) {  // two-port matching ?
    // determinante of S-parameter matrix
    double DetReal = S11real*S22real - S11imag*S22imag
                   - S12real*S21real + S12imag*S21imag;
    double DetImag = S11real*S22imag + S11imag*S22real
                   - S12real*S21imag - S12imag*S21real;

        switch(TopoCombo->currentIndex())
        {
           case 0: // LC
               if(!MatchDialog::calc2PortMatch(S11real, S11imag, S22real, S22imag, DetReal, DetImag, Z1, Z2, Freq))return;
               break;
           case 1: // Single stub
               openStub = OpenRadioButton->isChecked();
               if(!MatchDialog::calcMatchingCircuitSingleStub2Ports(S11real, S11imag, S12real, S12imag, S21real, S21imag, S22real, S22imag, Z1, Z2, Freq, openStub, Z0))return;
               break;
           case 2: // Double stub
               openStub = OpenRadioButton->isChecked();
               if(!MatchDialog::calcMatchingCircuitDoubleStub2Ports(S11real, S11imag, S12real, S12imag, S21real, S21imag, S22real, S22imag, Z1, Z2, Freq, openStub, Z0))return;
               break;
           case 3: // Quarter wave cascaded sections
                order = OrderEdit->text().toInt()+1 ;
                if(!MatchDialog::calcMatchingCircuitLambda42Ports(S11real, S11imag, S12real, S12imag, S21real, S21imag, S22real, S22imag, Z1, Z2, Freq, order, Z0))return;
                break;

        }




  }
  else{
      switch(TopoCombo->currentIndex())
      {
          case 0: // LC
                  if(!calcMatchingCircuitLC(S11real, S11imag, Z1, Freq))return;
                  break;
          case 1: // Single stub
									openStub = OpenRadioButton->isChecked();
                  if(!calcMatchingCircuitSingleStub(S11real, S11imag, Z1, Freq, openStub))return;
                  break;
          case 2: // Double stub
									openStub = OpenRadioButton->isChecked();
                  if(!calcMatchingCircuitDoubleStub(S11real, S11imag, Z1, Freq, openStub))return;
                  break;
          case 3: // Quarter wave cascaded sections
									order = OrderEdit->text().toInt()+1;
                  if(!calcMatchingCircuitLambda4(S11real, S11imag, Z1, Freq, order))return;
                  break;

      }
   }

  QucsMain->slotEditPaste(true);
  accept();
}


// -----------------------------------------------------------------------
// transform real/imag into mag/deg (cartesian to polar)
void MatchDialog::c2p(double& Real, double& Imag)
{
  double Real_ = Real;
  Real = sqrt(Real*Real + Imag*Imag);     // magnitude
  Imag = 180.0/pi * atan2(Imag, Real_); // phase in degree
}

// -----------------------------------------------------------------------
// transform mag/deg into real/imag (polar to cartesian)
void MatchDialog::p2c(double& Real, double& Imag)
{
  double Real_ = Real;
  Real = Real_ * cos(Imag * pi/180.0);  // real part
  Imag = Real_ * sin(Imag * pi/180.0);  // imaginary part
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

    // In high-Q circuits, Zreal often becomes somewhat about -1e-16
    // because of numerical inaccuracy.
    Zreal = 0.0;
  }


  double X1, X2, Omega = 2.0 * pi * Freq;
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
    Str += ':' + misc::num2str(-1.0 / Omega / X1) + 'F';
  else   // inductance
    Str += ':' + misc::num2str(X1 / Omega) + 'H';

  // parallel component
  if (X2 < 0.0)   // inductance ?
    Str += ':' + misc::num2str(-1.0 / Omega / X2) + 'H';
  else   // capacitance
    Str += ':' + misc::num2str(X2 / Omega) + 'F';

  return Str;
}

bool MatchDialog::calcMatchingCircuitSingleStub(double r_real, double r_imag, double Z0, double Freq, bool open_short)
{
        double RL = r_real;
        double XL = r_imag;
        double t=0, t1 = 0, t2 = 0;
        double dl, dl1, dl2, B;
        double B1, B2, d, lstub, ll;
        double lambda = 3e8/Freq;

        r2z(RL, XL, Z0);
        if (RL == Z0)
        {
             t = -XL / (2*Z0);
             (t<0) ? dl = (M_PI + atan(t))/ (2*M_PI): dl = (atan(t))/ (2*M_PI);
             B = (RL*RL * t - (Z0-XL*t)*(Z0*t + XL))/(Z0*(RL*RL + (Z0*t + XL)*(Z0*t + XL)));
        }
        else
        {
            t1 = (XL + sqrt(((RL/Z0)*abs((Z0 - RL)*(Z0 - RL) - XL*XL))))/(RL - Z0);
            (t1 < 0) ? dl1 = (M_PI + atan(t1))/ (2*M_PI) : dl1 = (atan(t1))/ (2*M_PI);
            B1 = (RL*RL * t1 - (Z0-XL*t1)*(Z0*t1 + XL))/(Z0*(RL*RL + (Z0*t1 + XL)*(Z0*t1 + XL)));

            t2  = (XL - sqrt((RL*abs((Z0 - RL)*(Z0 - RL) - XL*XL))/(Z0)))/(RL - Z0);
            (t2 < 0) ? dl2 = (M_PI + atan(t2))/ (2*M_PI) : dl2 = (atan(t2))/ (2*M_PI);
            B2 = (RL*RL * t2 - (Z0-XL*t2)*(Z0*t2 + XL))/(Z0*(RL*RL + (Z0*t2 + XL)*(Z0*t2 + XL)));
        }

        if (t!=0)
        {
            d=dl*lambda;
            (open_short) ? ll = -(atan(B*Z0))/(2*M_PI) :ll= (atan(1./(B*Z0)))/(2*M_PI);
						if ((open_short)&&(ll<0))ll+=0.5;
						if ((!open_short)&&(ll>0.5))ll-=0.5;
            lstub = ll*lambda;
        }

        if (t1 != 0)
        {
             d=dl1*lambda;
             (open_short) ? ll = -(atan(B1*Z0))/(2*M_PI) : ll= (atan(1./(1.*B1*Z0)))/(2*M_PI);
						 if ((open_short)&&(ll<0))ll+=0.5;
 						 if ((!open_short)&&(ll>0.5))ll-=0.5;
             lstub = ll*lambda;

        }else{
            if (t2 != 0)
            {
            d=dl2*lambda;
            (open_short) ? ll = -(atan(B2*Z0))/(2*M_PI) : ll= (atan(1./(1.*B2*Z0)))/(2*M_PI);
            if ((open_short)&&(ll<0))ll+=0.5;
						if ((!open_short)&&(ll>0.5))ll-=0.5;
            lstub = ll*lambda;
            }
        }


        QString s = "<Qucs Schematic " PACKAGE_VERSION ">\n";
        int x = 60;
        s += "<Components>\n";
        s += QString("<Pac P1 1 %1 330 18 -26 0 1 \"1\" 1 \"%2 Ohm\" 1 \"0 dBm\" 0 \"1 GHz\" 0>\n").arg(x).arg(Z0);
        s += QString("<GND * 1 %1 360 0 0 0 0>\n").arg(x);
        x += 60;
        s += QString("<TLIN Line1 1 %1 120 -26 20 0 -1 \"%2\" 1 \"%3\" 1 \"0 dB\" 0 \"26.85\" 0>\n").arg(x).arg(Z0).arg(lstub);
				if (!open_short)s += QString("<GND * 1 %1 90 0 0 -1 1>\n").arg(x);
        x+=40;
        s += QString("<TLIN Line1 1 %1 180 -26 20 0 0 \"%2\" 1 \"%3\" 1 \"0 dB\" 0 \"26.85\" 0>\n").arg(x).arg(Z0).arg(d);
        x += 80;
        if ((RL > 0)&&(XL < 0))
        {
        s += QString("<R R1 1 %1 270 15 -26 0 -1 \"%2 Ohm\" 1 \"26.85\" 0 \"US\" 0>\n").arg(x).arg(RL);
        s += QString("<C C1 1 %1 330 15 -26 0 -1 \"%2 F\" 1 0>\n").arg(x).arg(1/(abs(XL)*2*M_PI*Freq));
        }
        if ((RL > 0)&&(XL > 0))
        {
        s += QString("<R R1 1 %1 270 15 -26 0 -1 \"%2 Ohm\" 1 \"26.85\" 0 \"US\" 0>\n").arg(x).arg(RL);
        s += QString("<L L1 1 %1 330 15 -26 0 -1 \"%2 H\" 1 0>\n").arg(x).arg(XL/(2*M_PI*Freq));
        }
        if ((RL > 0)&&(XL == 0))
        {
        s += QString("<R R1 1 %1 270 15 -26 0 -1 \"%2 Ohm\" 1 \"26.85\" 0 \"US\" 0>\n").arg(x).arg(RL);
        }

        s += QString("<GND * 1 %1 360 0 0 0 0>\n").arg(x);

        double freq_start = std::max(0., Freq-1e9);
        double freq_stop = Freq+1e9;
        s += QString("<.SP SP1 1 70 460 0 67 0 0 \"lin\" 1 \"%2Hz\" 1 \"%3Hz\" 1 \"300\" 1 \"no\" 0 \"1\" 0 \"2\" 0>\n").arg((freq_start)).arg((freq_stop));
        s += QString("<Eqn Eqn1 1 450 560 -28 15 0 0 \"S11_dB=dB(S[1,1])\" 1 \"yes\" 0>\n");
        s += "</Components>\n";
        s += "<Wires>\n";

        // connect left source
          s += QString("<60 180 60 300 \"\" 0 0 0>\n");
          s += QString("<60 180 130 180 \"\" 0 0 0>\n");
          s += QString("<120 150 120 180 \"\" 0 0 0>\n");
        // connect right source
        if ((RL > 0)&&(XL != 0))
        {
          s += QString("<%1 180 %2 240 \"\" 0 0 0>\n").arg(x).arg(x);
          s += QString("<%1 180 %2 180 \"\" 0 0 0>\n").arg(x-50).arg(x);
        }
        if ((RL > 0)&&(XL == 0))
        {
          s += QString("<%1 180 %2 240 \"\" 0 0 0>\n").arg(x).arg(x);
          s += QString("<%1 300 %2 360 \"\" 0 0 0>\n").arg(x).arg(x);
          s += QString("<%1 180 %2 180 \"\" 0 0 0>\n").arg(x-50).arg(x);
        }
        s += "</Wires>\n";

          s += "<Diagrams>\n";
          s += "</Diagrams>\n";

					QApplication::clipboard()->setText(s, QClipboard::Clipboard);
				  return true;
}
bool MatchDialog::calcMatchingCircuitDoubleStub(double r_real, double r_imag, double Z0, double Freq, bool open_short)
{
	   double RL = r_real;
	   double XL = r_imag;
		 r2z(RL, XL, Z0);
	   double Y0=1./Z0;
     double GL = (1/((RL*RL)+(XL*XL)))*RL;
     double BL = -(1/((RL*RL)+(XL*XL)))*XL;
     double lambda = 3e8/Freq;
     double beta=(2*M_PI)/lambda;
     double d = lambda/8;
     double t = tan(beta*d);
		 double ll1, ll2;

     // Stubs susceptance
     double B11 = -BL + (Y0 + sqrt((1+t*t)*GL*Y0 - GL*GL*t*t))/(t); // 1st solution
     double B12 = -BL + (Y0 - sqrt((1+t*t)*GL*Y0 - GL*GL*t*t))/(t); // 2nd solution

     double B21 = ((Y0*sqrt((1+t*t)*GL*Y0 - GL*GL*t*t)) + GL*Y0)/(GL*t); // 1st solution
     double B22 = ((-Y0*sqrt((1+t*t)*GL*Y0 - GL*GL*t*t)) + GL*Y0)/(GL*t);// 2nd solution

     // Open circuit solution
		 (open_short) ? ll1 = (atan(B11*Z0))/(2*M_PI) : ll1= -(atan(1./(1.*B11*Z0)))/(2*M_PI);
		 (open_short) ? ll2 = (atan(B21*Z0))/(2*M_PI) : ll2= -(atan(1./(1.*B21*Z0)))/(2*M_PI);
         if (ll1<0)ll1+=0.5;
         if (ll2<0)ll2+=0.5;
		 if ((!open_short)&&(ll1>0.5))ll1-=0.5;
		 if ((!open_short)&&(ll2>0.5))ll2-=0.5;


     double lstub1 = ll1*lambda, lstub2=ll2*lambda;

     QString s = "<Qucs Schematic " PACKAGE_VERSION ">\n";
     int x = 60;
     s += "<Components>\n";
     s += QString("<Pac P1 1 %1 330 18 -26 0 1 \"1\" 1 \"%2 Ohm\" 1 \"0 dBm\" 0 \"1 GHz\" 0>\n").arg(x).arg(Z0);
     s += QString("<GND * 1 %1 360 0 0 0 0>\n").arg(x);
     x += 60;
     s += QString("<TLIN Line1 1 %1 80 -26 20 0 0 \"%2\" 1 \"%3\" 1 \"0 dB\" 0 \"26.85\" 0>\n").arg(x+30).arg(Z0).arg(lstub2);
		 if (!open_short)s += QString("<GND * 1 %1 80 0 0 -1 1>\n").arg(x+60);
     x+=40;
     s += QString("<TLIN Line1 1 %1 180 -26 20 0 0 \"%2\" 1 \"%3\" 1 \"0 dB\" 0 \"26.85\" 0>\n").arg(x).arg(Z0).arg(d);
     x += 80;
     s += QString("<TLIN Line1 1 %1 80 -26 20 0 0 \"%2\" 1 \"%3\" 1 \"0 dB\" 0 \"26.85\" 0>\n").arg(x+30).arg(Z0).arg(lstub1);
		 if (!open_short)s += QString("<GND * 1 %1 80 0 0 -1 1>\n").arg(x+60);
     x += 80;
     if ((RL > 0)&&(XL< 0))
     {
     s += QString("<R R1 1 %1 270 15 -26 0 -1 \"%2 Ohm\" 1 \"26.85\" 0 \"US\" 0>\n").arg(x).arg(RL);
     s += QString("<C C1 1 %1 330 15 -26 0 -1 \"%2 F\" 1 0>\n").arg(x).arg(1/(abs(XL)*2*M_PI*Freq));
     }
     if ((RL > 0)&&(XL > 0))
     {
     s += QString("<R R1 1 %1 270 15 -26 0 -1 \"%2 Ohm\" 1 \"26.85\" 0 \"US\" 0>\n").arg(x).arg(RL);
     s += QString("<L L1 1 %1 330 15 -26 0 -1 \"%2 H\" 1 0>\n").arg(x).arg(XL/(2*M_PI*Freq));
     }
     if ((RL > 0)&&(XL == 0))
     {
     s += QString("<R R1 1 %1 270 15 -26 0 -1 \"%2 Ohm\" 1 \"26.85\" 0 \"US\" 0>\n").arg(x).arg(RL);
     }

     s += QString("<GND * 1 %1 360 0 0 0 0>\n").arg(x);

     double freq_start = std::max(0., Freq-1e9);
     double freq_stop = Freq+1e9;
     s += QString("<.SP SP1 1 70 460 0 67 0 0 \"lin\" 1 \"%2Hz\" 1 \"%3Hz\" 1 \"300\" 1 \"no\" 0 \"1\" 0 \"2\" 0>\n").arg((freq_start)).arg((freq_stop));
     s += QString("<Eqn Eqn1 1 450 560 -28 15 0 0 \"S11_dB=dB(S[1,1])\" 1 \"yes\" 0>\n");
     s += "</Components>\n";
     s += "<Wires>\n";

     // connect left source
     s += QString("<60 180 60 300 \"\" 0 0 0>\n");
     s += QString("<60 180 130 180 \"\" 0 0 0>\n");
     s += QString("<120 80 120 180 \"\" 0 0 0>\n");
     // connect right source
     if ((RL > 0)&&(XL != 0))
     {
       s += QString("<%1 180 %2 240 \"\" 0 0 0>\n").arg(x).arg(x);
     }
     if ((RL > 0)&&(XL == 0))
     {
       s += QString("<%1 180 %2 240 \"\" 0 0 0>\n").arg(x).arg(x);
       s += QString("<%1 300 %2 360 \"\" 0 0 0>\n").arg(x).arg(x);
     }
     s += QString("<%1 180 %2 180 \"\" 0 0 0>\n").arg(x-130).arg(x);
     s += QString("<%1 180 %2 80 \"\" 0 0 0>\n").arg(x-80).arg(x-80);
     s += "</Wires>\n";

     s += "<Diagrams>\n";
     s += "</Diagrams>\n";

		 QApplication::clipboard()->setText(s, QClipboard::Clipboard);
		 return true;

}

double factorial(double n)
{
  return (n == 1 || n == 0) ? 1 : factorial(n - 1) * n;
}

bool MatchDialog::calcMatchingCircuitLambda4(double r_real, double r_imag, double Z0, double Freq, int order)
{
	double RL = r_real;
	double XL = r_imag;
	r2z(RL, XL, Z0);

	if (RL ==0)
	 {
			 QMessageBox::warning(0, QObject::tr("Error"),
								QObject::tr("The load has not resistive part. It cannot be matched using the quarter wavelength method"));
			 return NULL;
	 }
	 if (XL !=0)
 	 {
 			 QMessageBox::warning(0, QObject::tr("Warning"),
 								QObject::tr("Reactive loads cannot be matched. Only the real part will be matched"));
 	 }

	 double lambda = 3e8/Freq;
	 double lambda4 = lambda/4;
	 double Ci, Zi, Zaux=Z0;

	 QString s = "<Qucs Schematic " PACKAGE_VERSION ">\n";
	 int x = 60;
	 s += "<Components>\n";
	 s += QString("<Pac P1 1 %1 330 18 -26 0 1 \"1\" 1 \"%2 Ohm\" 1 \"0 dBm\" 0 \"1 GHz\" 0>\n").arg(x).arg(Z0);
	 s += QString("<GND * 1 %1 360 0 0 0 0>\n").arg(x);

	 for (int i = 1; i <order; i++)
	 {
	 	x += 90;
		Ci = (factorial(order-1))/(factorial(order-1-(i-1))*factorial(i-1));
		Zi = Zaux*exp((Ci/pow(2,order-1))*log(RL/Z0));
		Zaux=Zi;
	 	s += QString("<TLIN Line1 1 %1 180 -26 20 0 0 \"%2\" 1 \"%3\" 1 \"0 dB\" 0 \"26.85\" 0>\n").arg(x+30).arg(Zi).arg(lambda4);
	 }
	 x += 90;
	 s += QString("<Pac P1 1 %1 270 15 -26 0 1 \"1\" 1 \"%2 Ohm\" 1 \"0 dBm\" 0 \"1 GHz\" 0>\n").arg(x+20).arg(RL);
	 s += QString("<GND * 1 %1 360 0 0 0 0>\n").arg(x+20);

	 double freq_start = std::max(0., Freq-2e9);
	 double freq_stop = Freq+2e9;
	 s += QString("<.SP SP1 1 70 460 0 67 0 0 \"lin\" 1 \"%2Hz\" 1 \"%3Hz\" 1 \"300\" 1 \"no\" 0 \"1\" 0 \"2\" 0>\n").arg((freq_start)).arg((freq_stop));
	 s += QString("<Eqn Eqn1 1 450 560 -28 15 0 0 \"S21_dB=dB(S[2,1])\" 1 \"S11_dB=dB(S[1,1])\" 1 \"yes\" 0>\n");
	 s += "</Components>\n";
	 s += "<Wires>\n";

	 // connect left source
		 s += QString("<60 180 60 300 \"\" 0 0 0>\n");
		 s += QString("<60 180 150 180 \"\" 0 0 0>\n");

	 x=210;
	 for (int i = 1; i<order-1; i++)
	 {
			 s += QString("<%1 180 %2 180 \"\" 0 0 0>\n").arg(x).arg(x+30);
			 x+=90;
	 }

	 // connect right source
	 x+=50;
	 s += QString("<%1 180 %2 180 \"\" 0 0 0>\n").arg(x-50).arg(x);
	 s += QString("<%1 180 %2 240 \"\" 0 0 0>\n").arg(x).arg(x);
	 s += QString("<%1 300 %2 360 \"\" 0 0 0>\n").arg(x).arg(x);


	 s += "</Wires>\n";
	 s += "<Diagrams>\n";
	 s += "</Diagrams>\n";

		 QApplication::clipboard()->setText(s, QClipboard::Clipboard);
		 return true;
}




// -----------------------------------------------------------------------
bool MatchDialog::calcMatchingCircuitLC(double r_real, double r_imag,
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
    Schematic += " 1 50 10 -26 10 0 0 \"" + Str.section(':', 1,1) + "\" 1>\n";

    if(Str.section(':', 2,2).right(1) == "F")
      Schematic += "<C C2";
    else
      Schematic += "<L L2";
    Schematic += " 1 130 70 17 -26 0 1 \"" + Str.section(':', 2,2) + "\" 1>\n";

    Schematic +=
       "<GND * 1 130 100 0 0 0 0>\n"
       "</Components>\n"
       "<Wires>\n"
       "<130 10 130 40 \"\" 0 0 0 \"\">\n"
       "<130 10 150 10 \"\" 0 0 0 \"\">\n"
       "<80 10 130 10 \"\" 0 0 0 \"\">\n";
  }

  Schematic += QString(
     "</Wires>\n"
     "<Diagrams>\n"
     "</Diagrams>\n"
     "<Paintings>\n"
     "  <Text -20 -10 12 #000000 0 \"%1 Ohm\">\n"
     "  <Text 120 -10 12 #000000 0 \"device\">\n"
     "</Paintings>\n").arg(Z0);

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
            double Z1, double Z2, double Freq)
{
  QString Input = calcBiMatch(S11real, S11imag, S22real, S22imag,
                              DetReal, DetImag, Z1, Freq);
  if(Input.isEmpty()) return false;

  QString Output = calcBiMatch(S22real, S22imag, S11real, S11imag,
                              DetReal, DetImag, Z2, Freq);
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
     "  <Text -200 -10 12 #000000 0 \"Port 1\">\n"
     "  <Text -20 -10 12 #000000 0 \"device\">\n"
     "  <Text 160 -10 12 #000000 0 \"Port 2\">\n"
     "</Paintings>\n";

  QApplication::clipboard()->setText(Schematic, QClipboard::Clipboard);
  return true;
}


double angle(double r, double i)
{

             if (r>=0)
             {
                 //Quadrant 1 or Quadrant 4
                return atan(i/r);

             }
             else //Quadrant 2 or 3
             {
                 if(i>=0)//Quadrant 2
                 {
                     return atan(i/r)+pi;
                 }
                 else //Quadrant 3
                 {
                      return atan(i/r)-pi;
                 }

             }
}

 bool MatchDialog::calcMatchingCircuitSingleStub2Ports(double s11_r, double s11_i, double s12_r, double s12_i, double s21_r, double s21_i, double s22_r, double s22_i, double Z1, double Z2, double Freq, bool open_short, double Z0)
 {

     double gammaS = (Z1-Z0)/(Z1+Z0);
     double gammaL = (Z2-Z0)/(Z2+Z0);
     double phi21 = atan(s21_i/s21_r);
     double phi12 = atan(s12_i/s12_r);
     double mag12 = sqrt(s12_r*s12_r + s12_i*s12_i);
     double mag21 = sqrt(s21_r*s21_r + s21_i*s21_i);
     double gamma_in_r= s11_r + ((gammaL*mag12*mag21)/(sqrt(pow(1-s22_r*gammaL,2) + pow(s22_i*gammaL,2))))*cos(phi12 + phi21 - angle((-s22_i*gammaL),(1-s22_r*gammaL)));
     double gamma_in_i= s11_i + ((gammaL*mag12*mag21)/(sqrt(pow(1-s22_r*gammaL,2) + pow(s22_i*gammaL,2))))*sin(phi12 + phi21 - angle((-s22_i*gammaL),(1-s22_r*gammaL)));

     double gamma_out_r= s22_r + ((gammaS*mag12*mag21)/(sqrt(pow(1-s11_r*gammaS,2) + pow(s11_i*gammaS,2))))*cos(phi12 + phi21 - angle((-s11_i*gammaS),(1-s11_r*gammaS)));
     double gamma_out_i= s22_i + ((gammaS*mag12*mag21)/(sqrt(pow(1-s11_r*gammaS,2) + pow(s11_i*gammaS,2))))*sin(phi12 + phi21 - angle((-s11_i*gammaS),(1-s11_r*gammaS)));


	 double t=0, t1 = 0, t2 = 0;
	 double dl, dl1, dl2;
     double B1, B2, B, d, lstub, ll;
	 double lambda = 3e8/Freq;

	 double D_port1, L_port1, D_port2, L_port2;

     double RL = gamma_in_r, XL= gamma_in_i;
     r2z(RL, XL, Z0);

	 if (RL == Z1)
	 {
				t = -XL / (2*Z1);
				(t<0) ? dl = (M_PI + atan(t))/ (2*M_PI): dl = (atan(t))/ (2*M_PI);
				B = (RL*RL * t - (Z1-XL*t)*(Z1*t + XL))/(Z1*(RL*RL + (Z1*t + XL)*(Z1*t + XL)));
	 }
	 else
	 {
			 t1 = (XL + sqrt(((RL/Z1)*abs((Z1 - RL)*(Z1 - RL) - XL*XL))))/(RL - Z1);
			 (t1 < 0) ? dl1 = (M_PI + atan(t1))/ (2*M_PI) : dl1 = (atan(t1))/ (2*M_PI);
			 B1 = (RL*RL * t1 - (Z1-XL*t1)*(Z1*t1 + XL))/(Z1*(RL*RL + (Z1*t1 + XL)*(Z1*t1 + XL)));

			 t2  = (XL - sqrt((RL*abs((Z1 - RL)*(Z1 - RL) - XL*XL))/(Z1)))/(RL - Z1);
			 (t2 < 0) ? dl2 = (M_PI + atan(t2))/ (2*M_PI) : dl2 = (atan(t2))/ (2*M_PI);
			 B2 = (RL*RL * t2 - (Z1-XL*t2)*(Z1*t2 + XL))/(Z1*(RL*RL + (Z1*t2 + XL)*(Z1*t2 + XL)));
	 }

	 if (t!=0)
	 {
			 d=dl*lambda;
			 (open_short) ? ll = -(atan(B*Z1))/(2*M_PI) :ll= (atan(1./(B*Z1)))/(2*M_PI);
			 if ((open_short)&&(ll<0))ll+=0.5;
			 if ((!open_short)&&(ll>0.5))ll-=0.5;
			 lstub = ll*lambda;
	 }

	 if (t1 != 0)
	 {
				d=dl1*lambda;
				(open_short) ? ll = -(atan(B1*Z1))/(2*M_PI) : ll= (atan(1./(1.*B1*Z1)))/(2*M_PI);
				if ((open_short)&&(ll<0))ll+=0.5;
				if ((!open_short)&&(ll>0.5))ll-=0.5;
				lstub = ll*lambda;

	 }else{
			 if (t2 != 0)
			 {
			 d=dl2*lambda;
			 (open_short) ? ll = -(atan(B2*Z1))/(2*M_PI) : ll= (atan(1./(1.*B2*Z1)))/(2*M_PI);
			 if ((open_short)&&(ll<0))ll+=0.5;
			 if ((!open_short)&&(ll>0.5))ll-=0.5;
			 lstub = ll*lambda;
			 }
	 }
	 D_port1 = d;
	 L_port1 = lstub;

	 // OUTPUT Port
    /* B = 1.0 + S22real*S22real + S22imag*S22imag
	 							 - S11real*S11real - S11imag*S11imag
	 							 - DetReal*DetReal - DetImag*DetImag;
	 Creal = S22real - S11real*DetReal - S11imag*DetImag;
	 Cimag = S11real*DetImag - S22imag - S11imag*DetReal;
	 Cmag  = 2.0 * (Creal*Creal + Cimag*Cimag);
	 Creal /= Cmag;
	 Cimag /= Cmag;

	 Rreal = B*B - 2.0*Cmag;
     Rimag;
	 if(Rreal < 0.0) {
	 	Rimag = Cimag * B - Creal * sqrt(-Rreal);
	 	Rreal = Creal * B + Cimag * sqrt(-Rreal);
	 }
	 else {
	 	Rreal  = B - sqrt(Rreal);
	 	Rimag  = Cimag * Rreal;
	 	Rreal *= Creal;
	 }

	 RL = Rreal;
     XL = -Rimag;*/
     RL = gamma_out_r;
     XL= gamma_out_i;
     r2z(RL, XL, Z2);

	 if (RL == Z2)
	 {
				t = -XL / (2*Z2);
				(t<0) ? dl = (M_PI + atan(t))/ (2*M_PI): dl = (atan(t))/ (2*M_PI);
				B = (RL*RL * t - (Z2-XL*t)*(Z2*t + XL))/(Z2*(RL*RL + (Z2*t + XL)*(Z2*t + XL)));
	 }
	 else
	 {
			 t1 = (XL + sqrt(((RL/Z2)*abs((Z2 - RL)*(Z2 - RL) - XL*XL))))/(RL - Z2);
			 (t1 < 0) ? dl1 = (M_PI + atan(t1))/ (2*M_PI) : dl1 = (atan(t1))/ (2*M_PI);
			 B1 = (RL*RL * t1 - (Z2-XL*t1)*(Z2*t1 + XL))/(Z2*(RL*RL + (Z2*t1 + XL)*(Z2*t1 + XL)));

			 t2  = (XL - sqrt((RL*abs((Z2 - RL)*(Z2 - RL) - XL*XL))/(Z2)))/(RL - Z2);
			 (t2 < 0) ? dl2 = (M_PI + atan(t2))/ (2*M_PI) : dl2 = (atan(t2))/ (2*M_PI);
			 B2 = (RL*RL * t2 - (Z2-XL*t2)*(Z2*t2 + XL))/(Z2*(RL*RL + (Z2*t2 + XL)*(Z2*t2 + XL)));
	 }

	 if (t!=0)
	 {
			 d=dl*lambda;
			 (open_short) ? ll = -(atan(B*Z2))/(2*M_PI) :ll= (atan(1./(B*Z2)))/(2*M_PI);
			 if ((open_short)&&(ll<0))ll+=0.5;
			 if ((!open_short)&&(ll>0.5))ll-=0.5;
			 lstub = ll*lambda;
	 }

	 if (t1 != 0)
	 {
				d=dl1*lambda;
				(open_short) ? ll = -(atan(B1*Z2))/(2*M_PI) : ll= (atan(1./(1.*B1*Z2)))/(2*M_PI);
				if ((open_short)&&(ll<0))ll+=0.5;
				if ((!open_short)&&(ll>0.5))ll-=0.5;
				lstub = ll*lambda;

	 }else{
			 if (t2 != 0)
			 {
			 d=dl2*lambda;
			 (open_short) ? ll = -(atan(B2*Z2))/(2*M_PI) : ll= (atan(1./(1.*B2*Z2)))/(2*M_PI);
			 if ((open_short)&&(ll<0))ll+=0.5;
			 if ((!open_short)&&(ll>0.5))ll-=0.5;
			 lstub = ll*lambda;
			 }
	 }
	 D_port2 = d;
	 L_port2 = lstub;

	 QString s = "<Qucs Schematic " PACKAGE_VERSION ">\n";
	 int x = 60;
	 s += "<Components>\n";
	 //INPUT port
	 x += 60;
	 s += QString("<TLIN Line1 1 %1 120 -26 20 0 -1 \"%2\" 1 \"%3\" 1 \"0 dB\" 0 \"26.85\" 0>\n").arg(x).arg(Z1).arg(L_port1);
	 if (!open_short)s += QString("<GND * 1 %1 90 0 0 -1 1>\n").arg(x);
	 x+=40;
	 s += QString("<TLIN Line1 1 %1 180 -26 20 0 0 \"%2\" 1 \"%3\" 1 \"0 dB\" 0 \"26.85\" 0>\n").arg(x).arg(Z1).arg(D_port1);
	 x += 200;

// OUTPUT port
	 s += QString("<TLIN Line1 1 %1 180 -26 20 0 0 \"%2\" 1 \"%3\" 1 \"0 dB\" 0 \"26.85\" 0>\n").arg(x).arg(Z2).arg(D_port2);
	 x+=60;
	 s += QString("<TLIN Line1 1 %1 120 -26 20 0 -1 \"%2\" 1 \"%3\" 1 \"0 dB\" 0 \"26.85\" 0>\n").arg(x).arg(Z2).arg(L_port2);
	 if (!open_short)s += QString("<GND * 1 %1 90 0 0 -1 1>\n").arg(x);

	 x += 60;
	 s += "</Components>\n";
	 s += "<Wires>\n";


	// Connecting Stubs
	 s += QString("<120 150 120 180 \"\" 0 0 0>\n");
	 s += QString("<420 150 420 180 \"\" 0 0 0>\n");
  // Connecting lines
	s += QString("<100 180 130 180 \"\" 0 0 0>\n");
	s += QString("<390 180 440 180 \"\" 0 0 0>\n");


	 s += "</Wires>\n";
	 s += "<Diagrams>\n";
	 s += "</Diagrams>\n";

	 s+="<Paintings>\n<Text 240 170 12 #000000 0 \"Device\">\n</Paintings>\n";

		 QApplication::clipboard()->setText(s, QClipboard::Clipboard);
		 return true;

 }


bool MatchDialog::calcMatchingCircuitDoubleStub2Ports(double s11_r, double s11_i, double s12_r, double s12_i, double s21_r, double s21_i, double s22_r, double s22_i, double Z1, double Z2, double Freq, bool open_short, double Z0)
{
    double gammaS = (Z1-Z0)/(Z1+Z0);
    double gammaL = (Z2-Z0)/(Z2+Z0);
    double phi21 = angle(s21_r,s21_i);
    double phi12 = angle(s12_r, s12_i);
    double mag12 = sqrt(s12_r*s12_r + s12_i*s12_i);
    double mag21 = sqrt(s21_r*s21_r + s21_i*s21_i);
    double gamma_in_r= s11_r + ((gammaL*mag12*mag21)/(sqrt(pow(1-s22_r*gammaL,2) + pow(s22_i*gammaL,2))))*cos(phi12 + phi21 - angle((1-s22_r*gammaL),(-s22_i*gammaL)));
    double gamma_in_i= s11_i + ((gammaL*mag12*mag21)/(sqrt(pow(1-s22_r*gammaL,2) + pow(s22_i*gammaL,2))))*sin(phi12 + phi21 - angle((1-s22_r*gammaL),(-s22_i*gammaL)));

    double gamma_out_r= s22_r + ((gammaS*mag12*mag21)/(sqrt(pow(1-s11_r*gammaS,2) + pow(s11_i*gammaS,2))))*cos(phi12 + phi21 - angle((1-s11_r*gammaS),(-s11_i*gammaS)));
    double gamma_out_i= s22_i + ((gammaS*mag12*mag21)/(sqrt(pow(1-s11_r*gammaS,2) + pow(s11_i*gammaS,2))))*sin(phi12 + phi21 - angle((1-s11_r*gammaS),(-s11_i*gammaS)));

    // INPUT PORT

    double RL = gamma_in_r;
    double XL = gamma_in_i;
    r2z(RL, XL, Z0);
    double Y0=1./Z0;
    double GL = (1/((RL*RL)+(XL*XL)))*RL;
    double BL = -(1/((RL*RL)+(XL*XL)))*XL;
    double lambda = 3e8/Freq;
    double beta=(2*M_PI)/lambda;
    double d = lambda/8;
    double t = tan(beta*d);
    double ll1, ll2;

    // Stubs susceptance
    double B11 = -BL + (Y0 + sqrt((1+t*t)*GL*Y0 - GL*GL*t*t))/(t); // 1st solution
    double B12 = -BL + (Y0 - sqrt((1+t*t)*GL*Y0 - GL*GL*t*t))/(t); // 2nd solution

    double B21 = ((Y0*sqrt((1+t*t)*GL*Y0 - GL*GL*t*t)) + GL*Y0)/(GL*t); // 1st solution
    double B22 = ((-Y0*sqrt((1+t*t)*GL*Y0 - GL*GL*t*t)) + GL*Y0)/(GL*t);// 2nd solution

  // Open circuit solution
    (open_short) ? ll1 = (atan(B11*Z0))/(2*M_PI) : ll1= -(atan(1./(1.*B11*Z0)))/(2*M_PI);
    (open_short) ? ll2 = (atan(B21*Z0))/(2*M_PI) : ll2= -(atan(1./(1.*B21*Z0)))/(2*M_PI);
    if ((ll1<0))ll1+=0.5;
    if ((ll2<0))ll2+=0.5;
    if ((ll1>0.5))ll1-=0.5;
    if ((ll2>0.5))ll2-=0.5;

    double lstub1_in = ll1*lambda, lstub2_in=ll2*lambda;

    // OUTPUT PORT

    RL = gamma_out_r;
    XL = gamma_out_i;
    r2z(RL, XL, Z0);
    Y0=1./Z0;
    GL = (1/((RL*RL)+(XL*XL)))*RL;
    BL = -(1/((RL*RL)+(XL*XL)))*XL;
    lambda = 3e8/Freq;
    beta=(2*M_PI)/lambda;
    d = lambda/8;
    t = tan(beta*d);

    // Stubs susceptance
    B11 = -BL + (Y0 + sqrt((1+t*t)*GL*Y0 - GL*GL*t*t))/(t); // 1st solution
    B12 = -BL + (Y0 - sqrt((1+t*t)*GL*Y0 - GL*GL*t*t))/(t); // 2nd solution

    B21 = ((Y0*sqrt((1+t*t)*GL*Y0 - GL*GL*t*t)) + GL*Y0)/(GL*t); // 1st solution
    B22 = ((-Y0*sqrt((1+t*t)*GL*Y0 - GL*GL*t*t)) + GL*Y0)/(GL*t);// 2nd solution

  // Open circuit solution
    (open_short) ? ll1 = (atan(B11*Z0))/(2*M_PI) : ll1= -(atan(1./(1.*B11*Z0)))/(2*M_PI);
    (open_short) ? ll2 = (atan(B21*Z0))/(2*M_PI) : ll2= -(atan(1./(1.*B21*Z0)))/(2*M_PI);
    if ((ll1<0))ll1+=0.5;
    if ((ll2<0))ll2+=0.5;
    if ((ll1>0.5))ll1-=0.5;
    if ((ll2>0.5))ll2-=0.5;

    double lstub1_out = ll1*lambda, lstub2_out=ll2*lambda;

    // QUCS SCHEMATIC

    QString s = "<Qucs Schematic " PACKAGE_VERSION ">\n";
    s += "<Components>\n";
    //INPUT port
    s += QString("<TLIN Line1 1 40 120 -26 20 0 -1 \"%1\" 1 \"%2\" 1 \"0 dB\" 0 \"26.85\" 0>\n").arg(Z1).arg(lstub2_in);
    if (!open_short)s += QString("<GND * 1 40 90 0 0 -1 1>\n");

    s += QString("<TLIN Line1 1 80 180 -26 20 0 0 \"%1\" 1 \"%2\" 1 \"0 dB\" 0 \"26.85\" 0>\n").arg(Z1).arg(d);
    s += QString("<TLIN Line1 1 140 120 -26 20 0 -1 \"%1\" 1 \"%2\" 1 \"0 dB\" 0 \"26.85\" 0>\n").arg(Z1).arg(lstub1_in);
    if (!open_short)s += QString("<GND * 1 140 90 0 0 -1 1>\n");

    s += QString("<TLIN Line1 1 340 120 -26 20 0 -1 \"%1\" 1 \"%2\" 1 \"0 dB\" 0 \"26.85\" 0>\n").arg(Z2).arg(lstub2_out);
    if (!open_short)s += QString("<GND * 1 340 90 0 0 -1 1>\n");

    s += QString("<TLIN Line1 1 380 180 -26 20 0 0 \"%1\" 1 \"%2\" 1 \"0 dB\" 0 \"26.85\" 0>\n").arg(Z2).arg(d);
    s += QString("<TLIN Line1 1 440 120 -26 20 0 -1 \"%1\" 1 \"%2\" 1 \"0 dB\" 0 \"26.85\" 0>\n").arg(Z2).arg(lstub1_out);
    if (!open_short)s += QString("<GND * 1 440 90 0 0 -1 1>\n");


    s += "</Components>\n";
    s += "<Wires>\n";


   // Connecting Stubs
    s += QString("<0 180 50 180 \"\" 0 0 0>\n");
    s += QString("<110 180 160 180 \"\" 0 0 0>\n");
    s += QString("<40 150 40 180 \"\" 0 0 0>\n");
    s += QString("<140 150 140 180 \"\" 0 0 0>\n");

    s += QString("<300 180 350 180 \"\" 0 0 0>\n");
    s += QString("<410 180 460 180 \"\" 0 0 0>\n");
    s += QString("<340 150 340 180 \"\" 0 0 0>\n");
    s += QString("<440 150 440 180 \"\" 0 0 0>\n");
     // Connecting lines



    s += "</Wires>\n";
    s += "<Diagrams>\n";
    s += "</Diagrams>\n";

    s+="<Paintings>\n<Text 240 170 12 #000000 0 \"Device\">\n</Paintings>\n";

    QApplication::clipboard()->setText(s, QClipboard::Clipboard);
    return true;

}
bool MatchDialog::calcMatchingCircuitLambda42Ports(double s11_r, double s11_i, double s12_r, double s12_i, double s21_r, double s21_i, double s22_r, double s22_i, double Z1, double Z2, double Freq, int order, double Z0)
{
    double gammaS = (Z1-Z0)/(Z1+Z0);
    double gammaL = (Z2-Z0)/(Z2+Z0);
    double phi21 = atan(s21_i/s21_r);
    double phi12 = atan(s12_i/s12_r);
    double mag12 = sqrt(s12_r*s12_r + s12_i*s12_i);
    double mag21 = sqrt(s21_r*s21_r + s21_i*s21_i);
    double gamma_in_r= s11_r + ((gammaL*mag12*mag21)/(sqrt(pow(1-s22_r*gammaL,2) + pow(s22_i*gammaL,2))))*cos(phi12 + phi21 - angle((-s22_i*gammaL),(1-s22_r*gammaL)));
    double gamma_in_i= s11_i + ((gammaL*mag12*mag21)/(sqrt(pow(1-s22_r*gammaL,2) + pow(s22_i*gammaL,2))))*sin(phi12 + phi21 - angle((-s22_i*gammaL),(1-s22_r*gammaL)));

    double gamma_out_r= s22_r + ((gammaS*mag12*mag21)/(sqrt(pow(1-s11_r*gammaS,2) + pow(s11_i*gammaS,2))))*cos(phi12 + phi21 - angle((-s11_i*gammaS),(1-s11_r*gammaS)));
    double gamma_out_i= s22_i + ((gammaS*mag12*mag21)/(sqrt(pow(1-s11_r*gammaS,2) + pow(s11_i*gammaS,2))))*sin(phi12 + phi21 - angle((-s11_i*gammaS),(1-s11_r*gammaS)));


    // INPUT PORT

    double RL = gamma_in_r;
    double XL = gamma_in_i;
    r2z(RL, XL, Z0);

    if (RL ==0)
     {
             QMessageBox::warning(0, QObject::tr("Error"),
                                QObject::tr("The load has not resistive part. It cannot be matched using the quarter wavelength method"));
             return NULL;
     }
     if (XL !=0)
     {
             QMessageBox::warning(0, QObject::tr("Warning"),
                                QObject::tr("Reactive loads cannot be matched. Only the real part will be matched"));
     }

     double lambda = 3e8/Freq;
     double lambda4 = lambda/4;
     double Ci, Zi, Zaux=Z0;

     QString s = "<Qucs Schematic " PACKAGE_VERSION ">\n";
     int x = 0;
     s += "<Components>\n";

     for (int i = 1; i <order; i++)
     {
        x += 90;
        Ci = (factorial(order-1))/(factorial(order-1-(i-1))*factorial(i-1));
        Zi = Zaux*exp((Ci/pow(2,order-1))*log(RL/Z0));
        Zaux=Zi;
        s += QString("<TLIN Line1 1 %1 180 -26 20 0 0 \"%2\" 1 \"%3\" 1 \"0 dB\" 0 \"26.85\" 0>\n").arg(x+30).arg(Zi).arg(lambda4);
     }


     // OUTPUT PORT

     RL = gamma_out_r;
     XL = gamma_out_i;
     r2z(RL, XL, Z0);

     if (RL ==0)
      {
              QMessageBox::warning(0, QObject::tr("Error"),
                                 QObject::tr("The load has not resistive part. It cannot be matched using the quarter wavelength method"));
              return NULL;
      }
      if (XL !=0)
      {
              QMessageBox::warning(0, QObject::tr("Warning"),
                                 QObject::tr("Reactive loads cannot be matched. Only the real part will be matched"));
      }

      x+=200;

      x+=(order-1)*90;
      Zaux=Z0;
      for (int i = 1; i <order; i++)
      {

         Ci = (factorial(order-1))/(factorial(order-1-(i-1))*factorial(i-1));
         Zi = Zaux*exp((Ci/pow(2,order-1))*log(RL/Z0));
         Zaux=Zi;
         s += QString("<TLIN Line1 1 %1 180 -26 20 0 0 \"%2\" 1 \"%3\" 1 \"0 dB\" 0 \"26.85\" 0>\n").arg(x+30).arg(Zi).arg(lambda4);
         x -= 90;
      }

     s += "</Components>\n";
     s += "<Wires>\n";


     x=150;
     for (int i = 1; i<order-1; i++)
     {
             s += QString("<%1 180 %2 180 \"\" 0 0 0>\n").arg(x).arg(x+30);
             x+=90;
     }


    x+=290;

    for (int i = 1; i<order-1; i++)
    {
            s += QString("<%1 180 %2 180 \"\" 0 0 0>\n").arg(x).arg(x+30);
            x+=90;
    }



     s += "</Wires>\n";
     s += "<Diagrams>\n";
     s += "</Diagrams>\n";

         QApplication::clipboard()->setText(s, QClipboard::Clipboard);
         return true;

}

#define  MAX_ERROR  1e-7
void calcMicrostrip(tSubstrate *substrate,
                               double width, double freq, double& er_eff, double& zl)
{
  double a, b;
  double h  = substrate->height;
  double t  = substrate->thickness;
  double er = substrate->er;
  double Wh = width / h;
  t /= h;

  // quasi-static models by Hammerstad

  double w1 = Wh;
  if(t > 1e-100) { // width correction due to metal thickness?
    a = coth(sqrt(6.517*Wh));
    b = t / pi * log(1.0 + 10.873127 / t / a / a);
    w1 += b;
    Wh += 0.5 * b * (1.0 + sech(sqrt(er - 1.0)));
  }

  // relative effective permittivity
  a = Wh * Wh;
  b = a * a;
  er_eff  = -0.564 * pow((er-0.9) / (er+3.0), 0.053);
  er_eff *= 1.0 + log((b + a/2704.0) / (b + 0.432)) / 49.0 + log(1.0 + a*Wh/5929.741) / 18.7;
  er_eff  = (er+1.0) / 2.0 + (er-1.0) / 2.0 * pow(1.0 + 10.0/Wh, er_eff);

  // characteristic impedance
  zl = 6.0 + 0.2831853 * exp(-pow(30.666/Wh, 0.7528));
  zl = Z_FIELD / 2.0/pi * log(zl/Wh + sqrt(1.0 + 4.0/Wh/Wh));

  // characteristic impedance (same again for "w1")
  a = 6.0 + 0.2831853 * exp(-pow(30.666/w1, 0.7528));
  a = Z_FIELD / 2.0/pi * log(a/w1 + sqrt(1.0 + 4.0/w1/w1));

  a /= zl;
  zl /= sqrt(er_eff);
  er_eff *= a * a;


  // dispersion models by Kirschning

  freq *= h / 1e6;  // normalize frequency into GHz*mm

  // relative effective permittivity
  a = 0.0363 * exp(-4.6*Wh) * (1.0 - exp(-pow(freq/38.7, 4.97)));
  a *= 1.0 + 2.751 * (1.0 - exp(-pow(er/15.916, 8.0)));
  a = pow((0.1844 + a) * freq, 1.5763);
  a *= 0.27488 + Wh*(0.6315 + 0.525 / pow(1.0+0.0157*freq, 20.0)) - 0.065683*exp(-8.7513*Wh);
  a *= 0.33622 * (1.0 - exp(-0.03442*er));
  double er_freq = er - (er - er_eff) / (1.0 + a);

  // characteristic impedance
  a = -0.03891 * pow(er, 1.4);
  b = -0.267 * pow (Wh, 7.0);
  double R7 = 1.206 - 0.3144*exp(a) * (1.0 - exp(b));

  a = 0.016 + pow(0.0514*er, 4.524);
  b = pow(freq/28.843, 12.0);
  a = 5.086 * a * b / (0.3838 + 0.386*a) / (1.0 + 1.2992*b);
  b = -22.2 * pow(Wh, 1.92);
  a *= exp(b);
  b = pow(er - 1.0, 6.0);
  double R9 = a*b / (1.0 + 10.0*b);

  a = 4.766 * exp(-3.228 * pow(Wh, 0.641));  // = R3
  a = 1.0 + 1.275
    * (1.0 - exp(-0.004625*a*pow(er, 1.674) * pow(freq/18.365, 2.745))); // = R8

  b = 0.9408 * pow(er_freq, a) - 0.9603;  // = R13
  b /= (0.9408 - R9) * pow(er_eff, a) - 0.9603;
  R9 = b; // = R13 / R14

  a = 0.00044 * pow(er, 2.136) + 0.0184;  // = R10
  a *= 0.707 * pow(freq/12.3, 1.097); // = R15
  a = exp(-0.026*pow(freq, 1.15656) - a);
  b = pow(freq/19.47, 6.0);
  b /= 1.0 + 0.0962 * b;  // = R11
  b = 1.0 + 0.0503 *er*er* b * (1.0 - exp(-pow(Wh/15, 6.0))); // = R16
  R7 *= (1.0 - 1.1241 * a / b / (1.0 + 0.00245*Wh*Wh));  // = R17

  zl *= pow(R9, R7);
  er_eff = er_freq;
}

// -------------------------------------------------------------------
// Calculates the width 'width' and the relative effective permittivity 'er_eff'
// of a microstrip line. It uses an iterative search algorithm because
// synthesis equations doesn't exist.
void getMicrostrip(double Z0, double freq, tSubstrate *substrate,
                              double &width, double &er_eff)
{
  int iteration = 0;  // iteration counter
  double Z0_current, Z0_result, increment;

  width = 1e-3;  // start with 1mm

  do {
    // compute line parameters
    calcMicrostrip(substrate, width, freq, er_eff, Z0_current);

    if(fabs(Z0 - Z0_current) < MAX_ERROR)
      break;  // wanted value was found

    increment = width / 100.0;
    width += increment;

    // compute line parameters
    calcMicrostrip(substrate, width, freq, er_eff, Z0_result);

    // Newton iteration: w(n+1) = w(n) - f(w(n))/f'(w(n))
    //   with f(w(n))  = Z0_current - Z0
    //   and  f'(w(n)) = (Z0_result - Z0_current) / increment
    width -= (Z0_current - Z0) / (Z0_result - Z0_current) * increment;
    if(width <= 0.0)
      width = increment;

    iteration++;
  } while(iteration < 150);
}
