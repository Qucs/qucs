/***************************************************************************
                      qucssettingsdialog.cpp  -  description
                             -------------------
    begin                : Sun May 23 2004
    copyright            : (C) 2003 by Michael Margraf
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

#include "qucssettingsdialog.h"
#include "main.h"
#include "qucsview.h"

#include <qwidget.h>
#include <qlabel.h>
#include <qhbox.h>
#include <qtabwidget.h>
#include <qlayout.h>
#include <qcolordialog.h>
#include <qfontdialog.h>
#include <qvalidator.h>


QucsSettingsDialog::QucsSettingsDialog(QucsApp *parent,
			  const char *name)
			: QDialog(parent, name, TRUE, Qt::WDestructiveClose)
{
  App = parent;
  setCaption(tr("Edit Qucs Properties"));

  QVBoxLayout *all = new QVBoxLayout(this); // to provide the neccessary size
  QTabWidget *t = new QTabWidget(this);
  all->addWidget(t);

  // ...........................................................
  QWidget *Tab1 = new QWidget(t);
  QGridLayout *gp = new QGridLayout(Tab1,5,2,5,5);

  QLabel *l1 = new QLabel(tr("Font (set after reload):"), Tab1);
  gp->addWidget(l1,0,0);
  FontButton = new QPushButton(Tab1);
  connect(FontButton, SIGNAL(clicked()), SLOT(slotFontDialog()));
  gp->addWidget(FontButton,0,1);

  QLabel *l2 = new QLabel(tr("Document Background Color:"), Tab1);
  gp->addWidget(l2,1,0);
  BGColorButton = new QPushButton("      ", Tab1);
  connect(BGColorButton, SIGNAL(clicked()), SLOT(slotBGColorDialog()));
  gp->addWidget(BGColorButton,1,1);

  QLabel *l3 = new QLabel(tr("maximum undo operations:"), Tab1);
  gp->addWidget(l3,2,0);
  undoNumEdit = new QLineEdit(Tab1);
  undoNumEdit->setValidator(new QIntValidator(0, 200, this));
  gp->addWidget(undoNumEdit,2,1);

  QLabel *l4 = new QLabel(tr("text editor:"), Tab1);
  gp->addWidget(l4,3,0);
  editorEdit = new QLineEdit(Tab1);
  gp->addWidget(editorEdit,3,1);


  t->addTab(Tab1, tr("Settings"));

  // ...........................................................
/*  QWidget *Tab2 = new QWidget(t);
  QGridLayout *gp2 = new QGridLayout(Tab2,3,2,5,5);
  Check_GridOn = new QCheckBox(tr("show Grid"),Tab2);

  QLabel *l3 = new QLabel(tr("horizontal Grid:"), Tab2);
  gp2->addWidget(l3,1,0);
  Input_GridX = new QLineEdit(Tab2);
  gp2->addWidget(Input_GridX,1,1);

  QLabel *l4 = new QLabel(tr("vertical Grid:"), Tab2);
  gp2->addWidget(l4,2,0);
  Input_GridY = new QLineEdit(Tab2);
  gp2->addWidget(Input_GridY,2,1);

  t->addTab(Tab2, tr("Grid"));
*/
  // ...........................................................
  // buttons on the bottom of the dialog (independent of the TabWidget)
  QHBox *Butts = new QHBox(this);
  Butts->setSpacing(5);
  Butts->setMargin(5);
  all->addWidget(Butts);

  QPushButton *OkButt = new QPushButton(tr("OK"), Butts);
  connect(OkButt, SIGNAL(clicked()), SLOT(slotOK()));
  QPushButton *ApplyButt = new QPushButton(tr("Apply"), Butts);
  connect(ApplyButt, SIGNAL(clicked()), SLOT(slotApply()));
  QPushButton *CancelButt = new QPushButton(tr("Cancel"), Butts);
  connect(CancelButt, SIGNAL(clicked()), SLOT(reject()));
  QPushButton *DefaultButt = new QPushButton(tr("Default Values"), Butts);
  connect(DefaultButt, SIGNAL(clicked()), SLOT(slotDefaultValues()));

  OkButt->setDefault(true);

  // ...........................................................
  // fill the fields with the Qucs-Properties

  Font  = QucsSettings.font;
  FontButton->setText(Font.toString());
  BGColorButton->setPaletteBackgroundColor(
	App->view->viewport()->paletteBackgroundColor());
  undoNumEdit->setText(QString::number(QucsSettings.maxUndo));
  editorEdit->setText(QucsSettings.Editor);
}

QucsSettingsDialog::~QucsSettingsDialog()
{
}

// -----------------------------------------------------------
void QucsSettingsDialog::slotOK()
{
  slotApply();
  accept();
}

// -----------------------------------------------------------
void QucsSettingsDialog::slotApply()
{
  bool changed = false;

  if(App->view->viewport()->paletteBackgroundColor() !=
		BGColorButton->paletteBackgroundColor()) {
    App->view->viewport()->setPaletteBackgroundColor(
		BGColorButton->paletteBackgroundColor());
    changed = true;
  }

  if(savingFont != Font) {
    savingFont = Font;
//    App->setFont(Font);
//    App->ContentMenu->setFont(Font);
    changed = true;
  }

  bool ok;
  if(QucsSettings.maxUndo != undoNumEdit->text().toUInt(&ok)) {
    QucsSettings.maxUndo = undoNumEdit->text().toInt(&ok);
    changed = true;
  }
  if(QucsSettings.Editor != editorEdit->text()) {
    QucsSettings.Editor = editorEdit->text();
    changed = true;
  }

  if(changed) {
    saveApplSettings(App);  // also sets the small and large font
    App->repaint();
  }
}

// -----------------------------------------------------------
void QucsSettingsDialog::slotFontDialog()
{
  bool ok;
  QFont tmpFont = QFontDialog::getFont(&ok, Font, this);
  if(ok) {
    Font = tmpFont;
    FontButton->setText(Font.toString());
  }
}

// -----------------------------------------------------------
void QucsSettingsDialog::slotBGColorDialog()
{
  QColor c = QColorDialog::getColor(
		BGColorButton->paletteBackgroundColor(), this);
  if(c.isValid())
    BGColorButton->setPaletteBackgroundColor(c);
}

// -----------------------------------------------------------
void QucsSettingsDialog::slotDefaultValues()
{
  Font = QFont("Helvetica", 12);
  FontButton->setText(Font.toString());
  BGColorButton->setPaletteBackgroundColor(QColor(255,250,225));

  undoNumEdit->setText("20");
  editorEdit->setText(BINARYDIR "qucsedit");
}
