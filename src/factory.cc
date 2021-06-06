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

// ModelFactory, FactorySymbol: manage interactively created Models

#include "factory.h"
#include "symbol.h"
#include "parameter.h"
#include "common_sckt.h"
#include "sckt_base.h"
#include "dot.h"
#include "model.h"
#include "qucs_globals.h"
#include <vector>
/*--------------------------------------------------------------------------*/
namespace qucs{
/*--------------------------------------------------------------------------*/
Element* SymbolFactory::clone_instance() const
{
	assert(_proto);
	return _proto->clone();
}
/*--------------------------------------------------------------------------*/
SymbolFactory::SymbolFactory()
    : Model(nullptr)
{
}
/*--------------------------------------------------------------------------*/
SymbolFactory::SymbolFactory(SymbolFactory const& s)
    : Model(s)
{
}
/*--------------------------------------------------------------------------*/
FactorySymbol::FactorySymbol() : Symbol()
{
}
/*--------------------------------------------------------------------------*/
FactorySymbol::FactorySymbol(FactorySymbol const& p)
	: Symbol(p), _factory(p._factory)
{
}
/*--------------------------------------------------------------------------*/
FactorySymbol::~FactorySymbol()
{
	if(!_factory){
	}else if(_proto){
		_factory->decref(_proto);
	}else{
	}
}
/*--------------------------------------------------------------------------*/
SymbolFactory::~SymbolFactory()
{
	delete _proto;
	_proto = nullptr;
	for(auto i : _counts){
		if(i.second){
			// some clear call is missing.
			// or the factory is deleted early?
			incomplete();
			unreachable();
		}else{
		}
	}
}
/*--------------------------------------------------------------------------*/
void SymbolFactory::decref(Element const* model)
{
	assert(model);
	trace1("decref", model->short_label());
	unsigned& u = _counts[intptr_t(model)];
	assert(u);
	--u;
	if(!u){
		auto mh = const_cast<Element*>(model);
		_scope->erase(mh);
	}else{
	}
}
/*--------------------------------------------------------------------------*/
void SymbolFactory::incref(Element const* model)
{
	trace1("incref", model->short_label());
	++_counts[intptr_t(model)];
}
/*--------------------------------------------------------------------------*/
void SymbolFactory::stash(Element* model)
{
	assert(_scope);
	_scope->push_back(model);
}
/*--------------------------------------------------------------------------*/
void SymbolFactory::set_dev_type(std::string const& name)
{
	trace1("SymbolFactory::set_dev_type", name);
	if(name == "ModelFactory"){
		// abuse of paramset notation...
		return;
	}else{
	}
	Element* e = nullptr;
	try{
		e = find_looking_out(name)->clone();
		untested();
	}catch(qucs::ExceptionCantFind const&){
		e = qucs::symbol_dispatcher.clone(name);
	}

	if(auto ps = prechecked_cast<Symbol*>(e)){
		_proto = ps;
		if(owner()){
			_scope = owner()->scope();
		}else{
			_scope = &ElementList::card_list;
		}
		set_label(name);
	}else{ untested();
		throw qucs::ExceptionCantFind(name, short_label());
	}
}
/*--------------------------------------------------------------------------*/
static Element* find_recursive(Element* where, std::string const& name)
{
	Element* f=nullptr;
	while(where){
		try{
			trace3("find...", where->short_label(), name, where->scope());
			f = where->find_in_my_scope(name);
			break;
		}catch(qucs::ExceptionCantFind const&){
			trace2("owner?", where->short_label(), where->owner());
			where = where->owner();
		}
	}

	if(!where){
		// hack
		auto i = ElementList::card_list.find_(name);
		if (i == ElementList::card_list.end()) { untested();
		}else{
			f = *i;
		}
	}else{
	}

	return f;
}
/*--------------------------------------------------------------------------*/
void FactorySymbol::stash_proto(Element* proto)
{
	if(_factory){
		_factory->stash(proto);
	}else{ untested();
		incomplete();
	}
}
/*--------------------------------------------------------------------------*/
Element const* FactorySymbol::find_proto() /*const?*/
{
	assert(common());
	std::string n = common()->modelname();
	trace2("find_proto try", n, common()->modelname());
	SymbolFactory* f = _factory;
	ElementList* scope = &ElementList::card_list;
	if(f){
		scope = f->scope();
	}else{ untested();
	}

	istream_t cmd(istream_t::_STRING, n);

	assert(scope);
	auto ci = findbranch(cmd, scope);

	Element const* ret = nullptr;

	auto k=0;
	while(true){
		if (ci.is_end()) {
			break;
		}else{
			trace2("find_proto try", k, (*ci)->short_label());
			++k;
		}
		if(auto p=dynamic_cast<SubcktBase*>(*ci)){
			ret = p;
			break;
		}else if(auto m=dynamic_cast<Model*>(*ci)){
			if(m->is_valid(this)){
				trace2("find_proto gotit", m->short_label(), n);
				ret = m; // ->component_proto();
				break;
			}else{
				trace2("invalid", m->short_label(), n);
			}
		}else{ untested();
		}

		istream_t cmd(istream_t::_STRING, n);
		ci = findbranch(cmd, ++ci);
	}

	if(!ret){
		trace1("find_proto:, new_model", n);

		auto m = new_model();
		assert(m);
		stash_proto(m);
		ret = m;
	}else{
	}

	if(_proto == ret){
		// already there... (bug?)
	}else if(_factory){
		_factory->incref(ret);
	}else{ untested();
	}

	_proto = ret;
	return ret;
}
/*--------------------------------------------------------------------------*/
void FactorySymbol::set_dev_type(std::string const& name)
{
	trace1("FactorySymbol::set_dev_type", name);
	if(owner()){
		trace1("FactorySymbol::set_dev_type", owner()->short_label());
		Element* ee = find_recursive(this, name);
		SymbolFactory* f = dynamic_cast<SymbolFactory*>(ee);
		if(!f){ untested();
			message(qucs::MsgFatal, "cannot find " + name);
		}else{
		}
		_factory = f;
	}else{ untested();
	}
	Symbol::set_dev_type(name);
}
/*--------------------------------------------------------------------------*/
} // qucs
/*--------------------------------------------------------------------------*/
namespace {
/*--------------------------------------------------------------------------*/
using namespace qucs;
/*--------------------------------------------------------------------------*/
SymbolFactory d0;
static Dispatcher<Element>::INSTALL p(&element_dispatcher, "ModelFactory", &d0);
/*--------------------------------------------------------------------------*/
}// namespace
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
