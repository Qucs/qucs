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
#include "exception.h"
/* -------------------------------------------------------------------------------- */
namespace qucs {
/* -------------------------------------------------------------------------------- */
Simulator::~Simulator()
{
}
/* -------------------------------------------------------------------------------- */
Simulator::Simulator()
  : Data(),
    _doc(nullptr),
    _state(sst_idle),
    _ctrl(nullptr)
{
}
/* -------------------------------------------------------------------------------- */
void Simulator::attachCtrl(SimCtrl* ctrl)
{itested();
  if(!_ctrl){itested();
    _ctrl = ctrl;
  }else if(_ctrl==ctrl){ untested();
  }else{ untested();
    throw qucs::Exception("already controlled");
  }
}
/* -------------------------------------------------------------------------------- */
void Simulator::detachCtrl(SimCtrl const* ctrl)
{
  if(_ctrl == ctrl){
	 join();
    _ctrl = nullptr;
  }else{ untested();
    _ctrl = nullptr;
	 // throw?
	  unreachable();
  }
}
/* -------------------------------------------------------------------------------- */
void Simulator::message(MsgType lvl, std::string const& msg)
{
	if(_ctrl){
		_ctrl->message(lvl, msg);
	}else{ untested();
		Element::message(lvl, msg.c_str());
	}
}
/* -------------------------------------------------------------------------------- */
void Simulator::notifyState(Simulator::state_t st)
{
	setState(st);
	if(_ctrl){
		_ctrl->stateChange();
	}else{ untested();
	}
}
/* -------------------------------------------------------------------------------- */
} // qucs
/* -------------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------------- */
