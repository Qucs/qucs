/***************************************************************************
                               libcomp.h
                              ------------------
    begin                : yes
    copyright            : Qucs Developers
    authors              : Andres Martinez, Felix Salfelder
    email                : yes, feel free
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#ifndef LIBCOMP_H
#define LIBCOMP_H

#include "symbol.h"
#include "qucslib_common.h"
#include <QFontMetrics>//Compute text size
#include <QDebug>
#include <assert.h>

#include "symbolwidget.h" // Line etc.
/*!
 * \brief this library provides symbols. these.
 */

struct Line;
struct Arc;
struct Area;
struct Text;


class QucsLibComponent : public Symbol{
private:
  QString SymbolString, LibraryName, ComponentName;
public:
  QucsLibComponent( QString& , const QString&, const QString& );
  void draw(QWidget&) const;
public:
  void init(const QString& Lib_, const QString& Comp_); // help construction
public: // symbol interface overloads
  Symbol* newOne() const { return new QucsLibComponent(*this); }
  unsigned portNumber() const{return PortNumber;}
  unsigned width() const
  {
    assert(x2>=x1);
    return x2-x1;
  }
  unsigned height() const
  {
    assert(y2>=y1);
    return y2-y1;
  }
private: // implementation
  int  analyseLine(const QString&);
  bool getPen  (const QString&, QPen&, int);
  bool getBrush(const QString&, QBrush&, int);
private: // component specifics.
  mutable QString ModelString; // BUG: must not be mutable. construct properly
  QString VerilogModelString;
  QString VHDLModelString;
  QString Prefix;
  int Text_x, Text_y;
private://Text dimensions
  // int TextWidth;
  // int TextHeight;
  // int DragNDropWidth;

private: // gfx data
  int cx, cy, x1, x2, y1, y2;
  QList<Line *> Lines;
  QList<Arc *> Arcs;
  QList<Area *> Rects, Ellips;
  QList<Text *>  Texts;
  unsigned PortNumber;
public: // bogus obsolete XML creation.
        // don't use in new code... DONT.
  QString theModel() const{
  // single component line
  if(!ModelString.isEmpty())
    if(ModelString.count('\n') < 2)
      return ModelString.remove('\n');

  // real library component
  return "<Lib " + Prefix + " 1 0 0 " +
         QString::number(Text_x) + " " +
         QString::number(Text_y) + " 0 0 \"" +
         LibraryName + "\" 0 \"" + ComponentName + "\" 0>";
  }
public: // something
  void AdjustWidgetSize(QWidget& w) const;
public:  // more random access to private members (transitional?)
  QString modelString() const{ return ModelString;}
  QString verilogModelString() const{ return VerilogModelString;}
  QString vHDLModelString() const { return VHDLModelString;}
};

extern tQucsSettings QucsSettings;

inline QucsLibComponent::QucsLibComponent( QString& SymbolString_,
                            const QString& Lib_, const QString& Comp_)
	: SymbolString(SymbolString_), LibraryName(Lib_), ComponentName(Comp_),
  cx(0), cy(0), x1(0), x2(0), y1(0), y2(0)
{
  init(Lib_, Comp_);
  if (SymbolString.isEmpty())//Whenever SymbolString is empty, it tries to load the default symbol
  {
      //Load the default symbol for the current Qucs library
      ComponentLibrary parsedlib;
      QString libpath = QucsSettings.LibDir + "/" + Lib_ + ".lib";
      int result = parseComponentLibrary (libpath, parsedlib);

		// BUG: throw if there is an error. don't randomly spawn Messageboxes
		// (cleanup later)
      switch (result)//Check if the library was properly loaded
      {
        case QUCS_COMP_LIB_IO_ERROR:
            QMessageBox::critical(0, "Error", QString("Cannot open \"%1\".").arg (libpath));
            return;
        case QUCS_COMP_LIB_CORRUPT:
            QMessageBox::critical(0, "Error", "Library is corrupt.");
            return;
        default:
            break;
      }

    // copy the contents of default symbol section to a string
    SymbolString = parsedlib.defaultSymbol;
  }
  {

  QString Line;
  ///QString foo = SymbolString;
  QTextStream stream(&SymbolString, QIODevice::ReadOnly);

//  x1 = y1 = INT_MAX; ?!
//  x2 = y2 = INT_MIN; ?!


  int z=0, Result;

  int error=0; // BUG: that's what enums are for
               // BUG2: throw in case of input errors! (not yet)
  while(!stream.atEnd()) {
    Line = stream.readLine();
    Line = Line.trimmed();
    if(Line.isEmpty()) continue;

    if(Line.at(0) != '<'){
		 // missing for start char
		 error=-1;
		 break;
	 }
    if(Line.at(Line.length()-1) != '>'){
		 // missing for end char
		 error=-1;
		 break;
	 }
    Line = Line.mid(1, Line.length()-2); // cut off start and end character
    Result = analyseLine(Line);
    if(Result < 0){
		 // line format error
		 error=-6;
		 break;
	 }
    z += Result;
  }

  if(error){
//	  library is broken. what next?
  }

  x1 -= 4;   // enlarge component boundings a little
  x2 += 4;
  y1 -= 4;
  y2 += 4;
  cx  = -x1; //  + TextWidth;
  cy  = -y1;
//  qDebug() << "created symbol x1: " << x1 << "x2 " << x2 << "  Textw:" << TextWidth;
  qDebug() << "c" << cx << "   " << cy;
  }
}




#endif
// vim:ts=8:sw=2:noet
