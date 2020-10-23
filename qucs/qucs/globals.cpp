/***************************************************************************
    author               : 2018 Felix Salfelder
    license              : GPLv3+
 ***************************************************************************/
#include "globals.h"

Dispatcher<Diagram> diagram_dispatcher;
Dispatcher<Symbol> symbol_dispatcher;
Dispatcher<Simulator> simulator_dispatcher;
Dispatcher<DocumentLanguage> doclang_dispatcher; // Fmt baseclasses?
Dispatcher<DocumentFormat> docfmt_dispatcher;
Dispatcher<TaskElement> command_dispatcher; // taskElementSymbol ..
Dispatcher<Painting> painting_dispatcher;
