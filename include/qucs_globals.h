/***************************************************************************
    author               : 2018, 2020, 2021 Felix Salfelder
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
/*--------------------------------------------------------------------------*/
#include "dispatcher.h"
#include "platform.h"
/*--------------------------------------------------------------------------*/
class Command;
/*--------------------------------------------------------------------------*/
namespace qucs {
/*--------------------------------------------------------------------------*/
class TaskElement;
class Diagram;
class Element;
class Language;
class Data;
class Symbol;
class Widget;
class Action;
/*--------------------------------------------------------------------------*/
extern INTERFACE Dispatcher<Element> element_dispatcher;
extern INTERFACE Dispatcher<Command> command_dispatcher;
extern INTERFACE Dispatcher<Diagram> diagram_dispatcher;
extern INTERFACE Dispatcher<Language> language_dispatcher;
extern INTERFACE Dispatcher<Data> data_dispatcher;
extern INTERFACE Dispatcher<Symbol> symbol_dispatcher;
extern INTERFACE Dispatcher<Widget> widget_dispatcher;
extern INTERFACE Dispatcher<Action> action_dispatcher;
/*--------------------------------------------------------------------------*/
} // namespace
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
