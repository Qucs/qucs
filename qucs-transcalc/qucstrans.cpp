/***************************************************************************
                          qucstrans.cpp  -  description
                             -------------------
    begin                : Sun Feb 27 2005
    copyright            : (C) 2005 by Stefan Jahn
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
#include <qhgroupbox.h>
#include <qvgroupbox.h>
#include <qcombobox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qimage.h>
#include <qiconset.h>
#include <qscrollview.h>
#include <qtooltip.h>
#include <qradiobutton.h>
#include <qstatusbar.h>

#include "qucstrans.h"
#include "transline.h"
#include "microstrip.h"
#include "coax.h"
#include "rectwaveguide.h"
#include "c_microstrip.h"

// Defines maximum number of entries in each property category.
static const int TransMaxBox[MAX_TRANS_BOXES] = { 9, 1, 4, 3 };

// Helper #defines for the below transmission line types.
#define TRANS_RADIOS  { -1, -1, -1, -1 }
#define TRANS_QOBJS   NULL, NULL, NULL, NULL
#define TRANS_END     { NULL, 0, NULL, { NULL }, TRANS_QOBJS }
#define TRANS_LENGTHS { "mil", "cm", "mm", "m", "um", "in", "ft", NULL }
#define TRANS_NONES   { "NA", NULL }
#define TRANS_FREQS   { "GHz", "Hz", "kHz", "MHz", NULL }
#define TRANS_OHMS    { "Ohm", "kOhm", NULL }
#define TRANS_ANGLES  { "Deg", "Rad", NULL }
#define TRANS_RESULT  { NULL, NULL, NULL }
#define TRANS_RESULTS { TRANS_RESULT }

// Defines the available transmission line types.
struct TransType TransLineTypes[] = {
  { ModeMicrostrip, "Microstrip", NULL,
    { { {
      { "Er",    0, NULL, TRANS_NONES,   TRANS_QOBJS },
      { "Mur",   0, NULL, TRANS_NONES,   TRANS_QOBJS },
      { "H",     0, NULL, TRANS_LENGTHS, TRANS_QOBJS },
      { "H_t",   0, NULL, TRANS_LENGTHS, TRANS_QOBJS },
      { "T",     0, NULL, TRANS_LENGTHS, TRANS_QOBJS },
      { "Cond",  0, NULL, TRANS_NONES,   TRANS_QOBJS },
      { "Tand",  0, NULL, TRANS_NONES,   TRANS_QOBJS },
      { "Rough", 0, NULL, TRANS_LENGTHS, TRANS_QOBJS },
      TRANS_END
    } },
    { {
      { "Freq",  0, NULL, TRANS_FREQS, TRANS_QOBJS },
      TRANS_END
    } },
    { {
      { "W",     0, NULL, TRANS_LENGTHS, TRANS_QOBJS },
      { "L",     0, NULL, TRANS_LENGTHS, TRANS_QOBJS },
      TRANS_END
    } },
    { {
      { "Z0",    0, NULL, TRANS_OHMS,   TRANS_QOBJS },
      { "Ang_l", 0, NULL, TRANS_ANGLES, TRANS_QOBJS },
      TRANS_END
    } } },
    4, TRANS_RESULTS, TRANS_RADIOS
  },
  { ModeRectangular, "Rectangular", NULL,
    { { {
      { "Er",    0, NULL, TRANS_NONES, TRANS_QOBJS },
      { "Mur",   0, NULL, TRANS_NONES, TRANS_QOBJS },
      { "Cond",  0, NULL, TRANS_NONES, TRANS_QOBJS },
      { "Tand",  0, NULL, TRANS_NONES, TRANS_QOBJS },
      { "TanM",  0, NULL, TRANS_NONES, TRANS_QOBJS },
      TRANS_END
    } },
    { {
      { "Freq",  0, NULL, TRANS_FREQS, TRANS_QOBJS },
      TRANS_END
    } },
    { {
      { "a",     0, NULL, TRANS_LENGTHS, TRANS_QOBJS },
      { "b",     0, NULL, TRANS_LENGTHS, TRANS_QOBJS },
      { "L",     0, NULL, TRANS_LENGTHS, TRANS_QOBJS },
      TRANS_END
    } },
    { {
      { "Z0",    0, NULL, TRANS_OHMS,   TRANS_QOBJS },
      { "Ang_l", 0, NULL, TRANS_ANGLES, TRANS_QOBJS },
      TRANS_END
    } } },
    3, TRANS_RESULTS, { 0, 1, -1, -1 }
  },
  { ModeCoaxial, "Coaxial", NULL,
    { { {
      { "Er",    0, NULL, TRANS_NONES, TRANS_QOBJS },
      { "Mur",   0, NULL, TRANS_NONES, TRANS_QOBJS },
      { "Tand",  0, NULL, TRANS_NONES, TRANS_QOBJS },
      { "Sigma", 0, NULL, TRANS_NONES, TRANS_QOBJS },
      TRANS_END
    } },
    { {
      { "Freq",  0, NULL, TRANS_FREQS, TRANS_QOBJS },
      TRANS_END
    } },
    { {
      { "din",   0, NULL, TRANS_LENGTHS, TRANS_QOBJS },
      { "dout",  0, NULL, TRANS_LENGTHS, TRANS_QOBJS },
      { "L",     0, NULL, TRANS_LENGTHS, TRANS_QOBJS },
      TRANS_END
    } },
    { {
      { "Z0",    0, NULL, TRANS_OHMS,   TRANS_QOBJS },
      { "Ang_l", 0, NULL, TRANS_ANGLES, TRANS_QOBJS },
      TRANS_END
    } } },
    2, TRANS_RESULTS, { 0, 1, -1, -1 }
  },
  { ModeCoupledMicrostrip, "CoupledMicrostrip", NULL,
    { { {
      { "Er",    0, NULL, TRANS_NONES,   TRANS_QOBJS },
      { "Mur",   0, NULL, TRANS_NONES,   TRANS_QOBJS },
      { "H",     0, NULL, TRANS_LENGTHS, TRANS_QOBJS },
      { "H_t",   0, NULL, TRANS_LENGTHS, TRANS_QOBJS },
      { "T",     0, NULL, TRANS_LENGTHS, TRANS_QOBJS },
      { "Cond",  0, NULL, TRANS_NONES,   TRANS_QOBJS },
      { "Tand",  0, NULL, TRANS_NONES,   TRANS_QOBJS },
      { "Rough", 0, NULL, TRANS_LENGTHS, TRANS_QOBJS },
      TRANS_END
    } },
    { {
      { "Freq",  0, NULL, TRANS_FREQS, TRANS_QOBJS },
      TRANS_END
    } },
    { {
      { "W",     0, NULL, TRANS_LENGTHS, TRANS_QOBJS },
      { "S",     0, NULL, TRANS_LENGTHS, TRANS_QOBJS },
      { "L",     0, NULL, TRANS_LENGTHS, TRANS_QOBJS },
      TRANS_END
    } },
    { {
      { "Z0e",   0, NULL, TRANS_OHMS,   TRANS_QOBJS },
      { "Z0o",   0, NULL, TRANS_OHMS,   TRANS_QOBJS },
      { "Ang_l", 0, NULL, TRANS_ANGLES, TRANS_QOBJS },
      TRANS_END
    } } },
    7, TRANS_RESULTS, TRANS_RADIOS
  },
  { ModeNone, NULL, NULL, { { { TRANS_END } } }, 0,
    TRANS_RESULTS, TRANS_RADIOS }
};

/* Constructor setups the GUI. */
QucsTranscalc::QucsTranscalc() {
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

  // setup menu bar
  menuBar->insertItem (tr("&File"), fileMenu);
  menuBar->insertItem (tr("&Execute"), execMenu);
  menuBar->insertSeparator ();
  menuBar->insertItem (tr("&Help"), helpMenu);

  // main box
  QVBoxLayout * v = new QVBoxLayout (this);
  v->setSpacing (5);
  v->setMargin (5);

  // reserve space for menubar
  QWidget * Space = new QWidget (this);
  Space->setFixedSize (5, menuBar->height ());
  v->addWidget (Space);

  // main layout
  QHBox * h = new QHBox (this);
  h->setSpacing (5);
  v->addWidget (h);
//  QVBox * vl = new QVBox (h);

  // transmission line type choice
  QVGroupBox * lineGroup = new QVGroupBox (tr("Transmission Line Type"), h);
  tranType = new QComboBox (lineGroup);
  tranType->insertItem (tr("Microstrip"));
  tranType->insertItem (tr("Rectangular Waveguide"));
  tranType->insertItem (tr("Coaxial Line"));
  tranType->insertItem (tr("Coupled Microstrip"));
  connect(tranType, SIGNAL(activated(int)), SLOT(slotSelectType(int)));
  // setup transmission line picture
  pix = new QLabel (lineGroup);
  pix->setPixmap (QPixmap (QImage (QucsSettings.BitmapDir +
				   "microstrip.png")));

  QVBox * vm = new QVBox (h);
  vm->setSpacing (3);
  QVBox * vr = new QVBox (h);
  vr->setSpacing (3);

  // init additional translations
  setupTranslations ();

  // set current mode
  mode = ModeMicrostrip;

  // substrate parameter box
  QHGroupBox * substrate = new QHGroupBox (tr("Substrate Parameters"), vm);
  createPropItems (substrate, TRANS_SUBSTRATE);

  // component parameter box
  QHGroupBox * component = new QHGroupBox (tr("Component Parameters"), vm);
  createPropItems (component, TRANS_COMPONENT);

  // physical parameter box
  QHGroupBox * physical = new QHGroupBox (tr("Physical Parameters"), vr);
  createPropItems (physical, TRANS_PHYSICAL);

  // analyze/sythesize buttons
  QHBox * h2 = new QHBox (vr);
  QPushButton * analyze = new QPushButton (tr("Analyze"), h2);
  QToolTip::add (analyze, tr("Derive Electrical Parameters"));
  connect(analyze, SIGNAL(clicked()), SLOT(slotAnalyze()));
  QPushButton * synthesize = new QPushButton (tr("Synthesize"), h2);
  QToolTip::add (synthesize, tr("Compute Physical Parameters"));
  connect(synthesize, SIGNAL(clicked()), SLOT(slotSynthesize()));

  // electrical parameter box
  QHGroupBox * electrical = new QHGroupBox (tr("Electrical Parameters"), vr);
  createPropItems (electrical, TRANS_ELECTRICAL);

  calculated = new QHGroupBox (tr("Calculated Results"), vr);

  // status line
//  QVGroupBox * statGroup = new QVGroupBox (tr("Status"), this);
//  v->addWidget (statGroup);
  QLabel * statText = new QLabel ("Status:", this);
  statText->setAlignment (Qt::AlignCenter);
  v->addWidget (statText);

  // setup calculated result bix
  createResultItems (calculated);
  updateSelection ();

  // intantiate transmission lines
  TransLineTypes[0].line = new microstrip ();
  TransLineTypes[0].line->setApplication (this);
  TransLineTypes[1].line = new rectwaveguide ();
  TransLineTypes[1].line->setApplication (this);
  TransLineTypes[2].line = new coax ();
  TransLineTypes[2].line->setApplication (this);
  TransLineTypes[3].line = new c_microstrip ();
  TransLineTypes[3].line->setApplication (this);
}

