/****************************************************************************
**     Qucs Attenuator Synthesis
**     qucsattenuator.cpp
**
**
**
**
**
**
**
*****************************************************************************/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "attenuatorfunc.h"
#include "qucsattenuator.h"


#include <QGridLayout>
#include <QPixmap>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QComboBox>
#include <QValidator>
#include <QClipboard>
#include <QApplication>
#include <QDebug>


QucsAttenuator::QucsAttenuator()
{

  QWidget *centralWidget = new QWidget(this);  
  setCentralWidget(centralWidget);

  setWindowIcon(QPixmap(":/bitmaps/big.qucs.xpm"));
  setWindowTitle("Qucs Attenuator " PACKAGE_VERSION);

  QMenu *fileMenu = new QMenu(tr("&File"));

  QAction *fileQuit = new QAction(tr("&Quit"), this);
  fileQuit->setShortcut(Qt::CTRL+Qt::Key_Q);
  connect(fileQuit, SIGNAL(activated()), SLOT(slotQuit()));

  fileMenu->addAction(fileQuit);

  QMenu *helpMenu = new QMenu(tr("&Help"));

  QAction *helpHelp = new QAction(tr("&Help"), this);
  helpHelp->setShortcut(Qt::Key_F1);
  helpMenu->addAction(helpHelp);
  connect(helpHelp, SIGNAL(activated()), SLOT(slotHelpIntro()));

  QAction *helpAbout = new QAction(tr("&About"), this);
  helpMenu->addAction(helpAbout);
  connect(helpAbout, SIGNAL(activated()), SLOT(slotHelpAbout()));


  helpMenu->addSeparator();

  QAction *about = new QAction(tr("About Qt..."), this);
  helpMenu->addAction(about);
  connect(about, SIGNAL(activated()), SLOT(slotHelpAboutQt()));

  menuBar()->addMenu(fileMenu);
  menuBar()->addSeparator();
  menuBar()->addMenu(helpMenu);


  //==========Left
  QVBoxLayout *vboxLeft = new QVBoxLayout();

  QGroupBox *TopoGroup = new QGroupBox(tr("Topology"));
  QGridLayout * topoGrid = new QGridLayout(TopoGroup);

  ComboTopology = new QComboBox();//=================Topology Combobox
  ComboTopology->insertItem(1, "Pi");
  ComboTopology->insertItem(2, "Tee");
  ComboTopology->insertItem(3, "Bridged Tee");
  ComboTopology->insertItem(4, "Reflection attenuator");
  connect(ComboTopology, SIGNAL(activated(int)), SLOT(slotTopologyChanged()));
  topoGrid->addWidget(ComboTopology, 1,0,1,2);

  pixTopology = new QLabel(TopoGroup);//====================Pixmap for Topology
  pixTopology->setPixmap(QPixmap((":/bitmaps/att_pi.png")));
  topoGrid->addWidget(pixTopology,2,0,3,2);

  topoGrid->setSpacing(5);
  TopoGroup->setLayout(topoGrid);

  vboxLeft->addWidget(TopoGroup);


  //S-parameter box option
  SparBoxCheckbox = new QCheckBox("Add S-parameter simulation");
  SparBoxCheckbox->setChecked(false);
  vboxLeft->addWidget(SparBoxCheckbox);

  //==========Right
  QVBoxLayout *vboxRight = new QVBoxLayout();

  QGroupBox * InputGroup = new QGroupBox (tr("Input"));
  QGridLayout * inGrid = new QGridLayout();
  inGrid->setSpacing(1);

  DoubleVal = new QDoubleValidator(this);
  DoubleValPower = new QDoubleValidator(this);
  DoubleVal->setBottom(0);
  DoubleValPower->setBottom(-1e9);//The default power unit is dBm, so Pin < 0 is expected

  LabelAtten = new QLabel(tr("Attenuation:"), InputGroup);
  inGrid ->addWidget(LabelAtten, 1,0);
  QSpinBox_Attvalue = new QDoubleSpinBox();
  QSpinBox_Attvalue->setValue(1);
  QSpinBox_Attvalue->setMinimum(0.1);
  QSpinBox_Attvalue->setMaximum(1e6);
  connect(QSpinBox_Attvalue, SIGNAL(valueChanged(double)), this,
      SLOT(slotCalculate()) );
  inGrid->addWidget(QSpinBox_Attvalue, 1,1);
  QLabel *Label1 = new QLabel(tr("dB"), InputGroup);
  inGrid->addWidget(Label1, 1,2);

  LabelImp1 = new QLabel(tr("Zin:"), InputGroup);
  LabelImp1->setWhatsThis("Input impedance");
  inGrid->addWidget(LabelImp1, 2,0);
  QSpinBox_Zin = new QDoubleSpinBox();
  QSpinBox_Zin->setValue(50);
  QSpinBox_Zin->setMinimum(0);
  QSpinBox_Zin->setMaximum(1e6);
  connect(QSpinBox_Zin, SIGNAL(valueChanged(double)), this,
      SLOT(slotSetText_Zin(double)) );

  inGrid->addWidget(QSpinBox_Zin, 2,1);
  QLabel *Label2 = new QLabel(QChar(0xa9, 0x03), InputGroup);
  inGrid->addWidget(Label2, 2,2);

  LabelImp2 = new QLabel(tr("Zout:"), InputGroup);
  LabelImp2->setWhatsThis("Output impedance");
  inGrid->addWidget(LabelImp2, 3,0);
  QSpinBox_Zout = new QDoubleSpinBox();
  QSpinBox_Zout->setValue(50);
  QSpinBox_Zout->setMinimum(0);
  QSpinBox_Zout->setMaximum(1e6);
  connect(QSpinBox_Zout, SIGNAL(valueChanged(double)), this,
      SLOT(slotSetText_Zout(double)) );
  inGrid->addWidget(QSpinBox_Zout, 3,1);
  LabelImp2_Ohm = new QLabel(QChar(0xa9, 0x03), InputGroup);
  inGrid->addWidget(LabelImp2_Ohm, 3,2);

  Label_Pin = new  QLabel(tr("Pin:"), InputGroup);
  Label_Pin->setWhatsThis("Input power");
  inGrid->addWidget(Label_Pin, 4,0);
  QSpinBox_InputPower = new QDoubleSpinBox(0);
  QSpinBox_InputPower->setMinimum(-1e3);
  QSpinBox_InputPower->setMaximum(1e5);
  connect(QSpinBox_InputPower, SIGNAL(valueChanged(double)), this, SLOT(slotCalculate()));
  inGrid->addWidget(QSpinBox_InputPower, 4,1);
  QStringList powerunits;
  powerunits.append("mW");
  powerunits.append("W");
  powerunits.append("dBm");
  powerunits.append(QString("dB%1V [75%2]").arg(QChar(0xbc, 0x03)).arg(QChar(0xa9, 0x03)));
  powerunits.append(QString("dB%1V [50%2]").arg(QChar(0xbc, 0x03)).arg(QChar(0xa9, 0x03)));
  powerunits.append(QString("dBmV [75%1]").arg(QChar(0xa9, 0x03)));
  powerunits.append(QString("dBmV [50%1]").arg(QChar(0xa9, 0x03)));
  Combo_InputPowerUnits = new QComboBox();
  Combo_InputPowerUnits->addItems(powerunits);
  Combo_InputPowerUnits->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);
  Combo_InputPowerUnits->setCurrentIndex(2);//Input power is mostly given in dBm
  connect(Combo_InputPowerUnits, SIGNAL(currentIndexChanged(QString)), this,
      SLOT(slot_ComboInputPowerUnits_Changed(const QString&)) );
  inGrid->addWidget(Combo_InputPowerUnits, 4,2);

  minR_Reflection_Att = new QCheckBox("Use R > Z0");
  minR_Reflection_Att->hide();
  inGrid->addWidget(minR_Reflection_Att, 5,0);

  InputGroup->setLayout(inGrid);

  vboxRight->addWidget(InputGroup);

  Calculate = new QPushButton(tr("Put into Clipboard"));
  connect(Calculate, SIGNAL(clicked()), SLOT(slotCalculate()));

  vboxRight->addWidget(Calculate);

  QGroupBox * OutputGroup = new QGroupBox (tr("Output"));
  QGridLayout * outGrid = new QGridLayout(OutputGroup);
  outGrid->setSpacing(5);
  outGrid->setColumnMinimumWidth(3, 20);

  //Power dissipation label
  PdissLabel = new QLabel("Pdiss", OutputGroup);
  PdissLabel->setAlignment(Qt::AlignCenter);
  outGrid->addWidget(PdissLabel, 0,5);

  //R1 value and labels
  LabelR1 = new QLabel(tr("R1:"), OutputGroup);
  outGrid->addWidget(LabelR1, 1,0);
  lineEdit_R1 = new QLineEdit(tr("--"), OutputGroup);
  lineEdit_R1->setReadOnly(true);
  outGrid->addWidget(lineEdit_R1, 1,1);
  QLabel *Label4 = new QLabel(QChar(0xa9, 0x03), OutputGroup);
  outGrid->addWidget(Label4, 1,2);

  //R1 power dissipation
  lineEdit_R1_Pdiss = new QLineEdit(tr("--"), OutputGroup);
  lineEdit_R1_Pdiss->setReadOnly(true);
  outGrid->addWidget(lineEdit_R1_Pdiss, 1,5);
  ComboR1_PowerUnits = new QComboBox();
  ComboR1_PowerUnits->addItems(powerunits);
  ComboR1_PowerUnits->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);
  connect(ComboR1_PowerUnits, SIGNAL(currentIndexChanged(QString)), this,
      SLOT(slot_ComboR1PowerUnits_Changed(const QString&)) );
  outGrid->addWidget(ComboR1_PowerUnits, 1,6);

  //R2 value and labels
  LabelR2 = new QLabel(tr("R2:"), OutputGroup);
  outGrid->addWidget(LabelR2, 2,0);
  lineEdit_R2 = new QLineEdit(tr("--"), OutputGroup);
  lineEdit_R2->setReadOnly(true);
  outGrid->addWidget(lineEdit_R2, 2,1);
  QLabel *Label5 = new QLabel(QChar(0xa9, 0x03), OutputGroup);
  outGrid->addWidget(Label5, 2,2);

  //R2 power dissipation
  lineEdit_R2_Pdiss = new QLineEdit(tr("--"), OutputGroup);
  lineEdit_R2_Pdiss->setReadOnly(true);
  outGrid->addWidget(lineEdit_R2_Pdiss, 2,5);
  ComboR2_PowerUnits = new QComboBox();
  ComboR2_PowerUnits->addItems(powerunits);
  ComboR2_PowerUnits->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);
  connect(ComboR2_PowerUnits, SIGNAL(currentIndexChanged(QString)), this,
      SLOT(slot_ComboR2PowerUnits_Changed(const QString&)) );
  outGrid->addWidget(ComboR2_PowerUnits, 2,6);

  //R3 value and labels
  LabelR3 = new QLabel(tr("R3:"), OutputGroup);
  outGrid->addWidget(LabelR3, 3,0);
  lineEdit_R3 = new QLineEdit(tr("--"), OutputGroup);
  lineEdit_R3->setReadOnly(true);
  outGrid->addWidget(lineEdit_R3, 3,1);
  LabelR3_Ohm = new QLabel(QChar(0xa9, 0x03), OutputGroup);
  outGrid->addWidget(LabelR3_Ohm, 3,2);

  //R3 power dissipation
  lineEdit_R3_Pdiss = new QLineEdit(tr("--"), OutputGroup);
  lineEdit_R3_Pdiss->setReadOnly(true);
  outGrid->addWidget(lineEdit_R3_Pdiss, 3,5);
  ComboR3_PowerUnits = new QComboBox();
  ComboR3_PowerUnits->addItems(powerunits);
  ComboR3_PowerUnits->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);
  connect(ComboR3_PowerUnits, SIGNAL(currentIndexChanged(QString)), this,
      SLOT(slot_ComboR3PowerUnits_Changed(const QString&)) );
  outGrid->addWidget(ComboR3_PowerUnits, 3,6);

  //R4 value and labels
  LabelR4 = new QLabel(tr("R4:"), OutputGroup);
  outGrid->addWidget(LabelR4, 4,0);
  lineEdit_R4 = new QLineEdit(tr("--"), OutputGroup);
  lineEdit_R4->setReadOnly(true);
  outGrid->addWidget(lineEdit_R4, 4,1);
  LabelR4_Ohm = new QLabel(QChar(0xa9, 0x03), OutputGroup);
  outGrid->addWidget(LabelR4_Ohm, 4,2);

  //R4 power dissipation
  lineEdit_R4_Pdiss = new QLineEdit(tr("--"), OutputGroup);
  lineEdit_R4_Pdiss->setReadOnly(true);
  outGrid->addWidget(lineEdit_R4_Pdiss, 4,5);
  ComboR4_PowerUnits = new QComboBox();
  ComboR4_PowerUnits->addItems(powerunits);
  ComboR4_PowerUnits->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);
  connect(ComboR4_PowerUnits, SIGNAL(currentIndexChanged(QString)), this,
      SLOT(slot_ComboR4PowerUnits_Changed(const QString&)) );
  outGrid->addWidget(ComboR4_PowerUnits, 4,6);

  //Hide R4 widgets. R4 is only used in the Bridge Tee attenuator
  LabelR4->hide();
  lineEdit_R4->hide();
  LabelR4_Ohm->hide();
  lineEdit_R4_Pdiss->hide();
  ComboR4_PowerUnits->hide();


  // This variable is necessary to provide the power unit conversion when the corresponding power
  // comboboxes are changed
  LastUnits.append("dBm");//Input power
  LastUnits.append("mW");//Power dissipated by R1
  LastUnits.append("mW");//Power dissipated by R2
  LastUnits.append("mW");//Power dissipated by R3
  LastUnits.append("mW");//Power dissipated by R4


  vboxRight->addWidget(OutputGroup);

  // put Left and Right together
  QHBoxLayout *hbox = new QHBoxLayout();
  hbox->addLayout(vboxLeft);
  hbox->addLayout(vboxRight);

  // append the result label
  LabelResult = new QLabel(tr("Result:"));
  LabelResult->setAlignment(Qt::AlignHCenter);

  QVBoxLayout *vbox = new QVBoxLayout();
  vbox->addLayout(hbox);
  vbox->addWidget(LabelResult);

  centralWidget->setLayout(vbox);
  slotCalculate();
}

