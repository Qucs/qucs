// imported from gnucap.
// TODO: connect to qucs error console
//
#include <iostream>
#include <stdarg.h>
#include "io_trace.h"
#include "qio.h"
#include "exception.h"
#include "message.h"
#include "ap.h"

#define error message

void error(int, const char* fmt, ...)
{ untested();
  char buffer[1024] = "";
  va_list arg_ptr;
  va_start(arg_ptr,fmt);
  vsprintf(buffer,fmt,arg_ptr);
  va_end(arg_ptr);
  std::cerr << buffer;
}
/*--------------------------------------------------------------------------*/
void error(int i, const std::string& message)
{
	std::cerr << "error " << i << " " << message;
}
/*--------------------------------------------------------------------------*/
namespace qucs{
/*--------------------------------------------------------------------------*/
Exception_CS::Exception_CS(const std::string& Message, const istream_t& cmd)
  :Exception(Message),
   _cmd(cmd.fullstring()),
   _cursor(cmd.cursor())
{itested();
}
/*--------------------------------------------------------------------------*/
const std::string Exception_CS::message()const
{itested();
  std::string s;
  if (_cursor < 40) {itested();
    s = _cmd.substr(0,60)
      + '\n' + std::string(_cursor, ' ') + "^ ? " + Exception::message();
  }else{untested();
    s = "... " + _cmd.substr(_cursor-36, 56)
      + "\n                                        ^ ? " + Exception::message();
  }
  return s;
}
/*--------------------------------------------------------------------------*/
} // namespace qucs
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
