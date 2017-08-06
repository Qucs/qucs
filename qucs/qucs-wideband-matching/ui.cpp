 /*
 * ui.cpp - User interface class implementation
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


#include "ui.h"

ui::ui()
{
      setWindowIcon(QPixmap(":/bitmaps/big.qucs.xpm"));
      GNUplot_path = QCoreApplication::applicationDirPath() + "/GRABIM.dat";
      setMinimumSize(400,350);
      centralWidget =  new QWidget();
      Impedancelayout = new QGridLayout();
      ButtonsLayout = new QHBoxLayout();

      //Create source load impedance buttons
      SourceFileButton = new QPushButton(".s1p file");
      LoadFileButton = new QPushButton(".s1p file");
      connect(SourceFileButton, SIGNAL(clicked()), this, SLOT(SourceImpedance_clicked()));
      connect(LoadFileButton, SIGNAL(clicked()), this, SLOT(LoadImpedance_clicked()));


      SourceLayout = new QVBoxLayout();
      LoadLayout = new QVBoxLayout();

      ConstantZSLayout = new QHBoxLayout();
      ConstantZLLayout = new QHBoxLayout();

      FixedZSLineedit = new QLineEdit("50");
      FixedZLLineedit = new QLineEdit("50");
      ZSOhmLabel = new QLabel("Ohm");
      ZLOhmLabel = new QLabel("Ohm");


      ZSOhmLabel->setVisible(false);
      ZLOhmLabel->setVisible(false);

      FixedZSLineedit->setVisible(false);
      SourceFileButton->setVisible(true);

      FixedZLLineedit->setVisible(false);
      LoadFileButton->setVisible(true);

      ConstantZSLayout->addWidget(FixedZSLineedit);
      ConstantZSLayout->addWidget(ZSOhmLabel);

      ConstantZLLayout->addWidget(FixedZLLineedit);
      ConstantZLLayout->addWidget(ZLOhmLabel);

      SourceLabel = new QLabel("Source");
      SourceLabel->setStyleSheet("QLabel {font-weight: bold; border: 1px solid black;}");
      SourceLabel->setAlignment(Qt::AlignHCenter);
      SourceLayout->addWidget(SourceLabel);
      SourceLayout->addWidget(SourceFileButton);
      SourceLayout->addLayout(ConstantZSLayout);

      LoadLabel = new QLabel("Load");
      LoadLabel->setStyleSheet("QLabel {font-weight: bold; border: 1px solid black;}");
      LoadLabel->setAlignment(Qt::AlignHCenter);
      LoadLayout->addWidget(LoadLabel);
      LoadLayout->addWidget(LoadFileButton);
      LoadLayout->addLayout(ConstantZLLayout);

      //Checkboxes for selecting a constant impedance vs freq profile
      FixedZSCheckbox = new QCheckBox("Constant ZS");
      FixedZLCheckbox = new QCheckBox("Constant ZL");
      connect(FixedZSCheckbox, SIGNAL(clicked()), this, SLOT(FixedZSCheckbox_clicked()));
      connect(FixedZLCheckbox, SIGNAL(clicked()), this, SLOT(FixedZLCheckbox_clicked()));

      SourceLayout->addWidget(FixedZSCheckbox);
      LoadLayout->addWidget(FixedZLCheckbox);

      SourceImpGroupBox = new QGroupBox();
      SourceImpGroupBox->setMinimumSize(130,20);
      SourceImpGroupBox->setStyleSheet("QGroupBox{  border: 1px solid black;}");
      LoadImpGroupBox = new QGroupBox();
      LoadImpGroupBox->setStyleSheet("QGroupBox{  border: 1px solid black;}");
      LoadImpGroupBox->setMinimumSize(130,20);

      SourceImpGroupBox->setLayout(SourceLayout);
      LoadImpGroupBox->setLayout(LoadLayout);

      // Image
       QSize sz;
       QString s1 = ":/bitmaps/MatchingNetwork.svg";
       imgWidget = new QSvgWidget(s1);
       sz = imgWidget->size();
       imgWidget->setFixedSize(.2*sz);

      //Amplifier
      QPixmap pixmap(":/bitmaps/amplifier.svg");
      QIcon ButtonIcon(pixmap);
      AmplifierS2Pbutton = new QPushButton();
      AmplifierS2Pbutton->setIcon(ButtonIcon);
      AmplifierS2Pbutton->setIconSize(pixmap.rect().size());
      AmplifierS2Pbutton->setVisible(false);
      connect(AmplifierS2Pbutton, SIGNAL(clicked()), this, SLOT(BrowseAmplifier_S2P()));


      Impedancelayout->addWidget(SourceImpGroupBox,0,0);
      Impedancelayout->addWidget(imgWidget,0,1);
      Impedancelayout->addWidget(AmplifierS2Pbutton,0,1);
      Impedancelayout->addWidget(LoadImpGroupBox,0,2);


       // Matching band
       FreqgroupBox = new QGroupBox(tr("Matching band"));
       minFLabel = new QLabel("Min:");
       minFEdit = new QLineEdit("1000");
       minFUnitsCombo =  new QComboBox();
       maxFLabel = new QLabel("Max:");
       maxFEdit = new QLineEdit("2000");
       maxFUnitsCombo =  new QComboBox();


       // Fill combos
       minFUnitsCombo->addItem("kHz");
       minFUnitsCombo->addItem("MHz");
       minFUnitsCombo->addItem("GHz");
       minFUnitsCombo->setCurrentIndex(1);
       minFUnitsCombo->setMinimumContentsLength(5);

       maxFUnitsCombo->addItem("kHz");
       maxFUnitsCombo->addItem("MHz");
       maxFUnitsCombo->addItem("GHz");
       maxFUnitsCombo->setCurrentIndex(1);
       maxFUnitsCombo->setMinimumContentsLength(5);

       vbox = new QGridLayout;
       vbox->addWidget(minFLabel,0,0);
       vbox->addWidget(minFEdit,0,1);
       vbox->addWidget(minFUnitsCombo,0,2);

       vbox->addWidget(maxFLabel,0,3);
       vbox->addWidget(maxFEdit,0,4);
       vbox->addWidget(maxFUnitsCombo,0,5);
       FreqgroupBox->setLayout(vbox);
    
       //Use a size policy. Otherwise, large blank spaces appear
       QSizePolicy sizePolicy((QSizePolicy::Policy)QSizePolicy::Minimum,(QSizePolicy::Policy)QSizePolicy::Fixed);
       sizePolicy.setHorizontalStretch(0);
       sizePolicy.setVerticalStretch(0);
       FreqgroupBox->setSizePolicy( sizePolicy );

       // Options
       TwoPortMatchingCheckbox = new QCheckBox("2-port matching");
       connect(TwoPortMatchingCheckbox, SIGNAL(stateChanged(int)), this, SLOT(TwoPortMatchingCheckbox_state(int)));
       UseGNUplotCheckbox = new QCheckBox("Use GNUplot");
       UseGNUplotCheckbox->setChecked(true);
       RefineCheckbox = new QCheckBox("Refine");
       RefineCheckbox->setChecked(false);

       OptionsLayout = new QGridLayout();
       GNUplotButton = new QPushButton("Browse");
       connect(GNUplotButton, SIGNAL(clicked()), this, SLOT(GNUplotOutput_clicked()));

       OptionsLayout->addWidget(UseGNUplotCheckbox, 0, 0);
       OptionsLayout->addWidget(RefineCheckbox, 0, 1);
       OptionsLayout->addWidget(TwoPortMatchingCheckbox, 1, 0);
       OptionsLayout->addWidget(new QLabel("Temp folder GNUplot:"), 2, 0);
       OptionsLayout->addWidget(GNUplotButton, 2, 1);
       GNUplot_path = "/tmp";


      //Create go/cancel buttons
      RunButton = new QPushButton("Calculate and put into clipboard");
      ButtonsLayout->addWidget(RunButton);
      connect(RunButton, SIGNAL(clicked()), this, SLOT(go_clicked()));


      //Create main layout and add individual layouts
      mainLayout = new QVBoxLayout();
      mainLayout->addLayout(Impedancelayout);
      mainLayout->addWidget(FreqgroupBox);
      mainLayout->addLayout(OptionsLayout);
      mainLayout->addLayout(ButtonsLayout);

      LabelResult = new QLabel(this);
      ResultState = 100;
      slotShowResult();
      LabelResult->setAlignment(Qt::AlignHCenter);
      mainLayout->addWidget(LabelResult); 

      centralWidget->setLayout(mainLayout);
      setCentralWidget(centralWidget);
      setWindowTitle("Qucs wideband matching tool " PACKAGE_VERSION);
}

ui::~ui()
{
//Free memory. Prevent memory leaks
      delete Impedancelayout;
      delete RunButton;
      delete GNUplotButton;
      delete SearchModeCombo;
      delete SearchModeLabel;
      delete minFLabel;
      delete minFEdit;
      delete minFUnitsCombo;
      delete maxFLabel;
      delete maxFEdit;
      delete maxFUnitsCombo;
      delete imgWidget;
      delete FixedZSCheckbox;
      delete FixedZLCheckbox;
      delete SourceFileButton;
      delete LoadFileButton;
      delete FixedZSLineedit;
      delete FixedZLLineedit;
      delete ZSOhmLabel;
      delete ZLOhmLabel;
      delete centralWidget;
      delete OptionsLayout;
      delete mainLayout;
      delete vbox;
      delete FreqgroupBox;
      delete ConstantZSLayout;
      delete ConstantZLLayout;
      delete ButtonsLayout;
      delete SourceLayout;
      delete LoadLayout;

}
void ui::go_clicked()
{
    if ((system("gnuplot -V") != 0) && (UseGNUplotCheckbox->isChecked()))
    {//GNUplot is not installed
       QMessageBox::warning(0, QObject::tr("Error"),
                             QObject::tr("GNUplot is required to display results"));
        return;
    }

    //Before starting the matching engine, we must ensure that the impedance data is already loaded
    if (SourceFile.isEmpty() && (FixedZSLineedit->text().isEmpty()))
    {
        QMessageBox::warning(0, QObject::tr("Error"),
                             QObject::tr("Please select a Touchstone file for the source"));
        return;
    }

    if (LoadFile.isEmpty()&& (FixedZLLineedit->text().isEmpty()))
    {
        QMessageBox::warning(0, QObject::tr("Error"),
                             QObject::tr("Please select a Touchstone file for the load"));
        return;
    }

    //Check filename extension
    int formatSource=-1, formatLoad=-1;;
    if (SourceFile.contains(".s1p")) formatSource = 0;
    if (LoadFile.contains(".s1p")) formatLoad = 0;


    if ((formatSource != 0) && (!FixedZSCheckbox->isChecked()) && (!TwoPortMatchingCheckbox->isChecked()))
    {
        QMessageBox::warning(0, QObject::tr("Error"),
                             QObject::tr("Unknown source impedace"));
        return;
    }

    if ((formatLoad != 0)  && (!FixedZLCheckbox->isChecked()) && (!TwoPortMatchingCheckbox->isChecked()))
    {
        QMessageBox::warning(0, QObject::tr("Error"),
                             QObject::tr("Unknown load impedance"));
        return;
    }

    IO * inout_operations = new IO();

    if (TwoPortMatchingCheckbox->isChecked())
    {
       inout_operations->Two_Port_Matching=true;
       inout_operations->loadS2Pdata(AmpFile.toStdString());
    }
    else
    {
       inout_operations->Two_Port_Matching=false;
    }


    //Impedance data paths were already specified, let's proceed to bring the S-par data into memory

    if (!FixedZSCheckbox->isChecked() && (!TwoPortMatchingCheckbox->isChecked()))//Read source impedance from file
    {
        inout_operations->loadS1Pdata(SourceFile.toStdString(), SOURCE);//s1p
    }
    else//Set constant source impedance
    {
       complex<double> zs_temp;
       if (!TwoPortMatchingCheckbox->isChecked())
       {
       QByteArray ba = FixedZSLineedit->text().toLatin1();
       char * text = ba.data();
       zs_temp = getComplexImpedanceFromText(text);
       }
       else
       {
         zs_temp = inout_operations->AmpS2P.Z0;
       }
       if (real(zs_temp) == -1)//Check if the input value is correct
       {
           QMessageBox::warning(0, QObject::tr("Error"),
                                QObject::tr("Invalid source impedance"));
           return;
       }
       inout_operations->set_constant_ZS_vs_freq(zs_temp);
    }

    if (!FixedZLCheckbox->isChecked() && (!TwoPortMatchingCheckbox->isChecked()))
    {
       inout_operations->loadS1Pdata(LoadFile.toStdString(), LOAD);//s1p
    }
    else
    {
        complex<double> zl_temp;
        if (!TwoPortMatchingCheckbox->isChecked())
        {
        QByteArray ba = FixedZLLineedit->text().toLatin1();
        char * text = ba.data();
        zl_temp = getComplexImpedanceFromText(text);
        }
        else
        {
          zl_temp = inout_operations->AmpS2P.Z0;
        }

        if (zl_temp.real() == -1)//Check if the input value is correct
        {
            QMessageBox::warning(0, QObject::tr("Error"),
                                 QObject::tr("Invalid load impedance"));
            return;
        }
        inout_operations->set_constant_ZL_vs_freq(zl_temp);
    }

    //Check frequency specifications
    double fmatching_min = minFEdit->text().toDouble();
    double fmatching_max = maxFEdit->text().toDouble();

    if ((fmatching_min == -1) || (fmatching_max == -1) || (fmatching_min > fmatching_max))
    {
        QMessageBox::warning(0, QObject::tr("Error"),
                             QObject::tr("Wrong frequency settings"));
        return;
    }

    //Scale frequency according to the combobox units
    fmatching_min *= getFreqScale(minFUnitsCombo->currentIndex());
    fmatching_max *= getFreqScale(maxFUnitsCombo->currentIndex());
    inout_operations->set_matching_band(fmatching_min, fmatching_max);
    if (fmatching_min < min(inout_operations->getFrequency())-1e6)//The lower freq is not present at s1p/s2p
    {
        QMessageBox::warning(0, QObject::tr("Error"),
                             QObject::tr("One of the impedance data files does not contain the specified lower frequency"));
        return;
    }
    if (fmatching_max > max(inout_operations->getFrequency())+1e6)//The maximum freq is not present at s1p/s2p
    {
        QMessageBox::warning(0, QObject::tr("Error"),
                             QObject::tr("One of the impedance data files does not contain the specified upper frequency"));
        return;
    }

    //Check if the specified frequencies lie with the s1p/s2p data
    if (inout_operations->ResampleImpedances() == -2)
    {
        QMessageBox::warning(0, QObject::tr("Warning"),
                             QObject::tr("It is not possible to match the source and load ports simultaneously.\nThe unilateral approach will be used..."));
    }

    inout_operations->UseGNUplot = UseGNUplotCheckbox->isChecked();

    GRABIM * MatchingObject = new GRABIM();
    // Impedance and frequency settings
    MatchingObject->SetData(inout_operations->getMatchingData());
    MatchingObject->refine = RefineCheckbox->isChecked();
    MatchingObject->setSearchMode(0);//Default mode

    GRABIM_Result R, RS, RL;

    if (TwoPortMatchingCheckbox->isChecked())
    {
         RS = MatchingObject->RunGRABIM(inout_operations->ZS, inout_operations->Zin_maxg);//Input matching network
         RL = MatchingObject->RunGRABIM(inout_operations->Zout_maxg, inout_operations->ZL);//Output matching network
         (FixedZSCheckbox->isChecked()) ? RS.source_path = "" : RL.source_path = SourceFile.toStdString();
         (FixedZLCheckbox->isChecked()) ? RL.load_path = "": RL.load_path = LoadFile.toStdString();
         TwoPortCircuit TPC;
         TPC.InputMatching = RS;
         TPC.OutputMatching = RL;
         TPC.amplifier_path = AmpFile.toStdString();
         inout_operations->ExportQucsSchematic(TPC, "");

    }
    else
    {
        R = MatchingObject->RunGRABIM(inout_operations->ZS, inout_operations->ZL);
        (FixedZSCheckbox->isChecked()) ? R.source_path = "" : R.source_path = SourceFile.toStdString();
        (FixedZLCheckbox->isChecked()) ? R.load_path = "": R.load_path = LoadFile.toStdString();

        TwoPortCircuit TPC;
        TPC.InputMatching = R;
        TPC.QucsVersion = PACKAGE_VERSION;

        inout_operations->tmp_path = "/tmp";
        if (UseGNUplotCheckbox->isChecked())inout_operations->exportGNUplot(R, GNUplot_path.toStdString(), MatchingObject->refine);
        inout_operations->ExportQucsSchematic(TPC, "");
    }

    delete MatchingObject;
    delete inout_operations;
   
    // show result for some time
    ResultState = 0;
    LabelResult->setText(tr("Result:") + "<font color=\"#008000\"><b>  " +
                       tr("Successful") + "</b></font>");
    QTimer::singleShot(500, this, SLOT(slotShowResult()));

}

// Opens a file dialog to select the s1p file which contains source impedance data
void ui::SourceImpedance_clicked()
{
    SourceFile = QFileDialog::getOpenFileName(this,
        tr("Open S-parameter file"), QCoreApplication::applicationDirPath());
}

// Opens a file dialog to select the s1p file which contains load impedance data
void ui::LoadImpedance_clicked()
{
    LoadFile = QFileDialog::getOpenFileName(this,
        tr("Open S-parameter file"), QCoreApplication::applicationDirPath());
}

// Opens a file dialog to select the s2p data
void ui::BrowseAmplifier_S2P()
{
    AmpFile = QFileDialog::getOpenFileName(this,
        tr("Open S-parameter file"), QCoreApplication::applicationDirPath());
}

// Apart from generating a Qucs schematic, the S parameters of the network are stored in GNUplot format
// so the user can check the performance of the network without running Qucs
void ui::GNUplotOutput_clicked()
{
    GNUplot_path = QFileDialog::getExistingDirectory(this,
        tr("Save GNUplot script"), QCoreApplication::applicationDirPath());
}

// Shows hides button/lineedit for source impedance selection
void ui::FixedZSCheckbox_clicked()
{
    if (FixedZSCheckbox->isChecked())
    {
        FixedZSLineedit->setVisible(true);
        ZSOhmLabel->setVisible(true);
        SourceFileButton->setVisible(false);
    }
    else
    {
        FixedZSLineedit->setVisible(false);
        ZSOhmLabel->setVisible(false);
        SourceFileButton->setVisible(true);
    }
}

// Shows hides button/lineedit for load impedance selection
void ui::FixedZLCheckbox_clicked()
{
    if (FixedZLCheckbox->isChecked())
    {
        FixedZLLineedit->setVisible(true);
        ZLOhmLabel->setVisible(true);
        LoadFileButton->setVisible(false);
    }
    else
    {
        FixedZLLineedit->setVisible(false);
        ZLOhmLabel->setVisible(false);
        LoadFileButton->setVisible(true);
    }
}

//This function checks if the data introduced by the user contains text or strange characters
int ui::CheckInputText(string str)
{
  char c;
  for (unsigned int i = 0; i < str.length();i++)
  {
   c = str.at(i);
     if ((c < 48) || (c>57))//No letters allowed
       return -1;
  }
  return 0;

}

// The user can also specify a constant complex impedance vs frequency. This function parses user's input
// and return the cx_double value
complex<double> ui::getComplexImpedanceFromText(char *arg)
{
    string arg_str = arg;
    int index = arg_str.find_first_of("j");
    int sign = 1;
    double zreal, zimag;
    if (arg_str.find_first_of("j") != string::npos)//Then it is a complex impedance
    {
        string ZR_str = arg_str.substr(0, index-1);
        if (CheckInputText(ZR_str) == -1) return complex<double>(-1, -1);
        zreal = atof(ZR_str.c_str());
        if (!arg_str.substr(index-1, 1).compare("-")) sign = -1;

        string ZI_str = arg_str.substr(index+1);
        if (CheckInputText(ZI_str) == -1) return complex<double>(-1, -1);
        zimag = sign*atof(ZI_str.c_str());
        return complex<double>(zreal, zimag);
    }
    else//Real impedance
    {
        string ZR_str = arg;
        if (CheckInputText(ZR_str) == -1) return complex<double>(-1, -1);
        zreal = atof(arg);
        if (zreal > 0)return complex<double>(zreal, 0);
        else//Wrong input...
            return complex<double>(-1, -1);
    }
}

// Gets frequency scale from the combobox index
double ui::getFreqScale(int index)
{
   switch (index)
   {
      case 0: return 1e3;break;
      case 1: return 1e6;break;
      case 2: return 1e9;break;

   }
   return 1e6;
}

//This function display the result {success, failure} of the synthesis
//Copied from qucs-filter
void ui::slotShowResult()
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

//This function catches the event triggered by the 2-port checkbox
void ui::TwoPortMatchingCheckbox_state(int state)
{
   if (state == Qt::Unchecked)
   { 
      //Deactivate the S2P button
      imgWidget->setVisible(true);
      AmplifierS2Pbutton->setVisible(false);

      //Show the source and load options
      (FixedZSCheckbox->isChecked())? FixedZSLineedit->setVisible(true): SourceFileButton->setVisible(true);
      (FixedZLCheckbox->isChecked())? FixedZLLineedit->setVisible(true): LoadFileButton->setVisible(true);

      FixedZSCheckbox->setVisible(true);
      FixedZLCheckbox->setVisible(true);

      SourceLabel->setText("Source");
      LoadLabel->setText("Load");

   }
   else
   {
      //Activate S2P button
      imgWidget->setVisible(false);
      AmplifierS2Pbutton->setVisible(true);

      //Hide terms. Two-port matching can only be done at the impedance
      //at which the S2P device was measured:

      SourceFileButton->setVisible(false);
      LoadFileButton->setVisible(false);

      FixedZSCheckbox->setVisible(false);
      FixedZLCheckbox->setVisible(false);

      FixedZSLineedit->setVisible(false);
      FixedZLLineedit->setVisible(false);

      SourceLabel->setText("Source\nZ0");
      LoadLabel->setText("Load\nZ0");

   }
}
