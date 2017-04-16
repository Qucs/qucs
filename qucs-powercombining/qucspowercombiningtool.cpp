/*
 * qucspowercombiningtool.cpp - Power combining tool implementation
 *
 * copyright (C) 2017 Andres Martinez-Mera <andresmartinezmera@gmail.com>
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this package; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street - Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 *
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "qucspowercombiningtool.h"
#include "../qucs/qucs.h"
#include "../qucs/misc.h"
#include "../qucs-filter/material_props.h"



//---------------------------------------------------------
// Constructor. It sets up the user interface
QucsPowerCombiningTool::QucsPowerCombiningTool()
{
  setWindowTitle("Qucs Power Combining Tool " PACKAGE_VERSION);
  centralWidget = new QWidget();
  QHBoxLayout *hbox = new QHBoxLayout();
  ImplementationgroupBox = new QGroupBox("Implementation");
  MicrostripgroupBox  = new QGroupBox("Microstrip substrate");
  ImagegroupBox  = new QGroupBox("Preview");

  QSizePolicy policy(QSizePolicy::Preferred,QSizePolicy::Preferred);
  policy.setHeightForWidth(true);
  this->setSizePolicy(policy);
  centralWidget->setSizePolicy(policy);


  //Add elements to ImplementationgroupBox
  QVBoxLayout *VboxImplementation = new QVBoxLayout();
  //Topology
  QHBoxLayout *hboxImpl1 = new QHBoxLayout();
  TopoLabel = new QLabel("Topology");
  TopoCombo = new QComboBox();
  TopoCombo->addItem("Wilkinson");
  TopoCombo->addItem("Multistage Wilkinson");
  TopoCombo->addItem("T-junction");
  TopoCombo->addItem("Branchline");
  TopoCombo->addItem("Double box branchline");
  TopoCombo->addItem("Bagley");
  TopoCombo->addItem("Gysel");
  TopoCombo->addItem("Travelling Wave");
  TopoCombo->addItem("Tree");
  hboxImpl1->addWidget(TopoLabel);
  hboxImpl1->addWidget(TopoCombo);
  VboxImplementation->addLayout(hboxImpl1);

  //Number of outputs
  QHBoxLayout *hboxImpl2 = new QHBoxLayout();
  NLabel  =new QLabel("Number of outputs");
  BranchesCombo = new QComboBox();
  BranchesCombo->addItem("2");
  BranchesCombo->setFixedWidth(60);
  hboxImpl2->addWidget(NLabel);
  hboxImpl2->addWidget(BranchesCombo);
  VboxImplementation->addLayout(hboxImpl2);


  //Ref impedance
  QHBoxLayout *hboxImpl3 = new QHBoxLayout();
  RefImp = new QLabel("Z0");
  RefImplineEdit = new QLineEdit("50");
  RefImplineEdit->setFixedWidth(40);
  OhmLabel = new QLabel(QChar(0xa9, 0x03));
  OhmLabel->setFixedWidth(15);
  hboxImpl3->addWidget(RefImp);
  hboxImpl3->addWidget(RefImplineEdit);
  hboxImpl3->addWidget(OhmLabel);
  VboxImplementation->addLayout(hboxImpl3);


  //Frequency
  QHBoxLayout *hboxImpl4 = new QHBoxLayout();
  FreqLabel = new QLabel("Frequency");
  FreqlineEdit = new QLineEdit("1");
  FreqlineEdit->setFixedWidth(40);
  FreqScaleCombo=new QComboBox();
  FreqScaleCombo->addItem("GHz");
  FreqScaleCombo->addItem("MHz");
  FreqScaleCombo->addItem("kHz");
  FreqScaleCombo->addItem("Hz");
  FreqScaleCombo->setFixedWidth(60);
  hboxImpl4->addWidget(FreqLabel);
  hboxImpl4->addWidget(FreqlineEdit);
  hboxImpl4->addWidget(FreqScaleCombo);
  VboxImplementation->addLayout(hboxImpl4);

  // Output power ratio
  QHBoxLayout *hboxImpl5 = new QHBoxLayout();
  K1Label = new QLabel("Output Power ratio");
  K1lineEdit=new QLineEdit("0");
  K1lineEdit->setFixedWidth(40);
  K1LabeldB = new QLabel("dB");
  K1LabeldB->setFixedWidth(20);
  hboxImpl5->addWidget(K1Label);
  hboxImpl5->addWidget(K1lineEdit);
  hboxImpl5->addWidget(K1LabeldB);
  VboxImplementation->addLayout(hboxImpl5);

  // Number of stages
  QHBoxLayout *hboxImpl5_ = new QHBoxLayout();
  NStagesCombo = new QComboBox();
  NStagesCombo->addItem("2");
  NStagesCombo->setFixedWidth(60);
  NStagesLabel = new QLabel("Number of stages");
  hboxImpl5_->addWidget(NStagesLabel);
  hboxImpl5_->addWidget(NStagesCombo);
  VboxImplementation->addLayout(hboxImpl5_);
  NStagesLabel->hide();
  NStagesCombo->hide();

  //Ideal transmission line attenuation coeffient
  QHBoxLayout *hboxImplAlpha = new QHBoxLayout();
  AlphaLabel = new QLabel("Attenuation coefficient");
  AlphalineEdit=new QLineEdit("0");
  AlphadBLabel = new QLabel("dB/m");
  hboxImplAlpha->addWidget(AlphaLabel);
  hboxImplAlpha->addWidget(AlphalineEdit);
  hboxImplAlpha->addWidget(AlphadBLabel);
  AlphadBLabel->setVisible(false);
  AlphaLabel->setVisible(false);
  AlphalineEdit->setVisible(false);
  VboxImplementation->addLayout(hboxImplAlpha);

  QHBoxLayout *hboxLengthUnit = new QHBoxLayout();
  UnitsLabel = new QLabel("Length unit");
  UnitsCombo=new QComboBox();
  UnitsCombo->addItem("mm");
  UnitsCombo->addItem("mil");
  UnitsCombo->addItem("um");
  UnitsCombo->addItem("nm");
  UnitsCombo->addItem("inch");
  UnitsCombo->addItem("ft");
  UnitsCombo->addItem("m");
  UnitsCombo->setFixedWidth(60);
  hboxLengthUnit->addWidget(UnitsLabel);
  hboxLengthUnit->addWidget(UnitsCombo);
  VboxImplementation->addLayout(hboxLengthUnit);

  //Add S-param block?
  QHBoxLayout *hboxImpl6 = new QHBoxLayout();
  AddSparcheckBox=new QCheckBox("Add S-parameter simulation");
  AddSparcheckBox->setChecked(true);
  hboxImpl6->addWidget(AddSparcheckBox);
  VboxImplementation->addLayout(hboxImpl6);

  //Synthesize Microstrip
  QHBoxLayout *hboxImpl7 = new QHBoxLayout();
  MicrostripcheckBox=new QCheckBox("Microstrip implementation");
  MicrostripcheckBox->setChecked(false);
  hboxImpl7->addWidget(MicrostripcheckBox);
  VboxImplementation->addLayout(hboxImpl7);

  //Lumped elements implementation
  QHBoxLayout *hboxImpl9 = new QHBoxLayout();
  LumpedcheckBox=new QCheckBox("Lumped element implementation");
  LumpedcheckBox->setChecked(false);
  hboxImpl9->addWidget(LumpedcheckBox);
  VboxImplementation->addLayout(hboxImpl9);

  //Go! Button
  QHBoxLayout *hboxImpl8 = new QHBoxLayout();
  GenerateButton = new QPushButton("Generate schematic");
  hboxImpl8->addWidget(GenerateButton);
  VboxImplementation->addLayout(hboxImpl8);

  // Add elements to MicrostripgroupBox
  QVBoxLayout *VboxMicrostrip = new QVBoxLayout();

  //Relative permitivity
  QHBoxLayout *hboxMicr1 = new QHBoxLayout();
  RelPermlabel = new QLabel("Relative permitivity");
  RelPermcomboBox = new QComboBox();
  RelPermcomboBox->setEditable(true);
  const char **p = List_er;
  while(*(++p)) RelPermcomboBox->addItem(*p);
  hboxMicr1->addWidget(RelPermlabel);
  hboxMicr1->addWidget(RelPermcomboBox);
  VboxMicrostrip->addLayout(hboxMicr1);

  //Substrate height
  QHBoxLayout *hboxMicr2 = new QHBoxLayout();
  SubstrateHeightlabel = new QLabel("Substrate heigth");
  SubstrateHeightlineEdit = new QLineEdit("1.0");
  SubstrateMMlabel = new QLabel("mm");
  hboxMicr2->addWidget(SubstrateHeightlabel);
  hboxMicr2->addWidget(SubstrateHeightlineEdit);
  hboxMicr2->addWidget(SubstrateMMlabel);
  VboxMicrostrip->addLayout(hboxMicr2);

  //Metal thickness
  QHBoxLayout *hboxMicr3 = new QHBoxLayout();
  ThicknessLabel  = new QLabel("Metal thickness");
  ThicknesslineEdit = new QLineEdit("12.5");
  ThicknessumLabel  = new QLabel("um");
  hboxMicr3->addWidget(ThicknessLabel);
  hboxMicr3->addWidget(ThicknesslineEdit);
  hboxMicr3->addWidget(ThicknessumLabel);
  VboxMicrostrip->addLayout(hboxMicr3);

  //Minimum width
  QHBoxLayout *hboxMicr4 = new QHBoxLayout();
  MinWidthLabel= new QLabel("Minimum width");
  MinWidthlineEdit = new QLineEdit("0.4");
  MinWidthmmLabel = new QLabel("mm");
  hboxMicr4->addWidget(MinWidthLabel);
  hboxMicr4->addWidget(MinWidthlineEdit);
  hboxMicr4->addWidget(MinWidthmmLabel);
  VboxMicrostrip->addLayout(hboxMicr4);

  //Maximum width
  QHBoxLayout *hboxMicr5 = new QHBoxLayout();
  MaxWidthLabel= new QLabel("Maximum width");
  MaxWidthlineEdit = new QLineEdit("5");
  MaxWidthmmLabel = new QLabel("mm");
  hboxMicr5->addWidget(MaxWidthLabel);
  hboxMicr5->addWidget(MaxWidthlineEdit);
  hboxMicr5->addWidget(MaxWidthmmLabel);
  VboxMicrostrip->addLayout(hboxMicr5);

  //tan(delta)
  QHBoxLayout *hboxMicr6 = new QHBoxLayout();
  tanDLabel = new QLabel("tanD");
  tanDlineEdit = new QLineEdit("0.0125");
  hboxMicr6->addWidget(tanDLabel);
  hboxMicr6->addWidget(tanDlineEdit);
  VboxMicrostrip->addLayout(hboxMicr6);

  //Resistivity
  QHBoxLayout *hboxMicr7 = new QHBoxLayout();
  ResistivityLabel = new QLabel("Resistivity");
  ResistivitylineEdit  = new QLineEdit("2.43902e-08");
  hboxMicr7->addWidget(ResistivityLabel);
  hboxMicr7->addWidget(ResistivitylineEdit);
  VboxMicrostrip->addLayout(hboxMicr7);

  //Roughness
  QHBoxLayout *hboxMicr8 = new QHBoxLayout();
  RoughnessLabel = new QLabel("Roughness");
  RoughnesslineEdit = new QLineEdit("0.15e-6");
  hboxMicr8->addWidget(RoughnessLabel);
  hboxMicr8->addWidget(RoughnesslineEdit);
  VboxMicrostrip->addLayout(hboxMicr8);


  // Image
   QSize sz;
   QString s1 = ":/bitmaps/Wilkinson.svg";
   QGridLayout * imgLayout = new QGridLayout();
   imgWidget = new QSvgWidget(s1);

   sz = imgWidget->size();
   imgLayout->addWidget(imgWidget);
   ImagegroupBox->setLayout(imgLayout);

  ImplementationgroupBox->setLayout(VboxImplementation);
  MicrostripgroupBox->setLayout(VboxMicrostrip);
  hbox->addWidget(ImplementationgroupBox);
  hbox->addWidget(ImagegroupBox);
  hbox->addWidget(MicrostripgroupBox);
  centralWidget->setLayout(hbox);
  setCentralWidget(centralWidget);
 
  MicrostripgroupBox->hide();
  statusBar = new QStatusBar();
  setStatusBar(statusBar);
  connect(GenerateButton, SIGNAL(clicked()), SLOT(on_GenerateButton_clicked()));
  connect(TopoCombo, SIGNAL(currentIndexChanged(int)), SLOT(on_TopoCombo_currentIndexChanged(int)));
  connect(MicrostripcheckBox, SIGNAL(clicked()), SLOT(on_MicrostripcheckBox_clicked()));
  connect(LumpedcheckBox, SIGNAL(clicked()), SLOT(on_LCcheckBox_clicked()));

  //Get the screen dimensions and scale the main window according to that
  //the size policy is set to 'fixed' so as to allow the substrate parameters tab to be shown or hidden
   QDesktopWidget dw;
   QRect screenSize = dw.availableGeometry(this);
   DefaultSize = QSize(screenSize.width() * 0.5, screenSize.height() * 0.6);//Hide substrate tab
   ExtendedSize = QSize(screenSize.width() * 0.8, screenSize.height() * 0.6);//Show substrate tab
   this->setFixedSize(DefaultSize);


}

//------------------------------------------------
// Destructor
QucsPowerCombiningTool::~QucsPowerCombiningTool()
{
}

//---------------------------------------------------------------------------
// This function scales the frequency according to 'FreqScaleCombo' combobox
double QucsPowerCombiningTool::getScaleFreq()
{
    double exp=1;
    switch(FreqScaleCombo->currentIndex())
    {
    case 0:
        exp=9;
        break;
    case 1:
        exp=6;
        break;
    case 2:
        exp=3;
        break;
    case 3:
        exp=1;
        break;
    }
    return pow(10, exp);
}

//------------------------------------------------------------------
// This function changes the window according to the selected topology
void QucsPowerCombiningTool::on_TopoCombo_currentIndexChanged(int index)
{
    bool lumpedImplementation = LumpedcheckBox->isChecked();
    //Update image
    switch (index)
    {
       
       case 0: //Wilkinson
               (lumpedImplementation) ? imgWidget->load(QString(":/bitmaps/WilkinsonLC.svg")) : 
                                        imgWidget->load(QString(":/bitmaps/Wilkinson.svg"));
               break;
       case 1: //Mutistage Wilkinson
               (lumpedImplementation) ? imgWidget->load(QString(":/bitmaps/MultistageWilkinsonLC.svg")) : 
                                        imgWidget->load(QString(":/bitmaps/MultistageWilkinson.svg"));
               break;
       case 2: //Tee
               imgWidget->load(QString(":/bitmaps/Tee.svg")); break;
       case 3: //Branch-line
               imgWidget->load(QString(":/bitmaps/Branchline.svg"));break;
       case 4: //Double-box branch-line
               imgWidget->load(QString(":/bitmaps/DoubleBoxBranchline.svg"));break;
       case 5: //Bagley power combiner
               imgWidget->load(QString(":/bitmaps/Bagley.svg"));break;
       case 6: //Gysel power combiner
               imgWidget->load(QString(":/bitmaps/Gysel.svg")); break;
       // -------------  CORPORATE COMBINERS  -----------------
       case 7: //Travelling wave
               imgWidget->load(QString(":/bitmaps/TravellingWave.svg")); break;
       case 8: //Tree
               imgWidget->load(QString(":/bitmaps/Tree.svg")); break;   
    }

    // Change settings
    if ((index==0)|(index==2)||(index==3))//Wilkinson, Tee, Branchline
    {
        BranchesCombo->clear();
        BranchesCombo->addItem("2");
        K1lineEdit->setVisible(true);
        K1Label->setVisible(true);
        K1LabeldB->setVisible(true);
        BranchesCombo->setEditable(false);
    }
    else
    {
        K1lineEdit->setVisible(false);
        K1Label->setVisible(false);
        K1LabeldB->setVisible(false);
    }

    if (index == 7)//Gysel
    {
        BranchesCombo->clear();
        BranchesCombo->setEditable(false);
        BranchesCombo->clear();
        BranchesCombo->addItem("2");
        BranchesCombo->setEditable(false);
    }
    
    if ((index == 0)||(index==1))//Wilkinson
    {
        LumpedcheckBox->setVisible(true);
        NStagesLabel->show();
        NStagesCombo->show();
    }
    else//Not implemented
    {
        LumpedcheckBox->setVisible(false);
        NStagesLabel->hide();
        NStagesCombo->hide();
    }
  
    if (index == 1)//Multistage Wilkinson. So far, it is not possible to implement more than 7 stages
    {
      NStagesCombo->clear();
      NStagesCombo->addItem("2");
      NStagesCombo->addItem("3");
      NStagesCombo->addItem("4");
      NStagesCombo->addItem("5");
      NStagesCombo->addItem("6");
      NStagesCombo->addItem("7");
      NStagesLabel->setVisible(true);
      NStagesCombo->setVisible(true);
      BranchesCombo->clear();
      BranchesCombo->addItem("2");//2 outputs only
      BranchesCombo->setEditable(false);
    }
    else//There are no more multistage combiners implemented
    {
      NStagesLabel->setVisible(false);
      NStagesCombo->setVisible(false);
    }
    if(index == 5)//Bagley
    {
        BranchesCombo->clear();
        BranchesCombo->addItem("3");
        BranchesCombo->addItem("5");
        BranchesCombo->addItem("7");
        BranchesCombo->setEditable(true);
    }
    if(index == 7)// Travelling wave
    {
        BranchesCombo->clear();
        BranchesCombo->addItem("3");
        BranchesCombo->addItem("4");
        BranchesCombo->addItem("5");
        BranchesCombo->addItem("6");
        BranchesCombo->setEditable(true);//Let the user to specify an arbitrary number of outputs
    }
    if(index == 8)//Tree
    {
        BranchesCombo->clear();
        BranchesCombo->addItem("4");
        BranchesCombo->addItem("8");
        BranchesCombo->addItem("16");
        BranchesCombo->setEditable(true);//Let the user to specify an arbitrary number of outputs (power of 2)
    }
}

//---------------------------------------------------------------
// This function reads the input values and calls the different methods for generating a schematic
void QucsPowerCombiningTool::on_GenerateButton_clicked()
{
    double err=0;
    double Z0 = RefImplineEdit->text().toDouble();
    int N=BranchesCombo->currentText().toInt();
    double K=K1lineEdit->text().toDouble();//Power ratio. Output port 1
    K = pow(10, K/20.);//Conversion to natural units
    double Freq = FreqlineEdit->text().toDouble()*getScaleFreq();
    bool SP_block = AddSparcheckBox->isChecked();
    int NStages = NStagesCombo->currentText().toInt();
    tSubstrate Substrate;
    double alpha = AlphalineEdit->text().toDouble();//Attenuation coefficient in dB/m
    bool microcheck = MicrostripcheckBox->isChecked();
    bool LumpedElements = LumpedcheckBox->isChecked();//Lumped element implementation?

    if (microcheck)//Substrate
    {
        Substrate.er=RelPermcomboBox->currentText().section("  ", 0, 0).toDouble();
        Substrate.height=SubstrateHeightlineEdit->text().toDouble()*1e-3;
        Substrate.thickness=ThicknesslineEdit->text().toDouble()*1e-6;
        Substrate.maxWidth=MaxWidthlineEdit->text().toDouble()*1e-3;
        Substrate.minWidth=MinWidthlineEdit->text().toDouble()*1e-3;
        Substrate.resistivity=ResistivitylineEdit->text().toDouble();
        Substrate.tand=tanDlineEdit->text().toDouble();
        Substrate.roughness=RoughnesslineEdit->text().toDouble();
    }

    switch (TopoCombo->currentIndex())//Topology selection
    {
    case 0: // Wilkinson
        err = Wilkinson(Z0, Freq, K, SP_block, microcheck, Substrate, alpha, LumpedElements);
        break;
    case 1: // Multistage Wilkinson
        err = MultistageWilkinson(Z0, Freq, NStages, SP_block, microcheck, Substrate, alpha, LumpedElements);
        break;
    case 2: // Tee
        err = Tee(Z0, Freq, K*K, SP_block, microcheck, Substrate, alpha);
        break;
    case 3: //Branchline
        err = Branchline(Z0, Freq, K*K, SP_block, microcheck, Substrate, alpha);
        break;
    case 4: //Double box branchline
        err = DoubleBoxBranchline(Z0, Freq, K*K, SP_block, microcheck, Substrate, alpha);
        break;
    case 5: //Bagley
        err = Bagley(Z0, Freq, N, SP_block, microcheck, Substrate, alpha);
        break;
    case 6: // Gysel
        err = Gysel(Z0, Freq, SP_block, microcheck, Substrate, alpha);
        break;
    case 7: // Travelling wave
        err = TravellingWave(Z0, Freq, N, SP_block, microcheck, Substrate, alpha);
        break;
    case 8: //Tree
        err = Tree(Z0, Freq, N, SP_block, microcheck, Substrate, alpha);
        break;
    }
    if(!err)//Checking errors...
    {
        statusBar->showMessage(tr("Ready! Use CTRL+V to paste the schematic"), 2000);
    }
    else
    {
       statusBar->showMessage(tr("Error! The network could not be generated"), 2000);
    }
}

//--------------------------------------------------------------------------------
// This function calculates the parameters of the Wilkinson power divider according to the
// specifications. It is written outside of Wilkinson() because it is also used by the corporate power
// combining functions
QString QucsPowerCombiningTool::CalculateWilkinson(double Z0, double K)
{
    // Wilkinson divider design equations
    double K2 =K*K;
    double Z3 = Z0*sqrt((K2+1)/(K*K*K));
    double Z2 = K2*Z3;
    double R=Z0*((K2+1)/K);
    double R2 = Z0*K;
    double R3 = Z0/K;
    return QString("%1;%2;%3;%4;%5").arg(Z2).arg(Z3).arg(R).arg(R2).arg(R3);
}

//-----------------------------------------------------
// This function calculates a 2Way Wilkinson divider and generates the
// schematic
int QucsPowerCombiningTool::Wilkinson(double Z0, double Freq, double K, bool SP_block, bool microcheck, tSubstrate Substrate, double Alpha, bool LumpedElements)
{
    double er, width;
    double lambda4=SPEED_OF_LIGHT/(4*Freq);
    QString wilkstr = CalculateWilkinson(Z0, K);
    double Z2 = wilkstr.section(';', 0, 0).toDouble();
    double Z3 = wilkstr.section(';', 1, 1).toDouble();
    double R =  wilkstr.section(';', 2, 2).toDouble();
    double R2 =  wilkstr.section(';', 3, 3).toDouble();
    double R3 =  wilkstr.section(';', 4, 4).toDouble();
    double C2, C3, CC, C2_, C3_, L2, L3, L2_, L3_, Z4, Z5;

    if (LumpedElements)//Quarter wave transmission line Pi LC equivalent
    {
       double w = 2*pi*Freq;
       L2 = Z2/w;
       C2 = 1./(L2*w*w);
       L3 = Z3/w;
       C3 = 1./(L3*w*w);
       CC = C2+C3;
       if (R2 != R3)//Unequal output power rate => requires matching to Z0
       {
          Z4 = Z0*sqrt(K);
          Z5 = Z0/sqrt(K);
          L2_ = Z4/w;
          L3_ = Z5/w;
          C2_ = 1./(L2_*w*w);
          C3_ = 1./(L3_*w*w);
          //Embed the first capacitor of the Pi quarter wave equivalent in the last C of the Wilkinson structure  
          C2 += C2_;
          C3 += C3_;
       } 
    }

    //Qucs schematic

    QString s = "<Qucs Schematic " PACKAGE_VERSION ">\n";
    s += "<Components>\n";
    if (SP_block)
    {
        //Source
        s += QString("<Pac P1 1 0 0 18 -26 0 1 \"1\" 0 \"%1 Ohm\" 1 \"0 dBm\" 0 \"1 GHz\" 0>\n").arg(Z0);
        s += QString("<GND * 1 0 30 0 0 0 0>\n");
        //Output port 1
        s += QString("<Pac P1 1 500 -60 18 -26 0 1 \"1\" 0 \"%1 Ohm\" 1 \"0 dBm\" 0 \"1 GHz\" 0>\n").arg(Z0);
        s += QString("<GND * 1 500 -30 0 0 0 0>\n");
        //Output port 2
        s += QString("<Pac P1 1 500 60 18 -26 0 1 \"1\" 0 \"%1 Ohm\" 1 \"0 dBm\" 0 \"1 GHz\" 0>\n").arg(Z0);
        s += QString("<GND * 1 500 90 0 0 0 0>\n");
        //S-parameter analysis component
        double freq_start = std::max(0., Freq-1e9);
        double freq_stop = Freq+1e9;
        s += QString("<.SP SP1 1 200 200 0 67 0 0 \"lin\" 1 \"%2Hz\" 1 \"%3Hz\" 1 \"300\" 1 \"no\" 0 \"1\" 0 \"2\" 0>\n").arg((freq_start)).arg((freq_stop));
        s += QString("<Eqn Eqn1 1 50 200 -28 15 0 0 \"S11_dB=dB(S[1,1])\" 1 \"S21_dB=dB(S[2,1])\" 1  \"S31_dB=dB(S[3,1])\" 1 \"yes\" 0>\n");
        if (microcheck)s += QString("<SUBST Sub1 1 400 200 -30 24 0 0 \"%1\" 1 \"%2mm\" 1 \"%3um\" 1 \"%4\" 1 \"%5\" 1 \"%6\" 1>\n").arg(Substrate.er).arg(Substrate.height*1e3).arg(Substrate.thickness*1e6).arg(Substrate.tand).arg(Substrate.resistivity).arg(Substrate.roughness);
    }

    if (microcheck)//Microstrip implementation
    {
        er = Substrate.er;
        getMicrostrip(Z0, Freq, &Substrate, width, er);
        s += QString("<MLIN MS1 1 130 -30 -26 20 0 0 \"Sub1\" 0 \"%1\" 1 \"%2\" 1 \"Hammerstad\" 0 \"Kirschning\" 0 \"26.85\" 0>\n").arg(ConvertLengthFromM(width)).arg(ConvertLengthFromM(lambda4/sqrt(er)));
        er = Substrate.er;
        getMicrostrip(Z2, Freq, &Substrate, width, er);
        s += QString("<MLIN MS1 1 260 -90 -26 -71 0 0 \"Sub1\" 0 \"%1\" 1 \"%2\" 1 \"Hammerstad\" 0 \"Kirschning\" 0 \"26.85\" 0>\n").arg(ConvertLengthFromM(width)).arg(ConvertLengthFromM(lambda4/sqrt(er)));
        er = Substrate.er;
        getMicrostrip(Z3, Freq, &Substrate, width, er);
        s += QString("<MLIN MS1 1 260 30 -26 20 0 0 \"Sub1\" 0 \"%1\" 1 \"%2\" 1 \"Hammerstad\" 0 \"Kirschning\" 0 \"26.85\" 0>\n").arg(ConvertLengthFromM(width)).arg(ConvertLengthFromM(lambda4/sqrt(er)));
    }
    else
    {
        if (LumpedElements)// CLC equivalent
        {
        //First capacitor
        s += QString("<C C1 1 110 0 13 4 0 3 \"%1\" 1 \"\" 0 \"neutral\" 0>\n").arg(num2str(CC));
        s += QString("<GND * 1 110 30 0 1 0 0>\n");

        //Upper branch
        s += QString("<C C1 1 310 -140 -89 -28 0 3 \"%1\" 1 \"\" 0 \"neutral\" 0>\n").arg(num2str(C2));
        s += QString("<GND * 1 310 -170 0 0 0 2>\n");
        s += QString("<L L1 1 260 -90 -25 8 0 0 \"%1\" 1 \"\" 0 \"neutral\" 0>\n").arg(num2str(L2));
        //Lower branch
        s += QString("<C C1 1 310 80 -77 10 0 3 \"%1\" 1 \"\" 0 \"neutral\" 0>\n").arg(num2str(C3));
        s += QString("<GND * 1 310 110 0 0 0 0>\n");
        s += QString("<L L2 1 260 30 -28 12 0 0 \"%1\" 1 \"\" 0 \"neutral\" 0>\n").arg(num2str(L3));
        }
        else
        {
        s += QString("<TLIN Line1 1 130 -30 -26 20 0 0 \"%1 Ohm\" 1 \"%2\" 1 \"%3 dB\" 0 \"26.85\" 0>\n").arg(RoundVariablePrecision(Z0)).arg(ConvertLengthFromM(lambda4)).arg(Alpha);//Z0 line
        s += QString("<TLIN Line1 1 260 -90 -26 -71 0 0 \"%1 Ohm\" 1 \"%2\" 1 \"%3 dB\" 0 \"26.85\" 0>\n").arg(RoundVariablePrecision(Z2)).arg(ConvertLengthFromM(lambda4)).arg(Alpha);//Output branch 1
        s += QString("<TLIN Line1 1 260 30 -26 20 0 0 \"%1 Ohm\" 1 \"%2\" 1 \"%3 dB\" 0 \"26.85\" 0>\n").arg(RoundVariablePrecision(Z3)).arg(ConvertLengthFromM(lambda4)).arg(Alpha);//Output branch 2
        }
    }
    s += QString("<R R1 1 340 -20 20 -26 0 -1 \"%1 Ohm\" 1 \"26.85\" 0 \"US\" 0>\n").arg(R);//Isolation resistor
    if (K!=1)
    {// An unequal power ratio implies that the load impedance != 50, so it requires matching
        if (microcheck)//Microstrip
        {
            er = Substrate.er;
            getMicrostrip(sqrt(Z0*R2), Freq, &Substrate, width, er);
            s += QString("<MLIN MS1 1 410 -90 -26 -71 0 0 \"Sub1\" 0 \"%1\" 1 \"%2\" 1 \"Hammerstad\" 0 \"Kirschning\" 0 \"26.85\" 0>\n").arg(ConvertLengthFromM(width)).arg(ConvertLengthFromM(lambda4/sqrt(er)));
            er = Substrate.er;
            getMicrostrip(sqrt(Z0*R3), Freq, &Substrate, width, er);
            s += QString("<MLIN MS1 1 410 30 -26 20 0 0 \"Sub1\" 0 \"%1\" 1 \"%2\" 1 \"Hammerstad\" 0 \"Kirschning\" 0 \"26.85\" 0>\n").arg(ConvertLengthFromM(width)).arg(ConvertLengthFromM(lambda4/sqrt(er)));

        }
        else
        {
           if (LumpedElements)//CLC equivalent
           {
             // Upper branch
             s += QString("<L L2 1 410 -90 -22 5 0 0 \"%1\" 1 \"\" 0 \"neutral\" 0>\n").arg(num2str(L2_));
             s += QString("<C C1 1 450 -140 -80 -30 0 3 \"%1\" 1 \"\" 0 \"neutral\" 0>\n").arg(num2str(C2_));
             s += QString("<GND * 1 450 -170 0 0 0 2>\n");

             // Lower branch
             s += QString("<L L2 1 410 30 -41 7 0 0 \"%1\" 1 \"\" 0 \"neutral\" 0>\n").arg(num2str(L3_));
             s += QString("<C C1 1 450 90 -79 -7 0 3 \"%1\" 1 \"\" 0 \"neutral\" 0>\n").arg(num2str(C3_));
             s += QString("<GND * 1 450 120 0 0 0 0>\n");
           }
           else
           {
               s += QString("<TLIN Line1 1 410 -90 -26 -71 0 0 \"%1 Ohm\" 1 \"%2\" 1 \"%3 dB\" 0 \"26.85\" 0>\n").arg(RoundVariablePrecision(sqrt(Z0*R2))).arg(ConvertLengthFromM(lambda4)).arg(Alpha);//Quarter wave matching output branch 1
               s += QString("<TLIN Line1 1 410 30 -26 20 0 0 \"%1 Ohm\" 1 \"%2\" 1 \"%3 dB\" 0 \"26.85\" 0>\n").arg(RoundVariablePrecision(sqrt(Z0*R3))).arg(ConvertLengthFromM(lambda4)).arg(Alpha);//Quarter wave matching output branch 2
           }
           }
    }
    s += "</Components>\n";

    //Wiring
    s += "<Wires>\n";
    s += QString("<0 -30 100 -30 \"\" 0 0 0>\n");//Source to Z0 line
    s += QString("<160 -30 190 -30 \"\" 0 0 0 \"\">\n");//Z0 line to branches
    s += QString("<190 -90 190 -30 \"\" 0 0 0 \"\">\n");//Z0 line to upper branch
    s += QString("<190 -90 230 -90 \"\" 0 0 0 \"\">\n");//Z0 line (corner) to upper branch
    s += QString("<290 -90 340 -90 \"\" 0 0 0 \"\">\n");//Upper branch to isolation resistor
    s += QString("<340 -90 340 -50 \"\" 0 0 0 \"\">\n");//Upper branch (corner) to isolation resistor
    s += QString("<190 -30 190 30 \"\" 0 0 0 \"\">\n");//Z0 line to lower branch
    s += QString("<190 30 230 30 \"\" 0 0 0 \"\">\n");//Z0 line (corner) to lower branch
    s += QString("<290 30 340 30 \"\" 0 0 0 \"\">\n");//Lower branch to isolation resistor
    s += QString("<340 10 340 30 \"\" 0 0 0 \"\">\n");//Lower branch (corner) to isolation resistor

    if (LumpedElements)
    {
       s += QString("<90 -30 180 -30 \"\" 0 0 0>\n");
       s += QString("<310 -110 310 -90 \"\" 0 0 0>\n");//Upper branch
       s += QString("<310 30 310 50 \"\" 0 0 0>\n");//Lower branch
    }

    if (K!=1)//Unequal power split ratio => need additional matching
    {
        if (LumpedElements)
        {
         s += QString("<340 -90 380 -90 \"\" 0 0 0 \"\">\n");//Upper branch, R to L
         s += QString("<440 -90 500 -90 \"\" 0 0 0 \"\">\n");//Upper branch, L to port
         s += QString("<450 -110 450 -90 \"\" 0 0 0 \"\">\n");//Upper branch, L to C
         s += QString("<340 30 380 30 \"\" 0 0 0 \"\">\n");//Lower branch, R to L
         s += QString("<440 30 500 30 \"\" 0 0 0 \"\">\n");//Lower branch, L to port
         s += QString("<450 30 450 60 \"\" 0 0 0 \"\">\n");//Lower branch, L to C
        }
        else//Transmission lines
        {
         s += QString("<340 -90 380 -90 \"\" 0 0 0 \"\">\n");//Isolation resistor to matching line. Upper branch
         s += QString("<440 -90 500 -90 \"\" 0 0 0 \"\">\n");//Matching line to port 2. Upper branch
         s += QString("<340 30 380 30 \"\" 0 0 0 \"\">\n");//Isolation resistor to matching line. Lowe branch
         s += QString("<440 30 500 30 \"\" 0 0 0 \"\">\n");//Matching line to port 2. Lower branch
        }
    }
    else//Equal power split ratio
    {
        s += QString("<340 -90 500 -90 \"\" 0 0 0 \"\">\n");//Branch 2 to Port 2
        s += QString("<340 30 500 30 \"\" 0 0 0 \"\">\n");//Branch 2 to Port 3
    }

    s += "</Wires>\n";

    QApplication::clipboard()->setText(s, QClipboard::Clipboard);//Copy into clipboard
    return 0;

}

//-----------------------------------------------------------------------------------
// This function calculates a multistage lambda/4 matching using the Chebyshev weigthing.
// See Microwave Engineering. David Pozar. John Wiley and Sons. 4th Edition. Pg 256-261
QString QucsPowerCombiningTool::calcChebyLines(double RL, double Z0, double gamma, int N)
{
    if (N > 7)// So far, it is only available Chebyshev weighting up to 7 sections.
        // Probably, it makes no sense to use a higher number of sections because of the losses
    {
        QMessageBox::warning(0, QObject::tr("Error"),
                             QObject::tr("Chebyshev weighting for N>7 is not available"));
        return QString("");
    }
    QString s;
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

    double Zaux=Z0, Zi;
    for (int i = 0; i < N; i++)
    {
        (RL<Z0) ? Zi = exp(log(Zaux) - gamma*w[i]):Zi = exp(log(Zaux) + gamma*w[i]); // When RL<Z0, Z_{i}<Z_{i-1}
        Zaux=Zi;
        s+=QString("%1;").arg(Zi);

    }
    return s;
}

// This function calculates the isolation resistors given the impedance of the quarter wave lines
QString QucsPowerCombiningTool::calcMultistageWilkinsonIsolators(double Freq, QString Zlines, double L, std::complex<double> gamma, int NStages, double Z0)
{
  double Z_, R, Zaux = Zlines.section(';', NStages-1, NStages-1).toDouble();
  QString s;
  std::complex<double> Zi = 0;
  for (int i=NStages-1; i>=0;i--)
  {
    Z_ = abs(Zaux*(Zi + Zaux*tanh(gamma*L))/(Zaux+Zi*tanh(gamma*L)));
    Zaux  = Zlines.section(';', i-1, i-1).toDouble();
    R = Z0*Z_/(Z_ - Z0);
    Zi = Z0;
    s +=QString("%1;").arg(2*R);
  }
  return s;
}

//------------------------------------------------------------------------------
// This function synthesizes a multistage Wilkinson power divider.
// References:
// [1] A class of broadband three-port TEM-mode hybrids. Seymour B. Cohn. IEEE
// transactions on microwave theory and techniques. vol MTT-16, No 2, February 1968
// The approximation of the isolation resistances was taken from
// http://www.mathworks.com/matlabcentral/fileexchange/22996-rf-utilities-v1-2/content/RFutils_M/bwilk.m
int QucsPowerCombiningTool::MultistageWilkinson(double Z0, double Freq, int NStages, bool SP_block, bool microcheck, tSubstrate Substrate, double Alpha, bool LumpedElements)
{
    QString Zlines = calcChebyLines(2*Z0, Z0, 0.05, NStages);
    double er, width;
    microcheck ? er=Substrate.er : 1;
    double lambda = SPEED_OF_LIGHT/(Freq);
    double lambda4=lambda/4, W;
    double alpha;
    double C[NStages], L[NStages];
    if (LumpedElements)//CLC pi equivalent calculation
    {
        double w = 2*pi*Freq;
        int j = 0;
        for (int i = NStages-1; i>= 0;i--, j++)
        {
            double Zi = Zlines.section(';', i, i).toDouble();
            L[j] = Zi/w;
            C[j] = 1./(L[j]*w*w);
        }

    }



    if (microcheck)//Microstrip implementation
    {
    double Rs = sqrt((2*pi*Freq*4*pi*1e-7)/Substrate.resistivity);
    getMicrostrip(Z0, Freq, &Substrate, W, er);
    alpha = Rs/(Z0*W);//Conductor attenuation coefficient in (Np/m)
    }
    else
    {
      alpha = log(pow(0.1*Alpha, 10));//Alpha is given in dB/m, then it is necessary to convert it into Np/m units
    }
    std::complex<double> gamma(alpha, 2*pi/lambda);//It is only considered the attenation of the metal conductor since it tends to be much higher than the dielectric
    QString Risol = calcMultistageWilkinsonIsolators(Freq, Zlines, lambda4, gamma, NStages, Z0);


    QString wirestr = "<Wires>\n", str;
    QString s = "<Qucs Schematic " PACKAGE_VERSION ">\n";
    s += "<Components>\n";
    if (SP_block)//Add S-param simulation
    {
        //Source
        s += QString("<Pac P1 1 0 30 18 -26 0 1 \"1\" 1 \"%1 Ohm\" 1 \"0 dBm\" 0 \"1 GHz\" 0>\n").arg(Z0);
        s += QString("<GND * 1 0 60 0 0 0 0>\n");
        wirestr +=QString("<0 0 0 -30 \"\" 0 0 0>\n");//Vertical wire
        wirestr +=QString("<0 -30 70 -30 \"\" 0 0 0>\n");//Horizontal wire
        //S-parameter analysis component
        double freq_start = std::max(0., Freq-1e9);
        double freq_stop = Freq+1e9;
        s += QString("<.SP SP1 1 200 200 0 67 0 0 \"lin\" 1 \"%2Hz\" 1 \"%3Hz\" 1 \"300\" 1 \"no\" 0 \"1\" 0 \"2\" 0>\n").arg((freq_start)).arg((freq_stop));
        str += QString("\"S11_dB=dB(S[1,1])\" 1 ");
        str += QString("\"S21_dB=dB(S[2,1])\" 1 ");
        str +=QString("\"S31_dB=dB(S[3,1])\" 1 ");
        s += QString("<Eqn Eqn1 1 50 200 -28 15 0 0 ") + str + QString("\"yes\" 0>\n");
        if (microcheck)s += QString("<SUBST Sub1 1 400 200 -30 24 0 0 \"%1\" 1 \"%2mm\" 1 \"%3um\" 1 \"%4\" 1 \"%5\" 1 \"%6\" 1>\n").arg(Substrate.er).arg(Substrate.height*1e3).arg(Substrate.thickness*1e6).arg(Substrate.tand).arg(Substrate.resistivity).arg(Substrate.roughness);

    }

    int x=100;
    int spacing = 150;//Spacing between sections
    double Zi, Ri;
    if (microcheck)//Microstrip
    {
        er = Substrate.er;
        getMicrostrip(Z0, Freq, &Substrate, width, er);
        s += QString("<MLIN MS1 1 %3 -30 -26 20 0 0 \"Sub1\" 0 \"%1\" 0 \"%2\" 0 \"Hammerstad\" 0 \"Kirschning\" 0 \"26.85\" 0>\n").arg(ConvertLengthFromM(width)).arg(ConvertLengthFromM(lambda4/sqrt(er))).arg(x);
    }
    else
    {
       if (LumpedElements)//LC elements. Pi CLC equivalent of a lambda/4 line
       {
        //First capacitor
        s += QString("<C C1 1 %2 0 -35 -80 0 3 \"%1\" 1 \"\" 0 \"neutral\" 0>\n").arg(num2str(2*C[0])).arg(x);
        s += QString("<GND * 1 %2 30 0 1 0 0>\n").arg(x);
        wirestr +=QString("<%1 -30 %2 -30 \"\" 0 0 0>\n").arg(x-30).arg(x+30);
       }
       else//Ideal transmission lines
       {
       s += QString("<TLIN Line1 1 %1 -30 -26 20 0 0 \"%2 Ohm\" 1 \"%3\" 1 \"%4 dB\" 0 \"26.85\" 0>\n").arg(x).arg(RoundVariablePrecision(Z0)).arg(ConvertLengthFromM(lambda4)).arg(Alpha);//Z0 line
       }

    }
    wirestr +=QString("<%1 -90 %1 30 \"\" 0 0 0>\n").arg(x+30);
    int aux=1;
    for (int i = NStages-1; i>= 0;i--, aux++)
    {
        Zi = Zlines.section(';', i, i).toDouble();
        Ri = Risol.section(';', (NStages-1)-i, (NStages-1)-i).toDouble();

        wirestr +=QString("<%1 30 %2 30 \"\" 0 0 0>\n").arg(x+30).arg(x+70);
        wirestr +=QString("<%1 -90 %2 -90 \"\" 0 0 0>\n").arg(x+30).arg(x+70);

        wirestr +=QString("<%1 30 %2 30 \"\" 0 0 0>\n").arg(x+130).arg(x+180);
        wirestr +=QString("<%1 -90 %2 -90 \"\" 0 0 0>\n").arg(x+130).arg(x+180);

        //Wiring the isolation resistor
        wirestr +=QString("<%1 30 %1 10 \"\" 0 0 0>\n").arg(x+160);
        wirestr +=QString("<%1 -90 %1 -50 \"\" 0 0 0>\n").arg(x+160);

        if (microcheck)
        {
            er = Substrate.er;
            getMicrostrip(Zi, Freq, &Substrate, width, er);
            s += QString("<MLIN MS1 1 %3 -90 -30 -73 0 0 \"Sub1\" 0 \"%1\" 1 \"%2\" 1 \"Hammerstad\" 0 \"Kirschning\" 0 \"26.85\" 0>\n").arg(ConvertLengthFromM(width)).arg(ConvertLengthFromM(lambda4/sqrt(er))).arg(x+100);
            er = Substrate.er;
            getMicrostrip(Zi, Freq, &Substrate, width, er);
            s += QString("<MLIN MS1 1 %3 30 -26 20 0 0 \"Sub1\" 0 \"%1\" 1 \"%2\" 1 \"Hammerstad\" 0 \"Kirschning\" 0 \"26.85\" 0>\n").arg(ConvertLengthFromM(width)).arg(ConvertLengthFromM(lambda4/sqrt(er))).arg(x+100);
        }
        else
        {
            if (LumpedElements)//Lumped equivalent
            {
               if (i == 0)//Last element
               {
                 // Upper branch
                 s += QString("<L L2 1 %2 -90 -31 9 0 0 \"%1\" 1 \"\" 0 \"neutral\" 0>\n").arg(num2str(L[aux-1])).arg(x+100);
                 s += QString("<C C1 1 %2 -150 14 -21 0 3 \"%1\" 1 \"\" 0 \"neutral\" 0>\n").arg(num2str(C[aux-1])).arg(x+160);
                 wirestr +=QString("<%1 -90 %1 -120 \"\" 0 0 0>\n").arg(x+160);
                 s += QString("<GND * 1 %2 -180 0 0 0 2>\n").arg(x+160);

                 // Lower branch
                 s += QString("<L L2 1 %2 30 -31 9 0 0 \"%1\" 1 \"\" 0 \"neutral\" 0>\n").arg(num2str(L[aux-1])).arg(x+100);
                 s += QString("<C C1 1 %2 90 14 -23 0 3 \"%1\" 1 \"\" 0 \"neutral\" 0>\n").arg(num2str(C[aux-1])).arg(x+160);
                 wirestr +=QString("<%1 30 %1 60 \"\" 0 0 0>\n").arg(x+160);
                 s += QString("<GND * 1 %2 120 0 0 0 0>\n").arg(x+160);
                         
                }
                else
                {
                 // Upper branch
                 s += QString("<L L2 1 %2 -90 -31 9 0 0 \"%1\" 1 \"\" 0 \"neutral\" 0>\n").arg(num2str(L[aux-1])).arg(x+100);
                 s += QString("<C C1 1 %2 -150 14 -21 0 3 \"%1\" 1 \"\" 0 \"neutral\" 0>\n").arg(num2str(C[aux]+C[aux-1])).arg(x+160);
                 wirestr +=QString("<%1 -90 %1 -120 \"\" 0 0 0>\n").arg(x+160);
                 s += QString("<GND * 1 %2 -180 0 0 0 2>\n").arg(x+160);

                 // Lower branch
                 s += QString("<L L2 1 %2 30 -31 9 0 0 \"%1\" 1 \"\" 0 \"neutral\" 0>\n").arg(num2str(L[aux-1])).arg(x+100);
                 s += QString("<C C1 1 %2 90 14 -23 0 3 \"%1\" 1 \"\" 0 \"neutral\" 0>\n").arg(num2str(C[aux]+C[aux-1])).arg(x+160);
                 wirestr +=QString("<%1 30 %1 60 \"\" 0 0 0>\n").arg(x+160);
                 s += QString("<GND * 1 %2 120 0 0 0 0>\n").arg(x+160);
               }
            }
            else//Ideal transmission lines
            {
            s += QString("<TLIN Line1 1 %1 -90 -30 -73 0 0 \"%2 Ohm\" 1 \"%3\" 1 \"%4 dB\" 0 \"26.85\" 0>\n").arg(x+100).arg(RoundVariablePrecision(Zi)).arg(ConvertLengthFromM(lambda4)).arg(Alpha);//Upper branch
            s += QString("<TLIN Line1 1 %1 30 -26 20 0 0 \"%2 Ohm\" 1 \"%3\" 1 \"%4 dB\" 0 \"26.85\" 0>\n").arg(x+100).arg(RoundVariablePrecision(Zi)).arg(ConvertLengthFromM(lambda4)).arg(Alpha);//Lower branch
            }
        }
        s += QString("<R R1 1 %1 -20 11 -25 0 3 \"%2 Ohm\" 1 \"26.85\" 0 \"US\" 0>\n").arg(x+160).arg(RoundVariablePrecision(Ri));//Isolation resistor
        x+=spacing;

        if(SP_block && (i==1))//Add output ports at the last stage
        {
                s += QString("<Pac P1 1 %1 60 18 -26 0 1 \"1\" 1 \"%2 Ohm\" 1 \"0 dBm\" 0 \"1 GHz\" 0>\n").arg(x+300).arg(Z0);
                s += QString("<GND * 1 %1 90 0 0 0 0>\n").arg(x+300);
                wirestr +=QString("<%1 30 %2 30 \"\" 0 0 0>\n").arg(x+150).arg(x+300);
                s += QString("<Pac P1 1 %1 -60 18 -26 0 1 \"1\" 1 \"%2 Ohm\" 1 \"0 dBm\" 0 \"1 GHz\" 0>\n").arg(x+300).arg(Z0);
                s += QString("<GND * 1 %1 -30 0 0 0 0>\n").arg(x+300);
                wirestr +=QString("<%1 -90 %2 -90 \"\" 0 0 0>\n").arg(x+150).arg(x+300);
        }
      }
      s += "</Components>\n";
      wirestr+="</Wires>\n";;
      s += wirestr;
      QApplication::clipboard()->setText(s, QClipboard::Clipboard);
      return 0;
}

//------------------------------------------------------------------------------
// This function generates the schematic of a tee power divider
// Reference: "High Efficiency RF and Microwave Solid State Power Amplifiers". Paolo Colantonio, Franco Giannini and Ernesto Limiti, 2009, Wiley
int QucsPowerCombiningTool::Tee(double Z0, double Freq, double K, bool SP_block, bool microcheck, tSubstrate Substrate, double Alpha)
{
    double er, width;
    double lambda4=SPEED_OF_LIGHT/(4*Freq);

    QString s = "<Qucs Schematic " PACKAGE_VERSION ">\n";    s += "<Components>\n";
    if (SP_block)
    {
        //Source
        s += QString("<Pac P1 1 0 0 18 -26 0 1 \"1\" 1 \"%1 Ohm\" 1 \"0 dBm\" 0 \"1 GHz\" 0>\n").arg(Z0);
        s += QString("<GND * 1 0 30 0 0 0 0>\n");
        //Output port 1
        s += QString("<Pac P1 1 450 -60 18 -26 0 1 \"1\" 1 \"%1 Ohm\" 1 \"0 dBm\" 0 \"1 GHz\" 0>\n").arg(2*Z0);
        s += QString("<GND * 1 450 -30 0 0 0 0>\n");
        //Output port 2
        s += QString("<Pac P1 1 450 60 18 -26 0 1 \"1\" 1 \"%1 Ohm\" 1 \"0 dBm\" 0 \"1 GHz\" 0>\n").arg(2*Z0);
        s += QString("<GND * 1 450 90 0 0 0 0>\n");
        //S-parameter analysis component
        double freq_start = std::max(0., Freq-1e9);
        double freq_stop = Freq+1e9;
        s += QString("<.SP SP1 1 200 200 0 67 0 0 \"lin\" 1 \"%2Hz\" 1 \"%3Hz\" 1 \"300\" 1 \"no\" 0 \"1\" 0 \"2\" 0>\n").arg((freq_start)).arg((freq_stop));
        s += QString("<Eqn Eqn1 1 50 200 -28 15 0 0 \"S11_dB=dB(S[1,1])\" 1 \"S21_dB=dB(S[2,1])\" 1  \"S31_dB=dB(S[3,1])\" 1 \"yes\" 0>\n");
        if (microcheck)s += QString("<SUBST Sub1 1 400 200 -30 24 0 0 \"%1\" 1 \"%2mm\" 1 \"%3um\" 1 \"%4\" 1 \"%5\" 1 \"%6\" 1>\n").arg(Substrate.er).arg(Substrate.height*1e3).arg(Substrate.thickness*1e6).arg(Substrate.tand).arg(Substrate.resistivity).arg(Substrate.roughness);

    }

    if (microcheck)
    {
        er = Substrate.er;
        getMicrostrip(Z0, Freq, &Substrate, width, er);
        s += QString("<MLIN MS1 1 120 -30 -26 20 0 0 \"Sub1\" 0 \"%1\" 1 \"%2\" 1 \"Hammerstad\" 0 \"Kirschning\" 0 \"26.85\" 0>\n").arg(ConvertLengthFromM(width)).arg(ConvertLengthFromM(lambda4/sqrt(er)));
        er = Substrate.er;
        getMicrostrip(Z0*(K+1), Freq, &Substrate, width, er);
        s += QString("<MLIN MS1 1 270 -90 -26 20 0 0 \"Sub1\" 0 \"%1\" 1 \"%2\" 1 \"Hammerstad\" 0 \"Kirschning\" 0 \"26.85\" 0>\n").arg(ConvertLengthFromM(width)).arg(ConvertLengthFromM(lambda4/sqrt(er)));
        er = Substrate.er;
        getMicrostrip(Z0*(K+1)/K, Freq, &Substrate, width, er);
        s += QString("<MLIN MS1 1 270 30 -26 20 0 0 \"Sub1\" 0 \"%1\" 1 \"%2\" 1 \"Hammerstad\" 0 \"Kirschning\" 0 \"26.85\" 0>\n").arg(ConvertLengthFromM(width)).arg(ConvertLengthFromM(lambda4/sqrt(er)));
    }
    else
    {
        s += QString("<TLIN Line1 1 120 -30 -26 20 0 0 \"%1 Ohm\" 1 \"%2\" 1 \"%3 dB\" 0 \"26.85\" 0>\n").arg(RoundVariablePrecision(Z0)).arg(ConvertLengthFromM(lambda4)).arg(Alpha);//Z0 line
        s += QString("<TLIN Line1 1 270 -90 -26 20 0 0 \"%1 Ohm\" 1 \"%2\" 1 \"%3 dB\" 0 \"26.85\" 0>\n").arg(RoundVariablePrecision(Z0*(K+1))).arg(ConvertLengthFromM(lambda4)).arg(Alpha);//Output branch 1
        s += QString("<TLIN Line1 1 270 30 -26 20 0 0 \"%1 Ohm\" 1 \"%2\" 1 \"%3 dB\" 0 \"26.85\" 0>\n").arg(RoundVariablePrecision(Z0*(K+1)/K)).arg(ConvertLengthFromM(lambda4)).arg(Alpha);//Output branch 2
    }
    if (K!=1)
    {// An unequal power ratio implies that the load impedance != 50, so it requires matching
        if (microcheck)
        {
            er = Substrate.er;
            getMicrostrip(sqrt(2*Z0*Z0*(K+1)), Freq, &Substrate, width, er);
            s += QString("<MLIN MS1 1 370 -90 -26 20 0 0 \"Sub1\" 0 \"%1\" 0 \"%2\" 0 \"Hammerstad\" 0 \"Kirschning\" 0 \"26.85\" 0>\n").arg(width).arg(lambda4/sqrt(er));
            er = Substrate.er;
            getMicrostrip(sqrt(Z0*Z0*(K+1)/K), Freq, &Substrate, width, er);
            s += QString("<MLIN MS1 1 370 30 -26 20 0 0 \"Sub1\" 0 \"%1\" 0 \"%2\" 0 \"Hammerstad\" 0 \"Kirschning\" 0 \"26.85\" 0>\n").arg(width).arg(lambda4/sqrt(er));

        }
        else
        {
            s += QString("<TLIN Line1 1 370 -90 -26 20 0 0 \"%1\" 1 \"%2\" 1 \"%3 dB\" 0 \"26.85\" 0>\n").arg(RoundVariablePrecision(sqrt(2*Z0*Z0*(K+1)))).arg(ConvertLengthFromM(lambda4)).arg(Alpha);//Quarter wave matching output branch 1
            s += QString("<TLIN Line1 1 370 30 -26 20 0 0 \"%1\" 1 \"%2\" 1 \"%3 dB\" 0 \"26.85\" 0>\n").arg(RoundVariablePrecision(sqrt(2*Z0*Z0*(K+1)/K))).arg(ConvertLengthFromM(lambda4)).arg(Alpha);//Quarter wave matching output branch 2
        }
    }
    s += "</Components>\n";

    //Wiring
    s += "<Wires>\n";
    s += QString("<0 -30 90 -30 \"\" 0 0 0>\n");//Source to Z0 line
    s += QString("<150 -30 200 -30 \"\" 0 0 0>\n");//Source to Z0 line
    s += QString("<200 30 200 -90 \"\" 0 0 0>\n");//Z0 line to branches
    s += QString("<200 30 240 30 \"\" 0 0 0>\n");//Z0 line to branch 1
    s += QString("<200 -90 240 -90 \"\" 0 0 0>\n");//Z0 line to branch 2

    s += QString("<300 30 340 30 \"\" 0 0 0>\n");//Branch 2to R
    s += QString("<300 -90 340 -90 \"\" 0 0 0>\n");//Branch 1 to R
    if (K!=1)
    {
        s += QString("<400 30 450 30 \"\" 0 0 0>\n");//Branch 2 to Port 2
        s += QString("<400 -90 450 -90 \"\" 0 0 0>\n");//Branch 2 to Port 3
    }
    else
    {
        s += QString("<340 30 450 30 \"\" 0 0 0>\n");//Branch 2 to Port 2
        s += QString("<340 -90 450 -90 \"\" 0 0 0>\n");//Branch 2 to Port 3
    }

    s += "</Wires>\n";

    QApplication::clipboard()->setText(s, QClipboard::Clipboard);
    return 0;
}

//--------------------------------------------------------------------------------
// This function generates the schematic of a Branch-line coupler
int QucsPowerCombiningTool::Branchline(double Z0, double Freq, double K, bool SP_block, bool microcheck, tSubstrate Substrate, double Alpha)
{
    double er, width;
    double lambda4=SPEED_OF_LIGHT/(4*Freq);
    double ZA = Z0*sqrt(K/(K+1));
    double ZB = Z0*sqrt(K);

    QString s = "<Qucs Schematic " PACKAGE_VERSION ">\n";    s += "<Components>\n";
    if (SP_block)
    {
        //Source
        s += QString("<Pac P1 1 50 -120 18 -26 0 1 \"1\" 1 \"%1 Ohm\" 1 \"0 dBm\" 0 \"1 GHz\" 0>\n").arg(Z0);
        s += QString("<GND * 1 50 -90 0 0 0 0>\n");
        //Output port 1
        s += QString("<Pac P1 1 400 -120 18 -26 0 1 \"1\" 1 \"%1 Ohm\" 1 \"0 dBm\" 0 \"1 GHz\" 0>\n").arg(Z0);
        s += QString("<GND * 1 400 -90 0 0 0 0>\n");
        //Output port 2
        s += QString("<Pac P1 1 400 90 18 -26 0 1 \"1\" 1 \"%1 Ohm\" 1 \"0 dBm\" 0 \"1 GHz\" 0>\n").arg(Z0);
        s += QString("<GND * 1 400 120 0 0 0 0>\n");
        //S-parameter analysis component
        double freq_start = std::max(0., Freq-1e9);
        double freq_stop = Freq+1e9;
        s += QString("<.SP SP1 1 200 200 0 67 0 0 \"lin\" 1 \"%2Hz\" 1 \"%3Hz\" 1 \"300\" 1 \"no\" 0 \"1\" 0 \"2\" 0>\n").arg((freq_start)).arg((freq_stop));
        s += QString("<Eqn Eqn1 1 50 200 -28 15 0 0 \"S11_dB=dB(S[1,1])\" 1 \"S21_dB=dB(S[2,1])\" 1  \"S31_dB=dB(S[3,1])\" 1 \"yes\" 0>\n");
        if (microcheck)s += QString("<SUBST Sub1 1 400 200 -30 24 0 0 \"%1\" 1 \"%2mm\" 1 \"%3um\" 1 \"%4\" 1 \"%5\" 1 \"%6\" 1>\n").arg(Substrate.er).arg(Substrate.height*1e3).arg(Substrate.thickness*1e6).arg(Substrate.tand).arg(Substrate.resistivity).arg(Substrate.roughness);

    }

    //Branch line coupler
    if (microcheck)
    {
        er = Substrate.er;
        getMicrostrip(ZA, Freq, &Substrate, width, er);
        s += QString("<MLIN MS1 1 220 30 -42 19 0 0 \"Sub1\" 0 \"%1\" 1 \"%2\" 1 \"Hammerstad\" 0 \"Kirschning\" 0 \"26.85\" 0>\n").arg(ConvertLengthFromM(width)).arg(ConvertLengthFromM(lambda4/sqrt(er)));
        er = Substrate.er;
        getMicrostrip(ZA, Freq, &Substrate, width, er);
        s += QString("<MLIN MS1 1 220 -90 -41 -71 0 0 \"Sub1\" 0 \"%1\" 1 \"%2\" 1 \"Hammerstad\" 0 \"Kirschning\" 0 \"26.85\" 0>\n").arg(ConvertLengthFromM(width)).arg(ConvertLengthFromM(lambda4/sqrt(er)));
        er = Substrate.er;
        getMicrostrip(ZB, Freq, &Substrate, width, er);
        s += QString("<MLIN MS1 1 150 -20 -60 -30 0 1 \"Sub1\" 0 \"%1\" 1 \"%2\" 1 \"Hammerstad\" 0 \"Kirschning\" 0 \"26.85\" 0>\n").arg(ConvertLengthFromM(width)).arg(ConvertLengthFromM(lambda4/sqrt(er)));
        er = Substrate.er;
        getMicrostrip(ZB, Freq, &Substrate, width, er);
        s += QString("<MLIN MS1 1 290 -20 22 -31 0 1 \"Sub1\" 0 \"%1\" 1 \"%2\" 1 \"Hammerstad\" 0 \"Kirschning\" 0 \"26.85\" 0>\n").arg(ConvertLengthFromM(width)).arg(ConvertLengthFromM(lambda4/sqrt(er)));
    }
    else
    {
        s += QString("<TLIN Line1 1 220 30 -42 19 0 0 \"%1 Ohm\" 1 \"%2\" 1 \"%3 dB\" 0 \"26.85\" 0>\n").arg(RoundVariablePrecision(ZA)).arg(ConvertLengthFromM(lambda4)).arg(Alpha);//Z0 line
        s += QString("<TLIN Line1 1 220 -90 -41 -71 0 0 \"%1 Ohm\" 1 \"%2\" 1 \"%3 dB\" 0 \"26.85\" 0>\n").arg(RoundVariablePrecision(ZA)).arg(ConvertLengthFromM(lambda4)).arg(Alpha);//Output branch 1
        s += QString("<TLIN Line1 1 150 -20 -94 -29 0 1 \"%1 Ohm\" 1 \"%2\" 1 \"%3 dB\" 0 \"26.85\" 0>\n").arg(RoundVariablePrecision(ZB)).arg(ConvertLengthFromM(lambda4)).arg(Alpha);//Output branch 2
        s += QString("<TLIN Line1 1 290 -20 22 -31 0 1 \"%1 Ohm\" 1 \"%2\" 1 \"%3 dB\" 0 \"26.85\" 0>\n").arg(RoundVariablePrecision(ZB)).arg(ConvertLengthFromM(lambda4)).arg(Alpha);//Output branch 2
    }
    s += QString("<R R1 1 50 90 14 -19 0 3 \"%1 Ohm\" 1 \"26.85\" 0 \"US\" 0>\n").arg(Z0);//Isolated port
    s += QString("<GND * 1 50 120 0 0 0 0>\n");
    s += "</Components>\n";

    //Wiring
    s += "<Wires>\n";
    s += QString("<150 -90 190 -90 \"\" 0 0 0 \"\">\n");
    s += QString("<150 -90 150 -50 \"\" 0 0 0 \"\">\n");
    s += QString("<290 -90 290 -50 \"\" 0 0 0 \"\">\n");
    s += QString("<250 -90 290 -90 \"\" 0 0 0 \"\">\n");
    s += QString("<250 30 290 30 \"\" 0 0 0 \"\">\n");
    s += QString("<290 10 290 30 \"\" 0 0 0 \"\">\n");
    s += QString("<150 30 190 30 \"\" 0 0 0 \"\">\n");
    s += QString("<150 10 150 30 \"\" 0 0 0 \"\">\n");
    s += QString("<290 -150 290 -90 \"\" 0 0 0 \"\">\n");
    s += QString("<290 -150 400 -150 \"\" 0 0 0 \"\">\n");
    s += QString("<150 -150 150 -90 \"\" 0 0 0 \"\">\n");
    s += QString("<50 -150 150 -150 \"\" 0 0 0 \"\">\n");
    s += QString("<150 30 150 60 \"\" 0 0 0 \"\">\n");
    s += QString("<150 30 150 60 \"\" 0 0 0 \"\">\n");
    s += QString("<50 60 150 60 \"\" 0 0 0 \"\">\n");
    s += QString("<290 30 290 60 \"\" 0 0 0 \"\">\n");
    s += QString("<290 60 400 60 \"\" 0 0 0 \"\">\n");
    s += "</Wires>\n";


    QApplication::clipboard()->setText(s, QClipboard::Clipboard);
    return 0;

}

//--------------------------------------------------------------------------------
// This function generates the schematic of a double box branchline coupler.
// Reference: Kumar, S.; Danshin, Tom, "A Multisection Broadband Impedance Transforming Branchline Quad Hybrid Suitable for MMIC Realization,"
// in Microwave Conference, 1992. 22nd European , vol.2, no., pp.1301-1306, 5-9 Sept. 1992
int QucsPowerCombiningTool::DoubleBoxBranchline(double Z0, double Freq, double K, bool SP_block, bool microcheck, tSubstrate Substrate, double Alpha)
{
    double er, width;
    double lambda4=SPEED_OF_LIGHT/(4*Freq);
    double r=1;
    double t = sqrt((1+K)*r);
    double ZA = Z0*sqrt(r*(t*t -r))/(t-r);
    double ZD = Z0*sqrt(r*(t*t -r))/(t-1);
    double ZB = Z0*sqrt(r-(r*r)/(t*t));

    QString s = "<Qucs Schematic " PACKAGE_VERSION ">\n";    s += "<Components>\n";
    if (SP_block)
    {
        //Source
        s += QString("<Pac P1 1 40 -100 18 -26 0 1 \"1\" 1 \"%1 Ohm\" 1 \"0 dBm\" 0 \"1 GHz\" 0>\n").arg(Z0);
        s += QString("<GND * 1 40 -70 0 0 0 0>\n");
        //Output port 1
        s += QString("<Pac P1 1 500 -100 18 -26 0 1 \"1\" 1 \"%1 Ohm\" 1 \"0 dBm\" 0 \"1 GHz\" 0>\n").arg(Z0);
        s += QString("<GND * 1 500 -70 0 0 0 0>\n");
        //Output port 2
        s += QString("<Pac P1 1 500 90 18 -26 0 1 \"1\" 1 \"%1 Ohm\" 1 \"0 dBm\" 0 \"1 GHz\" 0>\n").arg(Z0);
        s += QString("<GND * 1 500 120 0 0 0 0>\n");
        //S-parameter analysis component
        double freq_start = std::max(0., Freq-1e9);
        double freq_stop = Freq+1e9;
        s += QString("<.SP SP1 1 200 200 0 67 0 0 \"lin\" 1 \"%2Hz\" 1 \"%3Hz\" 1 \"300\" 1 \"no\" 0 \"1\" 0 \"2\" 0>\n").arg((freq_start)).arg((freq_stop));
        s += QString("<Eqn Eqn1 1 50 200 -28 15 0 0 \"S11_dB=dB(S[1,1])\" 1 \"S21_dB=dB(S[2,1])\" 1  \"S31_dB=dB(S[3,1])\" 1 \"yes\" 0>\n");
        if (microcheck)s += QString("<SUBST Sub1 1 400 200 -30 24 0 0 \"%1\" 1 \"%2mm\" 1 \"%3um\" 1 \"%4\" 1 \"%5\" 1 \"%6\" 1>\n").arg(Substrate.er).arg(Substrate.height*1e3).arg(Substrate.thickness*1e6).arg(Substrate.tand).arg(Substrate.resistivity).arg(Substrate.roughness);

    }

    //Branch line coupler
    if (microcheck)
    {
        er = Substrate.er;
        getMicrostrip(ZB, Freq, &Substrate, width, er);
        s += QString("<MLIN MS1 1 220 30 -26 20 0 0 \"Sub1\" 0 \"%1\" 1 \"%2\" 1 \"Hammerstad\" 0 \"Kirschning\" 0 \"26.85\" 0>\n").arg(ConvertLengthFromM(width)).arg(ConvertLengthFromM(lambda4/sqrt(er)));
        er = Substrate.er;
        getMicrostrip(ZB, Freq, &Substrate, width, er);
        s += QString("<MLIN MS1 1 220 -90 -29 -72 0 0 \"Sub1\" 0 \"%1\" 1 \"%2\" 1 \"Hammerstad\" 0 \"Kirschning\" 0 \"26.85\" 0>\n").arg(ConvertLengthFromM(width)).arg(ConvertLengthFromM(lambda4/sqrt(er)));
        er = Substrate.er;
        getMicrostrip(ZB, Freq, &Substrate, width, er);
        s += QString("<MLIN MS1 1 290 -20 19 -33 0 1 \"Sub1\" 0 \"%1\" 1 \"%2\" 1 \"Hammerstad\" 0 \"Kirschning\" 0 \"26.85\" 0>\n").arg(ConvertLengthFromM(width)).arg(ConvertLengthFromM(lambda4/sqrt(er)));
        er = Substrate.er;
        getMicrostrip(ZB, Freq, &Substrate, width, er);
        s += QString("<MLIN MS1 1 350 30 -26 20 0 0 \"Sub1\" 0 \"%1\" 1 \"%2\" 1 \"Hammerstad\" 0 \"Kirschning\" 0 \"26.85\" 0>\n").arg(ConvertLengthFromM(width)).arg(ConvertLengthFromM(lambda4/sqrt(er)));
        er = Substrate.er;
        getMicrostrip(ZB, Freq, &Substrate, width, er);
        s += QString("<MLIN MS1 1 350 -90 -35 -71 0 0 \"Sub1\" 0 \"%1\" 1 \"%2\" 1 \"Hammerstad\" 0 \"Kirschning\" 0 \"26.85\" 0>\n").arg(ConvertLengthFromM(width)).arg(ConvertLengthFromM(lambda4/sqrt(er)));
        er = Substrate.er;
        getMicrostrip(ZA, Freq, &Substrate, width, er);
        s += QString("<MLIN MS1 1 150 -20 -82 -31 0 1 \"Sub1\" 0 \"%1\" 1 \"%2\" 1 \"Hammerstad\" 0 \"Kirschning\" 0 \"26.85\" 0>\n").arg(ConvertLengthFromM(width)).arg(ConvertLengthFromM(lambda4/sqrt(er)));
        er = Substrate.er;
        getMicrostrip(ZD, Freq, &Substrate, width, er);
        s += QString("<MLIN MS1 1 420 -20 21 -33 0 1 \"Sub1\" 0 \"%1\" 1 \"%2\" 1 \"Hammerstad\" 0 \"Kirschning\" 0 \"26.85\" 0>\n").arg(ConvertLengthFromM(width)).arg(ConvertLengthFromM(lambda4/sqrt(er)));

    }
    else
    {
        s += QString("<TLIN Line1 1 220 30 -26 20 0 0 \"%1\" 1 \"%2\" 1 \"%3 dB\" 0 \"26.85\" 0>\n").arg(RoundVariablePrecision(ZB)).arg(ConvertLengthFromM(lambda4)).arg(Alpha);
        s += QString("<TLIN Line1 1 220 -90 -29 -72 0 0 \"%1\" 1 \"%2\" 1 \"%3 dB\" 0 \"26.85\" 0>\n").arg(RoundVariablePrecision(ZB)).arg(ConvertLengthFromM(lambda4)).arg(Alpha);

        s += QString("<TLIN Line1 1 290 -20 19 -33 0 1 \"%1\" 1 \"%2\" 1 \"%3 dB\" 0 \"26.85\" 0>\n").arg(RoundVariablePrecision(ZB)).arg(ConvertLengthFromM(lambda4)).arg(Alpha);


        s += QString("<TLIN Line1 1 350 30 -26 20 0 0 \"%1\" 1 \"%2\" 1 \"%3 dB\" 0 \"26.85\" 0>\n").arg(RoundVariablePrecision(ZB)).arg(ConvertLengthFromM(lambda4)).arg(Alpha);
        s += QString("<TLIN Line1 1 350 -90 -35 -71 0 0 \"%1\" 1 \"%2\" 1 \"%3 dB\" 0 \"26.85\" 0>\n").arg(RoundVariablePrecision(ZB)).arg(ConvertLengthFromM(lambda4)).arg(Alpha);

        s += QString("<TLIN Line1 1 150 -20 -82 -31 0 1 \"%1\" 1 \"%2\" 1 \"%3 dB\" 0 \"26.85\" 0>\n").arg(RoundVariablePrecision(ZA)).arg(ConvertLengthFromM(lambda4)).arg(Alpha);
        s += QString("<TLIN Line1 1 420 -20 21 -33 0 1 \"%1\" 1 \"%2\" 1 \"%3 dB\" 0 \"26.85\" 0>\n").arg(RoundVariablePrecision(ZD)).arg(ConvertLengthFromM(lambda4)).arg(Alpha);
    }
    s += QString("<R R1 1 40 90 30 -26 0 -1 \"%1 Ohm\" 1 \"26.85\" 0 \"US\" 0>\n").arg(Z0);//Isolated port
    s += QString("<GND * 1 40 120 0 0 0 0>\n");
    s += "</Components>\n";

    //Wiring
    s += "<Wires>\n";
    s += QString("<290 -90 290 -50 \"\" 0 0 0 \"\">\n");
    s += QString("<290 10 290 30 \"\" 0 0 0 \"\">\n");
    s += QString("<290 -90 320 -90 \"\" 0 0 0 \"\">\n");
    s += QString("<250 -90 290 -90 \"\" 0 0 0 \"\">\n");
    s += QString("<290 30 320 30 \"\" 0 0 0 \"\">\n");
    s += QString("<250 30 290 30 \"\" 0 0 0 \"\">\n");
    s += QString("<380 -90 420 -90 \"\" 0 0 0 \"\">\n");
    s += QString("<420 -90 420 -50 \"\" 0 0 0 \"\">\n");
    s += QString("<380 30 420 30 \"\" 0 0 0 \"\">\n");
    s += QString("<420 10 420 30 \"\" 0 0 0 \"\">\n");
    s += QString("<40 30 40 60 \"\" 0 0 0 \"\">\n");
    s += QString("<40 30 150 30 \"\" 0 0 0 \"\">\n");
    s += QString("<150 30 190 30 \"\" 0 0 0 \"\">\n");
    s += QString("<150 10 150 30 \"\" 0 0 0 \"\">\n");
    s += QString("<150 -90 150 -50 \"\" 0 0 0 \"\">\n");
    s += QString("<150 -90 190 -90 \"\" 0 0 0 \"\">\n");
    s += QString("<420 -160 420 -90 \"\" 0 0 0 \"\">\n");
    s += QString("<420 -160 500 -160 \"\" 0 0 0 \"\">\n");
    s += QString("<500 -160 500 -130 \"\" 0 0 0 \"\">\n");
    s += QString("<500 30 500 60 \"\" 0 0 0 \"\">\n");
    s += QString("<420 30 500 30 \"\" 0 0 0 \"\">\n");
    s += QString("<130 -160 150 -90 \"\" 0 0 0 \"\">\n");
    s += QString("<40 -160 150 -160 \"\" 0 0 0 \"\">\n");
    s += QString("<40 -160 40 -130 \"\" 0 0 0 \"\">\n");
    s += "</Wires>\n";


    QApplication::clipboard()->setText(s, QClipboard::Clipboard);
    return 0;

}

//-----------------------------------------------------------------------------
// This function generates the schematic of a N-way Bagley combiner (N odd)
// Reference: "High Efficiency RF and Microwave Solid State Power Amplifiers". Paolo Colantonio, Franco Giannini and Ernesto Limiti, 2009, Wiley. Pg. 411
int QucsPowerCombiningTool::Bagley(double Z0, double Freq, int N, bool SP_block, bool microcheck, tSubstrate Substrate, double Alpha)
{
    if (N % 2 == 0)
    {
        N++;
        QString str = QString("The number of outputs must be an odd number. N=%1 will be used instead").arg(N);
        QMessageBox::warning(this, tr("Bagley"), str, QMessageBox::Close);
    }
    double er, width;
    double lambda4=SPEED_OF_LIGHT/(4*Freq);
    double lambda2=lambda4*2;

    double Zbranch = 2*Z0/sqrt(N);
    QString s = "<Qucs Schematic " PACKAGE_VERSION ">\n";
    s += "<Components>\n";
    if (SP_block)
    {
        //Source
        s += QString("<Pac P1 1 0 -60 18 -26 0 1 \"1\" 1 \"%1 Ohm\" 1 \"0 dBm\" 0 \"1 GHz\" 0>\n").arg(Z0);
        s += QString("<GND * 1 0 -30 0 0 0 0>\n");
        double freq_start = std::max(0., Freq-1e9);
        double freq_stop = Freq+1e9;
        s += QString("<.SP SP1 1 200 200 0 67 0 0 \"lin\" 1 \"%2Hz\" 1 \"%3Hz\" 1 \"300\" 1 \"no\" 0 \"1\" 0 \"2\" 0>\n").arg((freq_start)).arg((freq_stop));
        QString str = QString(" \"S11_dB=dB(S[1,1])\" 1 ");
        for (int i=2;i<=N+1; i++) str += QString("\"S%1_dB=dB(S[%2,1])\" 1 ").arg(10*i+1).arg(i);
        s += QString("<Eqn Eqn1 1 50 200 -28 15 0 0") + str + QString("\"yes\" 0>\n");
        if (microcheck)s += QString("<SUBST Sub1 1 400 200 -30 24 0 0 \"%1\" 1 \"%2mm\" 1 \"%3um\" 1 \"%4\" 1 \"%5\" 1 \"%6\" 1>\n").arg(Substrate.er).arg(Substrate.height*1e3).arg(Substrate.thickness*1e6).arg(Substrate.tand).arg(Substrate.resistivity).arg(Substrate.roughness);
    }

    //Input section
    if (microcheck)
    {
        er = Substrate.er;
        getMicrostrip(Zbranch, Freq, &Substrate, width, er);
        s += QString("<MLIN MS1 1 100 -140 21 -28 0 1 \"Sub1\" 0 \"%1\" 1 \"%2\" 1 \"Hammerstad\" 0 \"Kirschning\" 0 \"26.85\" 0>\n").arg(ConvertLengthFromM(width)).arg(ConvertLengthFromM(lambda4/sqrt(er)));
        er = Substrate.er;
        getMicrostrip(Zbranch, Freq, &Substrate, width, er);
        s += QString("<MLIN MS1 1 100 -30 19 -60 0 1 \"Sub1\" 0 \"%1\" 1 \"%2\" 1 \"Hammerstad\" 0 \"Kirschning\" 0 \"26.85\" 0>\n").arg(ConvertLengthFromM(width)).arg(ConvertLengthFromM(lambda4/sqrt(er)));

    }
    else
    {
        s += QString("<TLIN Line1 1 100 -140 21 -28 0 1 \"%1 Ohm\" 1 \"%2\" 1 \"%3 dB\" 0 \"26.85\" 0>\n").arg(RoundVariablePrecision(Zbranch)).arg(ConvertLengthFromM(lambda4)).arg(Alpha);
        s += QString("<TLIN Line1 1 100 -30 19 -60 0 1 \"%1 Ohm\" 1 \"%2\" 1 \"%3 dB\" 0 \"26.85\" 0>\n").arg(RoundVariablePrecision(Zbranch)).arg(ConvertLengthFromM(lambda4)).arg(Alpha);
    }
    //Output branches
    int x = 240;
    s += QString("<Pac P1 1 %1 60 18 -26 0 1 \"1\" 1 \"%2 Ohm\" 1 \"0 dBm\" 0 \"1 GHz\" 0>\n").arg(x-100).arg(Z0);
    s += QString("<GND * 1 %1 90 0 0 0 0>\n").arg(x-100);
    for (int i=1;i<N; i++)
    {
        if (microcheck)
        {
            er = Substrate.er;
            getMicrostrip(Zbranch, Freq, &Substrate, width, er);
            s += QString("<MLIN MS1 1 %1 30 -34 -67 0 0 \"Sub1\" 0 \"%2\" 1 \"%3\" 1 \"Hammerstad\" 0 \"Kirschning\" 0 \"26.85\" 0>\n").arg(x).arg(ConvertLengthFromM(width)).arg(ConvertLengthFromM(lambda2/sqrt(er)));
        }
        else
        {
            s += QString("<TLIN Line1 1 %1 30 -34 -67 0 0 \"%2 Ohm\" 1 \"%3\" 1 \"%4 dB\" 0 \"26.85\" 0>\n").arg(x).arg(RoundVariablePrecision(Zbranch)).arg(ConvertLengthFromM(lambda2)).arg(Alpha);
        }
        x+=100;
        if (SP_block)
        {
            //i-th output port
            s += QString("<Pac P1 1 %1 60 18 -26 0 1 \"1\" 1 \"%2 Ohm\" 1 \"0 dBm\" 0 \"1 GHz\" 0>\n").arg(x).arg(Z0);
            s += QString("<GND * 1 %1 90 0 0 0 0>\n").arg(x);
        }
        x+=100;
    }

    s += "</Components>\n";


    //Wiring
    s += "<Wires>\n";
    s += QString("<0 -90 100 -90 \"\" 0 0 0>\n");//Source to lambda/4 lines
    s += QString("<100 -110 100 -60 \"\" 0 0 0>\n");//lambda/4 lines
    s += QString("<100 30 140 30 \"\" 0 0 0>\n");//Lower lambda/4 line to the first lambda/2 section
    s += QString("<100 30 100 0 \"\" 0 0 0>\n");//Lower lambda/4 line to the first lambda/2 section

    //Wiring the rest of the lambda/2 sections
    x=140;
    for (int i=1;i<N;i++)
    {
        s += QString("<%1 30 %2 30 \"\" 0 0 0>\n").arg(x).arg(x+70);
        s += QString("<%1 30 %2 30 \"\" 0 0 0>\n").arg(x+130).arg(x+200);
        x+=200;
    }
    
    s += QString("<%1 30 %2 30 \"\" 0 0 0>\n").arg(x).arg(x+30);
    s += QString("<%1 30 %1 -200 \"\" 0 0 0>\n").arg(x+30);//Final lambda/2 section to the upper lambda/4 section. Vertical line
    s += QString("<100 -200 %1 -200 \"\" 0 0 0>\n").arg(x+30);//Final lambda/2 section to the upper lambda/4 section. Horizontal line
    s += QString("<100 -170 100 -200 \"\" 0 0 0>\n");
    s += "</Wires>\n";


    QApplication::clipboard()->setText(s, QClipboard::Clipboard);
    return 0;

}

//---------------------------------------------------------------------------
// This function generates a Gysel combiner
int QucsPowerCombiningTool::Gysel(double Z0, double Freq, bool SP_block, bool microcheck, tSubstrate Substrate, double Alpha)
{
    double er, width;
    double lambda4=SPEED_OF_LIGHT/(4*Freq);

    QString s = "<Qucs Schematic " PACKAGE_VERSION ">\n";
    s += "<Components>\n";
    if (SP_block)
    {
        //Source
        s += QString("<Pac P1 1 0 0 18 -26 0 1 \"1\" 1 \"%1 Ohm\" 1 \"0 dBm\" 0 \"1 GHz\" 0>\n").arg(Z0);
        s += QString("<GND * 1 0 30 0 0 0 0>\n");
        //Output port 1
        s += QString("<Pac P1 1 30 140 -96 -27 0 1 \"1\" 1 \"%1 Ohm\" 1 \"0 dBm\" 0 \"1 GHz\" 0>\n").arg(Z0);
        s += QString("<GND * 1 30 170 0 0 0 0>\n");
        //Output port 2
        s += QString("<Pac P1 1 30 -160 -96 -27 0 1 \"1\" 1 \"%1 Ohm\" 1 \"0 dBm\" 0 \"1 GHz\" 0>\n").arg(Z0);
        s += QString("<GND * 1 30 -130 0 0 0 0>\n");

        //S-parameter analysis component
        double freq_start = std::max(0., Freq-1e9);
        double freq_stop = Freq+1e9;
        s += QString("<.SP SP1 1 200 250 0 67 0 0 \"lin\" 1 \"%2Hz\" 1 \"%3Hz\" 1 \"300\" 1 \"no\" 0 \"1\" 0 \"2\" 0>\n").arg((freq_start)).arg((freq_stop));
        s += QString("<Eqn Eqn1 1 50 250 -28 15 0 0 \"S11_dB=dB(S[1,1])\" 1 \"S21_dB=dB(S[2,1])\" 1  \"S31_dB=dB(S[3,1])\" 1 \"yes\" 0>\n");
        if (microcheck)s += QString("<SUBST Sub1 1 400 250 -30 24 0 0 \"%1\" 1 \"%2mm\" 1 \"%3um\" 1 \"%4\" 1 \"%5\" 1 \"%6\" 1>\n").arg(Substrate.er).arg(Substrate.height*1e3).arg(Substrate.thickness*1e6).arg(Substrate.tand).arg(Substrate.resistivity).arg(Substrate.roughness);


    }

    if (microcheck)
    {
        er = Substrate.er;
        getMicrostrip(sqrt(2)*Z0, Freq, &Substrate, width, er);
        s += QString("<MLIN MS1 1 120 -70 21 -30 0 1 \"Sub1\" 0 \"%1\" 1 \"%2\" 1 \"Hammerstad\" 0 \"Kirschning\" 0 \"26.85\" 0>\n").arg(ConvertLengthFromM(width)).arg(ConvertLengthFromM(lambda4/sqrt(er)));
        er = Substrate.er;
        getMicrostrip(sqrt(2)*Z0, Freq, &Substrate, width, er);
        s += QString("<MLIN MS1 1 120 40 18 -27 0 1 \"Sub1\" 0 \"%1\" 1 \"%2\" 1 \"Hammerstad\" 0 \"Kirschning\" 0 \"26.85\" 0>\n").arg(ConvertLengthFromM(width)).arg(ConvertLengthFromM(lambda4/sqrt(er)));
        er = Substrate.er;
        getMicrostrip(Z0, Freq, &Substrate, width, er);
        s += QString("<MLIN MS1 1 220 -130 -42 -68 0 0 \"Sub1\" 0 \"%1\" 1 \"%2\" 1 \"Hammerstad\" 0 \"Kirschning\" 0 \"26.85\" 0>\n").arg(ConvertLengthFromM(width)).arg(ConvertLengthFromM(lambda4/sqrt(er)));
        er = Substrate.er;
        getMicrostrip(Z0, Freq, &Substrate, width, er);
        s += QString("<MLIN MS1 1 220 100 -26 20 0 0 \"Sub1\" 0 \"%1\" 1 \"%2\" 1 \"Hammerstad\" 0 \"Kirschning\" 0 \"26.85\" 0>\n").arg(ConvertLengthFromM(width)).arg(ConvertLengthFromM(lambda4/sqrt(er)));
        er = Substrate.er;
        getMicrostrip(Z0/sqrt(2), Freq, &Substrate, width, er);
        s += QString("<MLIN MS1 1 320 -20 18 -32 0 1 \"Sub1\" 0 \"%1\" 1 \"%2\" 1 \"Hammerstad\" 0 \"Kirschning\" 0 \"26.85\" 0>\n").arg(ConvertLengthFromM(width)).arg(ConvertLengthFromM(2*lambda4/sqrt(er)));

    }
    else
    {
        s += QString("<TLIN Line1 1 120 -70 21 -30 0 1 \"%1 Ohm\" 1 \"%2\" 1 \"%3 dB\" 0 \"26.85\" 0>\n").arg(RoundVariablePrecision(sqrt(2)*Z0)).arg(ConvertLengthFromM(lambda4)).arg(Alpha);
        s += QString("<TLIN Line1 1 120 40 18 -27 0 1 \"%1 Ohm\" 1 \"%2\" 1 \"%3 dB\" 0 \"26.85\" 0>\n").arg(RoundVariablePrecision(sqrt(2)*Z0)).arg(ConvertLengthFromM(lambda4)).arg(Alpha);
        s += QString("<TLIN Line1 1 220 -130 -42 -68 0 0 \"%1 Ohm\" 1 \"%2\" 1 \"%3 dB\" 0 \"26.85\" 0>\n").arg(Z0).arg(ConvertLengthFromM(lambda4)).arg(Alpha);
        s += QString("<TLIN Line1 1 220 100 -26 20 0 0 \"%1 Ohm\" 1 \"%2\" 1 \"%3 dB\" 0 \"26.85\" 0>\n").arg(Z0).arg(ConvertLengthFromM(lambda4)).arg(Alpha);
        s += QString("<TLIN Line1 1 320 -20 18 -32 0 1 \"%1 Ohm\" 1 \"%2\" 1 \"%3 dB\" 0 \"26.85\" 0>\n").arg(RoundVariablePrecision(Z0/sqrt(2))).arg(ConvertLengthFromM(2*lambda4)).arg(Alpha);
    }
    //Resistors
    s += QString("<R R1 1 400 -160 19 -16 0 3 \"%1 Ohm\" 1 \"26.85\" 0 \"US\" 0>\n").arg(Z0);//Isolation resistor
    s += QString("<GND * 1 400 -130 0 0 0 0>\n");
    s += QString("<R R1 1 400 130 19 -16 0 3 \"%1 Ohm\" 1 \"26.85\" 0 \"US\" 0>\n").arg(Z0);//Isolation resistor
    s += QString("<GND * 1 400 160 0 0 0 0>\n");

    s += "</Components>\n";

    s += "<Wires>\n";
    s += QString("<120 70 120 100 \"\" 0 0 0 \"\">\n");//Source to the lines at the input
    s += QString("<120 -130 120 -100 \"\" 0 0 0 \"\">\n");//Line between the two lines at the input
    s += QString("<120 -130 190 -130 \"\" 0 0 0 \"\">\n");
    s += QString("<120 -200 120 -130 \"\" 0 0 0 \"\">\n");
    s += QString("<250 -130 320 -130 \"\" 0 0 0 \"\">\n");
    s += QString("<320 -130 320 -50 \"\" 0 0 0 \"\">\n");
    s += QString("<120 100 190 100 \"\" 0 0 0 \"\">\n");//Line between the line on the top to the upper resistor
    s += QString("<250 100 320 100 \"\" 0 0 0 \"\">\n");//Line between the line on the top to the lower resistor
    s += QString("<320 10 320 100 \"\" 0 0 0 \"\">\n");
    s += QString("<30 -200 120 -200 \"\" 0 0 0 \"\">\n");
    s += QString("<30 -200 30 -190 \"\" 0 0 0 \"\">\n");
    s += QString("<400 -200 400 -190 \"\" 0 0 0 \"\">\n");
    s += QString("<320 -200 320 -130 \"\" 0 0 0 \"\">\n");
    s += QString("<320 -200 400 -200 \"\" 0 0 0 \"\">\n");
    s += QString("<320 100 400 100 \"\" 0 0 0 \"\">\n");
    s += QString("<30 100 120 100 \"\" 0 0 0 \"\">\n");
    s += QString("<30 100 30 110 \"\" 0 0 0 \"\">\n");
    s += QString("<120 -40 120 -30 \"\" 0 0 0 \"\">\n");
    s += QString("<120 -30 120 10 \"\" 0 0 0 \"\">\n");
    s += QString("<0 -30 120 -30 \"\" 0 0 0 \"\">\n");
    s += "</Wires>\n";

    QApplication::clipboard()->setText(s, QClipboard::Clipboard);
    return 0;
}

int QucsPowerCombiningTool::TravellingWave(double Z0, double Freq, int N, bool SP_block, bool microcheck, tSubstrate Substrate, double Alpha)
{
    double er, width;
    double lambda4=SPEED_OF_LIGHT/(4*Freq);

    QString wirestr = "<Wires>\n", str;
    QString s = "<Qucs Schematic " PACKAGE_VERSION ">\n";
    s += "<Components>\n";
    if (SP_block)
    {
        //Source
        s += QString("<Pac P1 1 0 30 18 -26 0 1 \"1\" 1 \"%1 Ohm\" 1 \"0 dBm\" 0 \"1 GHz\" 0>\n").arg(Z0);
        s += QString("<GND * 1 0 60 0 0 0 0>\n");
        wirestr +=QString("<0 0 0 -30 \"\" 0 0 0>\n");//Vertical wire
        wirestr +=QString("<0 -30 40 -30 \"\" 0 0 0>\n");//Horizontal wire
        //S-parameter analysis component
        double freq_start = std::max(0., Freq-1e9);
        double freq_stop = Freq+1e9;
        s += QString("<.SP SP1 1 200 200 0 67 0 0 \"lin\" 1 \"%2Hz\" 1 \"%3Hz\" 1 \"300\" 1 \"no\" 0 \"1\" 0 \"2\" 0>\n").arg((freq_start)).arg((freq_stop));
        for (int i=2;i<=N+1; i++) str += QString("\"S%1_dB=dB(S[%2,1])\" 1 ").arg(10*i+1).arg(i);
        s += QString("<Eqn Eqn1 1 50 200 -28 15 0 0 ") + str + QString("\"yes\" 0>\n");
        if (microcheck)s += QString("<SUBST Sub1 1 400 200 -30 24 0 0 \"%1\" 1 \"%2mm\" 1 \"%3um\" 1 \"%4\" 1 \"%5\" 1 \"%6\" 1>\n").arg(Substrate.er).arg(Substrate.height*1e3).arg(Substrate.thickness*1e6).arg(Substrate.tand).arg(Substrate.resistivity).arg(Substrate.roughness);

    }

    QString wilkstr, aux_str, aux_str_2;
    double Z2, Z3, R, R2, R3;
    int x=100;
    int spacing = 350;
    for (int n = N-1; n>0;n--)
    {
        wilkstr = CalculateWilkinson(Z0, sqrt(n));
        Z2 = wilkstr.section(';', 0, 0).toDouble();
        Z3 = wilkstr.section(';', 1, 1).toDouble();
        R =  wilkstr.section(';', 2, 2).toDouble();
        R2 =  wilkstr.section(';', 3, 3).toDouble();
        R3 =  wilkstr.section(';', 4, 4).toDouble();

        wirestr +=QString("<%1 -30 %2 -30 \"\" 0 0 0>\n").arg(x-60).arg(x-30);
        wirestr +=QString("<%1 -30 %2 -30 \"\" 0 0 0>\n").arg(x+30).arg(x+60);

        wirestr +=QString("<%1 30 %2 30 \"\" 0 0 0>\n").arg(x+60).arg(x+100);
        wirestr +=QString("<%1 -90 %2 -90 \"\" 0 0 0>\n").arg(x+60).arg(x+100);

        wirestr +=QString("<%1 -90 %1 30 \"\" 0 0 0>\n").arg(x+60);

        wirestr +=QString("<%1 30 %2 30 \"\" 0 0 0>\n").arg(x+160).arg(x+200);
        wirestr +=QString("<%1 -90 %2 -90 \"\" 0 0 0>\n").arg(x+160).arg(x+200);

        wirestr +=QString("<%1 30 %2 30 \"\" 0 0 0>\n").arg(x+260).arg(x+350);
        wirestr +=QString("<%1 -90 %2 -90 \"\" 0 0 0>\n").arg(x+260).arg(x+290);
        wirestr +=QString("<%1 30 %1 60 \"\" 0 0 0>\n").arg(x+350);

        //Wiring the isolation resistor
        wirestr +=QString("<%1 30 %1 10 \"\" 0 0 0>\n").arg(x+190);
        wirestr +=QString("<%1 -90 %1 -50 \"\" 0 0 0>\n").arg(x+190);

        if(n>1)wirestr +=QString("<%1 -90 %1 -30 \"\" 0 0 0>\n").arg(x+290);
        if(n==1)wirestr +=QString("<%1 -90 %2 -90 \"\" 0 0 0>\n").arg(x+290).arg(x+350);//Last power combiner

        if (microcheck)
        {
            er = Substrate.er;
            getMicrostrip(Z0, Freq, &Substrate, width, er);
            s += QString("<MLIN MS1 1 %3 -30 -38 -68 0 0 \"Sub1\" 0 \"%1\" 1 \"%2\" 1 \"Hammerstad\" 0 \"Kirschning\" 0 \"26.85\" 0>\n").arg(width).arg(lambda4/sqrt(er)).arg(x);
            er = Substrate.er;
            getMicrostrip(Z3, Freq, &Substrate, width, er);
            s += QString("<MLIN MS1 1 %3 -90 -40 -70 0 0 \"Sub1\" 0 \"%1\" 1 \"%2\" 1 \"Hammerstad\" 0 \"Kirschning\" 0 \"26.85\" 0>\n").arg(width).arg(lambda4/sqrt(er)).arg(x+130);
            er = Substrate.er;
            getMicrostrip(Z2, Freq, &Substrate, width, er);
            s += QString("<MLIN MS1 1 %3 30 -26 20 0 0 \"Sub1\" 0 \"%1\" 1 \"%2\" 1 \"Hammerstad\" 0 \"Kirschning\" 0 \"26.85\" 0>\n").arg(width).arg(lambda4/sqrt(er)).arg(x+130);
        }
        else
        {
            aux_str = ConvertLengthFromM(lambda4);
            s += QString("<TLIN Line1 1 %1 -30 -38 -68 0 0 \"%2 Ohm\" 1 \"%3\" 1 \"%4 dB\" 0 \"26.85\" 0>\n").arg(x).arg(Z0).arg(aux_str).arg(Alpha);//Z0 line
            s += QString("<TLIN Line1 1 %1 -90 -40 -70 0 0 \"%2 Ohm\" 1 \"%3\" 1 \"%4 dB\" 0 \"26.85\" 0>\n").arg(x+130).arg(RoundVariablePrecision(Z3)).arg(aux_str).arg(Alpha);//Output branch 1
            s += QString("<TLIN Line1 1 %1 30 -26 20 0 0 \"%2 Ohm\" 1 \"%3\" 1 \"%4 dB\" 0 \"26.85\" 0>\n").arg(x+130).arg(RoundVariablePrecision(Z2)).arg(aux_str).arg(Alpha);//Output branch 2
        }
        s += QString("<R R1 1 %1 -20 30 -26 0 -1 \"%2 Ohm\" 1 \"26.85\" 0 \"US\" 0>\n").arg(x+190).arg(RoundVariablePrecision(R));//Isolation resistor

        if ((R2!=50)||(R3 != 50))
        {// An unequal power ratio implies that the load impedance != 50, so it requires matching.
            if (microcheck)
            {   
                aux_str = ConvertLengthFromM(lambda4/sqrt(er));
                aux_str_2 = RoundVariablePrecision(width);
                er = Substrate.er;
                getMicrostrip(sqrt(Z0*R3), Freq, &Substrate, width, er);
                s += QString("<MLIN MS1 1 %3 -90 -40 -70 0 0 \"Sub1\" 0 \"%1\" 1 \"%2\" 1 \"Hammerstad\" 0 \"Kirschning\" 0 \"26.85\" 0>\n").arg(aux_str_2).arg(aux_str).arg(x+230);

                er = Substrate.er;
                getMicrostrip(sqrt(Z0*R2), Freq, &Substrate, width, er);
                s += QString("<MLIN MS1 1 %3 30 -26 20 0 0 \"Sub1\" 0 \"%1\" 1 \"%2\" 1 \"Hammerstad\" 0 \"Kirschning\" 0 \"26.85\" 0>\n").arg(aux_str_2).arg(aux_str).arg(x+230);

            }
            else
            {
                s += QString("<TLIN Line1 1 %1 -90 -40 -70 0 0 \"%2 Ohm\" 1 \"%3\" 1 \"%4 dB\" 0 \"26.85\" 0>\n").arg(x+230).arg(RoundVariablePrecision(sqrt(Z0*R3))).arg(ConvertLengthFromM(lambda4)).arg(Alpha);//Quarter wave matching output branch 1
                s += QString("<TLIN Line1 1 %1 30 -26 20 0 0 \"%2 Ohm\" 1 \"%3\" 1 \"%4 dB\" 0 \"26.85\" 0>\n").arg(x+230).arg(RoundVariablePrecision(sqrt(Z0*R2))).arg(ConvertLengthFromM(lambda4)).arg(Alpha);//Quarter wave matching output branch 2
            }
        }
        else
        {
            wirestr +=QString("<%1 30 %2 30 \"\" 0 0 0>\n").arg(x+200).arg(x+260);
            wirestr +=QString("<%1 -90 %2 -90 \"\" 0 0 0>\n").arg(x+200).arg(x+260);
        }
        x+=spacing;

        if(SP_block)//Add output terms
        {
            s += QString("<Pac P1 1 %1 90 18 -26 0 1 \"1\" 1 \"%2 Ohm\" 1 \"0 dBm\" 0 \"1 GHz\" 0>\n").arg(x).arg(Z0);
            s += QString("<GND * 1 %1 120 0 0 0 0>\n").arg(x);

            if(n==1)//The last Wilkinson divider
            {
                s += QString("<Pac P1 1 %1 -60 18 -26 0 1 \"1\" 1 \"%2 Ohm\" 1 \"0 dBm\" 0 \"1 GHz\" 0>\n").arg(x).arg(Z0);
                s += QString("<GND * 1 %1 -30 0 0 0 0>\n").arg(x);
            }
        }

    }
    s += "</Components>\n";
    wirestr+="</Wires>\n";;
    s += wirestr;

    QApplication::clipboard()->setText(s, QClipboard::Clipboard);
    return 0;
}


int QucsPowerCombiningTool::Tree(double Z0, double Freq, int N, bool SP_block, bool microcheck, tSubstrate Substrate, double Alpha)
{
    if ((N & (N - 1)) != 0)//Checking if the number of outputs is power of 2
    {
        N = pow(2, ceil(log(N)/log(2)));//Rounding to the next power of 2
        QString str = QString("The number of outputs must be a power of 2. A %1-way combiner will be designed").arg(N);
        QMessageBox::warning(this, tr("Tree combiner"), str, QMessageBox::Close);
    }
    double er, width;
    double lambda4=SPEED_OF_LIGHT/(4*Freq);
    double Zbranch = sqrt(2)*Z0;
    QString wirestr = "<Wires>\n", str;
    QString s = "<Qucs Schematic " PACKAGE_VERSION ">\n";
    s += "<Components>\n";

    int x=0;
    int y=60, yaux;//Separation between the output branches of a single Wilkinson divider
    int sp=60, spaux;//Vertical spacing between Wilkinson splitters
    int offset=0,offsetaux=offset;//Vertical coordinate of the dividers
    for (int n=N/2;n>=1;n=pow(2, floor(log(n-1)/log(2))))
    {//It starts drawing the last sections so as to avoid overlapping

        if (n!=N/2)
        {
            offset = offsetaux + yaux+0.5*spaux;
            offsetaux=offset;
        }
        for (int i=1; i<= n; i++)
        {
            if (microcheck)
            {
                er = Substrate.er;
                getMicrostrip(Z0, Freq, &Substrate, width, er);
                s += QString("<MLIN MS1 1 %3 %4 -26 20 0 0 \"Sub1\" 0 \"%1\" 0 \"%2\" 0 \"Hammerstad\" 0 \"Kirschning\" 0 \"26.85\" 0>\n").arg(width).arg(lambda4/sqrt(er)).arg(x).arg(offset);

                er = Substrate.er;
                getMicrostrip(Z0, Freq, &Substrate, width, er);
                s += QString("<MLIN MS1 1 %3 %4 -26 20 0 0 \"Sub1\" 0 \"%1\" 0 \"%2\" 0 \"Hammerstad\" 0 \"Kirschning\" 0 \"26.85\" 0>\n").arg(width).arg(lambda4/sqrt(er)).arg(x+100).arg(y+offset);

                er = Substrate.er;
                getMicrostrip(Z0, Freq, &Substrate, width, er);
                s += QString("<MLIN MS1 1 %3 %4 -26 20 0 0 \"Sub1\" 0 \"%1\" 0 \"%2\" 0 \"Hammerstad\" 0 \"Kirschning\" 0 \"26.85\" 0>\n").arg(width).arg(lambda4/sqrt(er)).arg(x+100).arg(-y+offset);

            }
            else
            {
                s += QString("<TLIN Line1 1 %1 %2 -26 20 0 0 \"%3\" 0 \"%4\" 0 \"%5 dB\" 0 \"26.85\" 0>\n").arg(x).arg(offset).arg(Z0).arg(lambda4).arg(Alpha);//Z0 line
                s += QString("<TLIN Line1 1 %1 %2 -26 20 0 0 \"%3\" 0 \"%4\" 0 \"%5 dB\" 0 \"26.85\" 0>\n").arg(x+100).arg(y+offset).arg(Zbranch).arg(lambda4).arg(Alpha);//Output branch 1
                s += QString("<TLIN Line1 1 %1 %2 -26 20 0 0 \"%3\" 0 \"%4\" 0 \"%5 dB\" 0 \"26.85\" 0>\n").arg(x+100).arg(-y+offset).arg(Zbranch).arg(lambda4).arg(Alpha);//Output branch 2
            }
            s += QString("<R R1 1 %1 %2 30 -26 0 -1 \"%3 Ohm\" 1 \"26.85\" 0 \"US\" 0>\n").arg(x+160).arg(offset).arg(2*Z0);//Isolation resistor

            wirestr +=QString("<%1 %3 %2 %3 \"\" 0 0 0>\n").arg(x+30).arg(x+70).arg(y+offset);
            wirestr +=QString("<%1 %3 %2 %3 \"\" 0 0 0>\n").arg(x+30).arg(x+70).arg(-y+offset);

            wirestr +=QString("<%1 %2 %1 %3 \"\" 0 0 0>\n").arg(x+30).arg(y+offset).arg(-y+offset);

            wirestr +=QString("<%1 %3 %2 %3 \"\" 0 0 0>\n").arg(x+130).arg(x+170).arg(y+offset);
            wirestr +=QString("<%1 %3 %2 %3 \"\" 0 0 0>\n").arg(x+130).arg(x+170).arg(-y+offset);

            //Wiring the isolation resistor
            wirestr +=QString("<%1 %2 %1 %3 \"\" 0 0 0>\n").arg(x+160).arg(y+offset).arg(offset+30);
            wirestr +=QString("<%1 %2 %1 %3 \"\" 0 0 0>\n").arg(x+160).arg(-y+offset).arg(offset-30);

            if (SP_block)
            {
                if (n==1)//Source
                {
                    int sindex=s.indexOf("<Components>\n");
                    s.insert(sindex+13, QString("<Pac P1 1 %2 %3 18 -26 0 1 \"1\" 1 \"%1 Ohm\" 1 \"0 dBm\" 0 \"1 GHz\" 0>\n").arg(Z0).arg(x-100).arg(offset+50));
                    s += QString("<GND * 1 %1 %2 0 0 0 0>\n").arg(x-100).arg(offset+80);
                    wirestr +=QString("<%1 %2 %1 %3 \"\" 0 0 0>\n").arg(x-100).arg(offset+20).arg(offset);//Vertical wire
                    wirestr +=QString("<%2 %1 %3 %1 \"\" 0 0 0>\n").arg(offset).arg(x-100).arg(x-30);//Horizontal wire

                    //S-parameter analysis component
                    double freq_start = std::max(0., Freq-1e9);
                    double freq_stop = Freq+1e9;
                    s += QString("<.SP SP1 1 %3 %4 0 67 0 0 \"lin\" 1 \"%1Hz\" 1 \"%2Hz\" 1 \"300\" 1 \"no\" 0 \"1\" 0 \"2\" 0>\n").arg(freq_start).arg(freq_stop).arg(x-200).arg(offset+200);
                    for (int i=2;i<=N+1; i++) str += QString("\"S%1_dB=dB(S[%2,1])\" 1 ").arg(10*i+1).arg(i);
                    s += QString("<Eqn Eqn1 1 %1 %2 -28 15 0 0 ").arg(x-400).arg(offset+200) + str + QString("\"yes\" 0>\n");
                    if (microcheck)s += QString("<SUBST Sub1 1 400 200 -30 24 0 0 \"%1\" 1 \"%2mm\" 1 \"%3um\" 1 \"%4\" 1 \"%5\" 1 \"%6\" 1>\n").arg(Substrate.er).arg(Substrate.height*1e3).arg(Substrate.thickness*1e6).arg(Substrate.tand).arg(Substrate.resistivity).arg(Substrate.roughness);

                }
                if(n==N/2)//Loads
                {
                    s += QString("<Pac P1 1 %2 %3 45 -29 0 0 \"1\" 1 \"%1 Ohm\" 1 \"0 dBm\" 0 \"1 GHz\" 0>\n").arg(Z0).arg(x+200).arg(offset+y);
                    s += QString("<GND * 1 %1 %2 0 0 0 0>\n").arg(x+230).arg(offset+y);
                    s += QString("<Pac P1 1 %2 %3 45 -29 0 0 \"1\" 1 \"%1 Ohm\" 1 \"0 dBm\" 0 \"1 GHz\" 0>\n").arg(Z0).arg(x+200).arg(offset-y);
                    s += QString("<GND * 1 %1 %2 0 0 0 0>\n").arg(x+230).arg(offset-y);
                }
            }

            offset+=sp+2*y;
        }
        yaux=y;
        spaux=sp;
        y=y+0.5*sp;
        sp=2*yaux+sp;
        x-=200;

    }
    s += "</Components>\n";
    wirestr+="</Wires>\n";;
    s += wirestr;
    QApplication::clipboard()->setText(s, QClipboard::Clipboard);
    return 0;
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
void QucsPowerCombiningTool::getMicrostrip(double Z0, double freq, tSubstrate *substrate,
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



void QucsPowerCombiningTool::on_MicrostripcheckBox_clicked()
{
    bool visible = MicrostripcheckBox->isChecked();
    if (visible)
    { 
      MicrostripgroupBox->show();
      AlphaLabel->setVisible(true);
      AlphalineEdit->setVisible(true);
      AlphadBLabel->setVisible(true);
      LumpedcheckBox->setChecked(false);
      this->setFixedSize(ExtendedSize);
    }
    else
    {
      MicrostripgroupBox->hide();
      AlphaLabel->setVisible(false);
      AlphalineEdit->setVisible(false);
      AlphadBLabel->setVisible(false);
      this->setFixedSize(DefaultSize);
    }
   
   LumpedcheckBox->setChecked(false);//Deactivate LC checkbox
   switch (TopoCombo->currentIndex())//Update preview
   { 
      case 0: imgWidget->load(QString(":/bitmaps/Wilkinson.svg")); break;
      case 1: imgWidget->load(QString(":/bitmaps/MultistageWilkinson.svg"));
   }
}

void QucsPowerCombiningTool::on_LCcheckBox_clicked()
{
   if (LumpedcheckBox->isChecked())//Selected lumped components implementation
   {
      if (TopoCombo->currentIndex() == 0) imgWidget->load(QString(":/bitmaps/WilkinsonLC.svg"));
      if (TopoCombo->currentIndex() == 1) imgWidget->load(QString(":/bitmaps/MultistageWilkinsonLC.svg")); 
      MicrostripcheckBox->setChecked(false);
      MicrostripgroupBox->hide();
      
      //Hide the length unit combo
      UnitsCombo->setVisible(false);
      UnitsLabel->setVisible(false);
      
      //Set window size
      this->setFixedSize(DefaultSize);
   }
   else//Selected TL implementations
   {
      if (TopoCombo->currentIndex() == 0) imgWidget->load(QString(":/bitmaps/Wilkinson.svg"));
      if (TopoCombo->currentIndex() == 1) imgWidget->load(QString(":/bitmaps/MultistageWilkinson.svg"));

      //Show the length unit combo
      UnitsCombo->setVisible(true);
      UnitsLabel->setVisible(true);
   }
}

//Rounds a double number using the minimum number of decimal places
QString QucsPowerCombiningTool::RoundVariablePrecision(double val)
{
  int precision = 0;//By default, it takes 2 decimal places
  while (val*pow(10, precision) < 100) precision++;//Adds another decimal place if the conversion is less than 0.1, 0.01, etc
  return QString::number(val, 'F', precision);// Round to 'precision' decimals. 
}


//This function creates a string for the transmission line length
QString QucsPowerCombiningTool::ConvertLengthFromM(double len)
{
  int index = UnitsCombo->currentIndex();
  double conv=len;

  switch (index)
  {
    case 0: //milimeters
          conv *=1e3;
          return QString("%1 mm").arg(RoundVariablePrecision(conv));
    case 1: //mils
          conv *= 39370.1;
          return QString("%1 mil").arg(RoundVariablePrecision(conv));
    case 2: //microns
          conv *= 1e6;
          return QString("%1 um").arg(RoundVariablePrecision(conv));
    case 3: //nanometers
          conv *= 1e9;
          return QString("%1 nm").arg(RoundVariablePrecision(conv));
    case 4: //inch
          conv *= 39.3701;
          return QString("%1 in").arg(RoundVariablePrecision(conv));
    case 5: //ft
          conv *= 3.280841666667; 
          return QString("%1 ft").arg(RoundVariablePrecision(conv));
    case 6: //m
          return QString("%1").arg(RoundVariablePrecision(len));
  }
}

// Copied from Qucs misc class
// Converts a double number into string adding the corresponding prefix
QString QucsPowerCombiningTool::num2str(double Num)
{
  char c = 0;
  double cal = fabs(Num);
  if(cal > 1e-20) {
    cal = log10(cal) / 3.0;
    if(cal < -0.2)  cal -= 0.98;
    int Expo = int(cal);

    if(Expo >= -5) if(Expo <= 4)
      switch(Expo) {
        case -5: c = 'f'; break;
        case -4: c = 'p'; break;
        case -3: c = 'n'; break;
        case -2: c = 'u'; break;
        case -1: c = 'm'; break;
        case  1: c = 'k'; break;
        case  2: c = 'M'; break;
        case  3: c = 'G'; break;
        case  4: c = 'T'; break;
      }

    if(c)  Num /= pow(10.0, double(3*Expo));
  }

  QString Str = RoundVariablePrecision(Num);
  if(c)  Str += c;

  return Str;
}
