/***************************************************************************
                          qucstrans.cpp  -  description
                             -------------------
    begin                : Sun Feb 27 2005
    copyright            : (C) 2004 by Stefan Jahn
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

#include <qlayout.h>
#include <qhbox.h>
#include <qvbox.h>
#include <qpushbutton.h>
#include <qfile.h>
#include <qtextstream.h>
#include <qmessagebox.h>
#include <qtoolbutton.h>
#include <qimage.h>
#include <qfiledialog.h>
#include <qmenubar.h>
#include <qaction.h>
#include <qpopupmenu.h>
#include <qvgroupbox.h>
#include <qcombobox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qimage.h>
#include <qiconset.h>
#include <qscrollview.h>

#include "qucstrans.h"

#define TRANS_QOBJS NULL, NULL, NULL
#define TRANS_END   { NULL, 0, { NULL }, TRANS_QOBJS }

#define TRANS_SUBSTRATE  0
#define TRANS_COMPONENT  1
#define TRANS_PHYSICAL   2
#define TRANS_ELECTRICAL 3

#define TRANS_BOXES 4
#define TRANS_TYPES 4

static const int TransMaxBox[TRANS_BOXES] = { 9, 3, 4, 3 };

struct TransType TransLineTypes[] = {
  { ModeMicrostrip, "Microstrip",
    { { {
      { "Er",    0, { "NA", NULL }, TRANS_QOBJS },
      { "Mur",   0, { "NA", NULL }, TRANS_QOBJS },
      { "H",     0, { "NA", NULL }, TRANS_QOBJS },
      { "H_t",   0, { "NA", NULL }, TRANS_QOBJS },
      { "T",     0, { "NA", NULL }, TRANS_QOBJS },
      { "Cond",  0, { "NA", NULL }, TRANS_QOBJS },
      { "Tand",  0, { "NA", NULL }, TRANS_QOBJS },
      { "Rough", 0, { "NA", NULL }, TRANS_QOBJS },
      TRANS_END
    } },
    { {
      { "Freq",  0, { "NA", NULL }, TRANS_QOBJS },
      TRANS_END
    } },
    { {
      { "W",     0, { "NA", NULL }, TRANS_QOBJS },
      { "L",     0, { "NA", NULL }, TRANS_QOBJS },
      TRANS_END
    } },
    { {
      { "Z0",    0, { "NA", NULL }, TRANS_QOBJS },
      { "E_eff", 0, { "NA", NULL }, TRANS_QOBJS },
      TRANS_END
    } }
  } },
  { ModeRectangular, "Rectangular",
    { { {
      { "Er",    0, { "NA", NULL }, TRANS_QOBJS },
      { "Mur",   0, { "NA", NULL }, TRANS_QOBJS },
      { "Cond",  0, { "NA", NULL }, TRANS_QOBJS },
      { "Tand",  0, { "NA", NULL }, TRANS_QOBJS },
      { "TanM",  0, { "NA", NULL }, TRANS_QOBJS },
      TRANS_END
    } },
    { {
      { "Freq",  0, { "NA", NULL }, TRANS_QOBJS },
      TRANS_END
    } },
    { {
      { "a",     0, { "NA", NULL }, TRANS_QOBJS },
      { "b",     0, { "NA", NULL }, TRANS_QOBJS },
      { "L",     0, { "NA", NULL }, TRANS_QOBJS },
      TRANS_END
    } },
    { {
      { "Z0",    0, { "NA", NULL }, TRANS_QOBJS },
      { "E_eff", 0, { "NA", NULL }, TRANS_QOBJS },
      TRANS_END
    } }
  } },
  { ModeCoaxial, "Coaxial",
    { { {
      { "Er",    0, { "NA", NULL }, TRANS_QOBJS },
      { "Mur",   0, { "NA", NULL }, TRANS_QOBJS },
      { "Tand",  0, { "NA", NULL }, TRANS_QOBJS },
      { "Sigma", 0, { "NA", NULL }, TRANS_QOBJS },
      TRANS_END
    } },
    { {
      { "Freq",  0, { "NA", NULL }, TRANS_QOBJS },
      TRANS_END
    } },
    { {
      { "din",   0, { "NA", NULL }, TRANS_QOBJS },
      { "dout",  0, { "NA", NULL }, TRANS_QOBJS },
      { "L",     0, { "NA", NULL }, TRANS_QOBJS },
      TRANS_END
    } },
    { {
      { "Z0",    0, { "NA", NULL }, TRANS_QOBJS },
      { "E_eff", 0, { "NA", NULL }, TRANS_QOBJS },
      TRANS_END
    } }
  } },
  { ModeCoupledMicrostrip, "CoupledMicrostrip",
    { { {
      { "Er",    0, { "NA", NULL }, TRANS_QOBJS },
      { "Mur",   0, { "NA", NULL }, TRANS_QOBJS },
      { "H",     0, { "NA", NULL }, TRANS_QOBJS },
      { "H_t",   0, { "NA", NULL }, TRANS_QOBJS },
      { "T",     0, { "NA", NULL }, TRANS_QOBJS },
      { "Cond",  0, { "NA", NULL }, TRANS_QOBJS },
      { "Tand",  0, { "NA", NULL }, TRANS_QOBJS },
      { "Rough", 0, { "NA", NULL }, TRANS_QOBJS },
      TRANS_END
    } },
    { {
      { "Freq",  0, { "NA", NULL }, TRANS_QOBJS },
      TRANS_END
    } },
    { {
      { "W",     0, { "NA", NULL }, TRANS_QOBJS },
      { "S",     0, { "NA", NULL }, TRANS_QOBJS },
      { "L",     0, { "NA", NULL }, TRANS_QOBJS },
      TRANS_END
    } },
    { {
      { "Z0e",   0, { "NA", NULL }, TRANS_QOBJS },
      { "Z0o",   0, { "NA", NULL }, TRANS_QOBJS },
      { "E_eff", 0, { "NA", NULL }, TRANS_QOBJS },
      TRANS_END
    } }
  } },
  { ModeNone, NULL, { { { TRANS_END } } } }
};

QucsTranscalc::QucsTranscalc()
{
  // set application icon
  setIcon (QPixmap(QucsSettings.BitmapDir + "big.qucs.xpm"));
  setCaption("Qucs Transcalc " PACKAGE_VERSION);

  QMenuBar * menuBar = new QMenuBar (this);

  // create file menu
  QPopupMenu * fileMenu = new QPopupMenu ();
  QAction * fileLoad =
    new QAction (tr("Load"), tr("&Load"), CTRL+Key_L, this);
  fileLoad->addTo (fileMenu);
  QAction * fileSave =
    new QAction (tr("Save"), tr("&Save"), CTRL+Key_S, this);
  fileSave->addTo (fileMenu);
  fileMenu->insertSeparator ();
  QAction * fileOption =
    new QAction (tr("Options"), tr("&Options"), CTRL+Key_O, this);
  fileOption->addTo (fileMenu);
  fileMenu->insertSeparator ();
  QAction * fileQuit =
    new QAction (tr("Quit"), tr("&Quit"), CTRL+Key_Q, this);
  fileQuit->addTo (fileMenu);
  connect(fileQuit, SIGNAL(activated()), SLOT(slotQuit()));

  // create execute menu
  QPopupMenu * execMenu = new QPopupMenu ();
  QAction * execAnalyze =
    new QAction (tr("Analyze"), tr("&Analyze"), Key_F3, this);
  execAnalyze->addTo (execMenu);
  connect(execAnalyze, SIGNAL(activated()), SLOT(slotAnalyze()));
  QAction * execSynthesize =
    new QAction (tr("Synthesize"), tr("&Synthesize"), Key_F4, this);
  execSynthesize->addTo (execMenu);
  connect(execSynthesize, SIGNAL(activated()), SLOT(slotSynthesize()));

  // create help menu
  QPopupMenu * helpMenu = new QPopupMenu ();
  QAction * helpHelp =
    new QAction (tr("Help"), tr("&Help"), Key_F1, this);
  helpHelp->addTo (helpMenu);
  QAction * helpAbout =
    new QAction (tr("About"), tr("About"), 0, helpMenu);
  helpAbout->addTo (helpMenu);
  connect(helpAbout, SIGNAL(activated()), SLOT(slotAbout()));

  menuBar->insertItem (tr("&File"), fileMenu);
  menuBar->insertItem (tr("&Execute"), execMenu);
  menuBar->insertSeparator ();
  menuBar->insertItem (tr("&Help"), helpMenu);
  
  QVBoxLayout * v = new QVBoxLayout (this);
  v->setSpacing (5);
  v->setMargin (5);
 
  QHBoxLayout * ht = new QHBoxLayout ();
  QVGroupBox * lineGroup = new QVGroupBox (tr("Transmission Line Type"), this);
  ht->addWidget (lineGroup);
  ht->addStretch ();
  v->addLayout (ht);
  tranType = new QComboBox (lineGroup);
  tranType->insertItem (tr("Microstrip"));
  tranType->insertItem (tr("Rectangular Waveguide"));
  tranType->insertItem (tr("Coaxial Line"));
  tranType->insertItem (tr("Coupled Microstrip"));
  connect(tranType, SIGNAL(activated(int)), SLOT(slotSelectType(int)));

  QHBox * h = new QHBox (this);
  h->setSpacing (5);
  v->addWidget (h);
  QVBox * vl = new QVBox (h);
  QVBox * vr = new QVBox (h);
  
  screen = new QVGroupBox (tr("Microstrip"), vl);
  pix = new QLabel (screen);
  pix->setPixmap (QPixmap (QImage (QucsSettings.BitmapDir +
				   "microstrip.png")));

  mode = ModeMicrostrip;

  QVGroupBox * substrate = new QVGroupBox (tr("Substrate Parameters"), vl);
  QVBox * v11 = createScrollable (substrate);
  createPropItems (v11, TRANS_SUBSTRATE);

  QVGroupBox * component = new QVGroupBox (tr("Component Parameters"), vr);
  QVBox * v12 = createScrollable (component);
  createPropItems (v12, TRANS_COMPONENT);

  QVGroupBox * physical = new QVGroupBox (tr("Physical Parameters"), vr);
  QVBox * v22 = createScrollable (physical);
  createPropItems (v22, TRANS_PHYSICAL);

  QHBox * h2 = new QHBox (vr);
  QPushButton * analyze = new QPushButton (tr("Analyze"), h2);
  connect(analyze, SIGNAL(clicked()), SLOT(slotAnalyze()));
  QPushButton * synthesize = new QPushButton (tr("Synthesize"), h2);
  connect(synthesize, SIGNAL(clicked()), SLOT(slotSynthesize()));

  QVGroupBox * electrical = new QVGroupBox (tr("Electrical Parameters"), vr);
  QVBox * v32 = createScrollable (electrical);
  createPropItems (v32, TRANS_ELECTRICAL);

  QVGroupBox * calculated = new QVGroupBox (tr("Calculated Results"), vr);
  QVBox * v42 = createScrollable (calculated);

  QVGroupBox * statGroup = new QVGroupBox (tr("Status"), this);
  v->addWidget (statGroup);
  QLabel * statText = new QLabel ("Status Line", statGroup);
  statText->setAlignment (Qt::AlignCenter);
}

QucsTranscalc::~QucsTranscalc()
{
}

QVBox * QucsTranscalc::createScrollable(QWidget * parent) {
  QScrollView * sv = new QScrollView (parent);
  QVBox * v = new QVBox (sv->viewport());
  v->setSpacing (3);
  v->setMargin (3);
  sv->addChild (v);
  return v;
}

void QucsTranscalc::addPropItem(QWidget * parent, TransValue * val) {
  QHBox * h = new QHBox (parent);
  h->setSpacing (2);
  QLabel * l = new QLabel (val->name, h);
  l->setAlignment (Qt::AlignRight);
  val->label = l;
  QLineEdit * e = new QLineEdit (h);
  e->setText (QString::number(val->value));
  e->setAlignment (Qt::AlignRight);
  if (!val->name) e->setDisabled(true);
  val->lineedit = e;
  QComboBox * c = new QComboBox (h);
  if (!val->units[0]) {
    c->insertItem ("NA");
    c->setDisabled(true);
  }
  else {
    for (int i = 0; val->units[i]; i++) {
      c->insertItem (val->units[i]);
    }
    c->setCurrentItem(0);
  }
  val->combobox = c;
  h->setStretchFactor (l, 2);
  h->setStretchFactor (e, 2);
  h->setStretchFactor (c, 2);
}

void QucsTranscalc::changePropItem(TransValue * val) {
  QLabel * l = val->label;
  l->setText (val->name);
  QLineEdit * e = val->lineedit;
  e->setText (QString::number(val->value));
  e->setDisabled(val->name == NULL);
  QComboBox * c = val->combobox;
  c->clear();
  if (!val->units[0]) {
    c->insertItem ("NA");
    c->setDisabled(true);
  }
  else {
    for (int i = 0; val->units[i]; i++) {
      c->insertItem (val->units[i]);
    }
    c->setCurrentItem(0);
    c->setDisabled(false);
  }
}

void QucsTranscalc::setMode(int _mode) {
  if (mode == _mode) return;
  mode = _mode;
  struct TransValue * val;
  for (int box = 0; box < TRANS_BOXES; box++) {
    val = & TransLineTypes[getTypeIndex()].array[box].item[0];
    for (int i = 0; i < TransMaxBox[box]; i++) {
      changePropItem (val);
      val++;
    }
  }
}

void QucsTranscalc::createPropItems(QWidget * parent, int box) {
  struct TransValue * val, * dup;
  int idx = getTypeIndex();
  val = & TransLineTypes[idx].array[box].item[0];
  for (int i = 0; i < TransMaxBox[box]; i++) {
    addPropItem (parent, val);
    for (int _mode = 0; _mode < TRANS_TYPES; _mode++) {
      if (idx != _mode) {
	dup = & TransLineTypes[_mode].array[box].item[i];
	dup->label = val->label;
	dup->lineedit = val->lineedit;
	dup->combobox = val->combobox;
      }
    }
    val++;
  }
}

int QucsTranscalc::getTypeIndex() {
  struct TransType * type = & TransLineTypes[0];
  for (int i = 0; type->type != ModeNone; i++) {
    if (type->type == mode) return i;
    type++;
  }
  return 0;
}

struct TransValue * QucsTranscalc::findProperty(QString prop) {
  struct TransValue * val = NULL;
  for (int box = 0; box < TRANS_BOXES; box++) {
    val = & TransLineTypes[getTypeIndex()].array[box].item[0];
    for (int i = 0; val->name; i++) {
      if (prop == val->name)
	return val;
      val++;
    }
  }
  return NULL;
}

void QucsTranscalc::setProperty(QString prop, double value) {
  struct TransValue * val = findProperty (prop);
  if (val) {
    val->lineedit->setText (QString::number(value));
  }
}

double QucsTranscalc::getProperty(QString prop) {
  struct TransValue * val = findProperty (prop);
  if (val) {
    QString str = val->lineedit->text();
    val->value = str.toDouble();
    return val->value;
  }
}

// ************************************************************
void QucsTranscalc::slotAbout()
{
  QMessageBox::about(this, tr("About..."),
    tr("Qucs TransCalc")+" "+PACKAGE_VERSION+"\n"+
    tr("Transmission Line Calculator for Qucs\n")+
    tr("Copyright (C) 2005 by Stefan Jahn\n")+
    "\nThis is free software; see the source for copying conditions."
    "\nThere is NO warranty; not even for MERCHANTABILITY or "
    "\nFITNESS FOR A PARTICULAR PURPOSE.");
}

// ************************************************************
void QucsTranscalc::slotQuit()
{
  int tmp;
  tmp = x();		// call size and position function in order to ...
  tmp = y();		// ... set them correctly before closing the ...
  tmp = width();	// dialog !!!  Otherwise the frame of the window ...
  tmp = height();	// will not be recognized (a X11 problem).

  accept();
}

// ************************************************************
// To get all close events.
void QucsTranscalc::closeEvent(QCloseEvent *Event)
{
  int tmp;
  tmp = x();		// call size and position function in order to ...
  tmp = y();		// ... set them correctly before closing the ...
  tmp = width();	// dialog !!!  Otherwise the frame of the window ...
  tmp = height();	// will not be recognized (a X11 problem).

  Event->accept();
}

// ************************************************************
void QucsTranscalc::slotSelectType(int Type)
{
  int _mode = mode;
  screen->setTitle (tranType->currentText ());
  switch (Type) {
  case 0:
    pix->setPixmap (QPixmap (QImage (QucsSettings.BitmapDir +
				     "microstrip.png")));
    _mode = ModeMicrostrip;
    break;
  case 1:
    pix->setPixmap (QPixmap (QImage (QucsSettings.BitmapDir +
				     "rectwaveguide.png")));
    _mode = ModeRectangular;
    break;
  case 2:
    pix->setPixmap (QPixmap (QImage (QucsSettings.BitmapDir +
				     "coax.png")));
    _mode = ModeCoaxial;
    break;
  case 3:
    pix->setPixmap (QPixmap (QImage (QucsSettings.BitmapDir +
				     "c_microstrip.png")));
    _mode = ModeCoupledMicrostrip;
    break;
  }
  setMode (_mode);
}

// ************************************************************
void QucsTranscalc::slotAnalyze()
{
  setProperty ("Z0", 50);
}

// ************************************************************
void QucsTranscalc::slotSynthesize()
{
  setProperty ("W", 0.1);
}
