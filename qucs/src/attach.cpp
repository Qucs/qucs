/***************************************************************************
copyright            : 2016, 2020 Felix Salfelder
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

// attach plugins.
// basic functionality inspired by gnucap.

#include <iostream>
#include <map>
#include <ctype.h>

#include "platform.h"
#include "command.h"
#include "ap.h"
#include "io_trace.h"
#include "globals.h"
#include <unistd.h>	// TODO: io_.h

std::string findfile(const std::string& filename, const std::string& path, int mode);

namespace{

class plugins : public Command{
public:
	explicit plugins() : Command(){ untested();
	}
public:
  ~plugins(){ untested();
    for (std::map<std::string, void*>::iterator ii = attach_list.begin();
	ii != attach_list.end(); ++ii) { untested();
      void* m=ii->second;
      if(m){ untested();
	dlclose(m);
      }
    }
  }

  void attach(std::string what) const{ untested();
    // RTLD_NOW means to resolve symbols on loading
    // RTLD_LOCAL means symbols defined in a plugin are local
    int dl_scope = RTLD_LOCAL;
    int check = RTLD_NOW;
    void* handle;

    handle = dlopen((what).c_str(), check | dl_scope);
    if (handle) { untested();
      attach_list[what] = handle;
    }else{ untested();
      std::cerr << "failed to attach " << what << " (" << errno << ")\n";
      std::cerr << dlerror() << "\n";
      exit(1); // temporary, should actually throw.
      //	throw Exception_something(dlerror());
    }
  }

private:
  void do_it(istream_t&, SchematicModel*) override;
private:
  mutable std::map<std::string, void*> attach_list;
} my_plugins;
Dispatcher<Command>::INSTALL p(&command_dispatcher, "attach", &my_plugins);

static std::string plugpath()
{ untested();
  const char* ppenv=getenv("QUCS_PLUGPATH");
  if(!ppenv){ untested();
    unreachable();
	 return "";
  }else{ untested();
    return ppenv;
  }
}

void plugins::do_it(istream_t& cs, SchematicModel*)
{ untested();
	cs.reset();
	if(cs.umatch("attach")){ untested();
		auto path = plugpath();
		std::string what;
		cs >> what;

#if 0		// does not work
		const int n = strlen(SOEXT);
		if(what.size()<=n){ untested();
		}else if(what[what.size()-n]!='.'){
		}else{
		}
#else
		what += SOEXT;
#endif

		std::string full_file_name;
		if(what.size()==0){ untested();
		}else if(what[0]=='.'){ untested();
			full_file_name=what;
		}else{ untested();
			full_file_name = findfile(what, path, R_OK);
		}

		if (full_file_name != "") { untested();
			// found it, with search
		}else{untested();
			std::cerr << "cannot find plugin " + what + " in " +path + "\n";
			std::cerr << "(something wrong with installation?)\n";
			exit(1);
		}
		attach(full_file_name.c_str());

	}else{ untested();
		incomplete();
	}
}

}
