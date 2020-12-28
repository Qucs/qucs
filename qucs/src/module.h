/***************************************************************************
    copyright            : (C) 2009 by Stefan Jahn
                               2016, 18, 20 Felix Salfelder
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
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
#include "io_trace.h"
#include <assert.h>

class Element;

// function typedefs for circuits and analyses
//typedef Element * (* pInfoFunc) (QString&, char * &, bool);
#if 0 // pointless
typedef Element * (* pInfoVAFunc) (QString&, QString&, bool, QString);
//typedef Component * (* pCreatorFunc) ();
#endif

void guiRegisterElement (std::string const& category, Element const*);

/*!
 * \brief The Module class
 * sort of element wrapper for stuff displayed in the select menu.
 * Each module is part of a Category. see below.
 */
class Module {
public:
	Module (Element const* e) : _e(e) { }

	~Module ();

public: // BUG: this is not part of Module
//	static void registerComponent (QString, pInfoFunc);
	static void registerElement (QString, Element const*);
	static void intoCategory( std::string const& cat, Module *);

public:
	static QHash<QString, Module *> Modules;
	static QMap<QString, QString> vaComponents;

public:
	static void registerModules (void);
	static void unregisterModules (void);

	bool has_element() const{ return _e; }
	Element const* element() const{ return _e; }

public:
	// TODO: this is somewhat redundant...
        /*!
         * \brief The INSTALL class
         * Install element into a specified category
         * Every element must contain a variable of this class inside their source file (see
         * capacitor.cpp for example)
         * So the element gets registered and Qucs knows they are available and can be used in
         * the simulator. So easily parts can be removed and added.
         */
	class INSTALL {
	private:
		const std::string _cat;
		Element const* _p;
	public:
                /*!
                 * \brief INSTALL
                 * Install the element \p p into category \p cat
                 * \param cat
                 * \param p
                 */
		INSTALL(const std::string& cat, Element const* p) :
			_cat(cat),
			_p(p)
		{
			if(p){
				registerElement(QString::fromStdString(cat), p);
			}else{
				unreachable();
			}
		}

		~INSTALL() {
			// remove stuff from GUI display
			// incomplete();
		}
	};

private:
	Element const* _e;
	QString category; // why this?!
};

class Category;
class Categories : public QList<Category*>{};

// a category. something like "lumped" or "simulations"
// will show up in the dropdown thing.
class Category {
public:
	Category ();
	Category (const QString);
	~Category ();

public:
	static Categories categories;

public:
	static QStringList getCategories (void);
	static QList<Module *> getModules (QString);
	static int getModulesNr (QString);

public:
	virtual QString const& name(){return Name;}
	Module* operator[](unsigned x) const{
		return Content.at(x);
	}
    /*!
     * \brief push_back
     * Add new module to category.
     * \param x
     */
	void push_back(Module* x){
		// incomplete(); // must refresh gui.
		Content.append(x);
	}

private: // internal
	QString Name;
	QList<Module *> Content;
};

#endif /* __MODULE_H__ */
