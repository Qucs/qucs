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
#include "../../qucs-filter/material_props.h"


MatchDialog::MatchDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(tr("Create Matching Circuit"));
    DoubleVal = new QDoubleValidator(this);

    all = new QHBoxLayout(this);

    /* The main frame was divided into two vertical layouts. The first one, on the left side, is much the
  old matching tool whereas the other layout was included specifically for microstrip synthesis.
 */
    QVBoxLayout* matchFrame = new QVBoxLayout(); // Old window
    QVBoxLayout* micro_layout = new QVBoxLayout(); // Microstrip synthesis
    all->addLayout(matchFrame);
    all->addLayout(micro_layout);

    QGroupBox *MethodBox = new QGroupBox(tr("Implementation"));
    matchFrame->addWidget(MethodBox);
    QVBoxLayout *MethodLayout = new QVBoxLayout();
    MethodBox->setLayout(MethodLayout);

    SubstrateBox = new QGroupBox(tr("Microstrip Substrate"));
    QVBoxLayout *hsubs = new QVBoxLayout();

    SubstrateBox->setVisible(false);
    micro_layout->addWidget(SubstrateBox);
    SubstrateBox->setLayout(hsubs);
    QHBoxLayout * Erlayout =  new QHBoxLayout();
    RelPermLabel = new QLabel(tr("Relative Permitivity"));
    RelPermCombo = new QComboBox();
    RelPermCombo->setEditable(true);
    const char **p = List_er;
    while(*(++p)) RelPermCombo->addItem(*p);  // The dielectric coeff combobox is filled with the materials taken from "../../qucs-filter/material_props.h"
    RelPermCombo->lineEdit()->setText("9.8");
    Erlayout->addWidget(RelPermLabel);
    Erlayout->addWidget(RelPermCombo);
    hsubs->addLayout(Erlayout);

    QHBoxLayout *SubsLayout = new QHBoxLayout();
    subsHLabel = new QLabel(tr("Substrate height"));
    SubHeightEdit = new QLineEdit("1.0");
    SubHeightEdit->setFixedWidth(60);
    SubsHScale = new QLabel("mm");
    SubsLayout->addWidget(subsHLabel);
    SubsLayout->addWidget(SubHeightEdit);
    SubsLayout->addWidget(SubsHScale);
    hsubs->addLayout(SubsLayout);

    //Thickness
    QHBoxLayout *ThicknessLayout = new QHBoxLayout();
    thicknessLabel = new QLabel(tr("Metal thickness"));
    thicknessEdit = new QLineEdit("12.5");
    ThicknessScale = new QLabel("um");
    ThicknessLayout->addWidget(thicknessLabel);
    ThicknessLayout->addWidget(thicknessEdit);
    ThicknessLayout->addWidget(ThicknessScale);
    hsubs->addLayout(ThicknessLayout);

    //Minimum width
    QHBoxLayout *minWLayout = new QHBoxLayout();
    minWLabel = new QLabel(tr("Minimum width"));
    minWEdit = new QLineEdit("0.4");
    minWScale = new QLabel("mm");
    minWLayout->addWidget(minWLabel);
    minWLayout->addWidget(minWEdit);
    minWLayout->addWidget(minWScale);
    hsubs->addLayout(minWLayout);

    //Maximum width
    QHBoxLayout *maxWLayout = new QHBoxLayout();
    maxWLabel = new QLabel(tr("Maximum width"));
    maxWEdit = new QLineEdit("5.0");
    maxWScale = new QLabel("mm");
    maxWLayout->addWidget(maxWLabel);
    maxWLayout->addWidget(maxWEdit);
    maxWLayout->addWidget(maxWScale);
    hsubs->addLayout(maxWLayout);

    // tan(delta)
    QHBoxLayout *tanDLayout = new QHBoxLayout();
    tanDLabel = new QLabel(tr("tanD"));
    tanDEdit = new QLineEdit("0.0125");
    tanDLayout->addWidget(tanDLabel);
    tanDLayout->addWidget(tanDEdit);
    hsubs->addLayout(tanDLayout);

    // Resistivity
    QHBoxLayout *ResistivityLayout = new QHBoxLayout();
    ResistivityLabel = new QLabel(tr("Resistivity"));
    ResistivityEdit = new QLineEdit("2.43902e-08");
    ResistivityLayout->addWidget(ResistivityLabel);
    ResistivityLayout->addWidget(ResistivityEdit);
    hsubs->addLayout(ResistivityLayout);

    hsubs->insertStretch(-1, 1); // Remove spacing between layouts.

    QHBoxLayout *h4 = new QHBoxLayout();
    h4->setSpacing(3);
    TopoLabel = new QLabel(tr("Method"));
    h4->addWidget(TopoLabel);

    // Matching network topology
    TopoCombo = new QComboBox();
    TopoCombo->addItem(tr("LC matching"));
    TopoCombo->addItem(tr("Single stub"));
    TopoCombo->addItem(tr("Double stub"));
    QString str = tr("Multistage ") + QString(QChar(0xBB, 0x03)) + "/4";
    TopoCombo->addItem((str));
    TopoCombo->addItem("Cascaded LC sections");

    h4->addWidget(TopoCombo);
    connect(TopoCombo, SIGNAL(activated(int)), SLOT(slotChangeMode_TopoCombo()));
    h4->addStretch(5);
    MethodLayout->addLayout(h4);

    // When the stub implementation is selected, it is possible to select either the open or
    // the short circuit solution
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


    // Number of sections of the cascaded lambda/4 implementation
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

    // Weighting: Binomial or Chebyshev
    QHBoxLayout *h7 = new QHBoxLayout();
    WeightingLabel = new QLabel(tr("Weighting"));
    h7->addWidget(WeightingLabel);
    WeightingLabel->setVisible(false);
    BinRadio = new QRadioButton(tr("Binomial"), this);
    BinRadio->setVisible(false);
    h7->addWidget(BinRadio);
    ChebyRadio = new QRadioButton(tr("Chebyshev"), this);
    ChebyRadio->setVisible(false);
    h7->addWidget(ChebyRadio);
    MethodLayout->addLayout(h7);
    BinRadio->setChecked(true);
    ChebyRadio->setChecked(false);

    //Maximum ripple
    QHBoxLayout *h8 = new QHBoxLayout();
    maxRippleLabel = new QLabel(tr("Maximum ripple"));
    maxRippleLabel->setVisible(false);
    h8->addWidget(maxRippleLabel);
    MaxRippleEdit = new QLineEdit("0.05");
    MaxRippleEdit->setVisible(false);
    h8->addWidget(MaxRippleEdit);
    MaxRippleEdit->setFixedWidth(50);
    MaxRippleEdit->setAlignment(Qt::AlignLeft);
    h8->setAlignment(Qt::AlignLeft);
    MethodLayout->addLayout(h8);

    BalancedCheck = new QCheckBox(tr("Use balanced stubs"));
    BalancedCheck->setVisible(false);
    TwoCheck = new QCheckBox(tr("Calculate two-port matching"));
    TwoCheck->setChecked(true);
    AddSPBlock = new QCheckBox(tr("Add S-Parameter simulation"));// The user can choose whether add a S-param simulation or not
    AddSPBlock->setChecked(false);
    MicrostripCheck = new QCheckBox(tr("Synthesize microstrip lines"));
    MicrostripCheck->setVisible(false);
    MicrostripCheck->setChecked(false);

    matchFrame->addWidget(BalancedCheck);
    matchFrame->addWidget(TwoCheck);
    matchFrame->addWidget(AddSPBlock);
    matchFrame->addWidget(MicrostripCheck);


    connect(TwoCheck, SIGNAL(toggled(bool)), SLOT(slotSetTwoPort(bool)));
    connect(MicrostripCheck, SIGNAL(toggled(bool)), SLOT(slotSetMicrostripCheck()));
    connect(ChebyRadio, SIGNAL(toggled(bool)), SLOT(slotChebyCheck()));


    // ...........................................................
    QGroupBox *ImpBox = new QGroupBox(tr("Reference Impedance"));
    matchFrame->addWidget(ImpBox);
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
    matchFrame->addWidget(SParBox);
    QVBoxLayout *SParLayout = new QVBoxLayout();
    SParBox->setLayout(SParLayout);

    QHBoxLayout *h1 = new QHBoxLayout();
    h1->setSpacing(3);
    FormatLabel = new QLabel(tr("Input format"));
    h1->addWidget(FormatLabel);
    FormatCombo = new QComboBox();
    h1->addWidget(FormatCombo);
    FormatCombo->addItem(tr("real/imag"));
    FormatCombo->addItem(tr("mag/deg"));
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
    UnitCombo->addItem("Hz");
    UnitCombo->addItem("kHz");
    UnitCombo->addItem("MHz");
    UnitCombo->addItem("GHz");
    h2->addWidget(UnitCombo);
    h2->addStretch(5);
    SParLayout->addLayout(h2);

    // ...........................................................
    QHBoxLayout *h0 = new QHBoxLayout();
    h0->setSpacing(5);
    matchFrame->addLayout(h0);
    h0->addStretch(5);
    QPushButton *buttCreate = new QPushButton(tr("Create"));
    QPushButton *buttCancel = new QPushButton(tr("Cancel"));
    h0->addWidget(buttCreate);
    h0->addWidget(buttCancel);
    connect(buttCreate, SIGNAL(clicked()), SLOT(slotButtCreate()));
    connect(buttCancel, SIGNAL(clicked()), SLOT(reject()));

    slotReflexionChanged("");  // calculate impedance
    setFrequency(1e9);  // set 1GHz
    resize(500, 100);
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
    UnitCombo->setCurrentIndex(Expo);
    Freq_ /= pow(10.0, double(3*Expo));
    FrequencyEdit->setText(QString::number(Freq_));
}