/* Destructor destroys the application. */
QucsTranscalc::~QucsTranscalc()
{
  for (int i = 0; i < MAX_TRANS_TYPES; i++) {
    if (TransLineTypes[i].line)
      delete TransLineTypes[i].line;
  }
}

/* The function puts translatable text into the transmission line
   structures. */
void QucsTranscalc::setupTranslations () {
  // calculated results
  TransLineTypes[0].result[0].name = new QString(tr("er_eff"));
  TransLineTypes[0].result[1].name = new QString(tr("Conductor Losses"));
  TransLineTypes[0].result[2].name = new QString(tr("Dielectric Losses"));
  TransLineTypes[0].result[3].name = new QString(tr("Skin Depth"));

  TransLineTypes[1].result[0].name = new QString(tr("er_eff"));
  TransLineTypes[1].result[1].name = new QString(tr("Conductor Losses"));
  TransLineTypes[1].result[2].name = new QString(tr("Dielectric Losses"));
  TransLineTypes[1].result[3].name = new QString(tr("TE-Modes"));
  TransLineTypes[1].result[4].name = new QString(tr("TM-Modes"));

  TransLineTypes[2].result[0].name = new QString(tr("Conductor Losses"));
  TransLineTypes[2].result[1].name = new QString(tr("Dielectric Losses"));
  TransLineTypes[2].result[2].name = new QString(tr("TE-Modes"));
  TransLineTypes[2].result[3].name = new QString(tr("TM-Modes"));

  TransLineTypes[3].result[0].name = new QString(tr("er_eff_e"));
  TransLineTypes[3].result[1].name = new QString(tr("er_eff_o"));
  TransLineTypes[3].result[2].name = new QString(tr("Conductor Losses Even"));
  TransLineTypes[3].result[3].name = new QString(tr("Conductor Losses Odd"));
  TransLineTypes[3].result[4].name = new QString(tr("Dielectric Losses Even"));
  TransLineTypes[3].result[5].name = new QString(tr("Dielectric Losses Odd"));
  TransLineTypes[3].result[6].name = new QString(tr("Skin Depth"));

  // extra tool tips
  struct TransType * t = TransLineTypes;
  t->array[0].item[0].tip = new QString(tr("Relative Permittivity"));
  t->array[0].item[1].tip = new QString(tr("Relative Permeability"));
  t->array[0].item[2].tip = new QString(tr("Height of Substrate"));
  t->array[0].item[3].tip = new QString(tr("Height of Box Top"));
  t->array[0].item[4].tip = new QString(tr("Strip Thickness"));
  t->array[0].item[5].tip = new QString(tr("Strip Conductivity"));
  t->array[0].item[6].tip = new QString(tr("Dielectric Loss Tangent"));
  t->array[0].item[7].tip = new QString(tr("Conductor Roughness"));
  t->array[1].item[0].tip = new QString(tr("Frequency"));
  t->array[2].item[0].tip = new QString(tr("Line Width"));
  t->array[2].item[1].tip = new QString(tr("Line Length"));
  t->array[3].item[0].tip = new QString(tr("Characteristic Impedance"));
  t->array[3].item[1].tip = new QString(tr("Electrical Length"));

  t++;
  t->array[0].item[0].tip = new QString(tr("Relative Permittivity"));
  t->array[0].item[1].tip = new QString(tr("Relative Permeability"));
  t->array[0].item[2].tip = new QString(tr("Conductivity of Metal"));
  t->array[0].item[3].tip = new QString(tr("Dielectric Loss Tangent"));
  t->array[0].item[4].tip = new QString(tr("Magnetic Loss Tangent"));
  t->array[1].item[0].tip = new QString(tr("Frequency"));
  t->array[2].item[0].tip = new QString(tr("Width of Waveguide"));
  t->array[2].item[1].tip = new QString(tr("Height of Waveguide"));
  t->array[2].item[2].tip = new QString(tr("Waveguide Length"));
  t->array[3].item[0].tip = new QString(tr("Characteristic Impedance"));
  t->array[3].item[1].tip = new QString(tr("Electrical Length"));

  t++;
  t->array[0].item[0].tip = new QString(tr("Relative Permittivity"));
  t->array[0].item[1].tip = new QString(tr("Relative Permeability"));
  t->array[0].item[2].tip = new QString(tr("Dielectric Loss Tangent"));
  t->array[0].item[3].tip = new QString(tr("Conductivity of Metal"));
  t->array[1].item[0].tip = new QString(tr("Frequency"));
  t->array[2].item[0].tip = new QString(tr("Inner Diameter"));
  t->array[2].item[1].tip = new QString(tr("Outer Diameter"));
  t->array[2].item[2].tip = new QString(tr("Length"));
  t->array[3].item[0].tip = new QString(tr("Characteristic Impedance"));
  t->array[3].item[1].tip = new QString(tr("Electrical Length"));

  t++;
  t->array[0].item[0].tip = new QString(tr("Relative Permittivity"));
  t->array[0].item[1].tip = new QString(tr("Relative Permeability"));
  t->array[0].item[2].tip = new QString(tr("Height of Substrate"));
  t->array[0].item[3].tip = new QString(tr("Height of Box Top"));
  t->array[0].item[4].tip = new QString(tr("Strip Thickness"));
  t->array[0].item[5].tip = new QString(tr("Strip Conductivity"));
  t->array[0].item[6].tip = new QString(tr("Dielectric Loss Tangent"));
  t->array[0].item[7].tip = new QString(tr("Conductor Roughness"));
  t->array[1].item[0].tip = new QString(tr("Frequency"));
  t->array[2].item[0].tip = new QString(tr("Line Width"));
  t->array[2].item[1].tip = new QString(tr("Gap Width"));
  t->array[2].item[2].tip = new QString(tr("Length"));
  t->array[3].item[0].tip = new QString(tr("Even-Mode Impedance"));
  t->array[3].item[1].tip = new QString(tr("Odd-Mode Impedance"));
  t->array[3].item[2].tip = new QString(tr("Electrical Length"));
}

