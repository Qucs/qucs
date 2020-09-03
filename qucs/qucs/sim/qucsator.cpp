/***************************************************************************
    copyright            : (C) 2015, 2019, 2020 Felix Salfelder
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

// qucsator simulator "driver"

#include "sim/sim.h"
#include "sckt_proto.h"
#include "net.h"
#include "docfmt.h" // <<
#include "paintings/paintings.h" // really??
#include "legacy/subcircuit.h" // BUG BUG
#include "globals.h"

namespace {

static std::string netLabel(Net const* n)
{ untested();
	if(!n){ untested();
		unreachable();
		return("(null)");
	}else if(n->hasLabel()){ untested();
		return n->label().toStdString();
	}else{ untested();
		return "_net" + std::to_string(n->pos());
	}
}

static std::string mangleType(std::string& t)
{ untested();
	auto pos = t.find("$");
	std::string ret="";
	if(pos == std::string::npos){ untested();
	}else{ untested();
		ret = " Type=\"" + t.substr(pos+1) + "\"";
	}
	t = t.substr(0, pos);
	return ret;
}

// qucslang language implementation
class QucsatorLang : public NetLang {
private: // NetLang
  // inline void printItem(Element const* c, stream_t& s) const;

private: // local
  void printCommand(Command const*, stream_t&) const;
  void printSymbol(Symbol const*, stream_t&) const;
  void printSubckt(SubcktProto const*, stream_t&) const;
  void printComponent(Component const*, stream_t&) const;
}qucslang;
static Dispatcher<DocumentFormat>::INSTALL p(&doclang_dispatcher, "qucsator", &qucslang);


void QucsatorLang::printSymbol(Symbol const* d, stream_t& s) const
{ untested();
	if(!d){ untested();
		incomplete();
	}else if(auto c=dynamic_cast<SubcktProto const*>(d)){ untested();
		printSubckt(c, s);
	}else if(auto c=dynamic_cast<Command const*>(d)){ untested();
		printCommand(c, s);
	}else if(auto c=dynamic_cast<Component const*>(d)){ untested();
		printComponent(c, s);
	}else{ untested();
		s << "QucsatorLang::printSymbol incomplete\n";
		incomplete();
	}
}

// partly from Schematic::createSubnetlistplain
void QucsatorLang::printSubckt(SubcktProto const* p, stream_t& s) const
{ untested();
	trace2("prinSckt", p, p->subckt());
	Symbol const* sym = p;
	std::string label = p->label().toStdString();

	s << "\n";
	if(label.c_str()[3] == '$'){
		s << ".Def:" << label.substr(4);
	}else{
		incomplete();
	}

	// print_ports();
	//
	for(unsigned i=0; sym->portExists(i); ++i){ untested();
		std::string N = netLabel(p->portValue(i));
//		if(N=="0"){ untested();
//			N = "gnd";
//		}else{ untested();
//		}
		s << " " << N;
	}
	s << "\n";

	for(auto pi : p->symbolPaintings()){ untested();
		incomplete();
		if(pi->name() == ".ID ") { untested();
			incomplete();
			s<<"TODO " << pi->label() << pi->name() << "\n";
	//		ID_Text *pid = (ID_Text*)pi;
	//		QList<SubParameter *>::const_iterator it;
	//		for(it = pid->Parameter.constBegin(); it != pid->Parameter.constEnd(); it++) { untested();
	//			s = (*it)->Name; // keep 'Name' unchanged
	//			(*tstream) << " " << s.replace("=", "=\"") << '"';
	//			}
		}else{ untested();
		}
	//		break;
	}
	//(*tstream) << '\n';

	// TODO: deduplicate.
	trace1("sckt components", &p->schematicModel());
	trace1("sckt components", sym->scope());
	assert(sym->scope());
	for(auto i : p->schematicModel().components()){ untested();
      if(!i){ untested();
			incomplete();
		}else if(i->type() == "Port"){ untested();
		}else if(i->type() == "GND"){ untested();
		}else{ untested();
			trace1("ps", i->type());
			printSymbol(i, s);
		}
	}

	s << ".Def:End\n";
}

void QucsatorLang::printCommand(Command const* c, stream_t& s) const
{itested();
	assert(c);
	s << "." << c->name() << ":" << c->label();

	//for(auto p2 : c->params())
	for(auto p2 : c->Props){ // BUG
		if(p2->name() == "Symbol") { // hack??
		}else if(p2->name()=="p" && p2->value()==""){itested();
			// unreachable
		}else{ untested();
			s << " " << p2->name() << "=\"" << p2->value() << "\"";
		}
	}
	s << '\n';
}

// print Component in qucsator language
void QucsatorLang::printComponent(Component const* c, stream_t& s) const
{ untested();
	if(c->isActive != COMP_IS_ACTIVE){ untested();
		// comment out?
		incomplete();
	}else{ untested();
	}
	assert(c);
	trace2("pc", c->label(), c->type());

	if(c->isOpen()) { untested();
		// nothing.
	}else if(c->isShort()){ untested();
		// replace by some resistors (hack?)
		incomplete();
		int z=0;
		QListIterator<Port *> iport(c->ports());
		Port *pp = iport.next(); // BUG
		unsigned k=0;
		std::string Node1 = netLabel(c->portValue(k));
		while (iport.hasNext()){ untested();
			++k;
			s << "R:" << c->label() << "." << QString::number(z++) << " "
				<< Node1 << " " << netLabel( c->portValue(k) ) << " R=\"0\"\n";
		}
	}else{ untested();

		std::string type = c->type();
		std::string hack_type = mangleType(type);

		s << type << ":" << c->label();

		Symbol const* sym=c;
		trace3("print", c->label(), sym->numPorts(), sym->label());
		for(unsigned i=0; i<sym->numPorts(); ++i){ untested();
			std::string N = netLabel(sym->portValue(i));

			s << " " << N;
		}

		for(unsigned ii=0; ii<sym->paramCount(); ++ii) { untested();
			trace3("param", c->label(), ii, sym->paramCount());
			std::string name = sym->paramName(ii);
			std::string value = sym->paramValue(ii);
			trace2("param", name, value);

			if(name==""){itested();
				incomplete(); // is_printable...
			}else if(name == "File") { untested();
				// hack
			}else if(name == "Symbol") { untested();
				// hack??
			}else{ untested();
				s << " " << name << "=\"" << value << "\"";
			}
		}

		s << hack_type;
#if 0
		if(dynamic_cast<Subcircuit const*>(c)) { untested();
			s << " Type=\"" << QString::fromStdString(c->type()) << "\"";
		}else{ untested();
		}
#endif
		s << '\n';
	}
}

// partly from Schematic::createSubnetlistplain

}//namespace
