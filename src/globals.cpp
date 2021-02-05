/***************************************************************************
    author               : 2018, 2020 Felix Salfelder
    license              : GPLv3+
 ***************************************************************************/
#include "qucs_globals.h"
//#include "painting.h"

Dispatcher<Diagram> diagram_dispatcher;
Dispatcher<Command> commandDispatcher;
Dispatcher<Symbol> symbol_dispatcher;
Dispatcher<Data> dataDispatcher;
Dispatcher<DocumentLanguage> languageDispatcher;
Dispatcher<Element> element_dispatcher;
Dispatcher<Painting> painting_dispatcher;
