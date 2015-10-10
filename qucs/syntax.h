/***************************************************************************
                                syntax.h
                               ----------
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

#ifndef SYNTAX_H
#define SYNTAX_H

#include <QtCore>
#include <QtGui>

#include "textdoc.h"
//#include "main.h"

enum language_type {
  LANG_NONE = 0,
  LANG_VHDL,
  LANG_VERILOG,
  LANG_VERILOGA,
  LANG_OCTAVE,
};

enum textstate_type {
  STATE_NONE = 0,
  STATE_COMMENT = 100,
};

class SyntaxHighlighter : public QSyntaxHighlighter {
public:
 SyntaxHighlighter(TextDoc*);
 virtual ~SyntaxHighlighter();

 void setLanguage(int);
 void highlightBlock(const QString&);

private:
  int language;
  TextDoc *Doc;

  struct HighlightingRule
     {
         QRegExp pattern;
         QTextCharFormat format;
     };
  
  QVector<HighlightingRule> highlightingRules;

  QTextCharFormat reservedWordFormat;
  QTextCharFormat unitFormat;
  QTextCharFormat datatypeFormat;
  QTextCharFormat directiveFormat;
  QTextCharFormat functionFormat;
  QTextCharFormat commentFormat;

};

#endif
