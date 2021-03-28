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
#include <QDebug>

#include "element.h"
#include "module.h"
#include "io_trace.h"
#include "qucs_globals.h"

namespace qucs {
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

// waah. need one plugin per symbol list.
void Module::registerModules (void) {itested();
  unreachable(); // obsolete.
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
/*--------------------------------------------------------------------------*/
} // qucs
/*--------------------------------------------------------------------------*/
// vim:ts=8:sw=2:noet