QucsAttenuator::~QucsAttenuator()
{
  delete DoubleVal;
}

void QucsAttenuator::slotHelpIntro()
{
  QMessageBox::about(this, tr("Qucs Attenuator Help"), 
    tr("QucsAttenuator is an attenuator synthesis program. "
         "To create a attenuator, simply enter all "
         "the input parameters and press the calculation button. "
         "Immediatly, the "
         "schematic of the attenuator is calculated and "
         "put into the clipboard. Now go to Qucs, "
         "open an schematic and press "
         "CTRL-V (paste from clipboard). The attenuator "
         "schematic can now be inserted. "
         "Have lots of fun!"));
}

void QucsAttenuator::slotHelpAboutQt()
{
      QMessageBox::aboutQt(this, tr("About Qt"));
}

void QucsAttenuator::slotHelpAbout()
{
    QMessageBox::about(this, tr("About..."),
    "QucsAttenuator Version " PACKAGE_VERSION+
    tr("\nAttenuator synthesis program\n")+
    tr("Copyright (C) 2006 by")+" Toyoyuki Ishikawa"
    "\n"+
    tr("Copyright (C) 2006 by")+" Stefan Jahn"
    "\n"
    "\nThis is free software; see the source for copying conditions."
    "\nThere is NO warranty; not even for MERCHANTABILITY or "
    "\nFITNESS FOR A PARTICULAR PURPOSE.\n\n");
}

void QucsAttenuator::slotQuit()
{
  int tmp;
  tmp = x();
  tmp = y();
  tmp = width();
  tmp = height();
  Q_UNUSED(tmp);

  qApp->quit();
}

void QucsAttenuator::slotSetText_Zin( double val )
{
  if((ComboTopology->currentIndex() == BRIDGE_TYPE) || (ComboTopology->currentIndex() == REFLECTION_TYPE)) {
    QSpinBox_Zout->blockSignals( TRUE );
    QSpinBox_Zout->setValue(val);
    QSpinBox_Zout->blockSignals( FALSE );
  }
  slotCalculate();
}

void QucsAttenuator::slotSetText_Zout( double val)
{
  if(ComboTopology->currentIndex() == BRIDGE_TYPE) {
      QSpinBox_Zin->blockSignals( TRUE );
      QSpinBox_Zin->setValue(val);
      QSpinBox_Zin->blockSignals( FALSE );
  }
  slotCalculate();
}

void QucsAttenuator::slotTopologyChanged()
{
  switch(ComboTopology->currentIndex())
    {
    case PI_TYPE:
      pixTopology->setPixmap(QPixmap((":/bitmaps/att_pi.png")));
      LabelImp1->setText("Zin:");
      LabelImp2->show();
      QSpinBox_Zout->show();
      LabelImp2_Ohm->show();
      LabelR2->setText("R2:");
      LabelR3->show();
      LabelR3->setText("R3:");
      LabelR4->hide();
      lineEdit_R3->show();
      lineEdit_R4->hide();
      LabelR4_Ohm->hide();
      LabelR3_Ohm->show();
      lineEdit_R4_Pdiss->hide();
      ComboR4_PowerUnits->hide();
      minR_Reflection_Att->hide();
      break;
    case TEE_TYPE:
      pixTopology->setPixmap(QPixmap((":/bitmaps/att_tee.png")));
      LabelImp1->setText("Zin:");
      LabelImp2->show();
      QSpinBox_Zout->show();
      LabelImp2_Ohm->show();
      LabelR2->setText("R2:");
      LabelR3->show();
      LabelR3->setText("R3:");
      LabelR4->hide();
      lineEdit_R3->show();
      lineEdit_R4->hide();
      LabelR4_Ohm->hide();
      LabelR3_Ohm->show();
      lineEdit_R4_Pdiss->hide();
      ComboR4_PowerUnits->hide();
      minR_Reflection_Att->hide();
      break;
    case BRIDGE_TYPE:
      pixTopology->setPixmap(QPixmap((":/bitmaps/att_bridge.png")));
      LabelImp1->setText("Z0:");
      LabelImp2->hide();
      QSpinBox_Zout->hide();
      LabelImp2_Ohm->hide();
      LabelR2->setText("R4:");
      LabelR3->show();
      LabelR4->show();
      LabelR3->setText("Z01:");
      LabelR4->setText("Z02:");
      lineEdit_R3->show();
      lineEdit_R4->show();
      LabelR3_Ohm->show();
      LabelR4_Ohm->show();
      lineEdit_R3_Pdiss->show();
      lineEdit_R4_Pdiss->show();
      ComboR3_PowerUnits->show();
      ComboR4_PowerUnits->show();
      QSpinBox_Zout->setValue(QSpinBox_Zin->value());
      minR_Reflection_Att->hide();
      break;
    case REFLECTION_TYPE:
      pixTopology->setPixmap(QPixmap((":/bitmaps/att_reflection.png")));
      LabelImp1->setText("Z0:");
      LabelImp2->hide();
      QSpinBox_Zout->hide();
      LabelImp2_Ohm->hide();
      LabelR2->setText("R2:");
      LabelR3->hide();
      LabelR4->hide();
      lineEdit_R3->hide();
      lineEdit_R4->hide();
      LabelR3_Ohm->hide();
      LabelR4_Ohm->hide();
      lineEdit_R3_Pdiss->hide();
      lineEdit_R4_Pdiss->hide();
      ComboR3_PowerUnits->hide();
      ComboR4_PowerUnits->hide();
      QSpinBox_Zout->setValue(QSpinBox_Zin->value());
      minR_Reflection_Att->show();
    break;
    }
    adjustSize();
    slotCalculate();
}

