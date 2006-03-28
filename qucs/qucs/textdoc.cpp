/***************************************************************************
                               textdoc.cpp
                              -------------
    begin                : Sat Mar 11 2006
    copyright            : (C) 2006 by Michael Margraf
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

#include "textdoc.h"
#include "main.h"
#include "qucs.h"

#include <qaction.h>
#include <qpixmap.h>
#include <qfileinfo.h>
#include <qtabwidget.h>


TextDoc::TextDoc(QucsApp *App_, const QString& Name_) : QucsDoc(App_, Name_)
{
  QFileInfo Info(Name_);
  if(App) {
    if(Name_.isEmpty())
      App->DocumentTab->addTab(this, QPixmap(empty_xpm),
                            QObject::tr("untitled"));
    else
      App->DocumentTab->addTab(this, QPixmap(empty_xpm),
                            Info.fileName());
    App->DocumentTab->setCurrentPage(App->DocumentTab->indexOf(this));
    viewport()->setFocus();
  }

  undoIsAvailable = redoIsAvailable = false;

  setPaletteBackgroundColor(QucsSettings.BGColor);
  setUndoDepth(QucsSettings.maxUndo);
  connect(this, SIGNAL(textChanged()), SLOT(slotSetChanged()));
  connect(this, SIGNAL(undoAvailable(bool)), SLOT(slotChangeUndo(bool)));
  connect(this, SIGNAL(redoAvailable(bool)), SLOT(slotChangeRedo(bool)));
}

TextDoc::~TextDoc()
{
  if(App)  App->DocumentTab->removePage(this);  // delete tab in TabBar
}

// ---------------------------------------------------
void TextDoc::setName(const QString& Name_)
{
  DocName = Name_;

  QFileInfo Info(DocName);
  if (App)
    App->DocumentTab->setTabLabel(this, Info.fileName());

  DataSet = Info.baseName()+".dat";
  DataDisplay = Info.baseName()+".dpl";
}

// ---------------------------------------------------
void TextDoc::becomeCurrent(bool)
{
  viewport()->setFocus();

  if(undoIsAvailable)  App->undo->setEnabled(true);
  else  App->undo->setEnabled(false);
  if(redoIsAvailable)  App->redo->setEnabled(true);
  else  App->redo->setEnabled(false);
}

// ---------------------------------------------------
void TextDoc::slotSetChanged()
{
  if((!DocChanged) && isModified()) {
    App->DocumentTab->setTabIconSet(this, QPixmap(smallsave_xpm));
    DocChanged = true;
  }
  else if(DocChanged && (!isModified())) {
    App->DocumentTab->setTabIconSet(this, QPixmap(empty_xpm));
    DocChanged = false;
  }
}

// ---------------------------------------------------
bool TextDoc::load()
{
  QFile file(DocName);
  if(!file.open(IO_ReadOnly))
    return false;

  QTextStream stream(&file);
  setText(stream.read());
  setModified(false);
  slotSetChanged();
  file.close();
  return true;
}

// ---------------------------------------------------
int TextDoc::save()
{
  QFile file(DocName);
  if(!file.open(IO_WriteOnly))
    return -1;

  QTextStream stream(&file);
  stream << text();
  setModified(false);
  slotSetChanged();
  file.close();
  return 0;
}

// ---------------------------------------------------
void TextDoc::slotChangeUndo(bool available)
{
  undoIsAvailable = available;
  App->undo->setEnabled(undoIsAvailable);
}

// ---------------------------------------------------
void TextDoc::slotChangeRedo(bool available)
{
  redoIsAvailable = available;
  App->redo->setEnabled(redoIsAvailable);
}

// ---------------------------------------------------
void TextDoc::outcommmentSelected()
{
  int i, paraStart, paraEnd, colStart, colEnd;
  getSelection(&paraStart, &colStart, &paraEnd, &colEnd);

  if(paraStart < 0)  return;  // no selection
  if(colStart > 0)  paraStart++;
  if(colEnd < 1)  paraEnd--;
  if(paraStart > paraEnd)  return;  // no complete line selected
  
  for(i=paraStart; i<=paraEnd; i++)
    if(text(i).left(3) == "-- ") {
      setSelection(i, 0, i, 3);
      removeSelectedText();
    }
    else
      insertAt("-- ", i, 0);
}
