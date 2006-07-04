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

  gbox = new QGridLayout(this, 10,3,5,5);

  QWidget *Space = new QWidget(this);   // reserve space for menubar
  Space->setFixedSize(5, bar->height());
  gbox->addWidget(Space, 0,0);

  LabelTopology = new QLabel(tr("Topology:"), this);
  gbox->addWidget(LabelTopology, 1, 0);

  ComboTopology = new QComboBox(this);//====================Topology Combobox
  ComboTopology->insertItem("Pi Type");
  ComboTopology->insertItem("Tee Type");
  gbox->addWidget(ComboTopology, 1, 1);
  connect(ComboTopology, SIGNAL(activated(int)), SLOT(slotTopologyChanged()));

  pixTopology = new QLabel(this);//====================Pixmap for Topology
  pixTopology->setPixmap(QPixmap (QImage (QucsSettings.BitmapDir + "pix_pi.png")));
  //pixTopology->setPixmap(QPixmap (QImage ("pix_pi.png")));
  gbox->addWidget(pixTopology, 2,1);

  IntVal = new QIntValidator(this);
  DoubleVal = new QDoubleValidator(this);

  LabelAtten = new QLabel(tr("Attenuation:"), this);
  gbox ->addWidget(LabelAtten, 3,0);
  lineEdit_Attvalue = new QLineEdit(tr("1"), this);
  lineEdit_Attvalue->setValidator(DoubleVal);
  gbox->addWidget(lineEdit_Attvalue, 3,1);
  QLabel *Label1 = new QLabel(tr("dB"), this);
  gbox->addWidget(Label1, 3,2);

  LabelImp1 = new QLabel(tr("Zin:"), this);
  gbox->addWidget(LabelImp1, 4,0);
  lineEdit_Zin = new QLineEdit(tr("50"), this);
  lineEdit_Zin->setValidator(DoubleVal);
  gbox->addWidget(lineEdit_Zin, 4,1);
  QLabel *Label2 = new QLabel(tr("dB"), this);
  gbox->addWidget(Label2, 4,2);

  LabelImp2 = new QLabel(tr("Zout:"), this);
  gbox->addWidget(LabelImp2, 5,0);
  lineEdit_Zout = new QLineEdit(tr("50"), this);
  lineEdit_Zout->setValidator(DoubleVal);
  gbox->addWidget(lineEdit_Zout, 5,1);
  QLabel *Label3 = new QLabel(tr("dB"), this);
  gbox->addWidget(Label3, 5,2);

  Calculate = new QPushButton(tr("Calculate and put into Clipboard"), this);
  gbox->addMultiCellWidget(Calculate, 6,7,0,2);
  connect(Calculate, SIGNAL(clicked()), SLOT(slotCalculate()));

  LabelR1 = new QLabel(tr("R1:"), this);
  gbox->addWidget(LabelR1, 8,0);
  lineEdit_R1 = new QLineEdit(tr("--"), this);
  gbox->addWidget(lineEdit_R1, 8,1);
  QLabel *Label4 = new QLabel(tr("Ohm"), this);
  gbox->addWidget(Label4, 8,2);

  LabelR2 = new QLabel(tr("R2:"), this);
  gbox->addWidget(LabelR2, 9,0);
  lineEdit_R2 = new QLineEdit(tr("--"), this);
  gbox->addWidget(lineEdit_R2, 9,1);
  QLabel *Label5 = new QLabel(tr("Ohm"), this);
  gbox->addWidget(Label5, 9,2);

  LabelR3 = new QLabel(tr("R3:"), this);
  gbox->addWidget(LabelR3, 10,0);
  lineEdit_R3 = new QLineEdit(tr("--"), this);
  gbox->addWidget(lineEdit_R3, 10,1);
  QLabel *Label6 = new QLabel(tr("Ohm"), this);
  gbox->addWidget(Label6, 10,2);

  LabelResult = new QLabel(tr("Result:"), this);
  LabelResult->setAlignment(Qt::AlignHCenter);
  gbox->addMultiCellWidget(LabelResult, 11,11,0,2);
}

QucsAttenuator::~QucsAttenuator()
{
  delete gbox;
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
      pixTopology->setPixmap(QPixmap(QImage(QucsSettings.BitmapDir + "pix_pi.png")));
      break;
    case TEE_TYPE:
      pixTopology->setPixmap(QPixmap(QImage(QucsSettings.BitmapDir + "pix_tee.png")));
      break;
    }
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
      LabelResult->setText("Result:Success!");
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
      LabelResult->setText("Error: Set Attenuation more than " + QString::number(Values.MinimumATT, 'f', 1) + "dB");
      lineEdit_R1->setText("--");
      lineEdit_R2->setText("--");
      lineEdit_R3->setText("--");
    }

}
