/***************************************************************************
    copyright            : (C) 2015, 2020, 2021 Felix Salfelder
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

// language base class, almost grown to u_lang.h

#ifndef QUCS_LANGUAGE_H
#define QUCS_LANGUAGE_H
/* -------------------------------------------------------------------------------- */
#include "object.h"
#include <assert.h>
/* -------------------------------------------------------------------------------- */
#define DEV_DOT Dot
// include u_lang.h // TODO
//class Command;
class DEV_DOT;
class Element;
class istream_t;
class Painting;
class SchematicModel;
class SubcktBase;
class Symbol;
class Diagram;
class TaskElement;
class ostream_t;

/* -------------------------------------------------------------------------------- */
// baseclass for schematic and net languages.
// use u_lang.h instead?
class DocumentLanguage : public Object{
protected:
        DocumentLanguage() : Object(){}
public:
	virtual ~DocumentLanguage() {}

	virtual void	parse_top_item(istream_t&, SchematicModel*) const {incomplete();}
	virtual Element* parseItem(istream_t&, Element*) const;
	virtual SubcktBase* parse_model(istream_t&, SubcktBase*) const {incomplete(); assert(false); return nullptr;} // == 0
	virtual DEV_DOT* parseCommand(istream_t&, DEV_DOT*) const{
	  untested(); return nullptr;
	}
  virtual std::string find_type_in_string(istream_t&) const = 0;
  void new__instance(istream_t& cmd, Element* owner,
                     SchematicModel* Scope) const;
  const Element* find_proto(const std::string&, const Element*) const;

public:
  virtual void printItem(ostream_t&, Element const*) const;
private: //called by printItem
  virtual void printPainting(Painting const*, ostream_t&) const = 0;
  virtual void printDiagram(Diagram const*, ostream_t&) const = 0;
  virtual void printSymbol(Symbol const*, ostream_t&) const = 0;
  virtual void printSubckt(SubcktBase const*, ostream_t&) const = 0;
  virtual void printElement(Element const*, ostream_t&) const = 0;
//  virtual void print_comment(ostream_t&, const DEV_COMMENT const*){incomplete();}
  virtual void print_command(ostream_t&, const DEV_DOT*) const{incomplete();}
};
/* -------------------------------------------------------------------------------- */
// group netlist languages. needed? obsolete?
class NetLang : public DocumentLanguage {
public:
  virtual ~NetLang(){}
};
/* -------------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------------- */
#endif
