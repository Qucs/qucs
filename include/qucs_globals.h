/***************************************************************************
    author               : 2018, 2020 Felix Salfelder
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
#include "action.h"

class Command;

namespace qucs {

class TaskElement;
class Diagram;
class Language;
class Data;
class Symbol;
class Widget;
class Action;

extern INTERFACE Dispatcher<Element> element_dispatcher;
extern INTERFACE Dispatcher<Command> command_dispatcher;
extern INTERFACE Dispatcher<Diagram> diagram_dispatcher;
extern INTERFACE Dispatcher<Language> language_dispatcher;
extern INTERFACE Dispatcher<Data> data_dispatcher;
extern INTERFACE Dispatcher<Symbol> symbol_dispatcher;
extern INTERFACE Dispatcher<Widget> widget_dispatcher;
extern INTERFACE Dispatcher<Action> action_dispatcher;

}

/// for now.
using qucs::element_dispatcher;
using qucs::command_dispatcher;
using qucs::diagram_dispatcher;
using qucs::language_dispatcher;
using qucs::data_dispatcher;
using qucs::symbol_dispatcher;
using qucs::widget_dispatcher;
using qucs::action_dispatcher;

#endif
