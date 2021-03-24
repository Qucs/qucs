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
#include "misc.h"
#include "symbol.h"
#include "qucs_globals.h"

#include <limits.h>

#include <QTextStream>
#include <QDir>
#include <QRegExp>
#include <QDebug>

namespace{

using namespace qucs;

//paramset hack; takes a copy of a Symbol...
class LibParamset : public Symbol{
public:
	explicit LibParamset():Symbol(){
		set_dev_type("LibParamset"); // really?
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
	void set_dev_type(std::string const& n) override{
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
	void set_param_by_name(std::string const& n, std::string const& v) override{
		if(_proto){
			_proto->set_param_by_name(n, v);
		}else{
			unreachable();
		}
	}
	void set_param_by_index(unsigned n, std::string const& v) override{
		// BUG: proto is not shared.
		if(_proto){
			trace3("paramset fwd", label(), n, v);

			_proto->set_param_by_index(n, v);
		}else{
		}
	}
	// used in list-entries
	std::string param_value_by_name(std::string const& n) const override{
		if (n=="$tx"){
			return "0";
		}else if (n=="$ty"){
			return "0";
		}else{
			return Symbol::param_value_by_name(n);
		}
	}
	rect_t bounding_rect() const override{ untested();
		if(_proto){itested();
			return _proto->bounding_rect();
		}else{ untested();
			return rect_t();
		}
	}
	void paint(ViewPainter* v) const override{ untested();
		if(auto p=dynamic_cast<Painting const*>(_proto)){untested();
			p->paint(v);
		}else{ untested();
		}
		Symbol::paint(v);
	}

private: // Symbol
//	ElementList const* symbolPaintings() const override{ untested();
//		if(!_proto){untested();
//		}else if(_proto){ untested();
//		  	ElementList const* m = _proto->subckt();
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
