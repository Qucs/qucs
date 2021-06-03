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
// ModelManager
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
	trace1("ModelFactory::set_dev_type", name);
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
		if (i == ElementList::card_list.end()) {
		}else{
			f = *i;
		}
	}else{
	}

	return f;
}
/*--------------------------------------------------------------------------*/
void FactorySymbol::stash_proto(Element* e)
{
	if(_factory){
		_factory->stash(e);
	}else{
		ElementList::card_list.push_back(e);
	}
}
/*--------------------------------------------------------------------------*/
Element const* FactorySymbol::find_proto(std::string const& n)
{
	trace1("find_proto try", n);
	SymbolFactory* f = _factory;
	ElementList* scope = &ElementList::card_list;
	if(f){
		scope = f->scope();
	}else{
	}

	istream_t cmd(istream_t::_STRING, n);

	assert(scope);
	auto ci = findbranch(cmd, scope);

	auto k=0;
	while(true){
		if (ci.is_end()) {
			return nullptr;
		}else{
			trace2("find_proto try", k, (*ci)->short_label());
			++k;
		}
		if(auto p=dynamic_cast<SubcktBase*>(*ci)){
			return p;
		}else if(auto m=dynamic_cast<Model*>(*ci)){
			if(m->is_valid(this)){
				trace2("find_proto gotit", m->short_label(), n);
				return m->component_proto();
			}else{
				trace2("invalid", m->short_label(), n);
			}
		}else{ untested();
		}

		istream_t cmd(istream_t::_STRING, n);
		ci = findbranch(cmd, ++ci);
	}
	return nullptr;
}
/*--------------------------------------------------------------------------*/
void FactorySymbol::set_dev_type(std::string const& name)
{
	trace1("FactorySymbol::set_dev_type", name);
	if(owner()){
		trace1("FactorySymbol::set_dev_type", owner()->short_label());
		Element* ee = find_recursive(this, name);
		SymbolFactory* f = dynamic_cast<SymbolFactory*>(ee);
		if(!f){
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
