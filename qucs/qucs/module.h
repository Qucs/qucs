/***************************************************************************
                               module.h
                              ----------
    begin                : Thu Nov 5 2009
    copyright            : (C) 2009 by Stefan Jahn
    email                : stefan@lkcc.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef MODULE_H
#define MODULE_H

#include <QDebug>
#include <QList>
#include <QHash>
#include <QMap>
#include "platform.h"

class Element;

// function typedefs for circuits and analyses
typedef Element * (* pInfoFunc) (QString&, char * &, bool);
#if 1 // pointless
typedef Element * (* pInfoVAFunc) (QString&, QString&, bool, QString);
typedef Component * (* pCreatorFunc) ();
#endif

INTERFACE void guiRegisterElement (std::string const& category, Element const*);

// sort of element wrapper for stuff displayed in the select menu.
// each module is part of a Category. see below.
class Module
{
 public:
  Module (Element const* e) : myElement(e) { }

  ~Module ();
  static void registerComponent (QString, pInfoFunc);
  static void registerElement (QString, Element const*);
  static void intoCategory (Module *);

  // static Component * getComponent (QString);

 public:
  static QHash<QString, Module *> Modules;
  static QMap<QString, QString> vaComponents;

 public:
  static void registerModules (void);
  static void unregisterModules (void);

  bool has_element() const{ return myElement; }
  Element const* element() const{ return myElement; }
 private:
  Element const* myElement;
  QString category; // why this?!

 public:
  // TODO: this is somewhat redundant...
  class INSTALL {
  private:
    const std::string _cat;
    Element const* _p;
  public:
    INSTALL(const std::string& cat, Element const* p) :
      _cat(cat),
      _p(p)
    {
		qDebug() << "INSTALL" << QString::fromStdString(cat);
      assert(p);
		registerElement(QString::fromStdString(cat), p);
    }
    
    ~INSTALL() {
		 // remove stuff from GUI display
		 incomplete();
    }
  };
};

// a category. something like "lumped" or "simulations"
// will show up in the dropdown thing.
class Category
{
 public:
  Category ();
  Category (const QString);
  ~Category ();

 public:
  static QList<Category *> Categories;

 public:
  static QStringList getCategories (void);
  static QList<Module *> getModules (QString);
  static int getModulesNr (QString);

 public:
  virtual QString const& name(){return Name;}
  Module* operator[](unsigned x) const{
	  return Content.at(x);
  }
  void push_back(Module* x){
	  incomplete(); // must refresh gui.
	  Content.append(x);
  }
 private: // internal
  QString Name;
  QList<Module *> Content;
};

#endif /* __MODULE_H__ */
