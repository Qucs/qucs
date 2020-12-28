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
#include "object.h"
/* -------------------------------------------------------------------------------- */
// data structure to deal with what a simulator outputs.
// this is organized as a tree with pluggable directories.
// the leaves are waveforms or parameter sets or something like that.
/* -------------------------------------------------------------------------------- */
// base class for output related classes
class QucsData : public Object{
private:
	QucsData(QucsData const&) = delete;
public:
	explicit QucsData() : Object(), _attach_count(0){}
	virtual ~QucsData() {}
	Type type() {return Object::Type::QucsData;}
public:
	virtual QucsData* clone() { return NULL; }
protected:
	virtual QucsData* resolve(const std::string&){assert(false); return nullptr;}

public:
	static void attach(QucsData*, QucsData**);
	static void detach(QucsData**);
	virtual QucsData const* refresh(){unreachable(); return nullptr;}

private:
	unsigned _attach_count;
};
/* -------------------------------------------------------------------------------- */
// borrowed from e_compon
inline void QucsData::attach(QucsData* d, QucsData** to)
{
	incomplete();
	assert(to);
	if (d == *to) {
		// The new and old are the same object.  Do nothing.
	}else if (!d) {untested();
		// There is no new common.  probably a simple element
		detach(to);
	}else if (!*to) {
		// No old one, but have a new one.
		++(d->_attach_count);
		trace1("++1", d->_attach_count);
		*to = d;
#if 0
	}else if (*d != **to) {
		// They are different, usually by edit.
		detach_common(to);
		++(d->_attach_count);
		trace1("++2", d->_attach_count);
		*to = d;
#endif
	}else if (d->_attach_count == 0) {
		// The new and old are identical.
		// Use the old one.
		// The new one is not used anywhere, so throw it away.
		trace1("delete", d->_attach_count);    
		delete d;
	}else{untested();
		// The new and old are identical.
		// Use the old one.
		// The new one is also used somewhere else, so keep it.
	}
}
/* -------------------------------------------------------------------------------- */
// borrowed from e_compon
inline void QucsData::detach(QucsData** from)
{
	assert(from);
	if (*from) {
		assert((**from)._attach_count > 0);
		--((**from)._attach_count);
		trace1("--", (**from)._attach_count);
		if ((**from)._attach_count == 0) {
			trace1("delete", (**from)._attach_count);
			delete *from;
		}else{
			trace1("nodelete", (**from)._attach_count);
		}
		*from = NULL;
	}else{
	}
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
	Type type() const override {return Object::Type::SimOutputDir;}

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
	void pushBack(QucsData*);

protected:
	mutable /*why?*/ std::map<std::string, QucsData*> _d;
}; // SimOutputDir
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
	Object::Type_T type() const override {return Object::Type_T::SimOutputRoot;}

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
		SimOutputRoot* _d{nullptr};
		const std::string _name;
		SimOutputDir* _p{nullptr};
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
