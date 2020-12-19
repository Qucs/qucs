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
#ifndef QUCS_GLOBALS_H
#define QUCS_GLOBALS_H

#include "command.h"
#include "dispatcher.h"
#include "platform.h"

class TaskElement;
class Diagram;
class DocumentLanguage;
class DocumentFormat;
class Painting;
class Simulator;
class Symbol;

extern INTERFACE Dispatcher<TaskElement> element_dispatcher;
extern INTERFACE Dispatcher<Command> command_dispatcher;
extern INTERFACE Dispatcher<Diagram> diagram_dispatcher;
extern INTERFACE Dispatcher<DocumentLanguage> language_dispatcher;
extern INTERFACE Dispatcher<Painting> painting_dispatcher;
extern INTERFACE Dispatcher<Simulator> simulator_dispatcher;
extern INTERFACE Dispatcher<Symbol> symbol_dispatcher;

#endif