void QucsAttenuator::slotCalculate()
{
    QUCS_Att qatt;
    int result;
    QString * s = NULL;
    struct tagATT Values;


    Values.Topology = ComboTopology->currentIndex();
    Values.Attenuation = QSpinBox_Attvalue->value();
    Values.Zin = QSpinBox_Zin->value();
    Values.Zout = QSpinBox_Zout->value();
    Values.minR = minR_Reflection_Att->isChecked();

    //Calculate the input power
    Values.Pin = ConvertPowerUnits(QSpinBox_InputPower->value(), Combo_InputPowerUnits->currentText(), "W");

    result = qatt.Calc(&Values);

    if(result != -1)
    {
      LabelResult->setText(tr("Result:")+" "+tr("Success!"));
      lineEdit_R1->setText(QString::number(Values.R1, 'f', 1));
      lineEdit_R2->setText(QString::number(Values.R2, 'f', 1));
      lineEdit_R3->setText(QString::number(Values.R3, 'f', 1));
      lineEdit_R4->setText(QString::number(Values.R4, 'f', 1));

      lineEdit_R1_Pdiss->setText(QString::number(ConvertPowerUnits(Values.PR1, QString("W"), ComboR1_PowerUnits->currentText()), 'f', 5));
      lineEdit_R2_Pdiss->setText(QString::number(ConvertPowerUnits(Values.PR2, "W", ComboR2_PowerUnits->currentText()), 'f', 5));
      lineEdit_R3_Pdiss->setText(QString::number(ConvertPowerUnits(Values.PR3, "W", ComboR3_PowerUnits->currentText()), 'f', 5));
      lineEdit_R4_Pdiss->setText(QString::number(ConvertPowerUnits(Values.PR4, "W", ComboR4_PowerUnits->currentText()), 'f', 5));
      
      s = qatt.createSchematic(&Values, this->SparBoxCheckbox->isChecked());
      if(!s) return;
      
      QClipboard *cb = QApplication::clipboard();
      cb->setText(*s);
      delete s;
    }
    else
    {
      LabelResult->setText(tr("Error: Set Attenuation less than %1 dB").arg(QString::number(Values.MinimumATT, 'f', 3)));
      lineEdit_R1->setText("--");
      lineEdit_R2->setText("--");
      lineEdit_R3->setText("--");
    }
    adjustSize();
}

