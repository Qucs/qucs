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
    : Element()
{
}
/*--------------------------------------------------------------------------*/
SymbolFactory::SymbolFactory(SymbolFactory const& s)
    : Element(s)
{
}
/*--------------------------------------------------------------------------*/
SymbolFactory::~SymbolFactory()
{
	delete _proto;
	_proto = nullptr;
}
/*--------------------------------------------------------------------------*/
void SymbolFactory::stash(Element* e)
{
	// TODO: keep track.
	assert(_scope);
	_scope->push_back(e);
}
/*--------------------------------------------------------------------------*/
void SymbolFactory::set_dev_type(std::string const& name)
{
	trace1("SubFactory::set_dev_type", name);
	Element* e = nullptr;
	try{
		e = find_looking_out(name)->clone();
		untested();
	}catch(qucs::ExceptionCantFind const&){
		e = qucs::symbol_dispatcher.clone(name);
	}

	if(auto ps = prechecked_cast<Symbol*>(e)){
		_proto = ps;
		_scope = owner()->scope();
		set_label(name);
	}else{ itested();
		throw qucs::ExceptionCantFind(name, short_label());
	}
}
/*--------------------------------------------------------------------------*/
static Element* find_recursive(Element* where, std::string const& name)
{
	Element* f;
	while(where){
		try{
			f = where->find_in_my_scope(name);
			break;
		}catch(qucs::ExceptionCantFind const&){
			where = where->owner();
		}
	}

	return f;
}
/*--------------------------------------------------------------------------*/
void FactorySymbol::stash_proto(Element* e)
{
	assert(_factory);
	_factory->stash(e);
}
/*--------------------------------------------------------------------------*/
Element const* FactorySymbol::find_proto(std::string const& n) const
{
	SymbolFactory const* f = _factory;
	auto i = f->scope()->find_(n);
	if(i==f->scope()->end()){
		return nullptr;
	}else{
		return *i;
	}
}
/*--------------------------------------------------------------------------*/
void FactorySymbol::set_dev_type(std::string const& name)
{
	trace1("set_dev_type", name);
	assert(owner());
	Element* ee = find_recursive(this, name);
	SymbolFactory* f = dynamic_cast<SymbolFactory*>(ee);
	if(!f){
		message(qucs::MsgFatal, "cannot find " + name);
	}else{
	}
	_factory = f;
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
