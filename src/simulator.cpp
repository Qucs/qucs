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
	}else{
	}
}
/* -------------------------------------------------------------------------------- */
Simulator::Simulator()
  : _doc(nullptr),
    _data_p(nullptr),
    _state(sst_idle),
    _ctrl(nullptr)
{
}
/* -------------------------------------------------------------------------------- */
void Simulator::attachCtrl(SimCtrl* ctrl)
{ untested();
  if(!_ctrl){ untested();
    _ctrl = ctrl;
  }else if(_ctrl==ctrl){ untested();
  }else{ untested();
    throw Exception("already controlled");
  }
}
/* -------------------------------------------------------------------------------- */
void Simulator::detachCtrl(SimCtrl const* ctrl)
{ untested();
  if(_ctrl == ctrl){ untested();
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
{ untested();
	if(_ctrl){ untested();
		_ctrl->message(lvl, msg);
	}else{ untested();
		Object::message(lvl, msg.c_str());
	}
}
/* -------------------------------------------------------------------------------- */
