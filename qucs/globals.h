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

#include "platform.h"
#include "dispatcher.h"

class CmdElement;
class Diagram;
class DocumentLanguage;
class DocumentFormat;
class Painting;
class Simulator;
class Symbol;

extern INTERFACE Dispatcher<CmdElement> command_dispatcher;
extern INTERFACE Dispatcher<Diagram> diagram_dispatcher;
extern INTERFACE Dispatcher<DocumentLanguage> doclang_dispatcher;
extern INTERFACE Dispatcher<DocumentFormat> docfmt_dispatcher;
extern INTERFACE Dispatcher<Painting> painting_dispatcher;
extern INTERFACE Dispatcher<Simulator> simulator_dispatcher;
extern INTERFACE Dispatcher<Symbol> symbol_dispatcher;

#endif
