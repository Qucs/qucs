/***************************************************************************
                           schematic_model.cpp
                             ---------------
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

#include "schematic.h"

SchematicModel::SchematicModel(Schematic* s)
  :_doc(s)
{
}

/// ACCESS FUNCTIONS.
// these are required to move model methods over to SchematicModel
// note that _doc->...() functions still involve pointer hacks
ComponentList& SchematicModel::components()
{
	assert(_doc);
	// temporary. move stuff here....
	return _doc->components();
}

WireList& SchematicModel::wires()
{
	assert(_doc);
	// temporary. move stuff here....
	return _doc->wires();
}

NodeList& SchematicModel::nodes()
{
	assert(_doc);
	// temporary. move stuff here....
	return _doc->nodes();
}

PaintingList& SchematicModel::paintings()
{
	assert(_doc);
	// temporary. move stuff here....
	return _doc->paintings();
}

DiagramList& SchematicModel::diagrams()
{
	// temporary. move stuff here....
	return _doc->diagrams();
}

// same, but const.
ComponentList const& SchematicModel::components() const
{
	// temporary. move stuff here....
	return _doc->components();
}

WireList const& SchematicModel::wires() const
{
	// temporary. move stuff here....
	return _doc->wires();
}

NodeList const& SchematicModel::nodes() const
{
	// temporary. move stuff here....
	return _doc->nodes();
}

PaintingList const& SchematicModel::paintings() const
{
	// temporary. move stuff here....
	return _doc->paintings();
}

DiagramList const& SchematicModel::diagrams() const
{
	// temporary. move stuff here....
	return _doc->diagrams();
}
