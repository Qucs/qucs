/***************************************************************************
    author               : 2018, 2020 Felix Salfelder
    license              : GPLv3+
 ***************************************************************************/
#include "qucs_globals.h"

namespace qucs{

Dispatcher<Diagram> diagram_dispatcher;
Dispatcher<Command> command_dispatcher;
Dispatcher<Symbol> symbol_dispatcher;
Dispatcher<Data> data_dispatcher;
Dispatcher<Language> language_dispatcher;
Dispatcher<Element> element_dispatcher;
Dispatcher<Widget> widget_dispatcher;

// use command_dispatcher?
Dispatcher<Action> action_dispatcher;

}
