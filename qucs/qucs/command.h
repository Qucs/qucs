/***************************************************************************
    copyright            : (C) 2018 Felix Salfelder
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef QUCS_COMMMAND_H
#define QUCS_COMMMAND_H

#include "components/component.h"
#include "platform.h"

class CmdElement : public Element {
protected:
  CmdElement(const CmdElement&);
  explicit CmdElement();
public:
  virtual ~CmdElement() {};

//  virtual Object* newOne(); from Symbol.
  virtual void recreate(SchematicDoc*) {};
  QString getNetlist();
private:
  Component* newOne(){ unreachable();
	  Element* hack=clone();
	  Component* e=prechecked_cast<Component*>(hack);
	  assert(e);
	  return e;
  }
  void    paint(ViewPainter*) const;
  QDialog* schematicWidget(QucsDoc* Doc) const;
public:
  void    paintScheme(SchematicDoc*) const;
  void    print(ViewPainter*, float);
  void    setCenter(int, int, bool relative=false);
  void    getCenter(int&, int&);
  int     textSize(int&, int&);
  void    entireBounds(int&, int&, int&, int&, float);
  int     getTextSelected(int, int, float);
  bool    load(const QString&);

//  QString const& type() const{ // HACK
//	  return name();
//  }

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

public:
  QString typeName() const{ return Name;}
  void setTypeName(QString const& s){
	  Name = s;
  }

private:
  virtual SchematicModel* scope() override;

protected:
  virtual QString netlist();

  int  analyseLine(const QString&, int);
  bool getIntegers(const QString&, int *i1=0, int *i2=0, int *i3=0,
                   int *i4=0, int *i5=0, int *i6=0);
  bool getPen(const QString&, QPen&, int);
  bool getBrush(const QString&, QBrush&, int);

  void copyComponent(Component*);
  Property * getProperty(const QString&);
  SchematicDoc* containingSchematic;
};


#endif
