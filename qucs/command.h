/***************************************************************************
                               command.h
                              -------------
    copyright            : (C) 2018 Felix Salfelder
    email                : yes
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef QUCS_COMMMAND_H
#define QUCS_COMMMAND_H

#include "components/component.h"

/*!
 * this is a bug. commands are not components, maybe elements?
 * decide later. we only need this base class.
 */
// need a baseclass for commands.
// formerly, they were components, but that does not make sense.
// (a command does not have ports etc.)
class Command : public Component /*BUG*/ {
public:
  Command();
  virtual ~Command() {};

//  virtual Object* newOne(); from Symbol.
  virtual void recreate(Schematic*) {};
  QString getNetlist();
  QString get_VHDL_Code(int);
  QString get_Verilog_Code(int);
private:
  Component* newOne(){ unreachable();
	  return nullptr;
  }
  void    paint(ViewPainter*);
public:
  void    paintScheme(Schematic*) const;
  void    print(ViewPainter*, float);
  void    setCenter(int, int, bool relative=false);
  void    getCenter(int&, int&);
  int     textSize(int&, int&);
  void    Bounding(int&, int&, int&, int&);
  void    entireBounds(int&, int&, int&, int&, float);
  bool    getSelected(int, int);
  int     getTextSelected(int, int, float);
  void    rotate();
  void    mirrorX();  // mirror about X axis
  void    mirrorY();  // mirror about Y axis
  bool    load(const QString&);

  // to hold track of the component appearance for saving and copying
  bool mirroredX;   // is it mirrored about X axis or not
  int  rotated;     // rotation angle divided by 90 degrees

  virtual QString getSubcircuitFile() { return ""; }
  // set the pointer scematic associated with the component
  // do somehting with buttons. can sb think of a more descriptive name?
  virtual void dialgButtStuff(ComponentDialog&)const;

  QList<Line *>     Lines;
  QList<struct Arc *>      Arcs;
  QList<Area *>     Rects;
  QList<Area *>     Ellips;
  QList<Port *>     Ports;
  QList<Text *>     Texts;
  mutable /*BUG*/ Q3PtrList<Property> Props;

  #define COMP_IS_OPEN    0
  #define COMP_IS_ACTIVE  1
  #define COMP_IS_SHORTEN 2
  int  isActive; // should it be used in simulation or not ?
  // BUG: abused in some display function
  mutable int  tx, ty;   // upper left corner of text (position)
  bool showName;
  QString  Name;
  QString  Description;

protected:
public: // HACK
  void obsolete_name_override_hack(QString x){
	  Name = x;
  }

protected:
  virtual QString netlist();
  virtual QString vhdlCode(int);
  virtual QString verilogCode(int);

  int  analyseLine(const QString&, int);
  bool getIntegers(const QString&, int *i1=0, int *i2=0, int *i3=0,
                   int *i4=0, int *i5=0, int *i6=0);
  bool getPen(const QString&, QPen&, int);
  bool getBrush(const QString&, QBrush&, int);

  void copyComponent(Component*);
  Property * getProperty(const QString&);
  Schematic* containingSchematic;
};


#endif
