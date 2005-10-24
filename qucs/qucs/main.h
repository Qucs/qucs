/***************************************************************************
                          main.h  -  description
                             -------------------
    begin                : Mon May 24  2004
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

#ifndef MAIN_H
#define MAIN_H

#include <qfont.h>
#include <qcolor.h>
#include <qptrlist.h>

#include "wire.h"
#include "node.h"
#include "diagrams/diagram.h"

class QucsApp;
class Component;

// constants may be missing on windows systems
#include <math.h>
#ifndef M_PI
#define M_PI     3.1415926535897932384626433832795029
#endif

struct tQucsSettings {
  int x, y, dx, dy;    // position and size of main window
  QFont font;
  float largeFontSize;
  QColor BGColor;      // background color of view area

  unsigned int maxUndo;    // size of undo stack
  QString Editor;
  QString BinDir;
  QString BitmapDir;
  QString LangDir;
  QString LibDir;
};

extern tQucsSettings QucsSettings;  // extern because nearly everywhere used
extern QFont savingFont;       // to remember which font to save in "qucsrc"

extern QucsApp *QucsMain;    // the Qucs application itself

bool saveApplSettings(QucsApp*);

QString complexRect(double, double, int Precision=3);
QString complexDeg (double, double, int Precision=3);
QString complexRad (double, double, int Precision=3);
QString StringNum  (double, char form='g', int Precision=3);
void    str2num    (const QString&, double&, QString&, double&);
QString num2str    (double);
QString StringNiceNum(double);
void    convert2Unicode(QString&);
QString properName (const QString&);

// just dummies for empty lists
extern QPtrList<Wire>      SymbolWires;
extern QPtrList<Node>      SymbolNodes;
extern QPtrList<Diagram>   SymbolDiags;
extern QPtrList<Component> SymbolComps;

#endif

