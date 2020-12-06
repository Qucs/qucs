// imported from gnucap.
// TODO: connect to qucs error console
//
#include <iostream>
#include <stdarg.h>

void error(int, const char* fmt, ...)
{
  char buffer[1024] = "";
  va_list arg_ptr;
  va_start(arg_ptr,fmt);
  vsprintf(buffer,fmt,arg_ptr);
  va_end(arg_ptr);
  std::cerr << buffer;
}
/*--------------------------------------------------------------------------*/
void error(int, const std::string& message)
{
	std::cerr << message;
}
