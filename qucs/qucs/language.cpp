#include "element.h"
#include "command.h"
#include "symbol.h"
#include "language.h"

void DocumentLanguage::printItem(Element const* c, stream_t& s) const
{
  assert(c);
  if (auto C=dynamic_cast<const Command*>(c)) {
    printCommand(C, s);
  }else if (auto C=dynamic_cast<const Symbol*>(c)) {
    printSymbol(C, s);
  }else{
    incomplete();
  }
}

