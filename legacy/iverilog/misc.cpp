
stuff found that related to "digital simulation"

void ElementList::collectDigitalSignals(void)
{
  incomplete();
// Node *pn=nullptr;
//
//  for(pn = nodes().first(); pn != 0; pn = nodes().next()) {
//    DigMap::Iterator it = Signals.find(pn->name());
//    if(it == Signals.end()) { // avoid redeclaration of signal
//      Signals.insert(pn->name(), DigSignal(pn->name(), pn->DType));
//    } else if (!pn->DType.isEmpty()) {
//      it.value().Type = pn->DType;
//    }
//  }
}

//#define VHDL_SIGNAL_TYPE "bit"
//#define VHDL_LIBRARIES   ""
static const std::string VHDL_SIGNAL_TYPE("std_logic");
static const std::string VHDL_LIBRARIES("\nlibrary ieee;\nuse ieee.std_logic_1164.all;\n");
