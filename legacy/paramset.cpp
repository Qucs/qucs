/***************************************************************************
    copyright            : 2020 Felix Salfelder
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

// paramset style models from .lib files.

#include "qucs_app.h"
#include "schematic_doc.h"
#include "misc.h"
#include "qucs_globals.h"

#include <limits.h>

#include <QTextStream>
#include <QDir>
#include <QRegExp>
#include <QDebug>

namespace{

//paramset hack; takes a copy of a Symbol...
class LibParamset : public Symbol{
public:
	explicit LibParamset():Symbol(){
		setTypeName("LibParamset"); // really?
	}
	LibParamset( LibParamset const& l) : Symbol(l), _proto(nullptr){}
	~LibParamset() { delete _proto; }

private: // Element
	Symbol* clone()const override{
		return new LibParamset(*this);
	}

private: // Symbol
	Symbol* clone_instance()const override{
		untested();
		assert(_proto);
		return prechecked_cast<Symbol*>(_proto->clone());
	}
	void setTypeName(std::string const& n) override{
		trace1("paramset type", n);
		_proto = symbol_dispatcher.clone(n);
		if(_proto){
		}else{
			// list-entries
		}
	}
	unsigned numPorts() const override{ incomplete(); return 0; }
	Port& port(unsigned) override {unreachable(); return *new Port();}
	pos_t portPosition(unsigned) const override{
		incomplete(); return pos_t(0,0);
	}
	void setParameter(std::string const& n, std::string const& v) override{
		if(_proto){
			_proto->setParameter(n, v);
		}else{
			unreachable();
		}
		// Symbol::setParameter(n, v);
	}
	void setParameter(unsigned n, std::string const& v) override{
		// BUG: proto is not shared.
		if(_proto){
			trace3("paramset fwd", label(), n, v);

			_proto->setParameter(n, v);
		}else{
		}
	}
	// used in list-entries
	std::string paramValue(std::string const& n) const override{
		if (n=="$tx"){
			return "0";
		}else if (n=="$ty"){
			return "0";
		}else{
			return Symbol::paramValue(n);
		}
	}
	rect_t bounding_rect() const override{ untested();
		if(_proto){itested();
			return _proto->bounding_rect();
		}else{ untested();
			return rect_t();
		}
	}
	void paint(ViewPainter* p) const override{ untested();
		if(_proto){untested();
			((Element*)_proto)->paint(p);
		}else{ untested();
		}
		Symbol::paint(p);
	}

private: // Symbol
//	ElementList const* symbolPaintings() const override{ untested();
//		if(!_proto){untested();
//		}else if(_proto){ untested();
//		  	SchematicModel const* m = _proto->subckt();
//		  	return &m->paintings();
//		}else{ untested();
//		}
//		return nullptr;
//	}

private:
	Symbol* _proto; // BUG. common.
}D;
static Dispatcher<Symbol>::INSTALL p2(&symbol_dispatcher, "LegacyParamset", &D);

} //namespace