//------------------------------------------------------
// This function sets the visibility of the microstrip synthesis panel. The substrate properties
// are visible when the microstrip implementation is selected.
void MatchDialog::slotSetMicrostripCheck()
{
    if (MicrostripCheck->isChecked())
    {
        SubstrateBox->setVisible(true);
        resize(650, 100);
    }
    else
    {
        SubstrateBox->setVisible(false);
        setMaximumSize(500, 100);
    }
}

//---------------------------------------------------------
// When the Chebyshev weighting is selected, the window show the MaxRipple textbox and label
// so as to let the user to enter this parameter. It makes no sense to show it otherwise since
// when using the binomial weighting, the maximum ripple is given by the number of sections
void MatchDialog::slotChebyCheck()
{
    if (ChebyRadio->isChecked())
    {
        MaxRippleEdit->setVisible(true);
        maxRippleLabel->setVisible(true);
    }
    else
    {
        MaxRippleEdit->setVisible(false);
        maxRippleLabel->setVisible(false);
    }
}

// -----------------------------------------------------------------------
// It is called when the checkbox for two-port matching changes.
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
// This function is called when a new topology is selected. According to the index selected, it makes visible (or invisible)
// certain window components.
void MatchDialog::slotChangeMode_TopoCombo()
{
    if ((TopoCombo->currentIndex() == 1)||(TopoCombo->currentIndex() == 2))//Single/Double stub selected
    {
        ShortRadioButton->setVisible(true);
        OpenRadioButton->setVisible(true);
        OpenRadioButton->setChecked(true);
        BalancedCheck->setVisible(true);
    }
    else
    {
        ShortRadioButton->setVisible(false);
        OpenRadioButton->setVisible(false);
        BalancedCheck->setVisible(false);
    }
    if ((TopoCombo->currentIndex() != 0) && (TopoCombo->currentIndex() != 4))//Obviously, the microstrip implementation panel cannot be used when LC is selected
    {
        MicrostripCheck->setVisible(true);
    }
    else
    {
        MicrostripCheck->setVisible(false);
    }

    if (TopoCombo->currentIndex() == 3)//Cascaded lambda/4 sections selected
    {
        OrderLabel->setVisible(true);
        OrderEdit->setVisible(true);
        BinRadio->setVisible(true);
        ChebyRadio->setVisible(true);
        WeightingLabel->setVisible(true);
    }
    if (TopoCombo->currentIndex() == 4)//Cascaded LC sections selected
    {
        OrderLabel->setVisible(true);
        OrderEdit->setVisible(true);
    }
    else
    {// Then it makes no sense to show these items
        OrderLabel->setVisible(false);
        OrderEdit->setVisible(false);
        BinRadio->setVisible(false);
        ChebyRadio->setVisible(false);
        WeightingLabel->setVisible(false);
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
    double Z1   = Ref1Edit->text().toDouble();//Port 1 impedance
    double Z2   = Ref2Edit->text().toDouble();//Port 2 impedance
    double Freq = FrequencyEdit->text().toDouble()*pow(10.0, 3.0*UnitCombo->currentIndex());

    // S matrix
    double S11real = S11magEdit->text().toDouble();
    double S11imag = S11degEdit->text().toDouble();
    double S12real = S12magEdit->text().toDouble();
    double S12imag = S12degEdit->text().toDouble();
    double S21real = S21magEdit->text().toDouble();
    double S21imag = S21degEdit->text().toDouble();
    double S22real = S22magEdit->text().toDouble();
    double S22imag = S22degEdit->text().toDouble();
    if(FormatCombo->currentIndex()) {  // are they polar ?
        p2c(S11real, S11imag);
        p2c(S12real, S12imag);
        p2c(S21real, S21imag);
        p2c(S22real, S22imag);
    }

    bool BalancedStubs = BalancedCheck->isChecked();
    bool micro_syn = MicrostripCheck->isChecked();//Microstrip implementation?
    bool SP_block = AddSPBlock->isChecked();//Add S-parameter block?
    bool open_short = OpenRadioButton->isChecked();//Open stub or short circuit stub configuration
    int order = OrderEdit->text().toInt()+1;//Order of the multisection lambda/4 matching

    double gamma_MAX = MaxRippleEdit->text().toDouble();//Maximum ripple (Chebyshev weighting only)

    tSubstrate Substrate;
    if (micro_syn)//In case microstrip implementation is selected. This reads the substrate properties given by the user
    {
        Substrate.er = RelPermCombo->currentText().section("  ", 0, 0).toDouble();
        Substrate.height = SubHeightEdit->text().toDouble() / 1e3;
        Substrate.thickness = thicknessEdit->text().toDouble() / 1e6;
        Substrate.tand = tanDEdit->text().toDouble();
        Substrate.resistivity = ResistivityEdit->text().toDouble();
        Substrate.roughness = 0.0;
        Substrate.minWidth = minWEdit->text().toDouble() / 1e3;
        Substrate.maxWidth = maxWEdit->text().toDouble() / 1e3;
    }

    if(TwoCheck->isChecked()) {  // two-port matching ?
        // determinant of S-parameter matrix
        double DetReal = S11real*S22real - S11imag*S22imag
                - S12real*S21real + S12imag*S21imag;
        double DetImag = S11real*S22imag + S11imag*S22real
                - S12real*S21imag - S12imag*S21real;
        calc2PortMatch(S11real, S11imag, S22real, S22imag, DetReal, DetImag, Z1, Z2, Freq, micro_syn, SP_block, open_short, Substrate, order, gamma_MAX, BalancedStubs);
    }
    else{
        calcMatchingCircuit(S11real, S11imag, Z1, Freq, micro_syn, SP_block, open_short, Substrate, order, gamma_MAX, BalancedStubs);
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
// This function calculates a LC matching section
QString MatchDialog::calcMatchingLC(double r_real, double r_imag, double Z0, double Freq)
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

    // Circuit topology and values
    QString laddercode;
    if ((Str == "ps") && (X2 < 0.0))   laddercode = QString("CS%2;LP%1;").arg(-1.0 / Omega / X2).arg(-1.0 / Omega / X1);
    if ((Str == "ps") && (X2 >= 0.0)) laddercode = QString("LS%2;CP%1;").arg(X2 / Omega).arg(X1 / Omega);
    if ((Str == "sp") && (X2 <0.0)) laddercode = QString("CP%2;LS%1;").arg(X1 / Omega).arg(-1.0 / Omega / X2);
    if ((Str == "sp") && (X2 >=0.0)) laddercode = QString("LP%2;CS%1;").arg(-1.0 / Omega / X1).arg(X2 / Omega);
    return laddercode;
}


// ----------------------------------------------------------------------------------
// This function calculates a matching network for a single port using a transmission line and
// a stub (short or open). It can also convert the transmission lines microstrip lines
// See Microwave Engineering. David Pozar. John Wiley and Sons. 4th Edition. Pg 234-240
bool MatchDialog::calcMatchingCircuitSingleStub(double r_real, double r_imag, double Z0, double Freq, bool microsyn, bool SP_Block, bool open_short, tSubstrate Substrate, bool BalancedStubs)
{
    QString laddercode = calcSingleStub(r_real, r_imag, Z0, Freq, open_short, BalancedStubs);
    double RL=r_real, XL=r_imag;
    int schcode=-1;
    r2z(RL, XL, Z0);
    QString Schematic = "<Qucs Schematic " PACKAGE_VERSION ">\n";

    if (SP_Block) schcode = 3;

    int x_pos = 0;
    QString wirestr = "";
    QString componentstr = "";
    QString paintingstr = "";
    SchematicParser(laddercode, x_pos, componentstr, wirestr, paintingstr, schcode, Freq, Z0, RL, XL, Substrate, microsyn);

    //Add components
    Schematic += "<Components>\n";
    Schematic+=componentstr;
    Schematic += "</Components>\n";

    //Add wires
    Schematic+= "<Wires>\n";
    Schematic += wirestr;
    Schematic+= "</Wires>\n";

    //Add paintings
    Schematic += "<Paintings>\n";
    Schematic += paintingstr;
    Schematic += "</Paintings>\n";
    QApplication::clipboard()->setText(Schematic, QClipboard::Clipboard);
    return true;
}

// ----------------------------------------------------------------------------------
// This function calculates a matching network for a single port using a transmission line and
// two stubs (short or open). It can also convert the transmission lines microstrip lines
// See Microwave Engineering. David Pozar. John Wiley and Sons. 4th Edition. Pg 241-245
bool MatchDialog::calcMatchingCircuitDoubleStub(double r_real, double r_imag, double Z0, double Freq, bool microsyn, bool SP_Block, bool open_short, tSubstrate Substrate, bool BalancedStubs)
{
    QString laddercode = calcDoubleStub(r_real, r_imag, Z0, Freq, open_short, BalancedStubs);
    double RL=r_real, XL=r_imag;
    int schcode = -1;
    r2z(RL, XL, Z0);
    QString Schematic = "<Qucs Schematic " PACKAGE_VERSION ">\n";

    if (SP_Block) schcode = 3;

    int x_pos = 0;
    QString wirestr = "";
    QString componentstr = "";
    QString paintingstr = "";
    SchematicParser(laddercode, x_pos, componentstr, wirestr, paintingstr, schcode, Freq, Z0, RL, XL, Substrate, microsyn);

    //Add components
    Schematic += "<Components>\n";
    Schematic+=componentstr;
    Schematic += "</Components>\n";

    //Add wires
    Schematic+= "<Wires>\n";
    Schematic += wirestr;
    Schematic+= "</Wires>\n";

    //Add paintings
    Schematic += "<Paintings>\n";
    Schematic += paintingstr;
    Schematic += "</Paintings>\n";
    QApplication::clipboard()->setText(Schematic, QClipboard::Clipboard);
    return true;
}


// ----------------------------------------------------------------------------------
// This function calculates a matching network for a single port using cascaded sections
// of lambda/4 lines. It can also convert these lines microstrip lines
// See Microwave Engineering. David Pozar. John Wiley and Sons. 4th Edition. Pg 252-256
bool MatchDialog::calcMatchingCircuitCascadedLambda4(double r_real, double r_imag, double Z0, double Freq, bool microsyn, bool SP_Block, tSubstrate Substrate, double gamma, int order)
{
    QString laddercode;
    (BinRadio->isChecked()) ? laddercode = calcBinomialLines(r_real, r_imag, Z0, order, Freq) : laddercode = calcChebyLines(r_real, r_imag, Z0, gamma, order, Freq);
    double RL = r_real, XL=r_imag;
    int schcode = -1;
    r2z(RL, XL, Z0);

    QString Schematic = "<Qucs Schematic " PACKAGE_VERSION ">\n";

    if (SP_Block) schcode = 3;

    int x_pos = 0;
    QString wirestr = "";
    QString componentstr = "";
    QString paintingstr = "";
    SchematicParser(laddercode, x_pos, componentstr, wirestr, paintingstr, schcode, Freq, Z0, RL, XL, Substrate, microsyn);

    //Add components
    Schematic += "<Components>\n";
    Schematic+=componentstr;
    Schematic += "</Components>\n";

    //Add wires
    Schematic+= "<Wires>\n";
    Schematic += wirestr;
    Schematic+= "</Wires>\n";

    //Add paintings
    Schematic += "<Paintings>\n";
    Schematic += paintingstr;
    Schematic += "</Paintings>\n";

    QApplication::clipboard()->setText(Schematic, QClipboard::Clipboard);
    return true;
}


// -----------------------------------------------------------------------
// This function calls the specific matching network function so as to get the desired matching topology
bool MatchDialog::calcMatchingCircuit(double S11real, double S11imag,double Z0, double Freq, bool micro_syn, bool SP_block, bool open_short, tSubstrate Substrate, int order, double gamma_MAX, bool BalancedStubs)
{
    switch(TopoCombo->currentIndex())
    {
    case 0: // LC
        if(!calcMatchingCircuitLC(S11real, S11imag, Z0, Freq, SP_block))return false;
        break;
    case 1: // Single stub
        if(!calcMatchingCircuitSingleStub(S11real, S11imag, Z0, Freq, micro_syn, SP_block, open_short, Substrate, BalancedStubs))return false;
        break;
    case 2: // Double stub
        if(!calcMatchingCircuitDoubleStub(S11real, S11imag, Z0, Freq, micro_syn, SP_block, open_short, Substrate, BalancedStubs))return false;
        break;
    case 3: // Quarter wave cascaded sections
        if(!calcMatchingCircuitCascadedLambda4(S11real, S11imag, Z0, Freq, micro_syn, SP_block, Substrate, gamma_MAX, order))return false;
        break;
    case 4: //Cascaded LC sections
        if(!calcMatchingCircuitCascadedLCSections(S11real, S11imag, Z0, Freq, SP_block, order))return false;
        break;
    }
    return true;
}


// -----------------------------------------------------------------------
// This function calculates one port LC matching
bool MatchDialog::calcMatchingCircuitLC(double r_real, double r_imag, double Z0, double Freq, bool SP_Block)
{
    QString laddercode = calcMatchingLC(r_real, r_imag, Z0, Freq);//It calculates the LC matching circuit.
    double RL=r_real, XL=r_imag;
    int schcode=-1;
    r2z(RL, XL, Z0);

    //Header
    QString Schematic = "<Qucs Schematic " PACKAGE_VERSION ">\n";

    if (SP_Block) schcode = 3;

    int x_pos = 0;
    QString wirestr = "";
    QString componentstr = "";
    QString paintingstr = "";
    tSubstrate Subs;

    SchematicParser(laddercode, x_pos, componentstr, wirestr, paintingstr, schcode, Freq, Z0, RL, XL, Subs, false);

    //Add components
    Schematic += "<Components>\n";
    Schematic+=componentstr;
    Schematic += "</Components>\n";

    //Add wires
    Schematic+= "<Wires>\n";
    Schematic += wirestr;
    Schematic+= "</Wires>\n";

    //Add paintings
    Schematic += "<Paintings>\n";
    Schematic += paintingstr;
    Schematic += "</Paintings>\n";

    QApplication::clipboard()->setText(Schematic, QClipboard::Clipboard);
    return true;
}



//------------------------------------------------------------------------
//This function generates the Qucs schematic of a cascaded LC matching network
// Reference: Inder J. Bahl. "Fundamentals of RF and microwave transistor amplifiers". John Wiley and Sons. 2009. Pages 169 - 170
bool MatchDialog::calcMatchingCircuitCascadedLCSections(double r_real, double r_imag, double Z0, double Freq, bool SP_Block, int N)
{
    QString laddercode = calcMatchingCascadedLCSections(r_real, r_imag, Z0, Freq, N-1);
    double RL=r_real, XL=r_imag;
    int schcode = -1;
    r2z(RL, XL, Z0);

    //Header
    QString Schematic = "<Qucs Schematic " PACKAGE_VERSION ">\n";

    if (SP_Block) schcode = 3;

    int x_pos = 0;
    QString wirestr = "";
    QString componentstr = "";
    QString paintingstr = "";
    tSubstrate Subs;
    SchematicParser(laddercode, x_pos, componentstr, wirestr, paintingstr, schcode, Freq, Z0, RL, XL, Subs, false);

    //Add components
    Schematic += "<Components>\n";
    Schematic+=componentstr;
    Schematic += "</Components>\n";

    //Add wires
    Schematic+= "<Wires>\n";
    Schematic += wirestr;
    Schematic+= "</Wires>\n";

    //Add paintings
    Schematic += "<Paintings>\n";
    Schematic += paintingstr;
    Schematic += "</Paintings>\n";

    QApplication::clipboard()->setText(Schematic, QClipboard::Clipboard);
    return true;
}


// -----------------------------------------------------------------------
// Fundamental calculations for concurrent 2-port matching.
QString MatchDialog::calcBiMatch(double S11real, double S11imag,
                                 double S22real, double S22imag, double DetReal, double DetImag,
                                 double Z0, double Freq, bool open_short, double gamma_MAX, int order,
                                 bool BalancedStubs)
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

    QString laddercode;

    switch(TopoCombo->currentIndex()) //Matches both the input and the output port to external sources (typically, 50 Ohms)
    {
    case 0: // LC
        laddercode = calcMatchingLC(Rreal, -Rimag, Z0, Freq);
        break;
    case 1: // Single stub
        laddercode = calcSingleStub(Rreal, -Rimag, Z0, Freq, open_short, BalancedStubs);
        break;
    case 2: // Double stub
        laddercode = calcDoubleStub(Rreal,-Rimag, Z0, Freq, open_short, BalancedStubs);
        break;
    case 3: // Quarter wave cascaded sections
        (BinRadio->isChecked()) ? laddercode = calcBinomialLines(Rreal, -Rimag, Z0, order, Freq) : laddercode = calcChebyLines(Rreal, -Rimag, Z0, gamma_MAX, order, Freq);
        break;
    case 4: // Cascaded LC sections
        laddercode = calcMatchingCascadedLCSections(Rreal, -Rimag, Z0, Freq, order);
        break;
    }

    return laddercode;
}

// -----------------------------------------------------------------------
// This function calculates both the input and the output matching networks. In this sense,
// it calls 'calcBiMatch' function to get the input/output impedance. Then it synthesize the network
// according to the user choice
bool MatchDialog::calc2PortMatch(double S11real, double S11imag,
                                 double S22real, double S22imag, double DetReal, double DetImag,
                                 double Z1, double Z2, double Freq, bool microsyn, bool SP_Block, bool open_short,
                                 tSubstrate Substrate, int order, double gamma_MAX, bool BalancedStubs)
{
    QString InputNetworkComponents, OutputNetworkComponents;
    QString InputNewtworkWires, OutputNetworkWires;
    // Input port network
    // The result is a string which gives the structure of the matching network
    QString InputLadderCode = calcBiMatch(S11real, S11imag, S22real, S22imag,
                                DetReal, DetImag, Z1, Freq, open_short, gamma_MAX, order, BalancedStubs);
    if(InputLadderCode.isEmpty()) return false;

    // Output port network
    QString OutputLadderCode = calcBiMatch(S22real, S22imag, S11real, S11imag,
                                 DetReal, DetImag, Z2, Freq, open_short, gamma_MAX, order, BalancedStubs);
    if(OutputLadderCode.isEmpty()) return false;


    int schcode = -1;
    QString Schematic = "<Qucs Schematic " PACKAGE_VERSION ">\n";

    if (SP_Block) schcode = 0;
    int x_pos = 0;
    QString input_wirestr = "", output_wirestr = "";
    QString input_componentstr = "", output_componentstr = "";
    QString paintingstr = "";
    SchematicParser(InputLadderCode, x_pos, input_componentstr, input_wirestr, paintingstr, schcode, Freq, Z1, 0, 0, Substrate, microsyn);
    if (SP_Block) schcode = 2;
    x_pos += 200;
    SchematicParser(OutputLadderCode, x_pos, output_componentstr, output_wirestr, paintingstr, schcode, Freq, 0, Z2, 0, Substrate, microsyn);

    //Add components
    Schematic += "<Components>\n";
    Schematic +=input_componentstr;
    Schematic += QString("");
    Schematic+=output_componentstr;
    Schematic += "</Components>\n";

    //Add wires
    Schematic+= "<Wires>\n";
    Schematic += input_wirestr;
    Schematic += output_wirestr;
    Schematic+= "</Wires>\n";
    QApplication::clipboard()->setText(Schematic, QClipboard::Clipboard);
    return true;
}


// FUNCTIONS FOR THE MICROSTRIP LINE SYNTHESIS. JUST COPIED FROM THE QUCS-FILTER TOOL
/////////////////////////////////////////////////////////////////////////////////////////////////
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
void MatchDialog::getMicrostrip(double Z0, double freq, tSubstrate *substrate,
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
////////////////////////////////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------------
// Calculates a matching network according to the stub+line method
// See Microwave Engineering. David Pozar. John Wiley and Sons. 4th Edition. Pg 234-241
QString MatchDialog::calcSingleStub(double r_real, double r_imag, double Z0, double Freq, bool open_short, bool BalancedStubs)
{
    double t=0, t1 = 0, t2 = 0;
    double dl, dl1, dl2, B;
    double B1, B2, d, lstub, ll;
    double lambda = SPEED_OF_LIGHT/(Freq);
    double RL = r_real, XL = r_imag;
    r2z(RL, XL, Z0);


    // Stub+line method formulas
    if (RL == Z0)
    {
        t = -XL / (2*Z0);
        (t<0) ? dl = (pi + atan(t))/ (2*pi): dl = (atan(t))/ (2*pi);
        B = (RL*RL * t - (Z0-XL*t)*(Z0*t + XL))/(Z0*(RL*RL + (Z0*t + XL)*(Z0*t + XL)));
    }
    else
    {
        t1 = (XL + sqrt(((RL/Z0)*fabs((Z0 - RL)*(Z0 - RL) + XL*XL))))/(RL - Z0);
        (t1 < 0) ? dl1 = (pi + atan(t1))/ (2*pi) : dl1 = (atan(t1))/ (2*pi);
        B1 = (RL*RL * t1 - (Z0-XL*t1)*(Z0*t1 + XL))/(Z0*(RL*RL + (Z0*t1 + XL)*(Z0*t1 + XL)));

        t2  = (XL - sqrt((RL*fabs((Z0 - RL)*(Z0 - RL) + XL*XL))/(Z0)))/(RL - Z0);
        (t2 < 0) ? dl2 = (pi + atan(t2))/ (2*pi) : dl2 = (atan(t2))/ (2*pi);
        B2 = (RL*RL * t2 - (Z0-XL*t2)*(Z0*t2 + XL))/(Z0*(RL*RL + (Z0*t2 + XL)*(Z0*t2 + XL)));
    }

    if (t!=0)
    {
        d=dl*lambda;
        (open_short) ? ll = -(atan(B*Z0))/(2*pi) :ll= (atan(1./(B*Z0)))/(2*pi);
        if ((open_short)&&(ll<0))ll+=0.5;
        if ((!open_short)&&(ll>0.5))ll-=0.5;
        lstub = ll*lambda;
    }

    if (t1 != 0)
    {
        d=dl1*lambda;
        (open_short) ? ll = -(atan(B1*Z0))/(2*pi) : ll= (atan(1./(1.*B1*Z0)))/(2*pi);
        if ((open_short)&&(ll<0))ll+=0.5;
        if ((!open_short)&&(ll>0.5))ll-=0.5;
        lstub = ll*lambda;

    }else{
        if (t2 != 0)
        {
            d=dl2*lambda;
            (open_short) ? ll = -(atan(B2*Z0))/(2*pi) : ll= (atan(1./(1.*B2*Z0)))/(2*pi);
            if ((open_short)&&(ll<0))ll+=0.5;
            if ((!open_short)&&(ll>0.5))ll-=0.5;
            lstub = ll*lambda;
        }
    }

    if (BalancedStubs)
    {   //Balanced stub implementation
        double K;
        (open_short) ? K=0.5 : K=2;
        lstub = (lambda/(2*pi))*atan(K*tan((2*pi*lstub)/lambda));
        if (lstub < 0) lstub+=0.5*lambda;
    }

    //String code
    QString laddercode;
    if ((open_short) && (!BalancedStubs)) laddercode = QString("OL%1#%2;TL%1#%3;").arg(Z0).arg(lstub).arg(d);//Line + Open stub
    if ((open_short) && (BalancedStubs)) laddercode = QString("OU%1#%2;OL%1#%2;TL%1#%3;").arg(Z0).arg(lstub).arg(d);//Open circuit balanced stubs
    if ((!open_short) && (!BalancedStubs)) laddercode = QString("SL%1#%2;TL%1#%3;").arg(Z0).arg(lstub).arg(d);//Line + Short circuited stub
    if ((!open_short) && (BalancedStubs)) laddercode = QString("SU%1#%2;SL%1#%2;TL%1#%3;").arg(Z0).arg(lstub).arg(d);//Short circuited balanced stubs


    return laddercode;
}


//--------------------------------------------------------------------------------
// Calculates a matching network according to the stub+line+stub method
// See Microwave Engineering. David Pozar. John Wiley and Sons. 4th Edition. Pg 241-245
QString MatchDialog::calcDoubleStub(double r_real, double r_imag, double Z0, double Freq, bool open_short, bool BalancedStubs)
{
    double RL=r_real, XL=r_imag;
    r2z(RL, XL, Z0);
    double Y0=1./Z0;
    double GL = (1/((RL*RL)+(XL*XL)))*RL;
    double BL = -(1/((RL*RL)+(XL*XL)))*XL;
    double lambda = SPEED_OF_LIGHT/Freq;
    double beta=(2*pi)/lambda;
    double d = lambda/8;
    double t = tan(beta*d);
    double ll1, ll2;


    // Double stub method formulas
    if (GL > Y0*((1+t*t)/(2*t*t)))//Not every load can be match using the double stub technique.
    {
        QString str = QString("It is not possible to match this load using the double stub method");
        QMessageBox::warning(0, QObject::trUtf8("Error"),
                             QObject::trUtf8(str.toUtf8()));
        return QString("");
    }

    // Stubs susceptance
    double B11 = -BL + (Y0 + sqrt((1+t*t)*GL*Y0 - GL*GL*t*t))/(t); // 1st solution
    // double B12 = -BL + (Y0 - sqrt((1+t*t)*GL*Y0 - GL*GL*t*t))/(t); // 2nd solution

    double B21 = ((Y0*sqrt((1+t*t)*GL*Y0 - GL*GL*t*t)) + GL*Y0)/(GL*t); // 1st solution
    //double B22 = ((-Y0*sqrt((1+t*t)*GL*Y0 - GL*GL*t*t)) + GL*Y0)/(GL*t);// 2nd solution

    // Open circuit solution
    (open_short) ? ll1 = (atan(B11*Z0))/(2*pi) : ll1= -(atan(1./(1.*B11*Z0)))/(2*pi);
    (open_short) ? ll2 = (atan(B21*Z0))/(2*pi) : ll2= -(atan(1./(1.*B21*Z0)))/(2*pi);



    if (ll1<0)ll1+=0.5;
    if (ll2<0)ll2+=0.5;
    if ((!open_short)&&(ll1>0.5))ll1-=0.5;
    if ((!open_short)&&(ll2>0.5))ll2-=0.5;


    double lstub1 = ll1*lambda, lstub2=ll2*lambda;

    if (BalancedStubs)
    {   //Balanced stub implementation
        double K;
        (open_short) ? K=0.5 : K=2;
        lstub1 = (lambda/(2*pi))*atan(K*tan((2*pi*lstub1)/lambda));
        lstub2 = (lambda/(2*pi))*atan(K*tan((2*pi*lstub2)/lambda));
        if (lstub1 < 0) lstub1+=0.5*lambda;
        if (lstub2 < 0) lstub2+=0.5*lambda;
    }

    QString laddercode;
    if ((open_short)&&(BalancedStubs))laddercode = QString("OU%1#%2;OL%1#%2;TL%1#%3;OL%1#%4;OU%1#%4;").arg(Z0).arg(lstub2).arg(d).arg(lstub1);
    if ((open_short)&&(!BalancedStubs))laddercode = QString("OL%1#%2;TL%1#%3;OL%1#%4").arg(Z0).arg(lstub2).arg(d).arg(lstub1);
    if ((!open_short)&&(BalancedStubs))laddercode = QString("SU%1#%2;SL%1#%2;TL%1#%3;SL%1#%4;SU%1#%4;").arg(Z0).arg(lstub2).arg(d).arg(lstub1);
    if ((!open_short)&&(!BalancedStubs))laddercode = QString("SL%1#%2;TL%1#%3;SL%1#%4").arg(Z0).arg(lstub2).arg(d).arg(lstub1);


    return laddercode;
}


// -----------------------------------------------------------------------------------------------
// Binomial coefficients using the multiplicative formula (more efficient than the recursive one)
int BinomialCoeffs(int n, int k)
{
    double coeff = 1;
    for (int i=1; i<=k; i++)
    {
        coeff*=(n+(1-i))/(1.0*i);
    }
    return (int) coeff;
}

//-----------------------------------------------------------------------------------
// This function calculates a multistage lambda/4 matching using binomial weigthing
// See Microwave Engineering. David Pozar. John Wiley and Sons. 4th Edition. Pg 252-256
QString MatchDialog::calcBinomialLines(double r_real, double r_imag, double Z0, int order, double Freq)
{
    double RL = r_real, XL = r_imag;
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
    double l4 = SPEED_OF_LIGHT/(4.*Freq);
    double Ci, Zi, Zaux=Z0;
    QString laddercode;
    for (int i = 1; i <order; i++)
    {
        Ci = BinomialCoeffs(order-1, i-1);
        Zi = exp(log(Zaux) + (Ci/pow(2,order-1))*log(RL/Z0));
        Zaux=Zi;
        laddercode+=QString("TL%1#%2;").arg(Zi).arg(l4);
    }
    return laddercode;
}

//-----------------------------------------------------------------------------------
// This function calculates a multistage lambda/4 matching using the Chebyshev weigthing.
// See Microwave Engineering. David Pozar. John Wiley and Sons. 4th Edition. Pg 256-261
QString MatchDialog::calcChebyLines(double r_real, double r_imag, double Z0, double gamma, int order, double Freq)
{
    int N = order-1;// Number of sections
    if (N > 7)// So far, it is only available Chebyshev weighting up to 7 sections.
        // Probably, it makes no sense to use a higher number of sections because of the losses
    {
        QMessageBox::warning(0, QObject::tr("Error"),
                             QObject::tr("Chebyshev weighting for N>7 is not available"));
        return QString("");
    }
    QString laddercode;
    double RL = r_real, XL = r_imag;
    r2z(RL, XL, Z0);// Calculation of the load impedance given the reflection coeffient
    double sec_theta_m;// = cosh((1/(1.*N))*acosh((1/gamma)*fabs((RL-Z0)/(Z0+RL))) );
    //double sec_theta_m = cosh((1/(1.*N))*acosh(fabs(log(RL/Z0)/(2*gamma))) );
    (fabs(log(RL/Z0)/(2*gamma)) < 1) ? sec_theta_m = 0 : sec_theta_m = cosh((1/(1.*N))*acosh(fabs(log(RL/Z0)/(2*gamma))) );

    double w[N];

    switch(N)//The weights are calculated by equating the reflection coeffient formula to the N-th Chebyshev polinomial
    {
    case 1:
        w[0] = sec_theta_m;
        break;
    case 2:
        w[0] = sec_theta_m*sec_theta_m;
        w[1] = 2*(sec_theta_m*sec_theta_m-1);
        break;
    case 3:
        w[0] = pow(sec_theta_m,3);
        w[1] = 3*(pow(sec_theta_m, 3) - sec_theta_m);
        w[2] = w[1];
        break;
    case 4:
        w[0] = pow(sec_theta_m, 4);
        w[1] = 4*sec_theta_m*sec_theta_m*(sec_theta_m*sec_theta_m-1);
        w[2] =2*( 1-4*sec_theta_m*sec_theta_m+3*pow(sec_theta_m, 4));
        w[3]= w[1];
        break;
    case 5:
        w[0] = pow(sec_theta_m, 5);
        w[1] = 5*(pow(sec_theta_m, 5) - pow(sec_theta_m, 3));
        w[2] = 10*pow(sec_theta_m, 5) - 15*pow(sec_theta_m, 3) + 5*sec_theta_m;
        w[3] = w[2];
        w[4] = w[1];
        break;
    case 6:
        w[0] = pow(sec_theta_m, 6);
        w[1] = 6*pow(sec_theta_m,4)*(sec_theta_m*sec_theta_m - 1);
        w[2] = 15*pow(sec_theta_m, 6) - 24*pow(sec_theta_m, 4) + 9*sec_theta_m*sec_theta_m;
        w[3] = 2*(10*pow(sec_theta_m, 6) - 18*pow(sec_theta_m, 4) + 9*sec_theta_m*sec_theta_m - 1);
        w[4] = w[2];
        w[5] = w[1];
        break;
    case 7:
        w[0] = pow(sec_theta_m, 7);
        w[1] = 7*pow(sec_theta_m, 5)*(sec_theta_m*sec_theta_m -1);
        w[2] = 21*pow(sec_theta_m, 7) - 35*pow(sec_theta_m, 5) + 14*pow(sec_theta_m, 3);
        w[3] = 35*pow(sec_theta_m, 7) - 70*pow(sec_theta_m, 5) + 42*pow(sec_theta_m, 3) -7*sec_theta_m ;
        w[4] = w[3];
        w[5] = w[2];
        w[6] = w[1];
        break;
    }
    double l4 = SPEED_OF_LIGHT/(4.*Freq);
    double Zaux=Z0, Zi;
    for (int i = 0; i < N; i++)
    {
        (RL<Z0) ? Zi = exp(log(Zaux) - gamma*w[i]):Zi = exp(log(Zaux) + gamma*w[i]); // When RL<Z0, Z_{i}<Z_{i-1}
        Zaux=Zi;
        laddercode+=QString("TL%1#%2;").arg(Zi).arg(l4);
    }
    return laddercode;
}


//--------------------------------------------------------------------------
//It calculates a cascaded LC matching network (only real impedances)
QString MatchDialog::calcMatchingCascadedLCSections(double r_real, double r_imag, double Z0, double Freq, int N)
{
    double RL = r_real, XL = r_imag, RS = Z0;
    double w = 2*pi*Freq, Q, C, L;
    r2z(RL, XL, Z0);
    double Raux, R, R1, R2;
    QString s="";

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


    if (RL > RS) R1 = RL, R2 = RS;
    else R1 = RS, R2 = RL;

    Raux = R1;
    for (int i = 0; i<N-1;i++)
    {
         R = pow(R1, 1.*(N-(i+1))/N)*pow(R2,1.*(i+1)/N);
         Q = sqrt(Raux/R - 1);
         C = Q/(w*Raux);
         L = Q*R/w;
         s += QString("CP%1;LS%2;").arg(C).arg(L);
         Raux = R;
    }

   Q = sqrt(R/R2 - 1);
   C = Q/(w*R);
   L = Q*R2/w;
   s += QString("CP%1;LS%2;").arg(C).arg(L);

   if (RL > RS)//Flip string
   {
     QString temp ="";
     QStringList strlist = s.split(";");
     for (int i = strlist.count()-1; i >= 0  ; i--) temp += strlist.at(i) + ";";
     s = temp;
   }

   return s;
}

// Given a string code of inductors, capacitors and transmission lines, it generates the Qucs network. Notice that the schematic is split into
// three part: components, wires and paintings, all of them are passed by reference.
int MatchDialog::SchematicParser(QString laddercode, int & x_pos, QString & componentstr, QString & wirestr, QString & paintingstr, int schcode, double Freq, double Z0, double RL, double XL, tSubstrate Substrate, bool microsyn)
{
   QStringList strlist = laddercode.split(";");
   QString component;
   double value, value2, er, width;
   int x_series = 120, x_shunt = 20;//x-axis spacing depending on whether the component is placed in a series or shunt configuration
   //Clear input strings (just in case)
   componentstr = "";
   wirestr = "";
   paintingstr = "";

   // Schematic code
   // 0: Port 1 + S parameter simulation
   // 1: Port 1
   // 2: Port 2
   // 3: Port 1, Port 2 and S parameter simulation

   if ((schcode == 0) || (schcode == 3))//Add port 1 + S parameter simulation
   {
       componentstr += QString("<Pac P1 1 %2 -30 18 -26 0 1 \"1\" 1 \"%1 Ohm\" 1 \"0 dBm\" 0 \"1 GHz\" 0>\n").arg(Z0).arg(x_pos);
       componentstr += QString("<GND * 1 %1 0 0 0 0 0>\n").arg(x_pos);
       wirestr += QString("<%1 -60 %1 -120>\n").arg(x_pos);
       wirestr += QString("<%1 -120 %2 -120>\n").arg(x_pos).arg(x_pos+60);
       x_pos +=60;
       //Add the frequency range for the S-param simulation
       double freq_start = std::max(0., Freq-1e9);
       double freq_stop = Freq+1e9;
       componentstr += QString("<.SP SP1 1 0 100 0 67 0 0 \"lin\" 1 \"%2Hz\" 1 \"%3Hz\" 1 \"300\" 1 \"no\" 0 \"1\" 0 \"2\" 0>\n").arg((freq_start)).arg((freq_stop));
       componentstr += QString("<Eqn Eqn1 1 200 100 -28 15 0 0 \"S11_dB=dB(S[1,1])\" 1 \"yes\" 0>\n");
   }

   if (schcode == 1)//Only port 1
   {
     componentstr += QString("<Pac P1 1 %2 -30 18 -26 0 1 \"1\" 1 \"%1 Ohm\" 1 \"0 dBm\" 0 \"1 GHz\" 0>\n").arg(Z0).arg(x_pos);
     componentstr += QString("<GND * 1 %1 0 0 0 0 0>\n").arg(x_pos);
     wirestr += QString("<%1 -60 %1 -120>\n").arg(x_pos);
     wirestr += QString("<%1 -120 %2 -120>\n").arg(x_pos).arg(x_pos+60);
     x_pos +=60;
   }

// The string format is as follows: "XX<value>;XX<value2>;...XX<valueN>;"
// where XX, YY, ZZ define the type of component and its configuration.
//    LS: Series inductance
//    CS: Series capacitor
//    LP: Shunt inductance
//    CP: Shunt capacitor
//    TL: Series transmission line
//    OU: Open stub (facing up)
//    OL: Open stub (facing down)
//    SU: Short circuited stub (facing up)
//    SL: Short circuited stub (facing down)

   for (int i = 0; i < strlist.count(); i++)
   {
      component = strlist.at(i);
      int index = component.indexOf("#");
      //Transmission lines are characterised by its impedance and its length whereas capacitors and inductors only depend on
      // its capacitance and inductance, respectively. So, the code below is aimed to handle such difference
      if (index != -1)//The component has two values
      {
          value = component.mid(2, index-2).toDouble();
          value2 = component.mid(index+1).toDouble();
      }
      else   value = component.mid(2).toDouble();
      component = component.mid(0, 2);

      if (!component.compare("LS"))//Series inductor
      {
         componentstr += QString("<L L1 1 %1 -120 -26 10 0 0 \"%2\" 1 "" 0>\n").arg(x_pos+60).arg(value);
         wirestr += QString("<%1 -120 %2 -120 "" 0 0 0 "">\n").arg(x_pos).arg(x_pos+30);
         wirestr += QString("<%1 -120 %2 -120 "" 0 0 0 "">\n").arg(x_pos+90).arg(x_pos+x_series);
         x_pos += x_series;
      }
      else if (!component.compare("CS"))//Series capacitor
      {
         componentstr += QString("<C C1 1 %1 -120 -26 17 0 0 \"%2\" 1 "" 0>\n").arg(x_pos+60).arg(value);
         wirestr += QString("<%1 -120 %2 -120 "" 0 0 0 "">\n").arg(x_pos).arg(x_pos+30);
         wirestr += QString("<%1 -120 %2 -120 "" 0 0 0 "">\n").arg(x_pos+90).arg(x_pos+x_series);
         x_pos += x_series;
      }
      else if (!component.compare("LP"))//Shunt inductor
      {
         componentstr += QString("<GND * 1 %1 0 0 0 0 0>\n").arg(x_pos);
         componentstr += QString("<L L1 1 %1 -30 -26 2 0 1 \"%2\" 1 "" 0>\n").arg(x_pos).arg(value);
         wirestr += QString("<%1 -60 %1 -120 "" 0 0 0 "">\n").arg(x_pos);
         wirestr += QString("<%1 -120 %2 -120 "" 0 0 0 "">\n").arg(x_pos).arg(x_pos+x_shunt);
         x_pos += x_shunt;
      }
      else if (!component.compare("CP"))//Shunt capacitor
      {
         componentstr += QString("<GND * 1 %1 0 0 0 0 0>\n").arg(x_pos);
         componentstr += QString("<C C1 1 %1 -30 -26 17 0 1 \"%2\" 1 "" 0>\n").arg(x_pos).arg(value);
         wirestr += QString("<%1 -60 %1 -120 "" 0 0 0 "">\n").arg(x_pos);
         wirestr += QString("<%1 -120 %2 -120 "" 0 0 0 "">\n").arg(x_pos).arg(x_pos+x_shunt);
         x_pos += x_shunt;
      }
      else if (!component.compare("TL"))//Transmission line
      {
        if (microsyn)//Microstrip implementation
        {
          er = Substrate.er;
          getMicrostrip(value, Freq, &Substrate, width, er);
          componentstr += QString("<MLIN MS1 1 %3 -120 -26 20 0 0 \"Sub1\" 1 \"%1\" 1 \"%2\" 1 \"Hammerstad\" 0 \"Kirschning\" 0 \"26.85\" 0>\n").arg(width).arg(value2/sqrt(er)).arg(x_pos+60);
        }
        else
        {
          componentstr += QString("<TLIN Line1 1 %3 -120 -26 20 0 0 \"%1\" 1 \"%2\" 1 \"0 dB\" 0 \"26.85\" 0>\n").arg(value).arg(value2).arg(x_pos+60);
        }
        wirestr += QString("<%1 -120 %2 -120 "" 0 0 0 "">\n").arg(x_pos).arg(x_pos+30);
        wirestr += QString("<%1 -120 %2 -120 "" 0 0 0 "">\n").arg(x_pos+90).arg(x_pos+x_series);
        x_pos += x_series;
      }
      else if (!component.compare("OU"))//Open stub (upper)
      {
        if (microsyn)//Microstrip implementation
        {
          er = Substrate.er;
          getMicrostrip(value, Freq, &Substrate, width, er);
          componentstr += QString("<MLIN MS1 1 %3 -180 -26 20 0 1 \"Sub1\" 1 \"%1\" 1 \"%2\" 1 \"Hammerstad\" 0 \"Kirschning\" 0 \"26.85\" 0>\n").arg(width).arg(value2/sqrt(er)).arg(x_pos);
        }
        else
        {
          componentstr += QString("<TLIN Line1 1 %3 -180 -26 20 0 1 \"%1\" 1 \"%2\" 1 \"0 dB\" 0 \"26.85\" 0>\n").arg(value).arg(value2).arg(x_pos);
        }
        wirestr += QString("<%1 -150 %1 -120 "" 0 0 0 "">\n").arg(x_pos);
        wirestr += QString("<%1 -120 %2 -120 "" 0 0 0 "">\n").arg(x_pos).arg(x_pos+x_shunt);
        //Here x_pos is not incremented since upper stubs does not overlap any other component
      }
      else if (!component.compare("OL"))//Open stub (lower)
      {
        if (microsyn)//Microstrip implementation
        {
          er = Substrate.er;
          getMicrostrip(value, Freq, &Substrate, width, er);
          componentstr += QString("<MLIN MS1 1 %3 -60 -26 20 0 1 \"Sub1\" 1 \"%1\" 1 \"%2\" 1 \"Hammerstad\" 0 \"Kirschning\" 0 \"26.85\" 0>\n").arg(width).arg(value2/sqrt(er)).arg(x_pos);
        }
        else
        {
          componentstr += QString("<TLIN Line1 1 %3 -60 -26 20 0 1 \"%1\" 1 \"%2\" 1 \"0 dB\" 0 \"26.85\" 0>\n").arg(value).arg(value2).arg(x_pos);
        }
        wirestr += QString("<%1 -90 %1 -120 "" 0 0 0 "">\n").arg(x_pos);
        wirestr += QString("<%1 -120 %2 -120 "" 0 0 0 "">\n").arg(x_pos).arg(x_pos+x_shunt);
        x_pos += x_shunt;
      }
      else if (!component.compare("SU"))//Short circuited stub (upper)
      {
        if (microsyn)//Microstrip implementation
        {
          er = Substrate.er;
          getMicrostrip(value, Freq, &Substrate, width, er);
          componentstr += QString("<MLIN MS1 1 %3 -180 -26 20 0 1 \"Sub1\" 1 \"%1\" 1 \"%2\" 1 \"Hammerstad\" 0 \"Kirschning\" 0 \"26.85\" 0>\n").arg(width).arg(value2/sqrt(er)).arg(x_pos);
        }
        else
        {
          componentstr += QString("<TLIN Line1 1 %3 -180 -26 20 0 1 \"%1\" 1 \"%2\" 1 \"0 dB\" 0 \"26.85\" 0>\n").arg(value).arg(value2).arg(x_pos);
        }
        componentstr += QString("<GND * 1 %1 -210 0 -1 0 0>\n").arg(x_pos);
        wirestr += QString("<%1 -150 %1 -120 "" 0 0 0 "">\n").arg(x_pos);
        wirestr += QString("<%1 -120 %2 -120 "" 0 0 0 "">\n").arg(x_pos).arg(x_pos+x_shunt);
        //Here x_pos is not incremented since upper stubs does not overlap any other component
      }
      else if (!component.compare("SL"))//Short circuited stub (lower)
      {
        if (microsyn)//Microstrip implementation
        {
          er = Substrate.er;
          getMicrostrip(value, Freq, &Substrate, width, er);
          componentstr += QString("<MLIN MS1 1 %3 -60 -26 20 0 1 \"Sub1\" 1 \"%1\" 1 \"%2\" 1 \"Hammerstad\" 0 \"Kirschning\" 0 \"26.85\" 0>\n").arg(width).arg(value2/sqrt(er)).arg(x_pos);
        }
        else
        {
          componentstr += QString("<TLIN Line1 1 %3 -60 -26 20 0 1 \"%1\" 1 \"%2\" 1 \"0 dB\" 0 \"26.85\" 0>\n").arg(value).arg(value2).arg(x_pos);
        }
        componentstr += QString("<GND * 1 %1 -30 0 0 0 0>\n").arg(x_pos);
        wirestr += QString("<%1 -90 %1 -120 "" 0 0 0 "">\n").arg(x_pos);
        wirestr += QString("<%1 -120 %2 -120 "" 0 0 0 "">\n").arg(x_pos).arg(x_pos+x_shunt);
        x_pos += x_shunt;
      }

   }

   if ((schcode == 2)|| (schcode == 3))//Port 2
   {
     x_pos += 100;
     if (abs(XL) < 1e-3)//The load is real so we can use a conventional port
     {
     componentstr += QString("<Pac P2 1 %2 -30 18 -26 0 1 \"1\" 1 \"%1 Ohm\" 1 \"0 dBm\" 0 \"1 GHz\" 0>\n").arg(RL).arg(x_pos);
     componentstr += QString("<GND * 1 %1 0 0 0 0 0>\n").arg(x_pos);
     wirestr += QString("<%1 -60 %1 -120>\n").arg(x_pos);//Vertical wire
     wirestr += QString("<%1 -120 %2 -120>\n").arg(x_pos-100).arg(x_pos);//Horizontal wire
     }
     else//The load has a reactive part... Since Qucs cannot handle complex ports we can use a tuned load, valid for narrowband
     {
           if ((RL > 1e-3)&&(XL < 1e-3))// R + C
           {
               componentstr += QString("<R R1 1 %1 -30 15 -26 0 -1 \"%2 Ohm\" 1 \"26.85\" 0 \"US\" 0>\n").arg(x_pos).arg(RL);
               componentstr += QString("<C C1 1 %1 -90 15 -26 0 -1 \"%2 F\" 1 0>\n").arg(x_pos).arg(1/(fabs(XL)*2*pi*Freq));
               paintingstr += QString("<Text %1 50 12 #000000 0 \"%4-j%5 %2 @ %3 GHz\">\n").arg(x_pos).arg(QChar (0x2126)).arg(Freq*1e-9).arg(RL).arg(abs(XL));
           }
           if ((RL > 1e-3)&&(XL > 1e-3))//R + L
           {
               componentstr += QString("<R R1 1 %1 -30 15 -26 0 -1 \"%2 Ohm\" 1 \"26.85\" 0 \"US\" 0>\n").arg(x_pos).arg(RL);
               componentstr += QString("<L L1 1 %1 -90 15 -26 0 -1 \"%2 H\" 1 0>\n").arg(x_pos).arg(XL/(2*pi*Freq));
               paintingstr += QString("<Text %1 50 12 #000000 0 \"%4+j%5 %2 @ %3 GHz\">\n").arg(x_pos).arg(QChar (0x2126)).arg(Freq*1e-9).arg(RL).arg(XL);
           }
           if ((RL > 1e-3)&&(abs(XL) < 1e-3))//R
           {
               componentstr += QString("<R R1 1 %1 -30 15 -26 0 -1 \"%2 Ohm\" 1 \"26.85\" 0 \"US\" 0>\n").arg(x_pos).arg(RL);
               wirestr += QString("<%1 -60 %1 -120>\n").arg(x_pos);//Vertical wire
               paintingstr += QString("<Text %1 50 12 #000000 0 \"%4 %2 @ %3 GHz\">\n").arg(x_pos).arg(QChar (0x2126)).arg(Freq*1e-9).arg(RL);
           }
           if ((RL < 1e-3)&&(XL > 1e-3))//L
           {
               componentstr += QString("<L L1 1 %1 -30 15 -26 0 -1 \"%2 H\" 1 \"26.85\" 0 \"US\" 0>\n").arg(x_pos).arg(RL);
               wirestr += QString("<%1 -60 %1 -120>\n").arg(x_pos);//Vertical wire
               paintingstr += QString("<Text %1 50 12 #000000 0 \"j%4 %2 @ %3 GHz\">\n").arg(x_pos).arg(QChar (0x2126)).arg(Freq*1e-9).arg(XL);
           }
           if ((RL < 1e-3)&&(XL < 1e-3))//C
           {
               componentstr += QString("<C C1 1 %1 -30 15 -26 0 -1 \"%2 F\" 1 \"26.85\" 0 \"US\" 0>\n").arg(x_pos).arg(RL);
               wirestr += QString("<%1 -60 %1 -120>\n").arg(x_pos);//Vertical wire
               paintingstr += QString("<Text %1 50 12 #000000 0 \"-j%4 %2 @ %3 GHz\">\n").arg(x_pos).arg(QChar (0x2126)).arg(Freq*1e-9).arg(abs(XL));
           }
           wirestr += QString("<%1 -120 %2 -120>\n").arg(x_pos-100).arg(x_pos);//Horizontal wire
           componentstr += QString("<GND * 1 %1 0 0 -1 0 0>\n").arg(x_pos);

           //Box surrounding the load
           paintingstr += QString("<Rectangle %1 -150 200 200 #000000 0 1 #c0c0c0 1 0>\n").arg(x_pos-30);
     }
   }

  // Substrate
  if (microsyn)componentstr += QString("<SUBST Sub1 1 400 200 -30 24 0 0 \"%1\" 1 \"%2mm\" 1 \"%3um\" 1 \"%4\" 1 \"%5\" 1 \"%6\" 1>\n").arg(Substrate.er).arg(Substrate.height*1e3).arg(Substrate.thickness*1e6).arg(Substrate.tand).arg(Substrate.resistivity).arg(Substrate.roughness);

 return 0;
}
