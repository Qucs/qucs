/***************************************************************************
                                 element.h
                                -----------
    begin                : 2016
    copyright            : (C) 2016 Felix Salfelder
    email                : felix@salfelder.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

/** \file symbol.h
  * \brief Defines symbols for components, to be used in schematics.
  *
  * a symbol mostly takes care of ports, parameters and connectivity.
  * (this is under construction)
  *
  */
#include <QWidget>
#ifndef SYMBOL_H
#define SYMBOL_H

/** \class Symbol
  * \brief Superclass of all circuit components (except wires).
  *
  *
  */


class Symbol {
public: // construct
  Symbol();
  virtual ~Symbol(){}

public: // interface
  virtual int draw(QWidget&) = 0;
  //...  more to come
};


#endif
