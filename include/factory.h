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

// create symbols on demand

#ifndef QUCS_SYMBOL_FACTORY
#define QUCS_SYMBOL_FACTORY

class SchematicModel;

struct SymbolFactory{
	SchematicModel* _scope{nullptr};
};

#endif
