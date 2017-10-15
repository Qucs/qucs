/***************************************************************************
                              globals.h
                             --------------------
    author               : 2018 Felix Salfelder
    license              : GPLv3+
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include "globals.h"

Dispatcher<Symbol> symbol_dispatcher;
Dispatcher<Simulator> simulator_dispatcher;
Dispatcher<NetLang> netlang_dispatcher;
Dispatcher<Command> command_dispatcher;
