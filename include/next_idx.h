/***************************************************************************
    copyright            : 2018, 2020 Felix Salfelder
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include "element_list.h"
#include "element.h"
#include "qio.h"

using qucs::ElementList;

unsigned next_idx(qucs::ElementList const* l, std::string const& name)
{
	assert(l);
	unsigned r = 0;

	if(name==""){ untested();
		return r;
	}else{
		auto n = name.size();
		istream_t cs(istream_t::_STRING, name + "*");

		auto L=const_cast<qucs::ElementList*>(l);
		ElementList::fat_iterator j = findbranch(cs, L);

		while (j!=j.end()){
			if((*j)->label().substr(0, n) == name){
				auto str = (*j)->label().substr(n);
				trace3("nextIdx", (*j)->label(), n, str);
				unsigned z;
				if(sscanf(str.c_str(), "%d", &z) == 1){ itested();
					r = std::max(z, r);
				}else{ itested();
				}

				++j; // kludge: linear search.
				findbranch(cs, j);
			}else{
				break;
			}
		}

	}

	auto oldidx = l->obsolete_nextIdx(name);
	assert(oldidx==r+1);

	return r+1;
}

// faster but HACK... remove eventually.
unsigned ElementList::obsolete_nextIdx(std::string const& s) const
{
	unsigned r=0;
	if(s==""){ untested();
		return 0;
	}else{
		auto j = _map.lower_bound(s);
		auto n = s.size();

		while (j!=_map.end()){
			if(j->second->label().substr(0, n) == s){
				auto str = j->second->label().substr(n);
				trace3("nextIdx", j->second->label().substr(0, n), n, str);
				unsigned z;
				if(sscanf(str.c_str(), "%d", &z) == 1){ itested();
					r = std::max(z, r);
				}else{ itested();
				}

				++j;
			}else{
				break;
			}
		}
	}

	trace2("nextId", s, r);
	return r+1;
}
/*--------------------------------------------------------------------------*/
