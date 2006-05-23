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

#include <qlabel.h>
#include <qaction.h>
#include <qpixmap.h>
#include <qprinter.h>
#include <qfileinfo.h>
#include <qtabwidget.h>
#include <qmessagebox.h>
#include <qpaintdevicemetrics.h>


TextDoc::TextDoc(QucsApp *App_, const QString& Name_) : QucsDoc(App_, Name_)
{
  tmpPosX = tmpPosY = 1;  // set to 1 to trigger line highlighting
  Scale = (float)QucsSettings.font.pointSize();
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
  App->printCursorPosition(x, y);
  tmpPosX = x;
  tmpPosY = y;
}

// ---------------------------------------------------
void TextDoc::slotSetChanged()
{
  if(isModified()) {
    if(!DocChanged) {
      App->DocumentTab->setTabIconSet(this, QPixmap(smallsave_xpm));
      DocChanged = true;
    }
  }
  else if(DocChanged) {
    App->DocumentTab->setTabIconSet(this, QPixmap(empty_xpm));
    DocChanged = false;
  }

  App->undo->setEnabled(isUndoAvailable());
  App->redo->setEnabled(isRedoAvailable());
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
  lastSaved = QDateTime::currentDateTime();
  return 0;
}

// -----------------------------------------------------------
void TextDoc::print(QPrinter *Printer, bool printAll)
{
  QPainter p(Printer);
  if(!p.device())   // valid device available ?
    return;
  p.setFont(QucsSettings.font);

  sync();   // formatting whole text

  QPaintDeviceMetrics metrics(p.device());
  int margin  = 54;    // margin at each side (unit is point)
  int marginX = margin * metrics.logicalDpiX() / 72;
  int marginY = margin * metrics.logicalDpiY() / 72;
  QRect printArea(
          marginX, marginY, metrics.width() - 2*marginX,
          metrics.height() - 2*marginY - p.fontMetrics().lineSpacing());

  int linesPerPage =printArea.height() / p.fontMetrics().lineSpacing();

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
    p.drawText(printArea, 0, printText.section('\n', 0, linesPerPage-1));
    printText = printText.section('\n', linesPerPage);

    s = tr("Page %1 of %2").arg(PageNo).arg(PageCount);
    p.drawText(printArea.right() - p.fontMetrics().width(s),
               printArea.bottom() + p.fontMetrics().lineSpacing(), s);
    if(printText.isEmpty())
      break;
    Printer->newPage();
    PageNo++;
  }
}

// ---------------------------------------------------
float TextDoc::zoom(float s)
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
  Scale = (float)QucsSettings.font.pointSize();
  zoomTo(QucsSettings.font.pointSize());
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



// *****************************************************************
// **********                                             **********
// **********  The class that do the syntax highlighting  **********
// **********                                             **********
// *****************************************************************

SyntaxHighlighter::SyntaxHighlighter(TextDoc *textEdit) 
                 : QSyntaxHighlighter(textEdit)
{
  Doc = textEdit;
}

SyntaxHighlighter::~SyntaxHighlighter()
{
}

// ---------------------------------------------------
int SyntaxHighlighter::highlightParagraph(const QString& text, int)
{
  QChar c;
  bool isFloat=false;
  int  iString=-1, iWord=-1, iNumber=-1, iExpo=-1, i=0;
  QFont font = QucsSettings.font;
  font.setPointSize((int)Doc->Scale);
  setFormat(0, text.length(), font, QPen::black);

  for(c = text.at(i); !c.isNull(); c = text.at(++i)) {
    if(iString >= 0) {
      setFormat(iString, i-iString+1, Qt::red);
      if(c == '"')
        iString = -1;
      continue;
    }
    // ----- word that might become a reserved word --------------
    else if(iWord >= 0) {
      if(c.isLetterOrNumber())
        continue;
      if(c == '_')
        continue;
      markWord(text, iWord, i-iWord);
      iWord = -1;
    }
    // ----- integer or floating point number --------------
    else if(iNumber >= 0) {
      if(c.isNumber())
        continue;
      if(c == '.') {
        if(iExpo < 0) {
          if(isFloat)
            iNumber = -1;
          isFloat = true;
        }
        else
          iNumber = -1;
        continue;
      }
      if((c == 'e') || (c == 'E')) {
        if(iExpo < 0) {
          iExpo = i;
          isFloat = true;
        }
        else
          iNumber = -1;
        continue;
      }
      if((c == '-') || (c == '+')) {
        if((iExpo+1) == i)
          continue;
      }
      if(c != '_')
        if(!c.isLetter()) {
          if(isFloat)
            setFormat(iNumber, i-iNumber, Qt::darkMagenta);
          else
            setFormat(iNumber, i-iNumber, Qt::blue);
        }
      iNumber = -1;
    }
    // ----- maybe a comment -------------------------------
    else if(c == '-') {
      if(i > 0)
        if(text.at(i-1) == '-') {  // VHDL comment starts with --
          setFormat(i-1, text.length()-i, Qt::gray);
          return 0;
        }
      continue;
    }
    // ----- no special syntax yet (or anymore) --------------
    else {
      if(c.isLetter())
        iWord = i;     // start a word
      else if(c.isNumber()) {
        iExpo = -1;
        iNumber = i;   // start a number
        isFloat = false;
        c = text.at(i-1);
/*        if((c == '-') || (c == '+'))  // include sign into number
          iNumber--;*/
      }
    }

    if(c == '\'') {
      if(i > 1)
        if(text.at(i-2) == '\'')
          setFormat(i-2, 3, Qt::magenta);
    }
    else if(c == '"')
      iString = i;
  }
  return 0;
}


