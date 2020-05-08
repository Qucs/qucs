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
private:
	friend class NetList; // TODO
	friend class Node; // TODO
	explicit Net() : Object(), _size(0), _pos(0) {}
	~Net(){ assert(_size==0); }
private:
	Net( const Net& ) {unreachable();}

public:
	size_t size() const{return _size;}

	void setPos(unsigned u){_pos=u;}
	unsigned getPos(){return _pos;}

	void inc_nodes(){++_size;}
	void dec_nodes(){assert(_size!=-1); --_size;}

private:
	size_t _size; // number of Conductors
	unsigned _pos; // position
};
