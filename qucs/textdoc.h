/***************************************************************************
                                textdoc.h
                               -----------
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

#ifndef TEXTDOC_H
#define TEXTDOC_H

#include "qucsdoc.h"

#include <qtextedit.h>
#include <qsyntaxhighlighter.h>

class SyntaxHighlighter;

extern const char *smallsave_xpm[];// icon for unsaved files (diskette)
extern const char *empty_xpm[];    // provides same height than "smallsave_xpm"


class TextDoc : public QTextEdit, public QucsDoc {
  Q_OBJECT
public: 
  TextDoc(QucsApp*, const QString&);
 ~TextDoc();

  void  setName(const QString&);
  bool  load();
  int   save();
  void  print(QPrinter*, QPainter*, bool, bool);
  float zoomBy(float);
  void  showAll();
  void  showNoZoom();
  void  becomeCurrent(bool);
  bool  loadSimulationTime(QString&);
  void  outcommmentSelected();

public slots:
  void slotCursorPosChanged(int, int);
  void slotSetChanged();

private:
  SyntaxHighlighter *syntaxHighlight;
};


class SyntaxHighlighter : public QSyntaxHighlighter {
public:
 SyntaxHighlighter(TextDoc*);
 virtual ~SyntaxHighlighter();

 int highlightParagraph(const QString&, int);

private:
  TextDoc *Doc;
  void markWord(const QString&, int, int);
  void markAttribute(const QString&, int, int);
};

#endif