//This function is caled when the units of the input power are changed
void QucsAttenuator::slot_ComboInputPowerUnits_Changed(const QString& new_units)
{
   //Convert power
   double P = QSpinBox_InputPower->value();
   P =ConvertPowerUnits(P, LastUnits[0], new_units);
   QSpinBox_InputPower->setValue(P);
   LastUnits[0] = new_units;

   //Change lineedit input policy
   if ((new_units == "W") || (new_units == "mW"))
      QSpinBox_InputPower->setMinimum(0);
   else//dB units
      QSpinBox_InputPower->setMinimum(-1e3);
   slotCalculate();
}

//This function is called when the units of the power dissipated by R1 are changed
void QucsAttenuator::slot_ComboR1PowerUnits_Changed(const QString& new_units)
{
   //Convert power
   double P = lineEdit_R1_Pdiss->text().toDouble();
   P =ConvertPowerUnits(P, LastUnits[0], new_units);
   lineEdit_R1_Pdiss->setText(QString("%1").arg(P));
   LastUnits[0] = new_units;

   //Change lineedit input policy
   if ((new_units == "W") || (new_units == "mW"))
      DoubleValPower->setBottom(0);
   else//dB units
      DoubleValPower->setBottom(-1e9);
   lineEdit_R1_Pdiss->setValidator(DoubleValPower);
}

