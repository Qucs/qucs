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

#include <qlabel.h>
#include <qaction.h>
#include <qpixmap.h>
#include <qprinter.h>
#include <qfileinfo.h>
#include <qtabwidget.h>
#include <qmessagebox.h>
#include <qpaintdevicemetrics.h>
#include <qfont.h>
#include <qpopupmenu.h>
#include <qsyntaxhighlighter.h>

#include "textdoc.h"
#include "syntax.h"
#include "main.h"
#include "qucs.h"

TextDoc::TextDoc(QucsApp *App_, const QString& Name_) : QucsDoc(App_, Name_)
{
  TextFont = QFont("Courier New");
  TextFont.setPointSize(QucsSettings.font.pointSize()-1);
  TextFont.setStyleHint(QFont::Courier);
  TextFont.setFixedPitch(true);
  setFont(TextFont);
  setCurrentFont(TextFont);

  simulation = true;
  Library = "";
  Libraries = "";
  SetChanged = false;

  tmpPosX = tmpPosY = 1;  // set to 1 to trigger line highlighting
  Scale = (float)TextFont.pointSize();
  setUndoDepth(QucsSettings.maxUndo);

  QFileInfo Info(Name_);
  if(App) {
    if(Name_.isEmpty())
      App->DocumentTab->addTab(this, QPixmap(empty_xpm),
                            QObject::tr("untitled"));
    else
      App->DocumentTab->addTab(this, QPixmap(empty_xpm),
                            Info.fileName());
    // calls indirectly "becomeCurrent"
    App->DocumentTab->setCurrentPage(App->DocumentTab->indexOf(this));
    viewport()->setFocus();

    setWordWrap(QTextEdit::NoWrap);
    setPaletteBackgroundColor(QucsSettings.BGColor);
    connect(this, SIGNAL(textChanged()), SLOT(slotSetChanged()));
    connect(this, SIGNAL(cursorPositionChanged(int, int)),
            SLOT(slotCursorPosChanged(int, int)));

    syntaxHighlight = new SyntaxHighlighter(this);
  }
}

TextDoc::~TextDoc()
{
  if(App) {
    delete syntaxHighlight;
    App->DocumentTab->removePage(this);  // delete tab in TabBar
  }
}

// ---------------------------------------------------
bool TextDoc::saveSettings(void)
{
  QFile file(DocName + ".cfg");
  if(!file.open(IO_WriteOnly))
    return false;

  QTextStream stream(&file);

  stream << "VHDL settings file, Qucs " PACKAGE_VERSION "\n"
	 << "Simulation=" << simulation << "\n"
	 << "Duration=" << SimTime << "\n"
	 << "Module=" << (!simulation) << "\n"
	 << "Library=" << Library << "\n"
	 << "Libraries=" << Libraries << "\n";

  file.close();
  SetChanged = false;
  return true;
}

// ---------------------------------------------------
bool TextDoc::loadSettings(void)
{
  QFile file(DocName + ".cfg");
  if(!file.open(IO_ReadOnly))
    return false;

  QTextStream stream(&file);
  QString Line, Setting;

  bool ok;
  while(!stream.atEnd()) {
    Line = stream.readLine();
    Setting = Line.section('=',0,0);
    Line    = Line.section('=',1).stripWhiteSpace();
    if(Setting == "Simulation") {
      simulation = Line.toInt(&ok);
    } else if(Setting == "Duration") {
      SimTime = Line;
    } else if(Setting == "Module") {
    } else if(Setting == "Library") {
      Library = Line;
    } else if(Setting == "Libraries") {
      Libraries = Line;
    }
  }

  file.close();
  return true;
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
  int x, y;
  getCursorPosition(&x, &y);
  slotCursorPosChanged(x, y);
  viewport()->setFocus();

  if(isUndoAvailable())  App->undo->setEnabled(true);
  else  App->undo->setEnabled(false);
  if(isRedoAvailable())  App->redo->setEnabled(true);
  else  App->redo->setEnabled(false);

  // update appropriate menu entry
  App->symEdit->setMenuText(tr("Edit Text Symbol"));
  App->symEdit->setStatusTip(tr("Edits the symbol for this text document"));
  App->symEdit->setWhatsThis(
	tr("Edit Text Symbol\n\nEdits the symbol for this text document"));
}

// ---------------------------------------------------
void TextDoc::slotCursorPosChanged(int x, int y)
{
  if(tmpPosX > x)
    clearParagraphBackground(tmpPosX);
  else
    for(int z=tmpPosX; z<x; z++)
      clearParagraphBackground(z);
  if(tmpPosX != x)
    setParagraphBackgroundColor(x, QColor(240, 240, 255));
  App->printCursorPosition(x+1, y+1);
  tmpPosX = x;
  tmpPosY = y;
}

