#include "element.h"
#include "task_element.h"
#include "symbol.h"
#include "language.h"

void DocumentLanguage::parseItem(Element* c, istream_t& s) const
{
	unreachable();
}

void DocumentLanguage::printItem(Element const* c, ostream_t& s) const
{
  assert(c);
  if (auto C=dynamic_cast<const TaskElement*>(c)) {
    printtaskElement(C, s);
  }else if (auto C=dynamic_cast<const Symbol*>(c)) {
    printSymbol(C, s);
  }else{
    incomplete();
  }
}

