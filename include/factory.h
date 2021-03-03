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

namespace qucs {
	class ElementList;
}

struct SymbolFactory{
	qucs::ElementList* _scope{nullptr};
};

#endif
