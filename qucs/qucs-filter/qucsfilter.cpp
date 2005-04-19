/***************************************************************************
                               qucsfilter.cpp
                             ------------------
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

#include "qucsfilter.h"
#include "helpdialog.h"
#include "bwfilter.h"

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

#include <math.h>




QucsFilter::QucsFilter()
{
  // set application icon
  setIcon (QPixmap(QucsSettings.BitmapDir + "big.qucs.xpm"));
  setCaption("QucsFilter " PACKAGE_VERSION);


  // --------  create menubar  -------------------
  QPopupMenu *fileMenu = new QPopupMenu();
  fileMenu->insertItem(tr("E&xit"), this, SLOT(slotQuit()), CTRL+Key_Q);

  QPopupMenu *helpMenu = new QPopupMenu();
  helpMenu->insertItem(tr("Help..."), this, SLOT(slotHelpIntro()), Key_F1);
  helpMenu->insertSeparator();
  helpMenu->insertItem(
                tr("&About QucsFilter..."), this, SLOT(slotHelpAbout()), 0);
  helpMenu->insertItem(tr("About Qt..."), this, SLOT(slotHelpAboutQt()), 0);

  QMenuBar *bar = new QMenuBar(this);
  bar->insertItem(tr("&File"), fileMenu);
  bar->insertItem(tr("&Help"), helpMenu);


  // -------  create main windows widgets --------
  gbox = new QGridLayout(this, 10,3,5,5);

  QWidget *Space = new QWidget(this);   // reserve space for menubar
  Space->setFixedSize(5, bar->height());
  gbox->addWidget(Space, 0,0);

  QLabel *Label1 = new QLabel(tr("Filter type:"), this);
  gbox->addWidget(Label1, 1,0);
  ComboType = new QComboBox(this);
  #define TYPE_BUTTERWORTH  0
  ComboType->insertItem("Butterworth");
  gbox->addWidget(ComboType, 1,1);
  connect(ComboType, SIGNAL(activated(int)), SLOT(slotTypeChanged(int)));

  QLabel *Label2 = new QLabel(tr("Filter class:"), this);
  gbox->addWidget(Label2, 2,0);
  ComboClass = new QComboBox(this);
  ComboClass->insertItem(tr("Low pass"));
  ComboClass->insertItem(tr("High pass"));
//  ComboClass->insertItem(tr("Band pass"));
//  ComboClass->insertItem(tr("Band stop"));
  gbox->addWidget(ComboClass, 2,1);
  connect(ComboClass, SIGNAL(activated(int)), SLOT(slotClassChanged(int)));

  IntVal = new QIntValidator(1, 200, this);
  DoubleVal = new QDoubleValidator(this);

  QLabel *Label3 = new QLabel(tr("Order:"), this);
  gbox->addWidget(Label3, 3,0);
  EditOrder = new QLineEdit("3", this);
  EditOrder->setValidator(IntVal);
  gbox->addWidget(EditOrder, 3,1);

  QLabel *Label4 = new QLabel(tr("Corner frequency:"), this);
  gbox->addWidget(Label4, 4,0);
  EditCorner = new QLineEdit("1", this);
  EditCorner->setValidator(DoubleVal);
  gbox->addWidget(EditCorner, 4,1);
  ComboCorner = new QComboBox(this);
  ComboCorner->insertItem("Hz");
  ComboCorner->insertItem("kHz");
  ComboCorner->insertItem("MHz");
  ComboCorner->insertItem("GHz");
  ComboCorner->setCurrentItem(3);
  gbox->addWidget(ComboCorner, 4,2);

  LabelStop = new QLabel(tr("Stop frequency:"), this);
  gbox->addWidget(LabelStop, 5,0);
  EditStop = new QLineEdit("2", this);
  EditStop->setValidator(DoubleVal);
  gbox->addWidget(EditStop, 5,1);
  ComboStop = new QComboBox(this);
  ComboStop->insertItem("Hz");
  ComboStop->insertItem("kHz");
  ComboStop->insertItem("MHz");
  ComboStop->insertItem("GHz");
  ComboStop->setCurrentItem(3);
  gbox->addWidget(ComboStop, 5,2);

  LabelRipple = new QLabel(tr("Pass band ripple:"), this);
  gbox->addWidget(LabelRipple, 6,0);
  EditRipple = new QLineEdit("1", this);
  EditRipple->setValidator(DoubleVal);
  gbox->addWidget(EditRipple, 6,1);
  Label_dB = new QLabel("dB", this);
  gbox->addWidget(Label_dB, 6,2);

  QLabel *Label9 = new QLabel(tr("Impedance:"), this);
  gbox->addWidget(Label9, 7,0);
  EditImpedance = new QLineEdit("50", this);
  EditImpedance->setValidator(DoubleVal);
  gbox->addWidget(EditImpedance, 7,1);
  QLabel *Label10 = new QLabel("Ohm", this);
  gbox->addWidget(Label10, 7,2);


  QPushButton *ButtonGo =
               new QPushButton(tr("Calculate and put into Clipboard"), this);
  connect(ButtonGo, SIGNAL(clicked()), SLOT(slotCalculate()));
  gbox->addMultiCellWidget(ButtonGo, 8,8,0,2);

  LabelResult = new QLabel(this);
  ResultState = 100;
  slotShowResult();
  LabelResult->setAlignment(Qt::AlignHCenter);
  gbox->addMultiCellWidget(LabelResult, 9,9,0,2);


  // -------  finally set initial state  --------
  slotTypeChanged(0);
  slotClassChanged(0);
}

QucsFilter::~QucsFilter()
{
  delete gbox;
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

  accept();
}

// ************************************************************
void QucsFilter::slotHelpAbout()
{
  QMessageBox::about(this, tr("About..."),
    tr("QucsFilter Version ")+PACKAGE_VERSION+
    tr("\nFilter synthesis program\n")+
    tr("Copyright (C) 2005 by Toyoyuki Ishikawa and Michael Margraf\n")+
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
void QucsFilter::slotCalculate()
{
  // get numerical values from input widgets
  int Order = EditOrder->text().toInt();
  double CornerFreq = EditCorner->text().toDouble();
  double StopFreq   = EditStop->text().toDouble();
//  double Ripple_dB  = EditRipple->text().toDouble();
  double Impedance  = EditImpedance->text().toDouble();

  CornerFreq *= pow(10, double(3*ComboCorner->currentItem())); // add exponent
  StopFreq   *= pow(10, double(3*ComboStop->currentItem()));


  QString *s = 0;
  // call appropriate filter synthesis function
  switch(ComboType->currentItem()) {
    case TYPE_BUTTERWORTH :
	switch(ComboClass->currentItem()) {
	  case CLASS_LOWPASS :
		s = bwfilter::createSchematic(CLASS_LOWPASS, Impedance,
						Order, CornerFreq);
		break;
	  case CLASS_HIGHPASS :
		s = bwfilter::createSchematic(CLASS_HIGHPASS, Impedance,
						Order, CornerFreq);
		break;
	}
	break;
  }
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
  QString s = QString("<font color=\"#00%100\"><b>  ").arg(c, 2, 16);
  LabelResult->setText(tr("Result:") + s + tr("Successful") + "</b></font>");

  c = 500;
  if(ResultState > 5)  c = 3000;
  QTimer::singleShot(c, this, SLOT(slotShowResult()));
}

// ************************************************************
void QucsFilter::slotTypeChanged(int index)
{
  switch(index) {
    case TYPE_BUTTERWORTH :
	LabelRipple->setEnabled(false);
	EditRipple->setEnabled(false);
	Label_dB->setEnabled(false);
	break;
  }
}

// ************************************************************
void QucsFilter::slotClassChanged(int index)
{
  switch(index) {
    case CLASS_LOWPASS :
    case CLASS_HIGHPASS :
	LabelStop->setEnabled(false);
	EditStop->setEnabled(false);
	ComboStop->setEnabled(false);
	break;
  }
}