// ---------------------------------------------------
void TextDoc::slotSetChanged()
{
  if((isModified() && !DocChanged) || SetChanged) {
    App->DocumentTab->setTabIconSet(this, QPixmap(smallsave_xpm));
    DocChanged = true;
  }
  else if((!isModified() && DocChanged)) {
    App->DocumentTab->setTabIconSet(this, QPixmap(empty_xpm));
    DocChanged = false;
  }

  App->undo->setEnabled(isUndoAvailable());
  App->redo->setEnabled(isRedoAvailable());
}

// ---------------------------------------------------
QPopupMenu *TextDoc::createPopupMenu( const QPoint &pos )
{
   QPopupMenu *popup = QTextEdit::createPopupMenu( pos );
   App->fileSettings->addTo(popup);
   return popup;
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
  lastSaved = QDateTime::currentDateTime();

  loadSettings();
  SimOpenDpl = simulation ? true : false;
  return true;
}

// ---------------------------------------------------
int TextDoc::save()
{
  saveSettings();

  QFile file(DocName);
  if(!file.open(IO_WriteOnly))
    return -1;

  QTextStream stream(&file);
  stream << text();
  setModified(false);
  slotSetChanged();
  file.close();

  QFileInfo Info(DocName);
  lastSaved = Info.lastModified();

  return 0;
}

// -----------------------------------------------------------
void TextDoc::print(QPrinter *Printer, QPainter *Painter, bool printAll, bool)
{
  Painter->setFont(TextFont);

  sync();   // formatting whole text

  QPaintDeviceMetrics smetrics(QPainter(this).device());
  QPaintDeviceMetrics pmetrics(Painter->device());
  int margin  = 54;    // margin at each side (unit is point)
  int marginX = margin * pmetrics.logicalDpiX() / smetrics.logicalDpiX();
  int marginY = margin * pmetrics.logicalDpiY() / smetrics.logicalDpiY();
  QRect printArea(
        marginX, marginY, pmetrics.width() - 2*marginX,
        pmetrics.height() - 2*marginY - Painter->fontMetrics().lineSpacing());

  int linesPerPage = printArea.height() / Painter->fontMetrics().lineSpacing();

  int PageCount, PageNo = 1;
  QString s, printText;
  if(printAll) {
    printText = text();
    PageCount = paragraphs();
  }
  else {
    int line1, col1, line2, col2;
    printText = selectedText();
    getSelection(&line1, &col1, &line2, &col2);
    if(line1 < 0) {
      QMessageBox::critical(this, tr("Error"), tr("There is no selection!"));
      return;
    }
    PageCount = line2 - line1 + 1;
  }
  PageCount = (PageCount + linesPerPage - 1) / linesPerPage;
  
  for(;;) {
    if(Printer->aborted())
      break;
    Painter->drawText(printArea, 0, printText.section('\n', 0, linesPerPage-1));
    printText = printText.section('\n', linesPerPage);

    s = tr("Page %1 of %2").arg(PageNo).arg(PageCount);
    Painter->drawText(printArea.right() - Painter->fontMetrics().width(s),
               printArea.bottom() + Painter->fontMetrics().lineSpacing(), s);
    if(printText.isEmpty())
      break;
    Printer->newPage();
    PageNo++;
  }
}

// ---------------------------------------------------
float TextDoc::zoomBy(float s)
{
  if(s < 1.0f) s = -1.0f/s;
  Scale += s;
  if(Scale > 40.0f) Scale = 40.0f;
  if(Scale <  4.0f) Scale =  4.0f;
  zoomTo((int)(Scale+0.5f));
  return Scale;
}

// ---------------------------------------------------
void TextDoc::showAll()
{
  sync();
  Scale *= float(visibleHeight()) / float(contentsHeight());
  if(Scale > 40.0f) Scale = 40.0f;
  if(Scale <  4.0f) Scale =  4.0f;
  zoomTo((int)(Scale+0.5f));
}

// ---------------------------------------------------
void TextDoc::showNoZoom()
{
  Scale = (float)TextFont.pointSize();
  zoomTo(TextFont.pointSize());
}

// ---------------------------------------------------
bool TextDoc::loadSimulationTime(QString& Time)
{
  if(!SimTime.isEmpty()) {
    Time = SimTime;
    return true;
  }

  return false;
}

// ---------------------------------------------------
void TextDoc::outcommmentSelected()
{
  QString s = selectedText();
  if(s.isEmpty())
    return;

  if(s.left(2) == "--")
    s.remove(0, 2);
  else
    s = "--" + s;

  for(int i=s.length()-2; i>=0; i--)
    if(s.at(i) == '\n') {
      if(s.mid(i+1, 2) == "--")
        s.remove(i+1, 2);
      else
        s.insert(i+1, "--");
    }
  insert(s);
}
