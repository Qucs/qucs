/* Copyright (C) 2001 Albert Davis
 *               2020 Felix Salfelder
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
 * list and save commands.
 * save is list with direction to file
 */
//testing=script 2006.07.17
#include "e_cardlist.h"
#include "u_lang.h"
//#include "c_comand.h"
#include "qucs_globals.h"
/*--------------------------------------------------------------------------*/
// QUCS hacks
#include "command.h"
#include "exception.h"
#include "qio.h"
#include "settings.h"
#include "command.h"
#define print_item printItem
#define CS istream_t
#define OPT tQucsSettings
#define CMD Command
/*--------------------------------------------------------------------------*/
namespace {
/*--------------------------------------------------------------------------*/
void list_save(CS& cmd, ostream_t& out, CARD_LIST* scope)
{
  assert(scope);
  scope->prepare();
  for(auto i: *scope){
    trace1("list??", i->label());
  }

  cmd >> "list";
  std::string lang_name = "";

  size_t here = cmd.cursor();
  do{
    Get(cmd, "l{anguage}", &lang_name);
  } while(cmd.more() && !cmd.stuck(&here));
  trace1("list_save", lang_name);

#if 0
  //out.setfloatwidth(7);
  switch (ENV::run_mode) {
  case rPRE_MAIN:
    unreachable();
    return;
  case rPRESET:
    /* do nothing */
    return;
  case rBATCH:
  case rINTERACTIVE:
  case rSCRIPT:
    /* keep going */
    break;
  }

  if (!OPT::language) { untested();
    throw qucs::Exception("no language");
  }else{ untested();
  }

  (out - IO::mstdout) << head << '\n';
#endif

  LANGUAGE* lang = OPT::language;
  if(lang_name!=""){
    lang = qucs::language_dispatcher[lang_name];
    assert(lang); // TODO
  }else if(!lang){
    incomplete();
    lang = qucs::language_dispatcher["qucsator"];
  }else{ untested();
  }

  assert(lang); // TODO

  if (cmd.is_end()) {
    /* no args: list all		    */
    for (CARD_LIST::const_iterator ci=scope->begin();ci!=scope->end();++ci) {
      lang->print_item(out, *ci);
    }
  }else{itested();
    /* some args: be selective	    */
    size_t arg1 = cmd.cursor();
    CARD_LIST::fat_iterator ci = (cmd.match1('-')) 
      ? CARD_LIST::fat_iterator(scope, scope->begin())
      : findbranch(cmd, scope);
    if (ci.is_end()) {untested();
      trace2("cantfind", cmd.fullstring(), cmd.tail());
      throw qucs::Exception_CS("can't find", cmd);
    }else{itested();
    }
    
    if (cmd.match1('-')) {		/* there is a dash:  a range	    */
      incomplete();
#if 0
      cmd.skip();
      if (cmd.is_end()) {	/* line ends with dash: all to end  */
	do { untested();
	  OPT::language->print_item(out, *ci);
	} while (++ci, !ci.is_end());
      }else{ untested();
	CARD_LIST::fat_iterator stop = ci;
	stop = findbranch(cmd, ++stop);
	if (stop.is_end()) {itested();
	  throw qucs::Exception_CS("can't find", cmd);
	}else{ untested();
	  do { untested();
	    OPT::language->print_item(out, *ci);
	  } while (ci++ != stop); // note subtle difference
	}
      }
#endif
    }else{itested();
      /* no dash: a list		    */
      do {itested();
	/* each arg			    */
	size_t next = cmd.cursor();
	do {itested();
	  /* all that match this arg	    */
	  trace1("list print", (*ci)->label());
	  lang->print_item(out, *ci);
	  cmd.reset(arg1);
	  assert(!ci.is_end());
	  ci = findbranch(cmd, ++ci); // next match
	} while (!ci.is_end());
	cmd.reset(arg1 = next);
	ci = findbranch(cmd, scope); // next arg
      } while (!ci.is_end());
    }
  }
}
/*--------------------------------------------------------------------------*/
class CMD_LIST : public Command {
public:
  void do_it(CS& cmd, CARD_LIST* Scope)
  {
    trace1("list", cmd.fullstring());
//    list_save(cmd, IO::mstdout, Scope);
    ostream_t out(stdout);
    list_save(cmd, out, Scope);
    untested();
//    IO::mstdout.flush(); // ??
    out.flush();
  }
} p1;
DISPATCHER<CMD>::INSTALL d1(&qucs::command_dispatcher, "list", &p1);
/*--------------------------------------------------------------------------*/
#if 0 // later
class CMD_SAVE : public CMD {
public:
  void do_it(CS& cmd, CARD_LIST* Scope)
  {itested();
    cmd.reset(); /* back up to beginning of input line */
    OMSTREAM out; // = IO::mstdout;
    list_save(cmd, *outset(cmd,&out), Scope);
  }
} p2;
DISPATCHER<CMD>::INSTALL d2(&command_dispatcher, "save", &p2);
#endif
/*--------------------------------------------------------------------------*/
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// vim:ts=8:sw=2:noet:
