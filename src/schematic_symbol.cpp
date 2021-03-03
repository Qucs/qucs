/***************************************************************************
    copyright            : 2018, 2020 Felix Salfelder
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#if 0
#include "schematic_symbol.h"
#include "element_list.h"
#include "schematic_doc.h"
/*--------------------------------------------------------------------------*/
ElementList empty;
/*--------------------------------------------------------------------------*/
SchematicSymbol::SchematicSymbol()
	: SubcktBase(),
     _paint(nullptr)
{
//	_paint = new PaintingList();
}
/*--------------------------------------------------------------------------*/
SchematicSymbol::SchematicSymbol(SchematicSymbol const&)
	: SubcktBase(), // p??
     _paint(nullptr)
{
	incomplete();
//	_paint = new PaintingList(); // copy??
}
/*--------------------------------------------------------------------------*/
SchematicSymbol::~SchematicSymbol(){
	// delete _paint;
	_paint = nullptr;
}
/*--------------------------------------------------------------------------*/
PaintingList& SchematicSymbol::symbolPaintings()
{
	assert(_paint);
	assert(false);
	throw 0;
//	return *_paint;
}
/*--------------------------------------------------------------------------*/
ElementList const* SchematicSymbol::symbolPaintings() const
{
	if(!_paint){
		assert(subckt());
		auto p_ = subckt()->find_(":SymbolSection:");
		if(p_==subckt()->end()){
		}else if(auto p = dynamic_cast<SubcktBase const*>(*p_)){
			assert(p->subckt());
			auto const* q = p->subckt();
			_paint = &q->components();
		}else{
		}
	}else{
	}
	if(!_paint){
		message(QucsMsgCritical, "no paint");
		_paint = &empty;
	}else{
	}
	return _paint;

}
/*--------------------------------------------------------------------------*/
NodeMap const& SchematicSymbol::nodes() const
{
	auto s = subckt();
	assert(s);
	return s->nodes();
}
/*--------------------------------------------------------------------------*/
DiagramList const& SchematicSymbol::diagrams() const
{
	auto s = subckt();
	assert(s);
	return s->diagrams(); // really?
}
/*--------------------------------------------------------------------------*/
PaintingList const& SchematicSymbol::paintings() const
{
	auto s = subckt();
	assert(s);
	return s->paintings(); // really?
}
/*--------------------------------------------------------------------------*/
ElementList const& SchematicSymbol::components() const // possibly "devices". lets see.
{
	auto s = subckt();
	assert(s);
	return s->components();
}
/*--------------------------------------------------------------------------*/
#if 0
CmdEltList const* SchematicSymbol::commands() const
{
	auto s = subckt();
	assert(s);
	auto d = s->doc();
	if (!d){
		return nullptr;
	}else{
		return &d->commands();
	}
}
#endif
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