/* Creates a property item 'val' in a parameter category specified by
   its 'box'. */
void QucsTranscalc::createPropItem (QVBox ** parentRows, TransValue * val,
				    int box) {
  QRadioButton * r = NULL;
  QLabel * l;
  QLineEdit * e;
  QComboBox * c;

  // name label
  l = new QLabel (val->name, parentRows[0]);
  l->setAlignment (Qt::AlignRight);
  if (val->tip) QToolTip::add (l, *(val->tip));
  val->label = l;

  // editable value text
  e = new QLineEdit (parentRows[1]);
  e->setText (QString::number (val->value));
  e->setAlignment (Qt::AlignRight);
  if (!val->name) e->setDisabled (true);
  val->lineedit = e;

  // unit choice
  c = new QComboBox (parentRows[2]);
  if (!val->units[0]) {
    c->insertItem ("NA");
    c->setDisabled(true);
  }
  else {
    int nounit = 0;
    for (int i = 0; val->units[i]; i++) {
      c->insertItem (val->units[i]);
      if (!strcmp (val->units[i], "NA")) nounit++;
    }
    c->setDisabled (nounit != 0);
    c->setCurrentItem (0);
  }
  val->combobox = c;

  // special synthesize-computation choice
  if (box == TRANS_PHYSICAL) {
    r = new QRadioButton (parentRows[3]);
    r->setDisabled (true);
    val->radio = r;
  }
}