// ---------------------------------------------------
// reserved VHDL words in alphabetical order
typedef char*  pChar;
typedef char** ppChar;
pChar List_A[] = {"abs", "access", "after", "alias", "all", "and",
                  "architecture", "array", "assert", "attribute", 0};
pChar List_B[] = {"begin", "block", "body", "buffer", "bus", 0};
pChar List_C[] = {"case", "component", "configuration", "constant", 0};
pChar List_D[] = {"disconnect", "downto", 0};
pChar List_E[] = {"else", "elsif", "end", "entity", "exit", 0};
pChar List_F[] = {"file", "for", "function", 0};
pChar List_G[] = {"generate", "generic", "group", "guarded", 0};
pChar List_I[] = {"if", "impure", "in", "inertial", "inout", "is", 0};
pChar List_L[] = {"label", "library", "linkage", "literal", "loop", 0};
pChar List_M[] = {"map", "mod", 0};
pChar List_N[] = {"nand", "new", "next", "nor", "not", "null", 0};
pChar List_O[] = {"of", "on", "open", "or", "others", "out", 0};
pChar List_P[] = {"package", "port", "postponed", "procedure", "process",
                  "pure", 0};
pChar List_R[] = {"range", "record", "register", "reject", "rem", "report",
                  "return", "rol", "ror", 0};
pChar List_S[] = {"select", "severity", "shared", "signal", "sla", "sll",
                  "sra", "srl", "subtype", 0};
pChar List_T[] = {"then", "to", "transport", "type", 0};
pChar List_U[] = {"unaffected", "units", "until", "use", 0};
pChar List_V[] = {"variable", 0};
pChar List_W[] = {"wait", "when", "while", "with", 0};
pChar List_X[] = {"xnor", "xor", 0};

ppChar WordList[] =
  {(char**)&List_A, (char**)&List_B, (char**)&List_C, (char**)&List_D,
   (char**)&List_E, (char**)&List_F, (char**)&List_G, 0, (char**)&List_I,
   0, 0, (char**)&List_L, (char**)&List_M, (char**)&List_N,
   (char**)&List_O, (char**)&List_P, 0, (char**)&List_R, (char**)&List_S,
   (char**)&List_T, (char**)&List_U, (char**)&List_V, (char**)&List_W,
   (char**)&List_X, 0, 0};

pChar List_Units[] = {"fs", "ps", "ns", "us", "ms", "sec", "min", "hr", 0};

// ---------------------------------------------------
void SyntaxHighlighter::markWord(const QString& text, int start, int len)
{
  QString Word = text.mid(start, len).lower();
  int idx = (int)(Word.at(0).latin1() - 'a');
  pChar *List = (idx >= 0 && idx <= 25) ? WordList[idx] : NULL;

  if(List)
    for( ; *List != 0; List++)
      if(Word == *List) {
        QFont boldFont = QucsSettings.font;
        boldFont.setPointSize((int)Doc->Scale);
        boldFont.setWeight(QFont::Bold);
        setFormat(start, len, boldFont);
        return;
      }

  for(List = List_Units; *List != 0; List++)
    if(Word == *List) {
      QFont boldFont = QucsSettings.font;
      boldFont.setWeight(QFont::Bold);
      boldFont.setPointSize((int)Doc->Scale);
      setFormat(start, len, boldFont, Qt::darkMagenta);
      return;
    }
}
