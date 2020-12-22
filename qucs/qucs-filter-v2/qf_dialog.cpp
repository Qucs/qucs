/***************************************************************************
                              qf_dialog.h
                             -------------
    begin                : Fri May 1 2009
    copyright            : (C) 2009 by Stefan Jahn
    email                : stefan@lkcc.org
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

#include <QMenu>
#include <QMenuBar>
#include <QLayout>
#include <QTabWidget>
#include <QLineEdit>
#include <QComboBox>
#include <QLabel>
#include <QCheckBox>
#include <QRadioButton>
#include <QGroupBox>
#include <QPushButton>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QGroupBox>

#include "qf_dialog.h"
//Added by qt3to4:
#include <QGridLayout>
#include <QVBoxLayout>

FilterDialog::FilterDialog (QWidget * parent) : QDialog (parent)
{
  // set application icon

  setWindowIcon(QPixmap(":/bitmaps/big.qucs.xpm"));
  setWindowTitle("Qucs Filter " PACKAGE_VERSION);

  all = new QVBoxLayout(this);

  // --------  create menubar  -------------------
  QMenu *fileMenu = new QMenu();
  fileMenu->addAction(tr("E&xit"), this, SLOT(slotQuit()), Qt::CTRL+Qt::Key_Q);

  QMenu *helpMenu = new QMenu();
  helpMenu->addAction(
                tr("&About Qucs Filter..."), this, SLOT(slotHelpAbout()), 0);
  helpMenu->addAction(tr("About Qt..."), this, SLOT(slotHelpAboutQt()), 0);

  QMenuBar *bar = new QMenuBar(this);
  bar->addAction(tr("&File"), fileMenu);
  bar->addSeparator();
  bar->addAction(tr("&Help"), helpMenu);
  all->addWidget(bar);

  // reserve space for menubar
  all->addSpacing (bar->height() + 2);

  QTabWidget *t = new QTabWidget(this);
  all->addWidget(t);

  // ...........................................................
  QWidget *Tab1 = new QWidget(t);
  QGridLayout *gp1 = new QGridLayout(Tab1);

  FilterName = new QComboBox(Tab1);
  FilterName->setEditable(false);
  gp1->addWidget(FilterName,0,0);
  TformName = new QComboBox(Tab1);
  TformName->setEditable(false);
  gp1->addWidget(TformName,0,1);

  OrderBox = new QCheckBox(tr("Specify order"), Tab1);
  gp1->addWidget(OrderBox,1,0);
  QHBoxLayout *h1 = new QHBoxLayout(Tab1);
  h1->setSpacing (5);
  OrderCombo = new QComboBox(this);
  h1->addWidget(OrderCombo);
  OrderCombo->setEnabled(true);
  SubOrderCombo = new QComboBox(this);
  h1->addWidget(SubOrderCombo);
  SubOrderCombo->setEnabled(false);
  SubOrderCombo->addAction(new QAction(tr( "b" )) );
  SubOrderCombo->addAction(new QAction(tr( "c" )) );
  gp1->addLayout(h1,1,1);

  CutoffLabel = new QLabel(tr("Cutoff/Center"),Tab1);
  gp1->addWidget(CutoffLabel,2,0);
  EnterCutoff = new QLineEdit(Tab1);
  gp1->addWidget(EnterCutoff,2,1);
  CutoffCombo = new QComboBox(Tab1);
  CutoffCombo->addItem(tr( "Hz" ) );
  CutoffCombo->addItem(tr( "kHz" ) );
  CutoffCombo->addItem(tr( "MHz" ) );
  CutoffCombo->addItem(tr( "GHz" ) );
  gp1->addWidget(CutoffCombo,2,2);

  RippleLabel = new QLabel(tr("Ripple"),Tab1);
  gp1->addWidget(RippleLabel,3,0);
  EnterRipple = new QLineEdit(Tab1);
  gp1->addWidget(EnterRipple,3,1);
  RippleUnitLabel = new QLabel(tr("dB"),Tab1);
  gp1->addWidget(RippleUnitLabel,3,2);

  AngleLabel = new QLabel(tr("Angle"),Tab1);
  gp1->addWidget(AngleLabel,3,3);
  EnterAngle = new QLineEdit(Tab1);
  gp1->addWidget(EnterAngle,3,4);
  AngleUnitLabel = new QLabel(tr("°"),Tab1);
  gp1->addWidget(AngleUnitLabel,3,5);

  ZinLabel = new QLabel(tr("Zin"),Tab1);
  gp1->addWidget(ZinLabel,7,0);
  EnterZin = new QLineEdit(Tab1);
  gp1->addWidget(EnterZin,7,1);
  OhmLabel = new QLabel(tr("Ohm"),Tab1);
  gp1->addWidget(OhmLabel,7,2);

  ZoutLabel = new QLabel(tr("Zout"),Tab1);
  ZoutLabel->setEnabled(false);
  gp1->addWidget(ZoutLabel,7,3);
  EnterZout = new QLineEdit(Tab1);
  gp1->addWidget(EnterZout,7,4);
  OhmLabel_2 = new QLabel(tr("Ohm"),Tab1);
  gp1->addWidget(OhmLabel_2,7,5);

  StopbandLabel = new QLabel(tr("Stopband corner"),Tab1);
  gp1->addWidget(StopbandLabel,5,0);
  EnterStopband = new QLineEdit(Tab1);
  gp1->addWidget(EnterStopband,5,1);
  StopbandCombo = new QComboBox(Tab1);
  StopbandCombo->setEditable(false);
  StopbandCombo->addItem( tr( "Hz" ) );
  StopbandCombo->addItem( tr( "kHz" ) );
  StopbandCombo->addItem( tr( "MHz" ) );
  StopbandCombo->addItem( tr( "GHz" ) );
  gp1->addWidget(StopbandCombo,5,2);

  BandwidthLabel = new QLabel(tr("Bandwidth"),Tab1);
  BandwidthLabel->setEnabled(false);
  gp1->addWidget(BandwidthLabel,4,0);
  EnterBandwidth = new QLineEdit(Tab1);
  gp1->addWidget(EnterBandwidth,4,1);
  BandwidthCombo = new QComboBox(Tab1);
  BandwidthCombo->setEditable(false);
  BandwidthCombo->setEnabled(false);
  BandwidthCombo->addItem( tr( "Hz" ) );
  BandwidthCombo->addItem( tr( "kHz" ) );
  BandwidthCombo->addItem( tr( "MHz" ) );
  BandwidthCombo->addItem( tr( "GHz" ) );
  gp1->addWidget(BandwidthCombo,4,2);

  AttenuationLabel = new QLabel(tr("Attenuation"),Tab1);
  gp1->addWidget(AttenuationLabel,6,0);
  EnterAttenuation = new QLineEdit(Tab1);
  gp1->addWidget(EnterAttenuation,6,1);
  dBLabel = new QLabel(tr("dB"),Tab1);
  gp1->addWidget(dBLabel,6,2);

  DualBox = new QCheckBox(tr("dual"),Tab1);
  gp1->addWidget(DualBox,8,8,0,2);
  CauerPoleBox = new QCheckBox(tr("Stopband is first pole"),Tab1);
  CauerPoleBox->setEnabled(false);
  gp1->addWidget(CauerPoleBox,9,9,0,2);
  OptimizeCauerBox = new QCheckBox(tr("Optimize cauer"),Tab1);
  OptimizeCauerBox->setEnabled(false);
  gp1->addWidget(OptimizeCauerBox,10,10,0,2);
  EqualInductorBox = new QCheckBox(tr("Equal inductors"),Tab1);
  EqualInductorBox->setEnabled(false);
  gp1->addWidget(EqualInductorBox,8,8,3,5);
  UseCrossBox = new QCheckBox(tr("+ rather than T"),Tab1);
  UseCrossBox->setEnabled(false);
  gp1->addWidget(UseCrossBox,9,9,3,5);

  Cboxes = new QGroupBox(tr("Optimize C"),Tab1);
  Cmin = new QRadioButton(tr("Cmin"),Cboxes);
  Cmax = new QRadioButton(tr("Cmax"),Cboxes);
  NoC = new QRadioButton(tr("noC"),Cboxes);
  gp1->addWidget(Cboxes,11,11,0,2);

  Lboxes = new Q3VButtonGroup(tr("Optimize L"),Tab1);
  Lmin = new QRadioButton(tr("Lmin"),Lboxes);
  Lmax = new QRadioButton(tr("Lmax"),Lboxes);
  NoL = new QRadioButton(tr("noL"),Lboxes);
  gp1->addWidget(Lboxes,11,11,3,5);

  t->addTab(Tab1, tr("LC Filters"));

  // ...........................................................
  QWidget *Tab2 = new QWidget(t);
  t->addTab(Tab2, tr("Microstrip Filters"));

  // ...........................................................
  QWidget *Tab3 = new QWidget(t);
  t->addTab(Tab3, tr("Active Filters"));

  // reserve space for vertical resizing
  all->addStretch();

  // ...........................................................
  // buttons on the bottom of the dialog (independent of the TabWidget)
  QHBoxLayout *Butts = new QHBoxLayout(this);
  Butts->setSpacing(3);
  Butts->setMargin(3);
  all->addWidget(Butts);

  cancelButton = new QPushButton(tr("Exit"),Butts);
  okButton = new QPushButton(tr("Calculate"),Butts);
  okButton->setEnabled(false);

  // signals and slots connections
  connect( cancelButton, SIGNAL( clicked() ), this, SLOT( reject() ) );
  connect( okButton, SIGNAL( clicked() ), this, SLOT( accept() ) );
}

FilterDialog::~FilterDialog ()
{
  delete all;
}

// ************************************************************
void FilterDialog::slotHelpAbout()
{
  QMessageBox::about(this, tr("About..."),
    "Qucs Filter Version " PACKAGE_VERSION+
    tr("\nFilter synthesis program\n")+
    tr("Copyright (C) 2009 by")+
    "\nVincent Habchi, Stefan Jahn\n"
    "\nThis is free software; see the source for copying conditions."
    "\nThere is NO warranty; not even for MERCHANTABILITY or "
    "\nFITNESS FOR A PARTICULAR PURPOSE.\n\n");
}

// ************************************************************
void FilterDialog::slotHelpAboutQt()
{
  QMessageBox::aboutQt(this, tr("About Qt"));
}

// ************************************************************
void FilterDialog::slotQuit()
{
  int tmp;
  tmp = x();            // call size and position function in order to ...
  tmp = y();            // ... set them correctly before closing the ...
  tmp = width();        // dialog !!!  Otherwise the frame of the window ...
  tmp = height();       // ... will not be recognized (a X11 problem).

  reject();
}
