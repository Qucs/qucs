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
 * delete and clear commands
 */
//BUG// If someone deletes an element inside an instance of a subckt
// (like ".delete r1.x1", there is no error message, and the deleted
// element will reappear next time an elaboration occurs, which is 
// usually before anything else.
//
#include "platform.h"
#include "qucs_globals.h"
#include "qio.h"
#define CARD_LIST ElementList
#define CS istream_t

#include "qucs_globals.h"
#include "element_list.h"
#include "command.h"
#define CARD_LIST ElementList
#define CMD Command
#define CS istream_t
/*--------------------------------------------------------------------------*/
namespace {
/*--------------------------------------------------------------------------*/
class CMD_DELETE : public CMD {
private:
  bool delete_one_name(const std::string& name, CARD_LIST* Scope)const
  {
    incomplete();
#if 0
    assert(Scope);
    
    std::string::size_type dotplace = name.find_first_of(".");
    if (dotplace != std::string::npos) {
      // has a dot, look deeper
      // Split the name into two parts:
      // "container" -- where to look (all following the dot)
      // "dev_name" -- what to look for (all before the dot)
      std::string dev_name  = name.substr(dotplace+1, std::string::npos);
      std::string container = name.substr(0, dotplace);
      // container name must be exact match
      CARD_LIST::iterator i = Scope->find_(container);
      if (i == Scope->end()) {
	// can't find "container" (probably .subckt) - no match
	// try reverse
	dotplace = name.find_last_of(".");
	container = name.substr(dotplace+1, std::string::npos);
	dev_name  = name.substr(0, dotplace);
	// container name must be exact match
	i = Scope->find_(container);
      }else{
      }
      if (i == Scope->end()) {
	// can't find "container" (probably .subckt) - no match
	return false;
      }else if ((**i).is_device()) {
	// found a match, but it isn't a container (subckt)
	return false;
      }else{
	// found the container, look inside
	return delete_one_name(dev_name, (**i).subckt());
      }
      unreachable();
    }else{
      // no dots, look here
      if (name.find_first_of("*?") != std::string::npos) {
	// there's a wild card.  do linear search for all matches
	bool didit = false;
	{
	  CARD_LIST::iterator i = Scope->begin();
	  while (i != Scope->end()) {
	    CARD_LIST::iterator old_i = i++;
	    // ^^^^^^^^^^^^ move i past the item being deleted
	    if (wmatch((**old_i).short_label(), name)) {
	      Scope->erase(old_i);
	      didit = true;
	    }
	  }
	}
	return didit;
      }else{
	// no wild card.  fast search for one exact match
	CARD_LIST::iterator i = Scope->find_(name);
	if (i != Scope->end()) {
	  Scope->erase(i);
	  return true;
	}else{
	  return false;
	}
      }
      unreachable();
    }
    unreachable();
#endif
    return false;
  }
  //-----------------------------------
  void do_it(CS& cmd, CARD_LIST* Scope)
  {
    if (cmd.umatch("all ")) {
//      CARD_LIST::card_list.erase_all();
      assert(Scope);
      Scope->clear();
    }else{
      while (cmd.more()) {
	size_t mark = cmd.cursor();
	bool didit = delete_one_name(cmd.ctos(), Scope);
	if (!didit) {
	  cmd.warn(bWARNING, mark, "no match");
	}
      }
    }
  }
} p1;
DISPATCHER<CMD>::INSTALL d1(&qucs::command_dispatcher, "delete|rm", &p1);
/*--------------------------------------------------------------------------*/
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// vim:ts=8:sw=2:noet:
