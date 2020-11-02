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
#ifndef QUCS_SIMOUTPUT_H_
#define QUCS_SIMOUTPUT_H_
/* -------------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------------- */
#include <map>
#include <assert.h>
#include <iostream>
#include "io_trace.h"
/* -------------------------------------------------------------------------------- */
// data structure to deal with what a simulator outputs.
// this is organized as a tree with pluggable directories.
// the leaves are waveforms or parameter sets or something like that.
/* -------------------------------------------------------------------------------- */
// base class for output related classes
class QucsData {
private:
	QucsData(QucsData const&) = delete;
public:
	explicit QucsData(): _attach_count(0){}
public:
	virtual QucsData* clone() { return NULL; }
protected:
	virtual QucsData* resolve(const std::string&){assert(false);}

public:
	static void attach(QucsData*, QucsData**);
	static void detach(QucsData**);

private:
	unsigned _attach_count;
};
/* -------------------------------------------------------------------------------- */
inline void QucsData::attach(QucsData* what, QucsData** where)
{
	incomplete();
	assert(!*where); // for now.
	*where = what;
	++(what->_attach_count);
}
/* -------------------------------------------------------------------------------- */
inline void QucsData::detach(QucsData** from)
{
	attach(nullptr, from);
}
/* -------------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------------- */
// base class for SimOutputs
// a SimOutputDir provides a named list of QucsData
// these can again be "directories", or data sets
class SimOutputDir : public QucsData {
protected:
	explicit SimOutputDir(){ }
	SimOutputDir(const SimOutputDir& s) : _d(s._d) { untested(); }

public:
	QucsData* clone() override{return new SimOutputDir(*this);}
	typedef std::map<std::string, QucsData*> container_t;
	virtual ~SimOutputDir(){}

public:
	virtual void set_var(std::string, std::string) { untested(); }
	class const_iterator : public container_t::iterator{
	public:
		const_iterator(const const_iterator& p) : container_t::iterator(p) { untested(); }
		const_iterator(const container_t::iterator& p) : container_t::iterator(p) {}
		const_iterator& operator=(container_t::iterator& p){ untested();
			container_t::iterator::operator=(p);
			return *this;
		}
		const SimOutputDir* dir() const{
			return dynamic_cast<const SimOutputDir*>(container_t::iterator::operator*().second);
		}
		const container_t::value_type operator*(){ untested(); return container_t::iterator::operator*(); }
	};
	class iterator : public container_t::iterator{
	public:
		iterator(const const_iterator& p) : container_t::iterator(p) { untested(); }
		iterator(const container_t::iterator& p) : container_t::iterator(p) {}
		iterator& operator=(container_t::iterator& p){ untested();
			container_t::iterator::operator=(p);
			return *this;
		}
		SimOutputDir* dir(){
			return dynamic_cast<SimOutputDir*>(container_t::iterator::operator*().second);
		}
		container_t::value_type operator*(){ untested(); return container_t::iterator::operator*(); }
	};
	virtual const_iterator begin() const { return _d.begin(); }
	const_iterator end() const { return _d.end(); }
	iterator begin() { untested(); return _d.begin(); }
	iterator end() { untested(); return _d.end(); }
	size_t size() const {
		return _d.size();
	}
	const QucsData* operator[](const_iterator& x){
		if(!x->second){
			x->second = resolve(x->first);
		}
		return x->second;
	}
protected:
	mutable std::map<std::string, QucsData*> _d;
};
/* -------------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------------- */
// namespace{
// the root directory. here,
// output plugins register as directories
class SimOutputRoot : public SimOutputDir {
private:
	SimOutputRoot(const SimOutputRoot& s) : SimOutputDir(s){
		for(auto i : _d){ untested();
			i.second=i.second->clone();
		}
	}
public:
	QucsData* clone(){return new SimOutputRoot(*this);}

public:
	SimOutputRoot(){ }
	virtual void set_var(std::string n, std::string v) { untested();
		for(auto i = begin(); i!=end(); ++i){ untested();
			assert(i.dir());
			i.dir()->set_var(n,v);
		}
	}
	void install(const std::string& s, QucsData* p){
		assert(p);
		_d[s] = p;
		trace2("installing ", s, _d.size());
		trace1("installing ", intptr_t(this));
	}
	void uninstall(QucsData*){ /* incomplete */ }
	class INSTALL {
	private:
		SimOutputRoot* _d;
		const std::string _name;
		SimOutputDir* _p;
	public:
		INSTALL(SimOutputRoot* d, const std::string& name, SimOutputDir* p) :
			_d(d),
			_name(name),
			_p(p)
		{
			std::cerr << "INSTALL\n";
			assert(_d);
			assert(p);
			_d->install(_name, p);
		}
		~INSTALL() {
			_d->uninstall(_p);
		}
	};
private:
	// std::map<std::string, QucsData*> *_d;
};
/* -------------------------------------------------------------------------------- */
// (dictionary) data from a simulator, maybe later.
class SimOutputParams : public QucsData{
};
/* -------------------------------------------------------------------------------- */
#endif