//This function is caled when the units of the power dissipated by R2 are changed
void QucsAttenuator::slot_ComboR2PowerUnits_Changed(const QString& new_units)
{
   //Convert power
   double P = lineEdit_R2_Pdiss->text().toDouble();
   P =ConvertPowerUnits(P, LastUnits[0], new_units);
   lineEdit_R2_Pdiss->setText(QString("%1").arg(P));
   LastUnits[0] = new_units;

   //Change lineedit input policy
   if ((new_units == "W") || (new_units == "mW"))
      DoubleValPower->setBottom(0);
   else//dB units
      DoubleValPower->setBottom(-1e9);
   lineEdit_R2_Pdiss->setValidator(DoubleValPower);
}

//This function is caled when the units of the power dissipated by R3 are changed
void QucsAttenuator::slot_ComboR3PowerUnits_Changed(const QString& new_units)
{
   //Convert power
   double P = lineEdit_R3_Pdiss->text().toDouble();
   P =ConvertPowerUnits(P, LastUnits[0], new_units);
   lineEdit_R3_Pdiss->setText(QString("%1").arg(P));
   LastUnits[0] = new_units;

   //Change lineedit input policy
   if ((new_units == "W") || (new_units == "mW"))
      DoubleValPower->setBottom(0);
   else//dB units
      DoubleValPower->setBottom(-1e9);
   lineEdit_R3_Pdiss->setValidator(DoubleValPower);
}

