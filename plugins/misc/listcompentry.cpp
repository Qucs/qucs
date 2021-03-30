
// createListNetEntry prints to stdout the available netlist formats
//
//  Prints the default component entries format for
//   - Qucs schematic
//   - Qucsator netlist
//   - Verilog netlist
//   - Verilog schematic
//
// table for quick reference and testing
#include "qucs_globals.h"
#include "command.h"
#include "module.h"
#include "task_element.h"
#include "docfmt.h"
#include "symbol.h"
#include "language.h"
#include "qio.h"
#include <QFile> // BUG

namespace {

using namespace qucs;

class createListComponentEntry : public Command {
public:
	void do_it(istream_t&, ElementList*) override;
}c;
Dispatcher<Command>::INSTALL p(&command_dispatcher, "listcompentry", &c);


void createListComponentEntry::do_it(istream_t&, ElementList*)
{
  QStringList cats = Category::getCategories ();
  QFile data("/dev/stdout");
  data.open (QFile::WriteOnly | QFile::Truncate);
  ostream_t s(&data);

  auto lang = language_dispatcher["leg_sch"];
  assert(lang);
  auto qucsatorlang = language_dispatcher["qucsator"];
  assert(qucsatorlang);
  auto verilog = language_dispatcher["verilog"];

  foreach(QString category, cats) {

    QList<Module *> Comps;
    Comps = Category::getModules(category);

    // \fixme, crash with diagrams, skip
    if(category == "diagrams") continue;

//    char * File;
    QString Name;

    foreach (Module *Mod, Comps) {
      qDebug() << "some module";
      Element const *e = Mod->element();
//      Component const *cc = prechecked_cast<Component const*>(e);
      Element *ce = e->clone();
      assert(ce);

      s << "=====" << e->label() << "=========\n";

      if(auto c = dynamic_cast<Symbol*>(ce)){
	ce->set_label("my_" + c->dev_type());
      }else if(auto c = dynamic_cast<TaskElement*>(ce)){
	ce->set_label("my_" + c->dev_type());
      }else{
	// not sure.
      }

      lang->printItem(s, ce);

      if(verilog){
			verilog->printItem(s, ce);
      }else{ untested();
      }

      qucsatorlang->printItem(s, ce);

    } // module
  } // category
}

}
