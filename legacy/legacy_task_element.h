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
#ifndef QUCS_LTE_H
#define QUCS_LTE_H
/* -------------------------------------------------------------------------------- */
#include "platform.h"
#include "task_element.h"
#include "qt_compat.h"
class ComponentDialog;//really??
class Property;
/* -------------------------------------------------------------------------------- */
// obsolete paintings and stuff
class Line;
class Text;
class Area;
class Arc;
class QString;
class QPen;
class QBrush;
class Schematic; // obsolete.
/* -------------------------------------------------------------------------------- */
class LegacyTaskElement : public TaskElement {
protected:
  LegacyTaskElement(const LegacyTaskElement&);
  explicit LegacyTaskElement();
public:
  virtual ~LegacyTaskElement() {};

//  virtual void recreate(SchematicDoc*) {};
  // QString getNetlist();

private:
	void paint(ViewPainter*) const;
	rect_t bounding_rect() const override;
	Widget* schematicWidget(QucsDoc* Doc) const;

	virtual index_t param_count() const;
	virtual std::string param_name(index_t n) const;
public: // callback hack.
	std::string param_value(index_t n) const override;
//	std::string param_value_by_name(std::string const& n) const override;

public: // legacy stuff.
  bool getPen(const QString&, QPen&, int);
  bool getBrush(const QString&, QBrush&, int);

  int     textSize(int&, int&);
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

//  QString  Name;
//  QString  Description;

  virtual std::string typeName() const{ return "taskNameIncomplete"; }
	// virtual std::string paramValue(std::string const& n) const;

protected:
//  virtual QString netlist();

//  int  analyseLine(const QString&, int);
//  bool getIntegers(const QString&, int *i1=0, int *i2=0, int *i3=0, int *i4=0, int *i5=0, int *i6=0);
//  bool getPen(const QString&, QPen&, int);
//  bool getBrush(const QString&, QBrush&, int);

  // void copyComponent(Component*);
  // Property * getProperty(const QString&);
  // SchematicDoc* containingSchematic;
private:
  mutable QRect _bb; // HACK
};
/* -------------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------------- */
#endif
