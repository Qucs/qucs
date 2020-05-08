/***************************************************************************
                                 symbol.h
                                -----------
    copyright            : (C) 2016, 2019 Felix Salfelder
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
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
//class Schematic;
class NodeMap;

// a component symbol. not necessarily one of the legacy components
class Symbol : public Element{
public: // construct
  Symbol() : Element(), containingSchematic(nullptr) {}
  virtual ~Symbol();

private: // Element
	void paint(ViewPainter*) const{unreachable();}	

public: // interface

  // obsolete
  virtual void setSchematic (SchematicModel const* p) { unreachable(); containingSchematic = p; }
  SchematicModel const* getSchematic() const{ unreachable(); return containingSchematic;}

  // use these, TODO: parent.
  void setScope (SchematicModel const* p) { containingSchematic = p; }
  SchematicModel const* getScope() const{return containingSchematic;}

  virtual void recreate();

  virtual std::string /* const& */ type() const{
	  return _type;
  }
  void setType(std::string const& x){
	  _type = x;
  }
  virtual void build(){ }

  virtual unsigned paramCount()const {return 0;}


public: // non-virtual (on purpose)
  QString const& netLabel(unsigned i) const;

public: // Node stuff
  virtual Node* connectNode(unsigned idx, NodeMap&);
  virtual Node* disconnectNode(unsigned idx, NodeMap&);

public: // Port access
  QString const& portValue(unsigned) const;
  // TODO: rethink Port/Node semantics
  virtual unsigned portCount() const = 0;
  Port const& port(unsigned) const;

private: // internal port access
  virtual Port& port(unsigned){ unreachable(); return *new Port(0,0);}

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
