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
#ifndef SYMBOL_H
#define SYMBOL_H

#include "element.h"

/** \class Symbol
  * \brief Superclass of all circuit components (except wires).
  *
  *
  */

class QPainter;

// a component symbol. not necessarily one of the legacy components
class Symbol : public Element{
public: // construct
  Symbol() {}
  virtual ~Symbol(){}

public: // interface
  virtual unsigned portNumber()const {return 0;}
  virtual void setSchematic (Schematic* p) { containingSchematic = p; }
  Schematic* getSchematic() const{return containingSchematic;}

public: // graphics
        // hmm, maybe just dispatch a gfx object.
		  // leave it like that, for now.
  virtual unsigned height()const {return 0;}
  virtual unsigned width()const {return 0;}
//  virtual void draw(QPainter&)const=0;
  //...  more to come
private:
  Schematic* containingSchematic;
};


#endif
