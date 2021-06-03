/***************************************************************************
    copyright            : (C) 2021 Felix Salfelder
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

// create models on demand

#ifndef QUCS_SYMBOL_FACTORY
#define QUCS_SYMBOL_FACTORY

#include "platform.h"
#include "symbol.h"
#include "model.h"
#include <map>

namespace qucs {
/*--------------------------------------------------------------------------*/
class ElementList;
class Symbol;
class Component;
/*--------------------------------------------------------------------------*/
// ModelFactory.
class SymbolFactory : public Model{
public:
	explicit SymbolFactory();
	virtual ~SymbolFactory();
protected:
	SymbolFactory(SymbolFactory const& s);

public:
	virtual Component const* new_symbol(std::string const&){unreachable();incomplete();return nullptr;}
	ElementList const* scope() const{ return _scope; }

private:
	ElementList* scope(){ return _scope; }

private:
	std::string dev_type() const override{
		return "ModelFactory";
	}
	void set_dev_type(std::string const& name);
	Element* clone_instance() const;

public:
	Element* clone()const{return new SymbolFactory(*this);}

private:
	friend class FactorySymbol; // (!)
	void stash(Element* e);

	std::string param_value_by_name(std::string const& name) const override {
		auto i = _params.find(name);
		if(i != _params.end()){
			return i->second;
		}else{ untested();
			return Model::param_value_by_name(name);
		}
	}
	void set_param_by_name(std::string const& name, std::string const& v) override {
		if(name=="dev_type"){
			// abuse of paramset notation..
			set_dev_type(v);
		}else{
			_params[name] = v;
		}
	}

private:
	Symbol* _proto{nullptr};
	ElementList* _scope{nullptr};
	std::map<std::string, std::string> _params;
};
/*--------------------------------------------------------------------------*/
class FactorySymbol : public Symbol {
public:
	explicit FactorySymbol() : Symbol() {}
	FactorySymbol(FactorySymbol const& p) : Symbol(p) {}

private:
	void set_dev_type(std::string const& name) override;

protected:
	std::string factory_param(std::string const& name) const{
		if(_factory){
			return _factory->param_value_by_name(name);
		}else{
			throw qucs::ExceptionCantFind(name, short_label());
		}
	}
	Element const* find_proto(std::string const&);
	void stash_proto(Element* e);

	index_t numPorts() const override{
		incomplete();
		return 0;
	}
	Port& port(index_t) override{
		throw "incomplete";
	}
	std::string const /*BUG*/ portName(index_t i) const override
	{ untested();
		return std::string("port_") + std::to_string(i);
	}
/*--------------------------------------------------------------------------*/

private:
	mutable SymbolFactory* _factory{nullptr};
};
/*--------------------------------------------------------------------------*/
} // qucs
/*--------------------------------------------------------------------------*/
#endif
