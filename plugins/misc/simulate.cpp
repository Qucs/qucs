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
#include "globals.h"
#include "settings.h"
#include "simulator.h"
/*--------------------------------------------------------------------------*/
namespace{
/*--------------------------------------------------------------------------*/
class Simulate : public Command{
	void do_it(istream_t& cs, SchematicModel* s) override;
}c;
Dispatcher<Command>::INSTALL p0(&command_dispatcher, "simulate", &c);
/*--------------------------------------------------------------------------*/
struct mySimCtrl : SimCtrl{
	mySimCtrl(Simulate* r) : _r(r){}
	void stateChange() override{ incomplete(); }
	void message(int level, std::string msg) override { untested();
		std::cout << level << " " << msg << "\n";
	}
	Simulate* _r;
};
/*--------------------------------------------------------------------------*/
void Simulate::do_it(istream_t& cmd, SchematicModel* sckt)
{ untested();
	assert(sckt);
	std::string fn;
	cmd >> "simulate";
	cmd >> fn;

	std::string which = "qucsator"; // for now

	auto f = sckt->find_(which);
	Simulator* sim = nullptr;

   if(f == sckt->end()){ untested();
      Simulator const* proto = QucsSettings.simulator();
      if(which!=""){ untested();
         proto = simulator_dispatcher[which];
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

	if(sim){ untested();
		sim->attachCtrl(&ctx);
		sim->do_it(cmd, sckt);
		sim->detachCtrl(&ctx);
	}else{ untested();
		incomplete();
	}
}
/*--------------------------------------------------------------------------*/
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
