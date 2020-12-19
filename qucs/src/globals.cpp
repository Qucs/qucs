/***************************************************************************
    author               : 2018, 2020 Felix Salfelder
    license              : GPLv3+
 ***************************************************************************/
#include "globals.h"
//#include "painting.h"

Dispatcher<Diagram> diagram_dispatcher;
Dispatcher<Command> command_dispatcher;
Dispatcher<Symbol> symbol_dispatcher;
Dispatcher<Simulator> simulator_dispatcher;
Dispatcher<DocumentLanguage> language_dispatcher; // language_dispatcher
Dispatcher<TaskElement> element_dispatcher; // taskElementSymbol ..
Dispatcher<Painting> painting_dispatcher;
