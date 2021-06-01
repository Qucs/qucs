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
// run a simulator.

#include "command.h"
#include "qucs_globals.h"
#include "settings.h"
#include "simulator.h"
#include "qio.h"
/*--------------------------------------------------------------------------*/
namespace{
/*--------------------------------------------------------------------------*/
using qucs::Simulator;
using qucs::SimCtrl;
/*--------------------------------------------------------------------------*/
class Simulate : public Command{
	void do_it(istream_t& cs, ElementList* s) override;
}c;
Dispatcher<Command>::INSTALL p0(&qucs::command_dispatcher, "simulate", &c);
/*--------------------------------------------------------------------------*/
struct mySimCtrl : SimCtrl{
	mySimCtrl(Simulate* r) : _r(r){}
	void stateChange() override{ incomplete(); }
	void message(int level, std::string msg) override {itested();
		if(level < _lvl){
		}else if(level == qucs::MsgDebug){ untested();
			std::cout << "debug " << msg << "\n";
		}else if(level == qucs::MsgLog){ untested();
			std::cout << "log " << msg << "\n";
		}else{ untested();
			std::cout << "other " << msg << "\n";
		}
	}
	Simulate* _r;
	int _lvl{qucs::MsgLog};
   void setSimulator(qucs::Simulator* sim){incomplete();} // BUG
};
/*--------------------------------------------------------------------------*/
void Simulate::do_it(istream_t& cmd, ElementList* sckt)
{itested();
	assert(sckt);
	std::string fn;
	int picky = -1;
	cmd >> "simulate";
	size_t here = cmd.cursor();
	do{
		ONE_OF
      || Set(cmd, "nonag",	   &picky,	int(qucs::MsgTrace))
      || Set(cmd, "trace",	   &picky,	int(qucs::MsgTrace))
      || Set(cmd, "notrace",  &picky,	int(qucs::MsgLog))
      || Set(cmd, "log",	   &picky,	int(qucs::MsgLog))
      || Set(cmd, "nolog",	   &picky,	int(qucs::MsgDebug))
      || Set(cmd, "debug",	   &picky,	int(qucs::MsgDebug));
		// [...]
	}while (cmd.more() && !cmd.stuck(&here));
	cmd >> fn;

	std::string which = "qucsator";
	if(auto s = QucsSettings.simulator()){
		which = s->label();
	}else{ untested();
	}

	auto f = sckt->find_(which);
	Simulator* sim = nullptr;

   if(f == sckt->end()){itested();
      Simulator const* proto = QucsSettings.simulator();
      if(which!=""){itested();
         auto p = qucs::data_dispatcher[which];
			proto = dynamic_cast<Simulator const*>(p);
      }else{ untested();
      }

		assert(proto);
      sim = proto->clone();
		sckt->push_back(sim);
   }else if((sim = dynamic_cast<Simulator*>(*f))){ untested();
	}else{ untested();
		incomplete();
	}

	mySimCtrl ctx(this);
	if(picky!=-1){
		ctx._lvl = picky;
	}else{ untested();
	}

	if(sim){
		sim->attachCtrl(&ctx);
		untested();
		sim->do_it(cmd, sckt);
		untested();
		sim->detachCtrl(&ctx);
		untested();
	}else{ untested();
		incomplete();
	}
}
/*--------------------------------------------------------------------------*/
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
