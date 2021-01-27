/*$Id: io_findf.cc 2016/05/15 al $ -*- C++ -*-
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
 * Modified by AD.  Sent to me by C-WARE
 * This file contains the routine to locate a file,
 *	using a path string for the directories to search.
 * Interface:
 *	findfile(filename, paths, mode)
 *	    filename is the name of the file to be searched for,
 *	    paths is the path to follow to find it.
 *	    mode is how you want to open the file
 *	returns full path name, if successful, else "".
 *
 * PATHSEP, ENDDIR are system dependent, defined in md.h
 */
//testing=script,complete 2017.03.12
//#include "library.h"
#include "io_trace.h"
#include "platform.h" // PATHSEP
/*--------------------------------------------------------------------------*/
// from io_ TODO
#include <unistd.h>	/* chdir, access, getcwd */
#include <string.h>

#define OS QOS //for now.
namespace OS{
inline bool access_ok(const std::string& file, int mode) { untested();
  return (::access(file.c_str(), mode) == 0/*file_ok*/);
}
}
/*--------------------------------------------------------------------------*/
std::string findFile(const std::string& filename, const std::string& path, int mode)
{
#ifdef CHECK_LOCAL_FIRST
  if (OS::access_ok(filename, mode)) {untested();
    return filename;
  }else{untested();
  }
#endif
					// for each item in the path
  for (std::string::const_iterator
	 p_ptr=path.begin(); p_ptr!=path.end(); ++p_ptr) {
    // p_ptr changed internally                  ^^^^^ skip sep
    std::string target = "";
    while (*p_ptr != PATHSEP  &&  p_ptr != path.end()) { // copy 1 path item
      target += *p_ptr++;
    }
    if (!target.empty() &&  !strchr(ENDDIR,p_ptr[-1])) {
      target += *ENDDIR;		// append '/' if needed
    }else{
    }
    
    target += filename;
    if (OS::access_ok(target, mode)) {	// found it
      return target;
    }else if (p_ptr==path.end()) {	// ran out of path, didn't find it
      return "";
    }else{				// else try again
    }
  }
  return ""; // path doesn't exist - didn't go thru loop at all
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// vim:ts=8:sw=2:noet:
