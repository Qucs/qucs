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
 * find a branch with matching label
 * returns the branch pointer
 */
#include "l_lib.h"
//#include "constant.h"
#include "e_cardlist.h"
#include "ap.h"
//#include "e_card.h"
/*--------------------------------------------------------------------------*/
// QUCS hacks.
#include "object.h"
#include "sckt_base.h"
#define CARD Object
#define CS istream_t
/*--------------------------------------------------------------------------*/
// stuff
#define BUFLEN 512
const char TOKENTERM[] = ",=()[]"; // constant.h
bool wmatch(const std::string& s1,const std::string& s2); // lib
/*--------------------------------------------------------------------------*/
/* findbranch: find a matching label, by (ugh) linear search
 *	label to look for is in command line (cmd).
 *	start "here".  Look only after "here".
 * return pointer to next match if exists (and eat input)
 *	  pointer to a non-existent branch if no match (don't eat input)
 * caution: caller must check return value before using
 */
CARD_LIST::fat_iterator findbranch(CS& cmd, CARD_LIST::fat_iterator here)
{
  size_t save = cmd.cursor();

  char labelwanted[BUFLEN+1];
  cmd.ctostr(labelwanted, BUFLEN, TOKENTERM);
  trace1("findbranch", labelwanted);
  
  if (!labelwanted[0]) {
    // nothing to look for
    cmd.reset(save);
    return here.end();
  }else{
  }
  
  char* local_part;	    // part before last dot, the thing inside
  char* last_part;	    // part after last dot, top level
  {
    char* dot = strrchr(labelwanted,'.');
    if (dot) {itested();		    // split the string into 2 parts at the last dot
      *dot = '\0';	    // before is the new "local_part", shortened
      last_part = dot + 1;  // after is the "last_part".
      local_part = labelwanted;
    }else{
      last_part = labelwanted;
      local_part = NULL;
    }
  }

  for (;;) {
    if (here.is_end()) {
      // at the end of the list - done, fails.
      cmd.reset(save);
      return here;
    }else if (wmatch((**here).short_label(), last_part)) { 
      // last_part matches
      if (!local_part) {
	// requested a simple name, found it .. done.
	return here;
      }else{untested();
	// there are dots, so look inside subckt
	auto b = dynamic_cast<SubcktBase*>(*here);
	if(!b){ untested();
	  ++here;
	}else if (b->subckt()) {untested();
	  // has a subckt, and its name matches, doing fine
	  CS want(CS::_STRING, local_part); // recursion
	  CARD_LIST::fat_iterator subbrh=findbranch(want, b->subckt());
	  if (!subbrh.is_end()) {untested();
	    // found it in a subckt
	    return subbrh;
	  }else{untested();
	    // didn't find anything in this subckt
	    // keep looking for another with the same name
	    // why?
	    ++here;
	  }
	}else{untested(); 
	  // no subckt
	  // keep looking for something with this name that has a subckt
	  // why?
	  untested();
	  ++here;
	}
      }
    }else{
      // label doesn't match
      // keep looking for one that does.  (linear search)
      ++here;
    }
  }
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// vim:ts=8:sw=2:noet:
