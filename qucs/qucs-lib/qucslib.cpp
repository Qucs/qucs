/***************************************************************************
                               qucslib.cpp
                              -------------
    begin                : Sat May 28 2005
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

#include <qmenubar.h>
#include <qaction.h>
#include <qpopupmenu.h>
#include <qcombobox.h>
#include <qclipboard.h>
#include <qapplication.h>
#include <qlayout.h>
#include <qhbox.h>
#include <qvgroupbox.h>
#include <qpushbutton.h>
#include <qmessagebox.h>
#include <qtextedit.h>
#include <qlistbox.h>
#include <qdir.h>
#include <qregexp.h>

#include "qucslib.h"
#include "displaydialog.h"
#include "symbolwidget.h"


/* Constructor setups the GUI. */
QucsLib::QucsLib()
{
  // set application icon
  setIcon (QPixmap(QucsSettings.BitmapDir + "big.qucs.xpm"));
  setCaption("Qucs Library Tool " PACKAGE_VERSION);

  QMenuBar * menuBar = new QMenuBar (this);

  // create file menu
  QPopupMenu * fileMenu = new QPopupMenu ();
  QAction * fileQuit =
    new QAction (tr("Quit"), tr("&Quit"), CTRL+Key_Q, this);
  fileQuit->addTo (fileMenu);
  connect(fileQuit, SIGNAL(activated()), SLOT(slotQuit()));

  // create help menu
  QPopupMenu * helpMenu = new QPopupMenu ();
  QAction * helpHelp =
    new QAction (tr("Help"), tr("&Help"), Key_F1, this);
  helpHelp->addTo (helpMenu);
  connect(helpHelp, SIGNAL(activated()), SLOT(slotHelp()));
  QAction * helpAbout =
    new QAction (tr("About"), tr("About"), 0, helpMenu);
  helpAbout->addTo (helpMenu);
  connect(helpAbout, SIGNAL(activated()), SLOT(slotAbout()));

  // setup menu bar
  menuBar->insertItem (tr("&File"), fileMenu);
  menuBar->insertSeparator ();
  menuBar->insertItem (tr("&Help"), helpMenu);

  // main box
  QVBoxLayout * all = new QVBoxLayout (this);
  all->setSpacing (0);
  all->setMargin (0);

  // reserve space for menubar
  QWidget * Space = new QWidget (this);
  Space->setFixedSize(5, menuBar->height() + 2);
  all->addWidget (Space);

  // main layout
  QHBox * h = new QHBox (this);
  h->setSpacing (5);
  h->setMargin (3);
  all->addWidget (h);

  // library and component choice
  QVGroupBox * LibGroup = new QVGroupBox (tr("Component Selection"), h);
  Library = new QComboBox (LibGroup);
  connect(Library, SIGNAL(activated(int)), SLOT(slotSelectLibrary(int)));
  CompList = new QListBox(LibGroup);
  connect(CompList, SIGNAL(clicked(QListBoxItem*)),
	SLOT(slotShowComponent(QListBoxItem*)));

  QHBox * h1 = new QHBox (LibGroup);
  QPushButton * SearchButton = new QPushButton (tr("Search..."), h1);
  connect(SearchButton, SIGNAL(clicked()), SLOT(slotSearchComponent()));
  h1->setStretchFactor(new QWidget(h1), 5); // stretchable placeholder


  // component display
  CompGroup = new QVGroupBox (tr("Component"), h);
  CompDescr = new QTextEdit(CompGroup);
  CompDescr->setTextFormat(Qt::PlainText);
  CompDescr->setReadOnly(true);
  CompDescr->setWordWrap(QTextEdit::NoWrap);

  Symbol = new SymbolWidget (CompGroup);

  QHBox * h2 = new QHBox (CompGroup);
  QPushButton * CopyButton = new QPushButton (tr("Copy to clipboard"), h2);
  connect(CopyButton, SIGNAL(clicked()), SLOT(slotCopyToClipBoard()));
  QPushButton * ShowButton = new QPushButton (tr("Show Model"), h2);
  connect(ShowButton, SIGNAL(clicked()), SLOT(slotShowModel()));

  // ......................................................
  // Put all available libraries into ComboBox.
  QDir LibDir(QucsSettings.LibDir);
  QStringList LibFiles = LibDir.entryList("*.lib", QDir::Files, QDir::Name);

  QStringList::iterator it;
  for(it = LibFiles.begin(); it != LibFiles.end(); it++)
    Library->insertItem((*it).left((*it).length()-4));

  slotSelectLibrary(0);
  CompList->setCurrentItem(0);
  slotShowComponent(CompList->selectedItem());
}

/* Destructor destroys the application. */
QucsLib::~QucsLib()
{
}

