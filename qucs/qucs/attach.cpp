/***************************************************************************
                                attach.cpp
                                ----------
    begin                : yes
    copyright            : 2016 by Felix Salfelder
    email                : felix@salfelder.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

// attach plugins.
// basic functionality inspired by gnucap, but without command base.

#include <iostream>
#include <map>
#include <ctype.h>

#include "platform.h"

class plugins{
  public:
    ~plugins(){
      for (std::map<std::string, void*>::iterator ii = attach_list.begin();
	  ii != attach_list.end(); ++ii) {
	void* m=ii->second;
	if(m){
	  dlclose(m);
	}
      }
    }

    void attach(std::string what){
      // RTLD_NOW means to resolve symbols on loading
      // RTLD_LOCAL means symbols defined in a plugin are local
      int dl_scope = RTLD_LOCAL;
      int check = RTLD_NOW;
      void* handle;

      handle = dlopen((what + SOEXT).c_str(), check | dl_scope);
      if (handle) {
	attach_list[what] = handle;
      }else{
	std::cerr << "failed to attach: " + std::string(dlerror()) + "\n";
	exit(1); // temporary, should actually throw.
//	throw Exception_something(dlerror());
      }
    }

  private:
    std::map<std::string, void*> attach_list;
} my_plugins;

void attach(const char* what)
{
  my_plugins.attach(std::string(what));
}


// vim:ts=8:sw=2:noet
