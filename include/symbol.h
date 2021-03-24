/***************************************************************************
    copyright            : (C) 2016, 2019, 2020 Felix Salfelder
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
// Base class for symbols, i.e. drawings that represent components

#ifndef QUCS_SYMBOL_H
#define QUCS_SYMBOL_H
/*--------------------------------------------------------------------------*/
#include "element.h"
#include "component.h"
#include "exception.h"
#include "geometry.h"
#include "exception.h"
#include "painting.h"
/*--------------------------------------------------------------------------*/
namespace qucs{
/*--------------------------------------------------------------------------*/
class ElementList;
class Net;
class NodeMap;
class Symbol;
class Widget;
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
class Symbol : public Component, public Painting {
protected:
	explicit Symbol(Symbol const&);
public: // construct
	explicit Symbol();
	virtual ~Symbol();

protected: // Element
	Widget* schematicWidget(Doc*) const override;

public: // Element
	pos_t center()const override;

public:
	// what is this?
	//  TODO: use prepare == precalc_first everywhere.
	// some kind of "init"??!
	virtual void recreate(); // ElementList const& ctx);
	virtual void build() {} // what does it do?

public: // TODO. something like this.
	virtual void expand() { untested(); }
	virtual void unExpand() { untested(); }
	// TODO: clarify relation to "recreate"...
	// virtual void prepare();
	// virtual void unPrepare();
	//
public:
//	ElementList const* scope() const;
	virtual ElementList const* symbolPaintings() const;

public: // Parameters
	virtual index_t param_count()const override;

	virtual void set_param_by_name(std::string const& name, std::string const& value) override;
	virtual void set_param_by_index(index_t i, std::string const&) override;

	virtual std::string param_value_by_name(std::string const&s) const override;
	virtual std::string param_value(index_t i) const override;
	virtual std::string param_name(index_t i) const override;

public: // Port stuff
	pos_t nodePosition(unsigned) const;
	Node const* portNode(unsigned) const;

public: // Painting
	rect_t bounding_rect() const override;
	void paint(ViewPainter*) const override;

public: // graphics
	// hmm, maybe just dispatch a gfx object.
	// leave it like that, for now.
	virtual unsigned height()const {return 0;}
	virtual unsigned width()const {return 0;}
	//  virtual void draw(QPainter&)const=0;
	//...  more to come
protected: // maybe not here. but need to rebase MultiViewComponent to ScktProto first.
	// such as subckt components. meta data or symbol gfx
	int vflip() const{return _vflip;}
	int hflip() const{return _hflip;}
	int angle() const{return _angle;}
	void setAngle(int a){_angle = a;}

private:
	int _vflip; // vscale??
	int _hflip; // hscale??
	int _angle;
	unsigned _param_display;
	bool _label_display;
}; // Symbol
/*--------------------------------------------------------------------------*/
} // qucs
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