// This function performs the power units conversion. It receives two arguments: the original units and the
// new units.
double QucsAttenuator::ConvertPowerUnits(double Pin, QString from_units, QString to_units)
{
  //Convert "from_units" to Watts
  if (from_units == "W")
      ;//Do nothing, this step is not needed
  else
      if (from_units == "dBm")
          Pin = pow(10, 0.1*(Pin-30));//dBm -> W
      else
          if (from_units == QString("dB%1V [75%2]").arg(QChar(0xbc, 0x03)).arg(QChar(0xa9, 0x03)))
              Pin = pow(10, (0.1*Pin-12))/75;//dBuV [75Ohm] -> W
         else
             if (from_units == QString("dB%1V [50%2]").arg(QChar(0xbc, 0x03)).arg(QChar(0xa9, 0x03)))
                 Pin = pow(10, (0.1*Pin-12))/50;//dBuV [50Ohm] -> W
             else
                 if (from_units == QString("dBmV [75%2]").arg(QChar(0xa9, 0x03)))
                     Pin = pow(10, (0.1*Pin-6))/75;//dBmV [75Ohm] -> W
                 else
                     if (from_units == QString("dBmV [50%2]").arg(QChar(0xa9, 0x03)))
                         Pin = pow(10, (0.1*Pin-6))/50;//dBmV [50Ohm] -> W
                     else
                         if (from_units == "mW")
                             Pin = Pin*1e-3;//mW -> W

  //Convert Watts to "to_units"
  if (to_units == "W") return Pin;//Already done
  if (to_units == "mW")
      return Pin*1e3;//W -> mW

  //Convert to dBm. The other units are easily converted from dBm
  Pin = 10*log10(Pin)+30;//W->dBm
  if (to_units == "dBm")
      return Pin;//Already done
  else
      if (to_units == QString("dB%1V [75%2]").arg(QChar(0xbc, 0x03)).arg(QChar(0xa9, 0x03)))
          Pin += 108.7506126339170004686755011380612925566374910126647878220;//W -> dBuV [75Ohm]
      else
          if (to_units == QString("dB%1V [50%2]").arg(QChar(0xbc, 0x03)).arg(QChar(0xa9, 0x03)))
              Pin += 106.9897000433601880478626110527550697323181011853789145868;//W -> dBuV [50Ohm]
          else
              if (to_units == QString("dBmV [75%2]").arg(QChar(0xa9, 0x03)))
                  Pin += 48.7506126339170004686755011380612925566374910126647878220;//W -> dBmV [75Ohm]
              else
                  if (to_units == QString("dBmV [50%2]").arg(QChar(0xa9, 0x03)))
                      Pin += 46.9897000433601880478626110527550697323181011853789145868;//W -> dBmV [50Ohm]

  return Pin;
}
