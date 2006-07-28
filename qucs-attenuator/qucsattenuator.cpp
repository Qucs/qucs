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
#include "helpdialog.h"

#include <qvgroupbox.h>
#include <qvbox.h>
#include <qmenubar.h>
#include <qpopupmenu.h>
#include <qmessagebox.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qlineedit.h>
#include <qcombobox.h>
#include <qvalidator.h>
#include <qtimer.h>
#include <qclipboard.h>
#include <qapplication.h>
#include <qimage.h>

QucsAttenuator::QucsAttenuator()
{
  setIcon(QPixmap(QucsSettings.BitmapDir + "big.qucs.xpm"));
  setCaption("Qucs Attenuator " PACKAGE_VERSION);

  QPopupMenu *fileMenu = new QPopupMenu();
  fileMenu->insertItem(tr("E&xit"), this, SLOT(slotQuit()), CTRL+Key_Q);

  QPopupMenu *helpMenu = new QPopupMenu();
  helpMenu->insertItem(tr("Help..."), this, SLOT(slotHelpIntro()), Key_F1);
  helpMenu->insertSeparator();
  helpMenu->insertItem(
            tr("&About QucsAttenuator..."), this, SLOT(slotHelpAbout()), 0);
  helpMenu->insertItem(tr("About Qt..."), this, SLOT(slotHelpAboutQt()), 0);

  QMenuBar *bar = new QMenuBar(this);
  bar->insertItem(tr("&File"), fileMenu);
  bar->insertSeparator ();
  bar->insertItem(tr("&Help"), helpMenu);


  QVBoxLayout * v2 = new QVBoxLayout (this);
  QVBox * vm = new QVBox (this);
  vm->setSpacing(0);
  v2->setSpacing(2);

  QWidget *Space = new QWidget(this);   // reserve space for menubar
  Space->setFixedSize(5, bar->height());
  v2->addWidget(Space);

  QHBox * h1 = new QHBox (this);
  v2->addWidget(h1);
  h1->setSpacing(2);
  h1->setMargin(5);
  QVBox * v1 = new QVBox (h1);
  v1->setSpacing(2);

  QGroupBox * TopoGroup = new QGroupBox (tr("Topology"), v1);
  QGridLayout * tbox = new QGridLayout(TopoGroup, 3,1,5,5);

  QWidget *Space2 = new QWidget(TopoGroup);
  Space2->setFixedSize(8, 8);
  tbox->addMultiCellWidget(Space2,0,0,0,0);

  ComboTopology = new QComboBox(TopoGroup);//=================Topology Combobox
  ComboTopology->insertItem("Pi");
  ComboTopology->insertItem("Tee");
  ComboTopology->insertItem("Bridged Tee");
  connect(ComboTopology, SIGNAL(activated(int)), SLOT(slotTopologyChanged()));
  tbox->addWidget(ComboTopology, 1,0);

  pixTopology = new QLabel(TopoGroup);//====================Pixmap for Topology
  pixTopology->setPixmap(QPixmap (QImage (QucsSettings.BitmapDir + "att_pi.png")));
  tbox->addMultiCellWidget(pixTopology,2,2,0,0);

  QVBox * vb = new QVBox (h1);
  vb->setSpacing(2);

  QGroupBox * InputGroup = new QGroupBox (tr("Input"), vb);
  QGridLayout * ibox = new QGridLayout(InputGroup, 5,3,5,5);
  ibox->addMultiCellWidget(Space2,0,0,0,2);

  IntVal = new QIntValidator(this);
  DoubleVal = new QDoubleValidator(this);

  LabelAtten = new QLabel(tr("Attenuation:"), InputGroup);
  ibox ->addWidget(LabelAtten, 1,0);
  lineEdit_Attvalue = new QLineEdit(tr("1"), InputGroup);
  lineEdit_Attvalue->setValidator(DoubleVal);
  ibox->addWidget(lineEdit_Attvalue, 1,1);
  QLabel *Label1 = new QLabel(tr("dB"), InputGroup);
  ibox->addWidget(Label1, 1,2);

  LabelImp1 = new QLabel(tr("Zin:"), InputGroup);
  ibox->addWidget(LabelImp1, 2,0);
  lineEdit_Zin = new QLineEdit(tr("50"), InputGroup);
  lineEdit_Zin->setValidator(DoubleVal);
  ibox->addWidget(lineEdit_Zin, 2,1);
  QLabel *Label2 = new QLabel(tr("Ohm"), InputGroup);
  ibox->addWidget(Label2, 2,2);

  LabelImp2 = new QLabel(tr("Zout:"), InputGroup);
  ibox->addWidget(LabelImp2, 3,0);
  lineEdit_Zout = new QLineEdit(tr("50"), InputGroup);
  lineEdit_Zout->setValidator(DoubleVal);
  ibox->addWidget(lineEdit_Zout, 3,1);
  QLabel *Label3 = new QLabel(tr("Ohm"), InputGroup);
  ibox->addWidget(Label3, 3,2);

  Calculate = new QPushButton(tr("Calculate and put into Clipboard"), vb);
  connect(Calculate, SIGNAL(clicked()), SLOT(slotCalculate()));

  QGroupBox * OutputGroup = new QGroupBox (tr("Output"), vb);
  QGridLayout * obox = new QGridLayout(OutputGroup, 5,3,5,5);
  obox->addMultiCellWidget(Space2,0,0,0,2);

  LabelR1 = new QLabel(tr("R1:"), OutputGroup);
  obox->addWidget(LabelR1, 1,0);
  lineEdit_R1 = new QLineEdit(tr("--"), OutputGroup);
  obox->addWidget(lineEdit_R1, 1,1);
  QLabel *Label4 = new QLabel(tr("Ohm"), OutputGroup);
  obox->addWidget(Label4, 1,2);

  LabelR2 = new QLabel(tr("R2:"), OutputGroup);
  obox->addWidget(LabelR2, 2,0);
  lineEdit_R2 = new QLineEdit(tr("--"), OutputGroup);
  obox->addWidget(lineEdit_R2, 2,1);
  QLabel *Label5 = new QLabel(tr("Ohm"), OutputGroup);
  obox->addWidget(Label5, 2,2);

  LabelR3 = new QLabel(tr("R3:"), OutputGroup);
  obox->addWidget(LabelR3, 3,0);
  lineEdit_R3 = new QLineEdit(tr("--"), OutputGroup);
  obox->addWidget(lineEdit_R3, 3,1);
  LabelR3_Ohm = new QLabel(tr("Ohm"), OutputGroup);
  obox->addWidget(LabelR3_Ohm, 3,2);

  LabelResult = new QLabel(tr("Result:"), this);
  v2->addWidget(LabelResult);
  LabelResult->setAlignment(Qt::AlignHCenter);

  QWidget *Space1 = new QWidget(this);   // reserve space for menubar
  Space1->setFixedSize(5,5);
  v2->addWidget(Space1);
}

