/***************************************************************************
    begin                : 2018
    copyright            : Felix
    email                : felix@salfelder.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "schematic_symbol.h"
#include "schematic_model.h"

SchematicSymbol::SchematicSymbol(){
	_paint = new PaintingList();
}

SchematicSymbol::~SchematicSymbol(){
	delete _paint;
	_paint = nullptr;
}

PaintingList& SchematicSymbol::symbolPaintings()
{
	assert(_paint);
	return *_paint;
}

PaintingList const& SchematicSymbol::symbolPaintings() const
{
	assert(_paint);
	return *_paint;
}

WireList const& SchematicSymbol::wires() const
{
	auto s = subckt();
	assert(s);
	return s->wires();
}
NodeMap const& SchematicSymbol::nodes() const
{
	auto s = subckt();
	assert(s);
	return s->nodes();
}
DiagramList const& SchematicSymbol::diagrams() const
{
	auto s = subckt();
	assert(s);
	return s->diagrams(); // really?
}
PaintingList const& SchematicSymbol::paintings() const
{
	auto s = subckt();
	assert(s);
	return s->paintings(); // really?
}
ComponentList const& SchematicSymbol::components() const // possibly "devices". lets see.
{
	auto s = subckt();
	assert(s);
	return s->components();
}