/* The function changes the content of a GUI's property entry
   depending on the given transmission line property. */
void QucsTranscalc::updatePropItem (TransValue * val) {
  // update label text
  val->label->setText (val->name);
  QToolTip::remove (val->label);
  if (val->tip) QToolTip::add (val->label, *(val->tip));

  // update editable value text
  val->lineedit->setText (QString::number (val->value));
  val->lineedit->setDisabled (!val->name);

  // update unit choice
  val->combobox->clear();
  if (!val->units[0]) {
    val->combobox->insertItem ("NA");
    val->combobox->setDisabled (true);
  }
  else {
    int nounit = 0;
    for (int i = 0; val->units[i]; i++) {
      val->combobox->insertItem (val->units[i]);
      if (!strcmp (val->units[i], "NA")) nounit++;
    }
    val->combobox->setCurrentItem(0);
    val->combobox->setDisabled (nounit);
  }
}

/* Switches to the current type of transmission line layout. */
void QucsTranscalc::setMode (int _mode) {
  // change necessary?
  if (mode == _mode) return;
  mode = _mode;

  // go through each type of parameter category
  for (int box = 0; box < MAX_TRANS_BOXES; box++) {
    int last = 0, idx = getTypeIndex();
    struct TransValue * val = TransLineTypes[idx].array[box].item;
    // update each property item
    for (int i = 0; i < TransMaxBox[box]; i++) {
      // fix uninitialized memory
      if (val->name == NULL) last++;
      if (last) {
	val->name = NULL;
	val->value = 0;
	val->tip = NULL;
	val->units[0] = NULL;
      }
      updatePropItem (val);
      val++;
    }
  }
  // update selection and results
  updateSelection ();
  updateResultItems ();
}

