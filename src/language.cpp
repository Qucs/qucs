/***************************************************************************
    copyright            : (C) 2018, 2020 Felix Salfelder
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include "command.h"
#include "element.h"
#include "qucs_globals.h"
#include "task_element.h"
#include "element_list.h"
#include "symbol.h"
#include "painting.h"
#include "language.h"
#include "dot.h"
#include "diagram.h"
#include "sckt_base.h"
#include "data.h"
#include "qio.h"

#define DEV_DOT Dot
/*--------------------------------------------------------------------------*/
namespace qucs {
/*--------------------------------------------------------------------------*/
Element* Language::parseItem(istream_t& s, Element* c) const
{ untested();
  if (DEV_DOT* d = dynamic_cast<DEV_DOT*>(c)) { untested();
    return parseCommand(s, d);
  }else{ untested();
	  incomplete();
	  return nullptr;
  }
#if 0
  assert(c);
  if (c->is_device()){
    assert(dynamic_cast<COMPONENT*>(c));
    return parse_instance(cmd, prechecked_cast<COMPONENT*>(c));
  }else if (BASE_SUBCKT* s = dynamic_cast<BASE_SUBCKT*>(c)) {
    return parse_module(cmd, s);
  }else if (MODEL_CARD* m = dynamic_cast<MODEL_CARD*>(c)) {untested();
    return parse_paramset(cmd, m);
  }else if (DEV_COMMENT* com = dynamic_cast<DEV_COMMENT*>(c)) {
    return parse_comment(cmd, com);
  }else if (DEV_DOT* d = dynamic_cast<DEV_DOT*>(c)) {
    return parse_command(cmd, d);
  }else{untested();
    incomplete();
    unreachable();
    return NULL;
  }
#endif
}
/*--------------------------------------------------------------------------*/
void Language::printItem(ostream_t& s, Element const* c) const
{
	if(!c){ untested();
		s << "unreachable, no item\n";
		// assert(c);
	}else if (auto C=dynamic_cast<const TaskElement*>(c)) {
		printElement(C, s);
	}else if (auto C=dynamic_cast<const SubcktBase*>(c)) {
		if(C->is_device()){
			printSymbol(C, s);
		}else{
			printSubckt(C, s);
		}
	}else if (auto C=dynamic_cast<const Symbol*>(c)) {
		printSymbol(C, s);
	}else if (auto C=dynamic_cast<const DEV_DOT*>(c)) {
		print_command(s, C);
	}else if (auto d=dynamic_cast<const Diagram*>(c)) {
		printDiagram(d, s);
	}else if (auto d=dynamic_cast<const Data*>(c)) {
		printElement(d, s);
	}else if (auto p=dynamic_cast<const Painting*>(c)) {
		printPainting(p, s);
	}else{
		incomplete();
	}
}
/*--------------------------------------------------------------------------*/
// borrowed from u_lang.h
void Language::new__instance(istream_t& cmd, Element* owner,
		ElementList* Scope) const
{
	assert(Scope);
	if (cmd.is_end()) { untested();
		return;
	}else{
		std::string type = find_type_in_string(cmd);
		trace3("new__instance", type, cmd.fullString(), Scope);
		if (const Element* proto = find_proto(type, owner)) {
			if (auto p = dynamic_cast<DEV_DOT const*>(proto)){
				trace0("new__instance found dot .. ");
				DEV_DOT* new_instance = p->clone();
				delete p;

				{ // get rid of it, eventually.
				//	assert(owner);
				//	assert(owner->scope() == Scope);
					new_instance->set_scope(Scope);
					if(new_instance->owner()){
						assert(Scope == new_instance->owner()->scope());
					}
				}

				new_instance->set_owner(owner); // owner is null, usually.
				parseItem(cmd, new_instance);
			}else if (Element* new_instance = proto->clone_instance()) {
				trace2("new__instance no dot .. ", cmd.fullString(), owner);
				new_instance->set_owner(owner); // owner is null, usually.
				Element* o = parseItem(cmd, new_instance);

				if (Element* x=dynamic_cast<Element*>(o)) {
					Scope->push_back(x);
				}else{
				}
			}else{ untested();
				cmd.warn(bDANGER, type + ": incomplete prototype");
			}
		}else{
			cmd.warn(bDANGER, type + ": no match");
		}
	}
}
/*--------------------------------------------------------------------------*/
Element const* Language::find_proto(const std::string& Name, const Element* Scope) const
{
	trace1("find_proto", Name);
	Element const* p=nullptr;
	if(Scope){
		try {
			p = Scope->find_looking_out(Name);
			trace2("found", Name, p);
		}catch (qucs::ExceptionCantFind& e) {
			assert(!p);
		}
	}else{
		// top_level? not reachable from here.
#if 0
    CARD_LIST::const_iterator i = CARD_LIST::card_list.find_(Name);
    if (i != CARD_LIST::card_list.end()) { untested();
      p = *i;
    }else{ untested();
      assert(!p);
    }
#else
  }
#endif
  if (p) {
    return p;
  }else if (auto cmd = command_dispatcher[Name]) {
    auto d = new DEV_DOT;	//BUG// memory leak
	 trace1("DOT", Name);
	 d->set(cmd); // really?
	 return d;
  }else if ((p = element_dispatcher[Name])) {
	  // TaskElements are here... (move to Data?)
    return p;
  }else if ((p = symbol_dispatcher[Name])) {
    return p;
//  }else if ((p = painting_dispatcher[Name])) {
//    return p;
  }else if ((p = diagram_dispatcher[Name])) {
    return p;
  }else if ((p = data_dispatcher[Name])) {
    return p;
  }else{
	  trace1("try more", Name);
#if 0
    assert(!p);
    std::string s;
    /* */if (Umatch(Name, "b{uild} "))      {untested(); s = "build";}
    else if (Umatch(Name, "del{ete} "))     {		 s = "delete";}
    else if (Umatch(Name, "fo{urier} "))    {untested(); s = "fourier";}
    else if (Umatch(Name, "gen{erator} "))  {		 s = "generator";}
    else if (Umatch(Name, "inc{lude} "))    {untested(); s = "include";}
    else if (Umatch(Name, "l{ist} "))       {untested(); s = "list";}
    else if (Umatch(Name, "m{odify} "))     {untested(); s = "modify";}
    else if (Umatch(Name, "opt{ions} "))    {            s = "options";}
    else if (Umatch(Name, "par{ameter} "))  {untested(); s = "param";}
    else if (Umatch(Name, "pr{int} "))      {untested(); s = "print";}
    else if (Umatch(Name, "q{uit} "))       {untested(); s = "quit";}
    else if (Umatch(Name, "st{atus} "))     {            s = "status";}
    else if (Umatch(Name, "te{mperature} ")){untested(); s = "temperature";}
    else if (Umatch(Name, "tr{ansient} "))  {            s = "transient";}
    else if (Umatch(Name, "!"))		    {untested(); s = "system";}
    else if (Umatch(Name, "<"))		    {untested(); s = "<";}
    else if (Umatch(Name, ">"))		    {untested(); s = ">";}
    else{ /* no shortcut available */
      s = Name;
    }
    if ((command_dispatcher[s])) { untested();
      return new DEV_DOT; //BUG// we will look it up twice, //BUG// memory leak
		 // d->set(cmd);?
    }else{ untested();
      return NULL;
    }
#endif
  }
      return NULL;
}
/*--------------------------------------------------------------------------*/
} // qucs
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
