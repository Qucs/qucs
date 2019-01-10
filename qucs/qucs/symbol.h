/***************************************************************************
                                 symbol.h
                                -----------
    begin                : 2016
    copyright            : (C) 2016 Felix Salfelder
    email                : felix@salfelder.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

/** \file symbol.h
  * \brief Defines symbols for components, to be used in schematics.
  *
  * a symbol mostly takes care of ports, parameters and connectivity.
  * (this is under construction)
  *
  */
#ifndef QUCS_SYMBOL_H
#define QUCS_SYMBOL_H

#include "element.h"

/** \class Symbol
  * \brief Superclass of all circuit components (except wires).
  *
  *
  */

class QPainter;
class Schematic;

// a component symbol. not necessarily one of the legacy components
class Symbol : public Element{
public: // construct
  Symbol() : Element(), containingSchematic(nullptr) {}
  virtual ~Symbol(){}

public: // interface

  // obsolete
  virtual void setSchematic (SchematicModel const* p) { unreachable(); containingSchematic = p; }
  SchematicModel const* getSchematic() const{ unreachable(); return containingSchematic;}

  // use these, TODO: parent.
  void setScope (SchematicModel const* p) { containingSchematic = p; }
  SchematicModel const* getScope() const{return containingSchematic;}


  virtual std::string /* const& */ type() const{
	  return _type;
  }
  void setType(std::string const& x){
	  _type = x;
  }
  virtual void build(){ }

  virtual unsigned portCount() const{ return 0; } // pure?
  virtual Port const* port(unsigned) const{ return nullptr; } // pure?
  virtual Node const& portValue(unsigned) const{ throw "incomplete"; } // pure?
  virtual void setPort(unsigned i, Node* n) = 0;

  virtual unsigned paramCount()const {return 0;}

public: // graphics
        // hmm, maybe just dispatch a gfx object.
		  // leave it like that, for now.
  virtual unsigned height()const {return 0;}
  virtual unsigned width()const {return 0;}
//  virtual void draw(QPainter&)const=0;
  //...  more to come
private: // good idea?
  SchematicModel const* containingSchematic;

private:
  std::string _type;
};


#endif
