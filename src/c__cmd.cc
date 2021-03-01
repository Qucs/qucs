/*$Id: c__cmd.cc  2016/09/11 $ -*- C++ -*-
 * Copyright (C) 2001 Albert Davis
 * Author: Albert Davis <aldavis@gnu.org>
 *
 * This file is part of "Gnucap", the Gnu Circuit Analysis Package
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 *------------------------------------------------------------------
 * command interpreter and dispatcher
 */
//testing=obsolete
// #include "u_status.h"
// #include "declare.h"	/* plclose */
// #include "error.h"
#include "qio.h"
#include "command.h"
#include "message.h"
#include "qucs_globals.h"
#include "schematic_model.h"
// #include "qucs_globals.h"
/*--------------------------------------------------------------------------*/
// Qucs wrappings
#define CS istream_t
#define CMD Command
#define CARD_LIST qucs::ElementList
#define error ::message
//#define fullstring fullString
// TODO: connect with qucs error facilities.
/*--------------------------------------------------------------------------*/
/* cmdproc: process a command
 * parse, and act on, a command string
 */
void CMD::cmdproc(CS& cmd, CARD_LIST* scope)
{itested();
  trace2("qucs cmdproc", cmd.fullstring(), cmd.cursor());

#if 0
  bool get_timer_was_running = ::status.get.is_running();
  ::status.get.stop();

  static TIMER timecheck;
#endif
  bool didsomething = true;
  
  error(bTRACE, ">>>>>" + cmd.fullstring() + "\n");
  
#if 0
  timecheck.stop().reset().start();

  cmd.umatch(ANTI_COMMENT);
  while (cmd.umatch(I_PROMPT)) {itested();
    /* skip any number of these */
  }
#endif

//  size_t here = cmd.cursor();
  std::string s;

#if 0
  // Map possible short names to full ones.
  // If this if/else block is removed, the only loss is the short names.
  // Although it looks like it can be used to make aliases, don't.
  if (cmd.umatch("'|*|#|//|\""))	{	     s = "xxxxcomment";}
  else if (cmd.umatch("b{uild} "))      {itested();  s = "build";}
  else if (cmd.umatch("del{ete} "))     {            s = "delete";}
  else if (cmd.umatch("fo{urier} "))    {            s = "fourier";}
  else if (cmd.umatch("gen{erator} "))  {	     s = "generator";}
  else if (cmd.umatch("inc{lude} "))    {itested();  s = "include";}
  else if (cmd.umatch("l{ist} "))       {            s = "list";}
  else if (cmd.umatch("m{odify} "))     {            s = "modify";}
  else if (cmd.umatch("opt{ions} "))    {            s = "options";}
  else if (cmd.umatch("par{ameter} "))  {            s = "param";}
  else if (cmd.umatch("pr{int} "))      {            s = "print";}
  else if (cmd.umatch("q{uit} "))       {	     s = "quit";}
  else if (cmd.umatch("st{atus} "))     {            s = "status";}
  else if (cmd.umatch("te{mperature} ")){itested();  s = "temperature";}
  else if (cmd.umatch("tr{ansient} "))  {            s = "transient";}
  else if (cmd.umatch("!"))		{	     s = "system";}
  else if (cmd.umatch("<"))		{itested();  s = "<";}
  else if (cmd.umatch(">"))		{itested();  s = ">";}
  else
#endif
  { /* no shortcut available */
    trace1("cmdproc", cmd.fullString());
    cmd >> s;
    trace1("got", s);
    didsomething = false;
  }

  if (s == "xxxxcomment") { untested();
    // nothing
  }else if (s != "") {itested();
    CMD* c = command_dispatcher[s];
    if (c) {itested();
      // assert(scope);
      c->do_it(cmd, scope);
      didsomething = true;
    }else{itested();
      trace1("no such command", s);
      incomplete();
      cmd.warn(bWARNING, 0, "what's this?");
    }
  }else if (!didsomething) { untested();
    incomplete();
//    cmd.check(bWARNING, "bad command");
    didsomething = false;
  }else{untested();
  }
  
#if 0
  if (OPT::acct  &&  didsomething) {itested();
    IO::mstdout.form("time=%8.2f\n", timecheck.check().elapsed());
  }else{ untested();
  }
  plclose();
  outreset();

  if (get_timer_was_running) { untested();
    ::status.get.start();
  }else{ untested();
  }
#endif
}
/*--------------------------------------------------------------------------*/
void CMD::command(const std::string& cs, CARD_LIST* scope)
{itested();
  CS cmd(CS::_STRING, cs); // from string, full command
  std::string s;
  cmd >> s;
  trace1("CMD::command", s);

  CMD* c = command_dispatcher[s];
  if (c) {itested();
    c->do_it(cmd, scope);
  }else{ untested();
    error(bDEBUG, "bad internal command: " + s + '\n');
  }
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// vim:ts=8:sw=2:noet:
