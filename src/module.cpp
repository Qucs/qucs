/***************************************************************************
    copyright            : (C) 2009 by Stefan Jahn
    refactored           : (C) 2016 Felix Salfelder
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include <QHash>
#include <QString>
#include <QStringList>
#include <QList>

#include "command.h"
#include "message.h"
#include "element.h"
#include "io_trace.h"
#include "module.h"
#include "qio.h"
#include "qucs_globals.h"
#include "sckt_base.h"
#include "symbol.h"

// why "Module"? this is supposed about Element listings in the gui.

namespace qucs {

using qucs::SubcktBase;

Module::INSTALL::INSTALL(const std::string& cat, Element const* p) :
		_cat(cat),
		_p(p)
{
	if(!p){
		unreachable();
	}else{
		registerElement(QString::fromStdString(cat), p);
		if(p->label()==""){
		}else if(auto pp=dynamic_cast<qucs::Component const*>(p)){
			std::string s = "protos add " + cat + " " + pp->dev_type();
			trace1("protos", s);

			// not yet
			Command::command(s, &ElementList::card_list);
		}else{
			trace0("protos, no label");
		}
	}
}

// Global category and component lists.
QHash<QString, Module *> Module::Modules;
Categories Category::categories;

// register Elements to the gui.
// the implementation is legacy, but the interface might survive.
INTERFACE void guiRegisterElement (std::string const& cat, Element const* e)
{ untested();
	Module::registerElement(QString::fromStdString(cat), e);
}

// Destructor removes instance of module object from memory.
Module::~Module () { untested();
}

// Module registration using a category name and the appropriate
// function returning a modules instance object.
void Module::registerElement (QString category, Element const* e)
{
  Module * m = new Module (e);
  // m->category = category; // OUCH, incomplete?
  intoCategory(category.toStdString(), m);
}

// The function appends the given module to the appropriate category.
// If there is no such category yet, then the category gets created.
void Module::intoCategory(std::string const& cat, Module * m)
{
  QString category = QString::fromStdString(cat);

  // look through existing categories
  // // BUG linear search
  QList<Category *>::const_iterator it;
  for (it = Category::categories.constBegin();
       it != Category::categories.constEnd(); it++) {
    if ((*it)->name() == category) {
      (*it)->push_back(m);
      break;
    }
  }

  // if there is no such category, then create it
  if (it == Category::categories.constEnd()) {
    Category *cat = new Category (category);
    Category::categories.append (cat);
    cat->push_back(m);
  }
}

// obsolete. hook into clear
void Module::unregisterModules (void)
{ untested();
  while(!Category::categories.isEmpty()) { untested();
    incomplete();
//    delete Category::categories.takeFirst();
  }

  //remove all modules by iterator, require in qhash
  QHashIterator<QString, Module *> it( Modules );
  while(it.hasNext()) { untested();
    it.next();
    delete it.value();
  }
  Modules.clear ();
}

// Constructor creates instance of module object.
Category::Category () { untested();
  Name = "#special";
  while(!Content.isEmpty()) { untested();
    delete Content.takeFirst();
  }
}

Category::Category (const QString name)
{
  Name = name;
  while(!Content.isEmpty()) { untested();
    delete Content.takeFirst();
  }
}

Category::~Category ()
{ untested();
  while(!Content.isEmpty()) { untested();
    delete Content.takeFirst();
  }
}

// Return the available category names in a list of strings.
QStringList Category::getCategories (void)
{itested();
  QStringList res;
  QList<Category *>::const_iterator it;
  for (it = Category::categories.constBegin(); 
       it != Category::categories.constEnd(); it++) {itested();
    res.append ((*it)->Name);
  }
  return res;
}
/*--------------------------------------------------------------------------*/
// Return the registered modules in the given category
// as a pointer list. The pointer list is empty if there is no such
// category available.
QList<Module *> Category::getModules (QString category)
{itested();
  QList<Module *> res;
  QList<Category *>::const_iterator it;
  for (it = Category::categories.constBegin();
       it != Category::categories.constEnd(); it++) {itested();
    if (category == (*it)->Name) {itested();
      res = (*it)->Content;
    }
  }
  return res;
}
/*--------------------------------------------------------------------------*/
// Return the index number into the category list for the given
// category name.  The function returns minus 1 if there is no such
// category.
int Category::getModulesNr (QString category)
{itested();
  for (int i = 0; i < Category::categories.size(); i++) {itested();
    if (category == Category::categories.at(i)->Name)
      return i;
  }
  return -1;
}
/*--------------------------------------------------------------------------*/
// group element prototypes by categories.
class Protostash : public Command{
public:
	~Protostash(){ incomplete(); }
private:
	SubcktBase* open(std::string what, ElementList* scope){
		auto p_ = scope->find_(what);
		Element* a = nullptr;
		if(p_!=scope->end()){itested();
			a = *p_;
		}else{
			a = qucs::device_dispatcher.clone("subckt_proto");
			a->set_label(what);
			scope->push_back(a);
		}
		auto sckt = prechecked_cast<SubcktBase*>(a);
		return sckt;
	}

	void do_it(istream_t& cmd, ElementList* scope){
//		assert(scope == &ElementList::card_list); // todo
		scope = &ElementList::card_list;

		cmd >> "protos";
		if(cmd >> "add"){
			add(cmd, scope);
		}else{
		}
	}

	void add(istream_t& cmd, ElementList* scope){
		std::string where;
		std::string what;
		cmd >> where;
		cmd >> what;

		SubcktBase* a = open("protos", scope);
		assert(a);
		scope = a->scope();
		assert(scope);

		SubcktBase* cat = open(where, scope);
		assert(cat);
		scope = cat->scope();

		Element* proto = qucs::symbol_dispatcher.clone(what);
		if(proto){
	//		guiRegisterElement(where, proto);
			scope->push_back(proto);
		}else{ untested();
			message(qucs::MsgWarning, "no proto >" + what + "<");
		}
	}
}p1;
Dispatcher<Command>::INSTALL d1(&qucs::command_dispatcher, "protos", &p1);
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
} // qucs
/*--------------------------------------------------------------------------*/
