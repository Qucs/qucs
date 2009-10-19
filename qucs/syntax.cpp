/***************************************************************************
                              syntax.cpp
                             ------------
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

// *****************************************************************
// **********                                             **********
// **********  The class that do the syntax highlighting  **********
// **********                                             **********
// *****************************************************************

#include <qsyntaxhighlighter.h>

#include "textdoc.h"
#include "syntax.h"
#include "main.h"

SyntaxHighlighter::SyntaxHighlighter(TextDoc *textEdit) 
                 : QSyntaxHighlighter(textEdit)
{
  Doc = textEdit;
  language = LANG_NONE;
}

SyntaxHighlighter::~SyntaxHighlighter()
{
}

// ---------------------------------------------------
void SyntaxHighlighter::setLanguage(int lang)
{
  language = lang;
}

// ---------------------------------------------------
int SyntaxHighlighter::highlightParagraph(const QString& text, int)
{
  QChar c;
  bool isFloat=false;
  int  iString=-1, iWord=-1, iNumber=-1, iExpo=-1, i=0;
  QFont font = Doc->TextFont;
  font.setPointSize((int)Doc->Scale);
  setFormat(0, text.length(), font, QPen::black);

  for(c = text.at(i); !c.isNull(); c = text.at(++i)) {
    if(iString >= 0) {
      setFormat(iString, i-iString+1, QucsSettings.VHDL_String);
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
      do {
        if(iWord > 0)
          if(text.at(iWord-1) == '\'') {
            markAttribute(text, iWord, i-iWord);
            break;
          }
        markWord(text, iWord, i-iWord);
      } while(false);
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
            setFormat(iNumber, i-iNumber, QucsSettings.VHDL_Real);
          else
            setFormat(iNumber, i-iNumber, QucsSettings.VHDL_Integer);
        }
      iNumber = -1;
    }
    // ----- maybe a comment -------------------------------
    else if(c == '-') {
      if(i > 0)
        if(text.at(i-1) == '-') {  // VHDL comment starts with --
          setFormat(i-1, text.length()-i, QucsSettings.VHDL_Comment);
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
          setFormat(i-2, 3, QucsSettings.VHDL_Character);
    }
    else if(c == '"')
      iString = i;
  }
  return 0;
}


// ---------------------------------------------------
// reserved VHDL words in alphabetical order
typedef const char*  pChar;
typedef const char** ppChar;
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
  {(ppChar)&List_A, (ppChar)&List_B, (ppChar)&List_C, (ppChar)&List_D,
   (ppChar)&List_E, (ppChar)&List_F, (ppChar)&List_G, 0, (ppChar)&List_I,
   0, 0, (ppChar)&List_L, (ppChar)&List_M, (ppChar)&List_N,
   (ppChar)&List_O, (ppChar)&List_P, 0, (ppChar)&List_R, (ppChar)&List_S,
   (ppChar)&List_T, (ppChar)&List_U, (ppChar)&List_V, (ppChar)&List_W,
   (ppChar)&List_X};

pChar List_Units[] = {"fs", "ps", "ns", "us", "ms", "sec", "min", "hr", 0};

pChar List_DataTypes[] = {
   "bit", "bit_vector", "boolean", "std_logic", "std_logic_vector",
   "std_ulogic", "std_ulogic_vector", "signed", "unsigned", "integer",
   "real", "time", "character", "natural", 0};

// ---------------------------------------------------
void SyntaxHighlighter::markWord(const QString& text, int start, int len)
{
  QString Word = text.mid(start, len).lower();
  int idx = (int)(Word.at(0).latin1() - 'a');
  if(idx < 0 || idx > 23)
    return;
  pChar *List = WordList[idx];

  QFont newFont = Doc->TextFont;
  newFont.setPointSize((int)Doc->Scale);

  if(List)
    for( ; *List != 0; List++)
      if(Word == *List) {
        newFont.setWeight(QFont::Bold);
        setFormat(start, len, newFont);
        return;
      }

  for(List = List_DataTypes; *List != 0; List++)
    if(Word == *List) {
      setFormat(start, len, QucsSettings.VHDL_Types);
      return;
    }

  for(List = List_Units; *List != 0; List++)
    if(Word == *List) {
      newFont.setWeight(QFont::Bold);
      setFormat(start, len, newFont, QucsSettings.VHDL_Real);
      return;
    }
}

// ---------------------------------------------------
pChar List_Attrib_A[] = {"active", "ascending", 0};
pChar List_Attrib_B[] = {"base", 0};
pChar List_Attrib_D[] = {"delayed", 0};
pChar List_Attrib_E[] = {"event", 0};
pChar List_Attrib_H[] = {"high", 0};
pChar List_Attrib_I[] = {"image", 0};
pChar List_Attrib_L[] =
   {"last_active", "last_event", "last_value", "left", "leftof", "length", "low", 0};
pChar List_Attrib_P[] = {"pos", "pred", 0};
pChar List_Attrib_Q[] = {"quiet", 0};
pChar List_Attrib_R[] = {"range", "reverse_range", "right", "rightof", 0};
pChar List_Attrib_S[] = {"stable", "succ", 0};
pChar List_Attrib_T[] = {"transaction", 0};
pChar List_Attrib_V[] = {"val", "value", 0};

ppChar Attribute_List[] =
  {(ppChar)&List_Attrib_A, (ppChar)&List_Attrib_B, 0, (ppChar)&List_Attrib_D,
   (ppChar)&List_Attrib_E, 0, 0, (ppChar)&List_Attrib_H, (ppChar)&List_Attrib_I,
   0, 0, (ppChar)&List_Attrib_L, 0, 0, 0, (ppChar)&List_Attrib_P,
   (ppChar)&List_Attrib_Q, (ppChar)&List_Attrib_R, (ppChar)&List_Attrib_S,
   (ppChar)&List_Attrib_T, 0, (ppChar)&List_Attrib_V};

void SyntaxHighlighter::markAttribute(const QString& text, int start, int len)
{
  QString Word = text.mid(start, len).lower();
  int idx = (int)(Word.at(0).latin1() - 'a');
  if(idx < 0 || idx > 22)
    return;
  pChar *List = Attribute_List[idx];

  if(List)
    for(; *List != 0; List++)
      if(Word == *List) {
        setFormat(start-1, len+1, QucsSettings.VHDL_Attributes);
        return;
      }
}
