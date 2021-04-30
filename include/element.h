/***************************************************************************
    copyright            : (C) 2001, 2007 Albert Davis
                               2018, 2020 Felix Salfelder
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

/* base class for anything in a netlist or circuit file
 */

#ifndef QUCS_ELEMENT_H
#define QUCS_ELEMENT_H
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#include "object.h"
#include "io_trace.h"
#include "port.h"
#include "geometry.h"
#include "platform.h"
/*--------------------------------------------------------------------------*/
class QWidget;
class QPainter;
/*--------------------------------------------------------------------------*/
namespace qucs {
/*--------------------------------------------------------------------------*/
class Diagram;
class Doc;
class ElementList;
class NetLang;
class Node;
class Painting;
class Symbol;
class ViewPainter;
class Widget;
/*--------------------------------------------------------------------------*/
// Element: has a position.
// TODO: base on sth like card with params but no position.
class Element : public Object {
public:
	friend class ElementGraphics; // BUG?
	typedef rect_t Rect;
protected:
	Element(Element const&);
public:
	Element();
	virtual ~Element();

public:	// "elaborate"
	virtual void	 prepare()	{} // precalc_first.

public: // make old variables accessible
	int const& cx() const { return _position.first; }
	int const& cy() const { return _position.second; }

	int const& x1_() const { return x1; }
	int const& y1_() const { return y1; }
//	void snapToGrid(SchematicDoc& s);

	void setObsoleteType(int){
		unreachable();
	}

public: // UI stuff.
#if 0 // TODO (Doc is not a QWidget yet);
	virtual QWidget* widget(QObject* parent) { return nullptr; }
#else
	virtual Widget* schematicWidget(Doc*) const { return nullptr; }
	virtual QWidget* newWidget() {return nullptr;}
#endif


public: // other stuff. move to painting?
	virtual bool showLabel() const{ return true; }
	//virtual bool showParam(int i) const{ return true; } // later

	void setPosition(pos_t const& c){ _position = c; }

	// BUG: remove "center"
	// BUG: not virtual
	virtual pos_t center()const;

	// move to painting?
	pos_t position()const{
		return _position;
	}

public:
	virtual Element* clone()const = 0;
	virtual Element* clone_instance()const{
		return clone();
	}
//	virtual QString const& description() const{return incomplete_description;}
	virtual char const* iconBasename() const{return nullptr;}

public: // params
	virtual index_t param_count()const{return 0;}
	virtual void set_param_by_index(index_t, std::string const&);
	virtual void set_param_by_name(std::string const& name, std::string const& v);
	virtual std::string param_value_by_name(std::string const& n) const;
	virtual std::string param_value(index_t) const{return "incomplete";}
	virtual std::string param_name(index_t) const{return "incomplete";}
	virtual bool param_is_printable(index_t)const{return true;}

public: // compatibility
	virtual bool legacyTransformHack() const{
		return false;
	}

public:
	virtual std::string dev_type()const{unreachable(); return "";}
	virtual void set_dev_type(std::string const& x);

	// create a declaration, e.g. subcircuit definition or include directive
	virtual Symbol const* proto(ElementList const*) const{return nullptr;}
	ElementList const* scope() const;
	virtual ElementList* scope();
	virtual bool makes_own_scope()const  {return false;}

public: // friend ElementGraphics?
	//  void attachToModel();
	void detachFromModel();

public:
	Element const* owner() const{return _owner;}
	Element* owner(){return _owner;}

public:
	void set_owner(Element* e) { assert(!_owner || e==_owner || !e); _owner=e;}
	const Element* find_looking_out(const std::string& name)const;
	const Element* find_in_parent_scope(const std::string& name)const;
	const Element* find_in_my_scope(const std::string& name)const;
	Element* find_in_my_scope(std::string const& name);

public:
	void message(MsgType, const char*) const;
	virtual void message(MsgType, std::string const&) const;

	std::string const& label() const{return short_label();}

private:
	pos_t _position; // BUG: store in symbol?

protected: // BUG in Painting
	int x1, y1;

private:
	Element* _owner; // can't be const it seems.
}; // Element
/*--------------------------------------------------------------------------*/
inline ElementList const* Element::scope() const
{
	auto e=const_cast<Element*>(this);
	return e->scope();
}
/*--------------------------------------------------------------------------*/
} // qucs
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
