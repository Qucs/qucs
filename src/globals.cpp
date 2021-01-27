/***************************************************************************
    author               : 2018, 2020 Felix Salfelder
    license              : GPLv3+
 ***************************************************************************/
#include "qucs_globals.h"
//#include "painting.h"

Dispatcher<Diagram> diagram_dispatcher;
Dispatcher<Command> commandDispatcher;
Dispatcher<Symbol> symbol_dispatcher;
Dispatcher<Simulator> simulator_dispatcher;
Dispatcher<DocumentLanguage> languageDispatcher;
Dispatcher<TaskElement> element_dispatcher; // taskElementSymbol ..
Dispatcher<Painting> painting_dispatcher;
