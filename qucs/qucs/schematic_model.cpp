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
	// presumably Q3PTRlist without this is just a QList<*> (check)
  Components.setAutoDelete(true);
  Nodes.setAutoDelete(true);
}

void SchematicModel::clear()
{
	// memory leak?!
  components().clear();
  nodes().clear();
  diagrams().clear();
  wires().clear();
  paintings().clear();
  //SymbolPaints.clear(); ??
}

void SchematicModel::parse(QTextStream& stream)
{
	QString Line;
  while(!stream.atEnd()) {
    Line = stream.readLine();
    if(Line == "<Components>") {
      if(!loadComponents(&stream)){
			incomplete();
//			throw exception...
		}
    }else if(Line == "<Wires>") {
      if(!loadWires(&stream)){
			incomplete();
//			throw exception... (in loadWires)
		}
    }else if(Line == "<Diagrams>") { untested();
      if(!loadDiagrams(&stream)){
			incomplete();
//			throw exception...
		}
    }else if(Line == "<Paintings>") { untested();
      incomplete(); // ignore Paintings fo rnow.
      PaintingList pl;
      if(!loadPaintings(&stream, &pl)){ untested();
			incomplete();
//			throw exception...
		}
    }else{ untested();
		 incomplete();
//		 throw something.
      // QMessageBox::critical(0, QObject::tr("Error"),
		//   QObject::tr("Clipboard Format Error:\nUnknown field!"));
    }
  }

}

/// ACCESS FUNCTIONS.
// these are required to move model methods over to SchematicModel
// note that _doc->...() functions still involve pointer hacks
ComponentList& SchematicModel::components()
{
	return Components;
}

WireList& SchematicModel::wires()
{
	assert(_doc);
	// temporary. move stuff here....
	return _doc->wires();
}

NodeList& SchematicModel::nodes()
{
	return Nodes;
}

PaintingList& SchematicModel::paintings()
{
	assert(_doc);
	// temporary. move stuff here....
	return _doc->paintings();
}

PaintingList& SchematicModel::symbolPaintings()
{
	assert(_doc);
	// temporary. move stuff here....
	return _doc->symbolPaintings();
}

DiagramList& SchematicModel::diagrams()
{
	// temporary. move stuff here....
	return _doc->diagrams();
}

// same, but const.
ComponentList const& SchematicModel::components() const
{
	return Components;
}

WireList const& SchematicModel::wires() const
{
	// temporary. move stuff here....
	return _doc->wires();
}

NodeList const& SchematicModel::nodes() const
{
	return Nodes;
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

void SchematicModel::merge(SchematicModel& src)
{
  for(auto i : src.components()){ itested();
	  components().append(i);
  }
  src.components().clear();
}