/* Updates the current choice of physical property selection. */
void QucsTranscalc::updateSelection () {
  int idx = getTypeIndex ();
  struct TransValue * val = TransLineTypes[idx].array[TRANS_PHYSICAL].item;
  for (int i = 0; i < TransMaxBox[TRANS_PHYSICAL]; i++) {
    if (TransLineTypes[idx].radio[i] != -1) {
      val->radio->setDisabled (false);
      if (TransLineTypes[idx].radio[i] == 1)
	val->radio->setDown (true);
    }
    else {
      val->radio->setDisabled (true);
    }
    val++;
  }
}

/* The function creates the property items for the given category of
   transmission line parameters. */
void QucsTranscalc::createPropItems (QHGroupBox * parent, int box) {
  struct TransValue * val, * dup;
  int last = 0, idx = getTypeIndex ();
  val = TransLineTypes[idx].array[box].item;

  QVBox *rows[4];
  rows[0] = new QVBox (parent);
  rows[1] = new QVBox (parent);
  rows[2] = new QVBox (parent);
  rows[3] = new QVBox (parent);

  parent->layout()->setSpacing (2);
  rows[0]->setSpacing (2);
  rows[1]->setSpacing (2);
  rows[2]->setSpacing (2);
  rows[3]->setSpacing (2);

  // go through each parameter category
  for (int i = 0; i < TransMaxBox[box]; i++) {
    // fix uninitialized memory
    if (val->name == NULL) last++;
    if (last) {
      val->name = NULL;
      val->value = 0;
      val->tip = NULL;
      val->units[0] = NULL;
    }
    createPropItem (rows, val, box);
    // publish the newly created widgets to the other transmission lines
    for (int _mode = 0; _mode < MAX_TRANS_TYPES; _mode++) {
      if (idx != _mode) {
	dup = & TransLineTypes[_mode].array[box].item[i];
	dup->label = val->label;
	dup->lineedit = val->lineedit;
	dup->combobox = val->combobox;
	dup->radio = val->radio;
      }
    }
    val++;
  }
}

