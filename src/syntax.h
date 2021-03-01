/***************************************************************************
    copyright            : (C) 2006 by Michael Margraf
    email                : michael.margraf@alumni.tu-berlin.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef QUCS_SYNTAX_H
#define QUCS_SYNTAX_H

#include <QSyntaxHighlighter>
#include "qucsdoc.h" // to textdoc??

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
 SyntaxHighlighter(qucs::Doc*);
 virtual ~SyntaxHighlighter();

 void setLanguage(int);
 void highlightBlock(const QString&);

private:
  int language;
  qucs::Doc *Doc;

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