QucsAttenuator::~QucsAttenuator()
{
  delete IntVal;
  delete DoubleVal;
}

void QucsAttenuator::slotHelpIntro()
{
  HelpDialog *d = new HelpDialog(this);
  d->show();
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

  accept();
  close();
}


void QucsAttenuator::slotTopologyChanged()
{
  switch(ComboTopology->currentItem())
    {
    case PI_TYPE:
      pixTopology->setPixmap(QPixmap(QImage(QucsSettings.BitmapDir + "att_pi.png")));
      LabelR2->setText("R2:");
      LabelR3->show();
      lineEdit_R3->show();
      LabelR3_Ohm->show();
      break;
    case TEE_TYPE:
      pixTopology->setPixmap(QPixmap(QImage(QucsSettings.BitmapDir + "att_tee.png")));
      LabelR2->setText("R2:");
      LabelR3->show();
      lineEdit_R3->show();
      LabelR3_Ohm->show();
      break;
    case BRIDGE_TYPE:
      pixTopology->setPixmap(QPixmap(QImage(QucsSettings.BitmapDir + "att_bridge.png")));
      LabelR2->setText("R4:");
      LabelR3->hide();
      lineEdit_R3->hide();
      LabelR3_Ohm->hide();
      break;
    }
  adjustSize();
}


void QucsAttenuator::slotCalculate()
{
    QUCS_Att qatt;
    int result;
    QString * s = NULL;
    struct tagATT Values;


    Values.Topology = ComboTopology->currentItem();
    Values.Attenuation = lineEdit_Attvalue->text().toDouble();
    Values.Zin = lineEdit_Zin->text().toDouble();
    Values.Zout = lineEdit_Zout->text().toDouble();
    result = qatt.Calc(&Values);

    if(result != -1)
    {
      LabelResult->setText(tr("Result:")+" "+tr("Success!"));
      lineEdit_R1->setText(QString::number(Values.R1, 'f', 1));
      lineEdit_R2->setText(QString::number(Values.R2, 'f', 1));
      lineEdit_R3->setText(QString::number(Values.R3, 'f', 1));
      
      s = qatt.createSchematic(&Values);
      if(!s) return;
      
      QClipboard *cb = QApplication::clipboard();
      cb->setText(*s);
      delete s;
    }
    else
    {
      LabelResult->setText(tr("Error: Set Attenuation more than %1 dB").arg(QString::number(Values.MinimumATT, 'f', 3)));
      lineEdit_R1->setText("--");
      lineEdit_R2->setText("--");
      lineEdit_R3->setText("--");
    }

}
