/***************************************************************************
    copyright            : (C) 2018, 2020 Felix Salfelder
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
/* -------------------------------------------------------------------------------- */
#include "platform.h"
#include "element.h"
#include "exception.h"
#include "painting.h"
class ComponentDialog;//really??
class Property;

// BUG. obsolete paintings
class Line;
class Text;
class Area;
class Arc;
class QString; // BUG
class QPen; // BUG
class QBrush; // BUG
/* -------------------------------------------------------------------------------- */
class TaskElement : public Element, public Painting {
protected:
  TaskElement(const TaskElement&);
  explicit TaskElement();
public:
  virtual ~TaskElement() {};

  virtual void recreate(SchematicDoc*) {};
  // QString getNetlist();

private:
  virtual rect_t bounding_rect() const override;
  void    paint(ViewPainter*) const;
  QDialog* schematicWidget(QucsDoc* Doc) const;

public:
  void    paintScheme(SchematicDoc*) const;
  void    print(ViewPainter*, float);
//  void    setCenter(int, int, bool relative=false);
  void    getCenter(int&, int&);
  void    entireBounds(int&, int&, int&, int&, float);
  int     getTextSelected(int, int, float);
  bool    load(const QString&);

  // set the pointer scematic associated with the component
  // do somehting with buttons. can sb think of a more descriptive name?
  virtual void dialgButtStuff(ComponentDialog&)const;

  void setTypeName(std::string const&){ incomplete(); }
  std::string typeName() const{ return "taskNameIncomplete"; }
  /// QList<Line *>     Lines;
  /// QList<struct Arc *>      Arcs;
  /// QList<Area *>     Rects;
  /// QList<Area *>     Ellips;
  /// QList<Port *>     Ports;
  /// QList<Text *>     Texts;
  // mutable /*BUG*/ Q3PtrList<Property> Props;

  // BUG: abused in some display function
  mutable int  tx, ty;   // upper left corner of text (position)
  int  x2, y2;

  bool showName;

public:
	virtual index_t param_count() const { return 0; }
	virtual std::string param_name(index_t) const{
		throw qucs::ExceptionCantFind();
	}
	virtual std::string param_value(index_t) const {
		throw qucs::ExceptionCantFind();
	}
	virtual std::string param_value_by_name(std::string const& key) const;

private:
  virtual SchematicModel* scope() override;

protected:
  virtual QString netlist();

//  int  analyseLine(const QString&, int);
  bool getIntegers(const QString&, int *i1=0, int *i2=0, int *i3=0,
                   int *i4=0, int *i5=0, int *i6=0);
  bool getPen(const QString&, QPen&, int);
  bool getBrush(const QString&, QBrush&, int);

  // void copyComponent(Component*);
  // Property * getProperty(const QString&);
  // SchematicDoc* containingSchematic;
};
/* -------------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------------- */
#endif
