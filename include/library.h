
#ifndef QUCS_L_LIB
#define QUCS_L_LIB
/*--------------------------------------------------------------------------*/
#include <string>
#include "platform.h"
#include "message.h"
#include "io_trace.h"
#include "exception.h"
#include <cstring> // strdup?
#include <assert.h>
/*--------------------------------------------------------------------------*/
#define error message
/*--------------------------------------------------------------------------*/
std::string to_string(int);
/*--------------------------------------------------------------------------*/
bool		Umatch(const std::string&, const std::string&);
/*--------------------------------------------------------------------------*/
// temporary stuff.
namespace OS{
  inline std::string getenv(const std::string& s) {
    char* ev = ::getenv(s.c_str());
    if (ev) {
      return ev;
    }else{itested();
      return "";
    }
  }
  inline void setenv(const std::string& Name, const std::string& Value, bool Overwrite=true) {
    if (Name == "") {untested();
      throw qucs::Exception("setenv: bad name " + Name);
    }else if (Value == "") {untested();
      throw qucs::Exception("setenv: bad value " + Value);
    }else if (!Overwrite && getenv(Name) != "") {
      error(bDEBUG, "setenv: " + Name + " overwrite prohibited");
    }else{
      std::string ev = Name + "=" + Value;
      char *es = ::strdup(ev.c_str());	//BUG// memory leak
      assert(es);
      if (::putenv(es) != 0) {untested();
	throw qucs::Exception("");
      }else{
      }
    }
  }
}
#endif