/* Creates a single result item. */
void QucsTranscalc::createResultItem (QVBox ** parentRows, TransResult * res) {
  QLabel * n =
      new QLabel (res->name ? *(res->name) + ":" : QString(), parentRows[0]);
  n->setAlignment (Qt::AlignRight);
  res->label = n;
  QLabel * v = new QLabel (parentRows[1]);
  v->setAlignment (Qt::AlignLeft);
  res->value = v;
  if (!res->name) {
    v->hide ();
    n->hide ();
  }
}

/* Updates the given result item. */
void QucsTranscalc::updateResultItem (TransResult * res) {
  if (res->name) {
    res->label->setText (*(res->name) + ":");
    res->label->show ();
    res->value->show ();
  } else {
    res->label->hide ();
    res->value->hide ();
  }
}

/* Creates all the result items. */
void QucsTranscalc::createResultItems (QHGroupBox * parent) {
  struct TransResult * res, * dup;
  int idx = getTypeIndex ();
  res = & TransLineTypes[idx].result[0];

  QVBox *rows[2];
  rows[0] = new QVBox (parent);
  rows[1] = new QVBox (parent);

  parent->layout()->setSpacing (2);
  rows[0]->setSpacing (2);
  rows[1]->setSpacing (2);

  for (int i = 0; i < MAX_TRANS_RESULTS; i++) {
    createResultItem (rows, res);
    for (int _mode = 0; _mode < MAX_TRANS_TYPES; _mode++) {
      if (idx != _mode) {
	dup = & TransLineTypes[_mode].result[i];
	dup->label = res->label;
	dup->value = res->value;
      }
    }
    res++;
  }
}

