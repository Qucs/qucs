/*$Id: l_dispatcher.h 2016/03/29 al -*- C++ -*-
 * Copyright (C) 2018 Felix Salfelder
 * dispatcher -- for dynamically loaded modules
 */
//testing=script 2015.01.21
#ifndef QUCS_DISPATCHER_H
#define QUCS_DISPATCHER_H
/*--------------------------------------------------------------------------*/
// TODO: connect with qucs error facilities.
INTERFACE void error(int,const char*,...);
INTERFACE void error(int,const std::string&);
/*--------------------------------------------------------------------------*/
class Object;
/*--------------------------------------------------------------------------*/
#include <string>
namespace OPT{
	static bool case_insensitive=false;
}
namespace notstd{
	static void to_lower(std::string*){}
}
using std::to_string;
#include <map>
#include <assert.h>
#include "platform.h"
#include "io_trace.h"
#define CKT_BASE Object
#define DISPATCHER Dispatcher
#include "l_dispatcher.h"
#undef CKT_BASE
#endif
