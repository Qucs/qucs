#include "platform.h"
#include "l_dispatcher.h"

class Symbol;
class Simulator;
class NetLang;

extern INTERFACE Dispatcher<Symbol> symbol_dispatcher;
extern INTERFACE Dispatcher<Simulator> simulator_dispatcher;
extern INTERFACE Dispatcher<NetLang> netlang_dispatcher;