/* Puts the given result into its widget. */
void QucsTranscalc::setResult (int line, char * text) {
  struct TransResult * res;
  res = & TransLineTypes[getTypeIndex ()].result[line];
  res->value->setText (text);
}

/* Updates the current result items. */
void QucsTranscalc::updateResultItems() {
  int idx = getTypeIndex ();
  struct TransResult * res = TransLineTypes[idx].result;
  for (int i = 0; i < MAX_TRANS_RESULTS; i++) {
    updateResultItem (res);
    res++;
  }
}

/* Returns the appropriate array index for the given mode. */
int QucsTranscalc::getTypeIndex () {
  struct TransType * type = TransLineTypes;
  for (int i = 0; type->type != ModeNone; i++) {
    if (type->type == mode) return i;
    type++;
  }
  return 0;
}

/* Returns the property value specified by its name. */
struct TransValue * QucsTranscalc::findProperty (QString prop) {
  struct TransValue * val = NULL;
  for (int box = 0; box < MAX_TRANS_BOXES; box++) {
    val = TransLineTypes[getTypeIndex ()].array[box].item;
    for (int i = 0; val->name; i++) {
      if (prop == val->name) return val;
      val++;
    }
  }
  return NULL;
}

/* Sets the given property to the given value. */
void QucsTranscalc::setProperty (QString prop, double value) {
  struct TransValue * val = findProperty (prop);
  if (val) {
    val->lineedit->setText (QString::number (value));
  }
}

/* Returns the given property value. */
double QucsTranscalc::getProperty (QString prop) {
  struct TransValue * val = findProperty (prop);
  if (val) {
    QString str = val->lineedit->text ();
    val->value = str.toDouble ();
    return val->value;
  }
  return 0;
}

/* Returns the given property unit. */
char * QucsTranscalc::getUnit (QString prop) {
  struct TransValue * val = findProperty (prop);
  if (val) {
    QString str = val->combobox->currentText ();
    for (int i = 0; val->units[i]; i++) {
      if (str == val->units[i])
	return val->units[i];
    }
  }
  return NULL;
}

/* Returns the given property selection. */
bool QucsTranscalc::isSelected (QString prop) {
  struct TransValue * val = findProperty (prop);
  if (val) {
    if (val->radio)
      if (val->radio->isChecked ()) {
	return true;
      }
  }
  return false;
}

void QucsTranscalc::slotAbout()
{
  QMessageBox::about(this, tr("About..."),
    tr("Qucs Transcalc")+" "+PACKAGE_VERSION+"\n"+
    tr("Transmission Line Calculator for Qucs\n")+
    tr("Copyright (C) 2001 by Gopal Narayanan\n")+
    tr("Copyright (C) 2002 by Claudio Girardi\n")+
    tr("Copyright (C) 2005 by Stefan Jahn\n")+
    "\nThis is free software; see the source for copying conditions."
    "\nThere is NO warranty; not even for MERCHANTABILITY or "
    "\nFITNESS FOR A PARTICULAR PURPOSE.");
}

void QucsTranscalc::slotQuit()
{
  int tmp;
  tmp = x();		// call size and position function in order to ...
  tmp = y();		// ... set them correctly before closing the ...
  tmp = width();	// dialog !!!  Otherwise the frame of the window ...
  tmp = height();	// will not be recognized (a X11 problem).

  accept();
}

void QucsTranscalc::closeEvent(QCloseEvent *Event)
{
  int tmp;
  tmp = x();		// call size and position function in order to ...
  tmp = y();		// ... set them correctly before closing the ...
  tmp = width();	// dialog !!!  Otherwise the frame of the window ...
  tmp = height();	// will not be recognized (a X11 problem).

  Event->accept();
}

void QucsTranscalc::slotSelectType (int Type)
{
  int _mode = mode;
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

void QucsTranscalc::slotAnalyze()
{
  if (TransLineTypes[getTypeIndex()].line)
    TransLineTypes[getTypeIndex()].line->analyze();
}

void QucsTranscalc::slotSynthesize()
{
  if (TransLineTypes[getTypeIndex()].line)
    TransLineTypes[getTypeIndex()].line->synthesize();
}