void QucsLib::slotAbout()
{
  QMessageBox::about(this, tr("About..."),
    "QucsLib " PACKAGE_VERSION "\n"+
    tr("Library Manager for Qucs\n")+
    tr("Copyright (C) 2005 by Michael Margraf\n")+
    "\nThis is free software; see the source for copying conditions."
    "\nThere is NO warranty; not even for MERCHANTABILITY or "
    "\nFITNESS FOR A PARTICULAR PURPOSE.");
}

void QucsLib::slotQuit()
{
  int tmp;
  tmp = x();		// call size and position function in order to ...
  tmp = y();		// ... set them correctly before closing the ...
  tmp = width();	// dialog !!!  Otherwise the frame of the window ...
  tmp = height();	// will not be recognized (a X11 problem).

  accept();
}

void QucsLib::closeEvent(QCloseEvent *Event)
{
  int tmp;
  tmp = x();		// call size and position function in order to ...
  tmp = y();		// ... set them correctly before closing the ...
  tmp = width();	// dialog !!!  Otherwise the frame of the window ...
  tmp = height();	// will not be recognized (a X11 problem).

  Event->accept();
}

void QucsLib::slotHelp()
{
  DisplayDialog *d = new DisplayDialog(this);
  d->setCaption("QucsLib Help");
  d->resize(250, 300);
  d->Text->setText(
     tr("QucsLib is a program to manage Qucs component libraries. "
	"On the left side of the application window the available "
	"libraries can be browsed to find the wanted component. "
	"By clicking on the component name its description can be "
	"seen on the right side. The selected component is "
	"transported to the Qucs application by clicking on the "
	"button \"Copy to Clipboard\". Being back in the schematic "
	"window the component can be inserted by pressing CTRL-V "
	" (paste from clipboard)."));
  d->show();
}

void QucsLib::slotCopyToClipBoard()
{
  QString s = "<Qucs Schematic " PACKAGE_VERSION ">\n";
  s += "<Components>\n";
  s += "  " + ModelString + "\n";
  s += "</Components>\n";

  // put resulting schematic into clipboard
  QClipboard *cb = QApplication::clipboard();
  cb->setText(s);
}

void QucsLib::slotShowModel()
{
  DisplayDialog *d = new DisplayDialog(this);
  d->setCaption("Model");
  d->resize(500, 150);
  d->Text->setText(ModelString);
  d->Text->setWordWrap(QTextEdit::NoWrap);
  d->show();
}

void QucsLib::slotSelectLibrary(int Index)
{
  CompList->clear();

  QFile file(QucsSettings.LibDir + Library->text(Index) + ".lib");
  if(!file.open(IO_ReadOnly)) {
    QMessageBox::critical(this, tr("Error"),
        tr("Cannot open \"%1\".").arg(
           QucsSettings.LibDir + Library->text(Index) + ".lib"));
    return;
  }

  QTextStream ReadWhole(&file);
  LibraryString = ReadWhole.read();
  file.close();
  
  int Start=0, End;
  while((Start=LibraryString.find("\n<Component ", Start)) > 0) {
    Start += 12;
    End = LibraryString.find('>', Start);
    if(End > 0)
      CompList->insertItem(LibraryString.mid(Start, End-Start));
  }
  file.close();
}

void QucsLib::slotSearchComponent()
{
}

void QucsLib::slotShowComponent(QListBoxItem *Item)
{
  if(!Item) return;

  int Start = LibraryString.find("<Component " + Item->text() + ">");
  if(Start < 0) {
    QMessageBox::critical(this, tr("Error"),
        tr("Cannot find component \"%1\".").arg(Item->text()));
    return;
  }

  int End = LibraryString.find("</Component>", Start);
  if(End < 0) {
    QMessageBox::critical(this, tr("Error"), tr("Library is corrupt."));
    return;
  }
  QString CompString = LibraryString.mid(Start, End-Start);
  CompGroup->setTitle(Item->text());

  CompDescr->clear();
  Start = CompString.find("<Description>");
  if(Start > 0) {
    Start += 13;
    End = CompString.find("</Description>", Start);
    if(End < 0) {
      QMessageBox::critical(this, tr("Error"), tr("Library is corrupt."));
      return;
    }
    CompDescr->setText(
      CompString.mid(Start, End-Start).replace(QRegExp("\\n\\x20+"), "\n").remove(0, 1));
  }


  Start = CompString.find("<Symbol>");
  if(Start > 0) {
    Start += 8;
    End = CompString.find("</Symbol>", Start);
    if(End < 0) {
      QMessageBox::critical(this, tr("Error"), tr("Library is corrupt."));
      return;
    }
    Symbol->setSymbol(CompString.mid(Start, End-Start));
  }


  Start = CompString.find("<Model>");
  if(Start > 0) {
    Start += 7;
    End = CompString.find("</Model>", Start);
    if(End < 0) {
      QMessageBox::critical(this, tr("Error"), tr("Library is corrupt."));
      return;
    }
    ModelString =
      CompString.mid(Start, End-Start).replace(QRegExp("\\n\\x20+"), "\n").remove(0, 1);
  }
}
