#include "element.h"
#include "command.h"
#include "symbol.h"
#include "language.h"

void DocumentLanguage::printItem(Element const* c, stream_t& s) const
{
  assert(c);
  if (auto C=dynamic_cast<const Command*>(c)) { untested();
    printCommand(C, s);
  }else if (auto C=dynamic_cast<const Symbol*>(c)) { untested();
    printSymbol(C, s);
  }else{
    incomplete();
  }
}

