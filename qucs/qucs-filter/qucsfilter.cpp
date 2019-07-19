/***************************************************************************
                               qucsfilter.cpp
                              ----------------
    begin                : Wed Mar 02 2005
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

#include <cmath>
#include <stdlib.h>
#include <string>

#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QComboBox>
#include <QValidator>
#include <QTimer>
#include <QClipboard>
#include <QApplication>
#include <QGridLayout>
#include <QPixmap>

#include "lc_filter.h"
#include "tl_filter.h"
#include "eqn_filter.h"
#include "line_filter.h"
#include "cline_filter.h"
#include "stepz_filter.h"
#include "quarterwave_filter.h"
#include "qw_coupled_ring_filter.h"
#include "ccoupled_shunt_resonators.h"

#include "qf_poly.h"
#include "qf_filter.h"
#include "qf_cauer.h"
#include "qucsfilter.h"
#include "helpdialog.h"
#include "material_props.h"

QucsFilter::QucsFilter()
{
  QWidget *centralWidget = new QWidget(this);  
  setCentralWidget(centralWidget);
  
  // set application icon
  setWindowIcon(QPixmap(":/bitmaps/big.qucs.xpm"));
  setWindowTitle("Qucs Filter " PACKAGE_VERSION);

  // --------  create menubar  -------------------
  QMenu *fileMenu = new QMenu(tr("&File"));

  QAction * fileQuit = new QAction(tr("E&xit"), this);
  fileQuit->setShortcut(Qt::CTRL+Qt::Key_Q);
  connect(fileQuit, SIGNAL(activated()), SLOT(slotQuit()));

  fileMenu->addAction(fileQuit);

  QMenu *helpMenu = new QMenu(tr("&Help"), this);
  QAction * helpHelp = new QAction(tr("Help..."), this);
  helpHelp->setShortcut(Qt::Key_F1);
  connect(helpHelp, SIGNAL(activated()), SLOT(slotHelpIntro()));

  QAction * helpAbout = new QAction(tr("&About QucsFilter..."), this);
  helpMenu->addAction(helpAbout);
  connect(helpAbout, SIGNAL(activated()), SLOT(slotHelpAbout()));

  QAction * helpAboutQt = new QAction(tr("About Qt..."), this);
  helpMenu->addAction(helpAboutQt);
  connect(helpAboutQt, SIGNAL(activated()), SLOT(slotHelpAboutQt()));

  helpMenu->addAction(helpHelp);
  helpMenu->addSeparator();
  helpMenu->addAction(helpAbout);
  helpMenu->addAction(helpAboutQt);

  menuBar()->addMenu(fileMenu);
  menuBar()->addSeparator();
  menuBar()->addMenu(helpMenu);

  // -------  create main windows widgets --------
  all  = new QGridLayout();
  all->setSpacing(3);

  // ...........................................................
  box1 = new QGroupBox(tr("Filter"), this);
  QHBoxLayout *hbox = new QHBoxLayout();

  gbox1 = new QGridLayout();
  gbox1->setSpacing(3);

  box1->setLayout(gbox1);

  QLabel *Label0 = new QLabel(tr("Realization:"), this);
  gbox1->addWidget(Label0, 0,0);
  ComboRealize = new QComboBox(this);
  ComboRealize->addItem("LC ladder (pi type)");
  ComboRealize->addItem("LC ladder (tee type)");
  ComboRealize->addItem("End coupled transmission lines");
  ComboRealize->addItem("Parallel coupled transmission lines");
  ComboRealize->addItem("Stepped-impedance");
  ComboRealize->addItem("Quarter wave resonators");
  ComboRealize->addItem("Quarter wave side coupled ring resonator");
  ComboRealize->addItem("Capacitively coupled shunt resonators");
  ComboRealize->addItem("Equation-defined");

  gbox1->addWidget(ComboRealize, 0,1);
  connect(ComboRealize, SIGNAL(activated(int)), SLOT(slotRealizationChanged(int)));

   //Synthesize Microstrip
  MicrostripcheckBox=new QCheckBox("Microstrip implementation");
  MicrostripcheckBox->setChecked(false);
  MicrostripcheckBox->setEnabled(false);
  gbox1->addWidget(MicrostripcheckBox, 1,0);
  connect(MicrostripcheckBox, SIGNAL(clicked()), SLOT(on_MicrostripcheckBox_clicked()));

  QLabel *Label1 = new QLabel(tr("Filter type:"), this);
  gbox1->addWidget(Label1, 2,0);
  ComboType = new QComboBox(this);
  ComboType->addItem("Bessel");
  ComboType->addItem("Butterworth");
  ComboType->addItem("Chebyshev");
  ComboType->addItem("Cauer");
  gbox1->addWidget(ComboType, 2,1);
  connect(ComboType, SIGNAL(activated(int)), SLOT(slotTypeChanged(int)));

  QLabel *Label2 = new QLabel(tr("Filter class:"), this);
  gbox1->addWidget(Label2, 3,0);
  ComboClass = new QComboBox(this);
  ComboClass->addItem(tr("Low pass"));
  ComboClass->addItem(tr("High pass"));
  ComboClass->addItem(tr("Band pass"));
  ComboClass->addItem(tr("Band stop"));
  gbox1->addWidget(ComboClass, 3,1);
  connect(ComboClass, SIGNAL(activated(int)), SLOT(slotClassChanged(int)));

  IntVal = new QIntValidator(1, 200, this);
  DoubleVal = new QDoubleValidator(this);

  LabelOrder = new QLabel(tr("Order:"), this);
  gbox1->addWidget(LabelOrder, 4,0);
  EditOrder = new QLineEdit("3", this);
  EditOrder->setValidator(IntVal);
  gbox1->addWidget(EditOrder, 4,1);

  LabelStart = new QLabel(tr("Corner frequency:"), this);
  gbox1->addWidget(LabelStart, 5,0);
  EditCorner = new QLineEdit("1", this);
  EditCorner->setValidator(DoubleVal);
  gbox1->addWidget(EditCorner, 5,1);
  ComboCorner = new QComboBox(this);
  ComboCorner->addItem("Hz");
  ComboCorner->addItem("kHz");
  ComboCorner->addItem("MHz");
  ComboCorner->addItem("GHz");
  ComboCorner->setCurrentIndex(3);
  gbox1->addWidget(ComboCorner, 5,2);

  LabelStop = new QLabel(tr("Stop frequency:"), this);
  gbox1->addWidget(LabelStop, 6,0);
  EditStop = new QLineEdit("2", this);
  EditStop->setValidator(DoubleVal);
  gbox1->addWidget(EditStop, 6,1);
  ComboStop = new QComboBox(this);
  ComboStop->addItem("Hz");
  ComboStop->addItem("kHz");
  ComboStop->addItem("MHz");
  ComboStop->addItem("GHz");
  ComboStop->setCurrentIndex(3);
  gbox1->addWidget(ComboStop, 6,2);

  LabelBandStop = new QLabel(tr("Stop band frequency:"), this);
  gbox1->addWidget(LabelBandStop, 7,0);
  EditBandStop = new QLineEdit("3", this);
  EditBandStop->setValidator(DoubleVal);
  gbox1->addWidget(EditBandStop, 7,1);
  ComboBandStop = new QComboBox(this);
  ComboBandStop->addItem("Hz");
  ComboBandStop->addItem("kHz");
  ComboBandStop->addItem("MHz");
  ComboBandStop->addItem("GHz");
  ComboBandStop->setCurrentIndex(3);
  gbox1->addWidget(ComboBandStop, 7,2);

  LabelRipple = new QLabel(tr("Pass band ripple:"), this);
  gbox1->addWidget(LabelRipple, 8,0);
  EditRipple = new QLineEdit("1", this);
  EditRipple->setValidator(DoubleVal);
  gbox1->addWidget(EditRipple, 8,1);
  LabelRipple_dB = new QLabel("dB", this);
  gbox1->addWidget(LabelRipple_dB, 8,2);

  LabelAtten = new QLabel(tr("Stop band attenuation:"), this);
  gbox1->addWidget(LabelAtten, 9,0);
  EditAtten = new QLineEdit("20", this);
  EditAtten->setValidator(DoubleVal);
  gbox1->addWidget(EditAtten, 9,1);
  LabelAtten_dB = new QLabel("dB", this);
  gbox1->addWidget(LabelAtten_dB, 9,2);

  LabelImpedance = new QLabel(tr("Impedance:"), this);
  gbox1->addWidget(LabelImpedance, 10,0);
  EditImpedance = new QLineEdit("50", this);
  EditImpedance->setValidator(DoubleVal);
  gbox1->addWidget(EditImpedance, 10,1);
  LabelOhm = new QLabel("Ohm", this);
  gbox1->addWidget(LabelOhm, 10,2);

  // ...........................................................

  box2 = new QGroupBox(tr("Microstrip Substrate"), this);
  box2->setEnabled(false);

  gbox2 = new QGridLayout();
  gbox2->setSpacing(3);

  box2->setLayout(gbox2);

  QLabel *Label3 = new QLabel(tr("Relative permittivity:"), this);
  gbox2->addWidget(Label3, 0,0);
  ComboEr = new QComboBox(this);
  ComboEr->setEditable(true);
  ComboEr->lineEdit()->setValidator(DoubleVal);
  connect(ComboEr, SIGNAL(activated(const QString&)), SLOT(slotTakeEr(const QString&)));
  gbox2->addWidget(ComboEr, 0,1);

  const char **p = List_er;
  while(*(++p))
    ComboEr->addItem(*p);  // put material properties into combobox
  ComboEr->lineEdit()->setText("9.8");

  QLabel *Label4 = new QLabel(tr("Substrate height:"), this);
  gbox2->addWidget(Label4, 1,0);
  EditHeight = new QLineEdit("1.0", this);
  EditHeight->setValidator(DoubleVal);
  gbox2->addWidget(EditHeight, 1,1);
  QLabel *Label5 = new QLabel("mm", this);
  gbox2->addWidget(Label5, 1,2);

  QLabel *Label6 = new QLabel(tr("metal thickness:"), this);
  gbox2->addWidget(Label6, 2,0);
  EditThickness = new QLineEdit("12.5", this);
  EditThickness->setValidator(DoubleVal);
  gbox2->addWidget(EditThickness, 2,1);
  QLabel *Label7 = new QLabel("um", this);
  gbox2->addWidget(Label7, 2,2);

  QLabel *Label8 = new QLabel(tr("minimum width:"), this);
  gbox2->addWidget(Label8, 3,0);
  EditMinWidth = new QLineEdit("0.4", this);
  EditMinWidth->setValidator(DoubleVal);
  gbox2->addWidget(EditMinWidth, 3,1);
  QLabel *Label9 = new QLabel("mm", this);
  gbox2->addWidget(Label9, 3,2);

  QLabel *Label10 = new QLabel(tr("maximum width:"), this);
  gbox2->addWidget(Label10, 4,0);
  EditMaxWidth = new QLineEdit("5.0", this);
  EditMaxWidth->setValidator(DoubleVal);
  gbox2->addWidget(EditMaxWidth, 4,1);
  QLabel *Label11 = new QLabel("mm", this);
  gbox2->addWidget(Label11, 4,2);

  QSpacerItem *mySpacer=new QSpacerItem(1,1, QSizePolicy::Minimum, QSizePolicy::Expanding);
  gbox2->addItem(mySpacer, 5, 0, 1, -1);

   QSize sz;
   boxImage = new QGroupBox(tr("Preview"), this);
   gboxImage = new QGridLayout();
   boxImage->setLayout(gboxImage);
   QString s1 = ":/bitmaps/LC_Ladder_Pi_LPF.svg";
   imgLayout = new QSvgWidget(s1);
   sz = imgLayout->size();
   imgLayout->setFixedSize(.5*sz);
   gboxImage->addWidget(imgLayout);
    
 

  // ...........................................................
  QVBoxLayout *vboxButtons = new QVBoxLayout();
  QPushButton *ButtonGo = new QPushButton(tr("Calculate and put into Clipboard"), this);
  connect(ButtonGo, SIGNAL(clicked()), SLOT(slotCalculate()));
  vboxButtons->addWidget(ButtonGo);

  LabelResult = new QLabel(this);
  ResultState = 100;
  slotShowResult();
  LabelResult->setAlignment(Qt::AlignHCenter);
  vboxButtons->addWidget(LabelResult);

  QVBoxLayout *vbox_all = new QVBoxLayout();
  QVBoxLayout *vboxRealisation = new QVBoxLayout();
  QVBoxLayout *vboxMicrostrip = new QVBoxLayout();

  vboxRealisation->addWidget(box1);
  hbox->addLayout(vboxRealisation);
  vboxMicrostrip->addWidget(box2);
  vboxMicrostrip->addWidget(boxImage);
  hbox->addLayout(vboxMicrostrip);
  vbox_all->addLayout(hbox);
  vbox_all->addLayout(vboxButtons);
  centralWidget->setLayout(vbox_all);

  // -------  finally set initial state  --------
  slotTypeChanged(0);
  slotClassChanged(0);
}

QucsFilter::~QucsFilter()
{
  delete gbox1;
  delete gbox2;
  delete all;
  delete IntVal;
  delete DoubleVal;
}

// ************************************************************
void QucsFilter::slotQuit()
{
  int tmp;
  tmp = x();		// call size and position function in order to ...
  tmp = y();		// ... set them correctly before closing the ...
  tmp = width();	// dialog !!!  Otherwise the frame of the window ...
  tmp = height();	// ... will not be recognized (a X11 problem).
  Q_UNUSED(tmp);

  qApp->quit();
}

// ************************************************************
void QucsFilter::slotHelpAbout()
{
  QMessageBox::about(this, tr("About..."),
    "QucsFilter Version " PACKAGE_VERSION+
    tr("\nFilter synthesis program\n")+
    tr("Copyright (C) 2005, 2006 by")+
    "\nVincent Habchi, Toyoyuki Ishikawa,\n"
    "Michael Margraf, Stefan Jahn\n"
    "\nThis is free software; see the source for copying conditions."
    "\nThere is NO warranty; not even for MERCHANTABILITY or "
    "\nFITNESS FOR A PARTICULAR PURPOSE.\n\n");
}

// ************************************************************
void QucsFilter::slotHelpAboutQt()
{
  QMessageBox::aboutQt(this, tr("About Qt"));
}

// ************************************************************
void QucsFilter::slotHelpIntro()
{
  HelpDialog *d = new HelpDialog(this);
  d->show();
}

// ************************************************************
void QucsFilter::setError(const QString& Message)
{
  LabelResult->setText(tr("Result:") + "<font color=\"#FF0000\"><b>  " +
                       tr("Error") + "</b></font>");
  QMessageBox::critical(this, tr("Error"), Message);
}

// ************************************************************
QString * QucsFilter::calculateFilter(struct tFilter * Filter)
{
    QString * s = NULL;

    tSubstrate Substrate;
    Substrate.er = ComboEr->currentText().toDouble();
    Substrate.height = EditHeight->text().toDouble() / 1e3;
    Substrate.thickness = EditThickness->text().toDouble() / 1e6;
    Substrate.tand = 0.0;
    Substrate.resistivity = 1e-10;
    Substrate.roughness = 0.0;
    Substrate.minWidth = EditMinWidth->text().toDouble() / 1e3;
    Substrate.maxWidth = EditMaxWidth->text().toDouble() / 1e3;

    //Throw an error for even Chebyshev filters
    if ((Filter->Type == TYPE_CHEBYSHEV) && (Filter->Order % 2 == 0))
    {
      QMessageBox::critical(0, "Error", "Even order Chebyshev can't be realized with passive filters.");
      return NULL;
    }

    bool isMicrostrip = MicrostripcheckBox->isChecked();
    switch(ComboRealize->currentIndex()) {
      case 2:  // End coupled transmission line filter
        s = Line_Filter::createSchematic(Filter, &Substrate, isMicrostrip);
        return s;
      case 3:  // Parallel coupled transmission line filter
        s = CoupledLine_Filter::createSchematic(Filter, &Substrate, isMicrostrip);
        return s;
      case 4:  // stepped-impedance transmission line filter
        s = StepImpedance_Filter::createSchematic(Filter, &Substrate, isMicrostrip);
        return s;
      case 5: // Quarter wave transmission line filter
        s = QuarterWave_Filter::createSchematic(Filter, &Substrate, isMicrostrip);
        return s;
      case 6: // Quarter wave side coupled ring resonator transmission
        s = QW_Coupled_Ring_Filter::createSchematic(Filter, &Substrate, isMicrostrip);
        return s;
      case 7: // Capacitively coupled shunt resonators
        s = CCoupled_Shunt_Resonator_Filter::createSchematic(Filter);
        return s;
      case 8:  // equation defined filter
        s = Equation_Filter::createSchematic(Filter);
        return s;


    }

    if (Filter->Type != TYPE_CAUER) {
      if(ComboRealize->currentIndex() == 0)
        s = LC_Filter::createSchematic(Filter, true);
      else
        s = LC_Filter::createSchematic(Filter, false);
    }
    else  {
      qf_cauer * F = NULL;
      double amin, amax, fc, fs, bw, r;
      fc = Filter->Frequency;
      amin = Filter->Ripple;
      fs = Filter->Frequency3;
      r = Filter->Impedance;
      amax = Filter->Attenuation;
      bw = Filter->Frequency2 - fc;

      switch (Filter->Class) {
      case CLASS_LOWPASS:
        F = new qf_cauer (amin, amax, fc, fs, r, 0, LOWPASS);
        break;
      case CLASS_HIGHPASS:
        F = new qf_cauer (amin, amax, fc, fs, r, 0, HIGHPASS);
        break;
      case CLASS_BANDPASS:
        F = new qf_cauer (amin, amax, fc + bw / 2, fs, r, bw, BANDPASS);
        break;
      case CLASS_BANDSTOP:
        F = new qf_cauer (amin, amax, fc + bw / 2, fs, r, bw, BANDSTOP);
        break;
      }
      if (F) {
        //F->dump();
        EditOrder->setText(QString::number(F->order()));
        s = new QString(F->to_qucs().c_str());
        delete F;
      }
      else {
        s = NULL;
      }
    }
    return s;
  }

// ************************************************************
void QucsFilter::slotCalculate()
{
  // get numerical values from input widgets
  double CornerFreq   = EditCorner->text().toDouble();
  double StopFreq     = EditStop->text().toDouble();
  double BandStopFreq = EditBandStop->text().toDouble();

  // add exponent
  CornerFreq   *= pow(10, double(3*ComboCorner->currentIndex()));
  StopFreq     *= pow(10, double(3*ComboStop->currentIndex()));
  BandStopFreq *= pow(10, double(3*ComboBandStop->currentIndex()));

  tFilter Filter;
  Filter.Type = ComboType->currentIndex();
  Filter.Class = ComboClass->currentIndex();
  Filter.Order = EditOrder->text().toInt();
  Filter.Ripple = EditRipple->text().toDouble();
  Filter.Attenuation = EditAtten->text().toDouble();
  Filter.Impedance = EditImpedance->text().toDouble();
  Filter.Frequency = CornerFreq;
  Filter.Frequency2 = StopFreq;
  Filter.Frequency3 = BandStopFreq;

  if(EditStop->isEnabled())
    if(Filter.Frequency >= Filter.Frequency2) {
      setError(tr("Stop frequency must be greater than start frequency."));
      return;
    }

  if(EditOrder->isEnabled()) {
    if (Filter.Order < 2) {
      setError(tr("Filter order must not be less than two."));
      return;
    }
    if(Filter.Order > 19) if(Filter.Type == TYPE_BESSEL) {
      setError(tr("Bessel filter order must not be greater than 19."));
      return;
    }
  }

  QString * s = calculateFilter(&Filter);
  if(!s) return;

  // put resulting filter schematic into clipboard
  QClipboard *cb = QApplication::clipboard();
  cb->setText(*s);
  delete s;

  // show result for some time
  ResultState = 0;
  LabelResult->setText(tr("Result:") + "<font color=\"#008000\"><b>  " +
                       tr("Successful") + "</b></font>");
  QTimer::singleShot(500, this, SLOT(slotShowResult()));
}

// ************************************************************
void QucsFilter::slotShowResult()
{
  if(ResultState > 5) {
    LabelResult->setText(tr("Result: --"));
    return;
  }

  int c;
  ResultState++;
  if(ResultState & 1)  c = 0xFF;
  else c = 0x80;
  QString s = QString("<font color=\"#00%1000\"><b>  ").arg(c, 2, 16);
  LabelResult->setText(tr("Result:") + s + tr("Successful") + "</b></font>");

  c = 500;
  if(ResultState > 5)  c = 3000;
  QTimer::singleShot(c, this, SLOT(slotShowResult()));
}

// ************************************************************
void QucsFilter::slotTypeChanged(int index)
{
  FlushImage();
  switch(index) {
    case TYPE_BESSEL:
    case TYPE_BUTTERWORTH:
	LabelRipple->setEnabled(false);
	EditRipple->setEnabled(false);
	LabelRipple_dB->setEnabled(false);
	break;
    case TYPE_CHEBYSHEV:
    case TYPE_CAUER:
	LabelRipple->setEnabled(true);
	EditRipple->setEnabled(true);
	LabelRipple_dB->setEnabled(true);
	break;
  }
  if (index == TYPE_CAUER) {
    LabelOrder->setEnabled(false);
    EditOrder->setEnabled(false);
    LabelAtten->setEnabled(true);
    EditAtten->setEnabled(true);
    LabelAtten_dB->setEnabled(true);
    LabelBandStop->setEnabled(true);
    EditBandStop->setEnabled(true);
    ComboBandStop->setEnabled(true);
  }
  else {
    LabelOrder->setEnabled(true);
    EditOrder->setEnabled(true);
    LabelAtten->setEnabled(false);
    EditAtten->setEnabled(false);
    LabelAtten_dB->setEnabled(false);
    LabelBandStop->setEnabled(false);
    EditBandStop->setEnabled(false);
    ComboBandStop->setEnabled(false);
  }
}

// ************************************************************
void QucsFilter::slotClassChanged(int index)
{
  FlushImage();
  switch(index) {
    case CLASS_LOWPASS:
    case CLASS_HIGHPASS:
      LabelStop->setEnabled(false);
      EditStop->setEnabled(false);
      ComboStop->setEnabled(false);
      LabelStart->setText(tr("Corner frequency:"));
      break;
    case CLASS_BANDPASS:
    case CLASS_BANDSTOP:
      LabelStop->setEnabled(true);
      EditStop->setEnabled(true);
      ComboStop->setEnabled(true);
      break;
  }
  if (index == CLASS_BANDPASS) {
    LabelBandStop->setText(tr("Stop band frequency:"));
    LabelRipple->setText(tr("Pass band ripple:"));
  }
  else if (index == CLASS_BANDSTOP) {
    LabelBandStop->setText(tr("Pass band frequency:"));
    LabelRipple->setText(tr("Pass band attenuation:"));
  }
}

//---------------------------------------------------------
// This function updates the preview window
void QucsFilter::FlushImage()
{
  QString s1;
  bool isMicrostrip = MicrostripcheckBox->isChecked();
  bool isCauer = (ComboType->currentIndex() == 3);
   switch(ComboRealize->currentIndex())
   {
      case 0://LC pi
            switch(ComboClass->currentIndex())
            {
               case 0: (isCauer) ? s1 = ":/bitmaps/LC_cauer_lowpass.svg" : s1 = ":/bitmaps/LC_Ladder_Pi_LPF.svg"; break;
               case 1: s1 = ":/bitmaps/LC_Ladder_Pi_HPF.svg"; break;
               case 2: (isCauer) ? s1 = ":/bitmaps/LC_cauer_bandpass.svg" :  s1 = ":/bitmaps/LC_Ladder_Pi_BPF.svg"; break;
               case 3: (isCauer) ? s1 = ":/bitmaps/LC_cauer_notch.svg" : s1 = ":/bitmaps/LC_Ladder_Pi_Notch.svg";break;
            }
            break;
      case 1://LC Tee
            switch(ComboClass->currentIndex())
            {
               case 0: (isCauer) ? s1 = ":/bitmaps/LC_cauer_lowpass.svg" : s1 = ":/bitmaps/LC_Ladder_Tee_LPF.svg"; break;
               case 1: s1 = ":/bitmaps/LC_Ladder_Tee_HPF.svg"; break;
               case 2: (isCauer) ? s1 = ":/bitmaps/LC_cauer_bandpass.svg" : s1 = ":/bitmaps/LC_Ladder_Tee_BPF.svg"; break;
               case 3: (isCauer) ? s1 = ":/bitmaps/LC_cauer_notch.svg" : s1 = ":/bitmaps/LC_Ladder_Tee_Notch.svg";break;
            }
            break;
      case 2:
           (isMicrostrip) ? s1 = ":/bitmaps/CCoupled_Microstrip.svg" : s1 = ":/bitmaps/CCoupled.svg";
           break;
      case 3:
           (isMicrostrip) ? s1 = ":/bitmaps/CCoupled_Lines_Microstrip.svg" : s1 = ":/bitmaps/CCoupled_Lines.svg";
           break;
      case 4:
           (isMicrostrip) ? s1 = ":/bitmaps/Stepped_Impedance_Microstrip.svg" : s1 = ":/bitmaps/Stepped_Impedance_Ideal.svg";
           break;
      case 5:
           switch(ComboClass->currentIndex())
           {
               case 2: (isMicrostrip) ? s1 = ":/bitmaps/Quarterwave_Bandpass_Microstrip.svg" : s1 = ":/bitmaps/Quarterwave_Bandpass_Ideal.svg"; break;
               case 3: (isMicrostrip) ? s1 = ":/bitmaps/Quarterwave_Notch_Microstrip.svg" : s1 = ":/bitmaps/Quarterwave_Notch_Ideal.svg";break;
           }
           break;
      case 6:
           s1 = ":/bitmaps/Quarterwave_Side_Coupled_Ring.svg";
           break;
      case 7:
           s1 = ":/bitmaps/CCoupled_Shunt_Resonators.svg";
           break;
      case 8:
           s1 = ":/bitmaps/RFEDD.svg";
           break;
   }
   imgLayout->load(s1);
}

// ************************************************************
void QucsFilter::slotRealizationChanged(int index)
{
  FlushImage();

  if ((index == LC_LADDER_PI) || (index == LC_LADDER_TEE))//LC filters
  {
     ComboType->clear();
     ComboType->addItem("Bessel");
     ComboType->addItem("Butterworth");
     ComboType->addItem("Chebyshev");
     ComboType->addItem("Cauer");
     MicrostripcheckBox->setEnabled(false);
     MicrostripcheckBox->setChecked(false);
     box2->setEnabled(false);//Microstrip substrate definition panel
     ComboClass->setEnabled(true);
     EditOrder->setEnabled(true);
     ComboType->setEnabled(true);//The other filters can implement canonical responses
     LabelStart->setText(tr("Corner frequency:"));
     LabelStop->setText(tr("Stop frequency:"));
     return;
  }

  //Remove Cauer filters from ComboClass since they cannot be implemented with the other topologies
  //Otherwise, the user may select it and face a number of annoying warnings...
  ComboType->clear();
  ComboType->addItem("Bessel");
  ComboType->addItem("Butterworth");
  ComboType->addItem("Chebyshev");


  //Shared settings for microwave BP filters
  if ((index == END_COUPLED) || (index == PARALLEL_COUPLED_TL) || (index == QW_RING))
  {
           ComboClass->setCurrentIndex(CLASS_BANDPASS);
           slotClassChanged(CLASS_BANDPASS);
           ComboClass->setEnabled(false);
           MicrostripcheckBox->setEnabled(true);
           MicrostripcheckBox->setChecked(false);//By default, the microstrip implementation is unchecked
           box2->setEnabled(false);//Microstrip substrate definition panel
  }
  
  // QW bandpass and notch filters
  if (index == QW_RESONATORS)
  {
    MicrostripcheckBox->setEnabled(true);
    ComboClass->setEnabled(true);
    MicrostripcheckBox->setChecked(
        false); // By default, the microstrip implementation is unchecked
    box2->setEnabled(false); // Microstrip substrate definition panel
  }

  if (index == STEPPED_IMPEDANCE)//Stepped impedance LPF
  {
     ComboClass->setCurrentIndex(CLASS_LOWPASS);
     slotClassChanged(CLASS_LOWPASS);
     ComboClass->setEnabled(false);
     MicrostripcheckBox->setEnabled(true);
     MicrostripcheckBox->setChecked(false);//By default, the microstrip implementation is unchecked
     box2->setEnabled(false);//Microstrip substrate definition panel
  }

  if (index == QW_RING)//Quarter wavelength side coupled ring resonator (BP)
  {
     ComboType->setEnabled(false);//Conventional frequency responses not available
     EditOrder->setEnabled(false);//Not possible to modify the order
     LabelStart->setText("1st transmission zero frequency:");
     LabelStop->setText("2nd transmission zero frequency:");
  }
  else
  {
     EditOrder->setEnabled(true);
     ComboType->setEnabled(true);//The other filters can implement canonical responses
     LabelStart->setText(tr("Corner frequency:"));
     LabelStop->setText(tr("Stop frequency:"));
  }

  //Shared settings for the qw-ring resonator and the capacitively coupled shunt resonators
  if ((index == QW_RING) || (index == CAP_COUPLED_SHUNT_RESONATORS))
  {
    MicrostripcheckBox->setEnabled(false);//Microstrip implementation does not work...
    ComboClass->setCurrentIndex(CLASS_BANDPASS);
    slotClassChanged(CLASS_BANDPASS);
    ComboClass->setEnabled(false);
  }

  if (index == EQN_DEFINED)//RF EDD filter
  {
     MicrostripcheckBox->setEnabled(false);
     MicrostripcheckBox->setChecked(false);
     ComboClass->setEnabled(true);
     ComboType->setEnabled(true);
  }

} 

void QucsFilter::on_MicrostripcheckBox_clicked()
{
    FlushImage();
    bool microcheck = MicrostripcheckBox->isChecked();
    box2->setEnabled(microcheck);
}


// **********************************************************
void QucsFilter::slotTakeEr(const QString& Text)
{
  // If the ComboBox text was a material property, the material name
  // is separated by two spaces. So, cut off the name.
  ComboEr->lineEdit()->setText(Text.section("  ", 0, 0));
}
