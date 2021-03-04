/*$Id: l_dispatcher.h 2016/03/29 al -*- C++ -*-
 * Copyright (C) 2018 Felix Salfelder
 * dispatcher -- for dynamically loaded modules
 */

#ifndef QUCS_DISPATCHER_H
#define QUCS_DISPATCHER_H
#include "platform.h"
#include "object.h"
/*--------------------------------------------------------------------------*/
// TODO: connect with qucs error facilities.
// INTERFACE void error(int,const char*,...);
// INTERFACE void error(int,const std::string&);
/*--------------------------------------------------------------------------*/
#include <string>
std::string to_string(int);
#include <map>
#include <assert.h>
#include "platform.h"
#include "io_trace.h"
#define DISPATCHER_BASE DispatcherBase
#define DISPATCHER Dispatcher
#define OPT tQucsSettings
#ifndef L_STLEXTRA_H
namespace notstd{
	static void to_lower(std::string*){}
}
#endif
#define error message
#include "l_dispatcher_.h"
#undef error
#endif
