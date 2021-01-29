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
{itested();
  if(_ctrl == ctrl){itested();
	 join();
    _ctrl = nullptr;
  }else{ untested();
    _ctrl = nullptr;
	 // throw?
	  unreachable();
  }
}
/* -------------------------------------------------------------------------------- */
void Simulator::message(QucsMsgType lvl, std::string const& msg)
{itested();
	if(_ctrl){itested();
		_ctrl->message(lvl, msg);
	}else{ untested();
		Object::message(lvl, msg.c_str());
	}
}
/* -------------------------------------------------------------------------------- */
void Simulator::notifyState(Simulator::state_t st)
{
	setState(st);
	if(_ctrl){itested();
		_ctrl->stateChange();
	}else{ untested();
	}
}
/* -------------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------------- */
