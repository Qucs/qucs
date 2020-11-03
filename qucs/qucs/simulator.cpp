/***************************************************************************
    copyright            : (C) 2015, 2020 by Felix Salfelder
    email                : felix@salfelder.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
/* -------------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------------- */
#include "simulator.h"
#include "qucsdoc.h"
#include "io_error.h"
/* -------------------------------------------------------------------------------- */
Simulator::~Simulator()
{
	if(_data_p){ untested();
		QucsData::detach(_data_p);
	}else{ untested();
	}
}
/* -------------------------------------------------------------------------------- */
// possibly wrong.
// void Simulator::attachData(QucsData** d)
// {
// 	assert(*d);
// 	assert(!_data_p);
// 	_data_p = d;
// }
/* -------------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------------- */
#include "simulator.h"

Simulator::Simulator()
  : _doc(nullptr),
    _data_p(nullptr),
    _state(sst_idle),
    _ctrl(nullptr)
{
}
/* -------------------------------------------------------------------------------- */
void Simulator::attachCtrl(SimCtrl* ctrl)
{
  if(!_ctrl){
    _ctrl = ctrl;
  }else if(_ctrl==ctrl){
  }else{
    throw Exception("already controlled");
  }
}
/* -------------------------------------------------------------------------------- */
void Simulator::detachCtrl(SimCtrl const* ctrl)
{
  if(_ctrl == ctrl){ untested();
    _ctrl = nullptr;
  }else{ untested();
    _ctrl = nullptr;
	 // throw?
	  unreachable();
  }
}
/* -------------------------------------------------------------------------------- */
void Simulator::message(QucsMsgType lvl, std::string const& msg)
{
	if(_ctrl){
		_ctrl->message(lvl, msg);
	}else{
		Object::message(lvl, msg.c_str());
	}
}
