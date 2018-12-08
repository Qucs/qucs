#include "l_dispatcher.h"
#include "symbol.h"

Dispatcher<Symbol> symbol_dispatcher;
Dispatcher<Simulator> simulator_dispatcher;
Dispatcher<NetLang> netlang_dispatcher;
Dispatcher<Command> command_dispatcher;
Dispatcher<Painting> painting_dispatcher;
