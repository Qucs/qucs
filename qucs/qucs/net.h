/***************************************************************************
    copyright            : (C) 2020 Felix Salfelder
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "object.h"
class Net : public Object{
public:
	explicit Net() : Object(), _size(0) {}
private:
	Net( const Net& ) {unreachable();}

public:
	size_t size() const{return _size;}
	size_t& size_hack(){return _size;}

	void setPos(unsigned u){_pos=u;}
	unsigned getPos(){return _pos;}

private:
	size_t _size; // number of Conductors
	unsigned _pos; // position
};
