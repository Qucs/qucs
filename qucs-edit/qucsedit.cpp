/***************************************************************************
                          qucsedit.cpp  -  description
                             -------------------
    begin                : Mon Nov 17 2003
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

#include "qucsedit.h"

#include <qlayout.h>
#include <qhbox.h>
#include <qpushbutton.h>
#include <qfile.h>
#include <qtextstream.h>
#include <qmessagebox.h>
#include <qtoolbutton.h>
#include <qimage.h>
#include <qfiledialog.h>


QucsEdit::QucsEdit(const QString& FileName_, bool readOnly)
{
  setIcon (QPixmap(BITMAPDIR "big.qucs.xpm"));  // set application icon
  setCaption("QucsEdit " PACKAGE_VERSION " - " + tr("File: "));

  QVBoxLayout *v = new QVBoxLayout(this);

  QHBox *h = new QHBox(this);
  v->addWidget(h);

  QToolButton *ButtLoad = new QToolButton(h);
  ButtLoad->setIconSet(QIconSet(QImage(BITMAPDIR "fileopen.png")));
  connect(ButtLoad, SIGNAL(clicked()), SLOT(slotLoad()));

  QToolButton *ButtSave = new QToolButton(h);
  ButtSave->setIconSet(QIconSet(QImage(BITMAPDIR "filesave.png")));
  connect(ButtSave, SIGNAL(clicked()), SLOT(slotSave()));
  ButtSave->setDisabled(readOnly);

  h->setStretchFactor(new QWidget(h),5); // stretchable placeholder

  QPushButton *ButtAbout = new QPushButton(tr("About"),h);
  connect(ButtAbout, SIGNAL(clicked()), SLOT(slotAbout()));

  QPushButton *ButtOK = new QPushButton(tr("Quit"),h);
  connect(ButtOK, SIGNAL(clicked()), SLOT(slotQuit()));
  ButtOK->setFocus();

  text = new QTextEdit(this);
  text->setTextFormat(Qt::PlainText);
  text->setReadOnly(readOnly);
  text->setWordWrap(QTextEdit::NoWrap);
  text->setMinimumSize(300,200);
  v->addWidget(text);

  // .................................................
  loadFile(FileName_);
}

QucsEdit::~QucsEdit()
{
}

// ************************************************************
void QucsEdit::slotAbout()
{
  QMessageBox::about(this, tr("About..."),
    tr("Qucs Editor Version ")+PACKAGE_VERSION+
    tr("\nVery simple text editor for Qucs\n")+
    tr("Copyright (C) 2004 by Michael Margraf\n")+
    "\nThis is free software; see the source for copying conditions."
    "\nThere is NO warranty; not even for MERCHANTABILITY or "
    "\nFITNESS FOR A PARTICULAR PURPOSE.");
}

// ************************************************************
void QucsEdit::slotLoad()
{
  QString s = QFileDialog::getOpenFileName(".", "*", this,
					"", tr("Enter a Filename"));
  if(s.isEmpty()) return;
  if(!closeFile()) return;
  loadFile(s);
}

// ************************************************************
void QucsEdit::slotSave()
{
  if(FileName.isEmpty()) return;
  QFile file(FileName);
  if(!file.open(IO_WriteOnly)) {
    QMessageBox::critical(this, tr("Error"),
		tr("Cannot write file: ")+FileName);
    return;
  }

  QTextStream stream(&file);
  stream << text->text();
  text->setModified(false);
  file.close();
}

// ************************************************************
void QucsEdit::slotQuit()
{
  if(!closeFile()) return;

  int tmp;
  tmp = x();		// call size and position function in order to ...
  tmp = y();		// ... set them correctly before closing the ...
  tmp = width();	// dialog !!!  Otherwise the frame of the window ...
  tmp = height();	// will not be recognized (a X11 problem).

  accept();
}

// ************************************************************
// To get all close events.
void QucsEdit::closeEvent(QCloseEvent *Event)
{
  int tmp;
  tmp = x();		// call size and position function in order to ...
  tmp = y();		// ... set them correctly before closing the ...
  tmp = width();	// dialog !!!  Otherwise the frame of the window ...
  tmp = height();	// will not be recognized (a X11 problem).

  Event->accept();
}

// ************************************************************
bool QucsEdit::loadFile(const QString& Name)
{
  if(Name.isEmpty()) return false;
  QFile file(Name);
  if(!file.open(IO_ReadOnly)) {
    QMessageBox::critical(this, tr("Error"),
		tr("Cannot read file: ")+Name);
    return false;
  }

  QTextStream stream(&file);
  text->setText(stream.read());
  file.close();

  QFileInfo info(Name);
  FileName = info.fileName();
  setCaption("QucsEdit " PACKAGE_VERSION " - " + tr("File: ")+FileName);
  return true;
}


// ************************************************************
bool QucsEdit::closeFile()
{
  if(text->isModified()) {
    switch(QMessageBox::warning(this,tr("Closing document"),
      tr("The text contains unsaved changes!\n")+
      tr("Do you want to save the changes?"),
      tr("&Save"), tr("&Discard"), tr("Cancel"), 0, 2)) {
      case 0: slotSave();
	      return true;
      case 2: return false;
    }
  }
  return true;
}
