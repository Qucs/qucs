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
#include "data.h"
/* -------------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------------- */
// base class for SimOutputs
// a SimOutputDir provides a named list of CommonData
// these can again be "directories", or data sets
class SimOutputDir : public CommonData {
protected:
	explicit SimOutputDir(){ }
	SimOutputDir(const SimOutputDir& s) : _d(s._d) { untested(); }

public:
	typedef std::map<std::string, CommonData*> container_t;
	virtual ~SimOutputDir(){}
	virtual void set_var(std::string, std::string) { untested(); }

private:
	CommonData* clone() override{return new SimOutputDir(*this);}

public:
	class iterator : public /*BUG*/ container_t::iterator{
	public:
//		iterator(const const_iterator& p) : container_t::iterator(p) { untested(); }
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
	class const_iterator : protected container_t::const_iterator{
	public:
		const_iterator(const_iterator const& p) : container_t::const_iterator(p) { untested(); }
		explicit const_iterator(const container_t::iterator& p) : container_t::const_iterator(p) {}
		explicit const_iterator(const container_t::const_iterator& p) : container_t::const_iterator(p) {}

	public: //ops
		CommonData const* operator->(){incomplete(); return nullptr;}
		CommonData const* operator*(){ untested();
			return container_t::const_iterator::operator*().second;
		}
		bool operator==(const_iterator const&x) const {untested();
			return container_t::const_iterator(*this)==(container_t::const_iterator(x));
		}
		bool operator!=(const_iterator const&x) const {untested();
			return container_t::const_iterator(*this)!=(container_t::const_iterator(x));
		}
		const_iterator& operator++(){untested();
			container_t::const_iterator::operator++();
			return *this;
		}
		//const container_t::value_type operator*(){ untested(); return container_t::iterator::operator*(); }

		const_iterator& operator=(container_t::const_iterator& p){ untested();
			container_t::const_iterator::operator=(p);
			return *this;
		}
//		const SimOutputDir* dir() const{
//			return dynamic_cast<const SimOutputDir*>(container_t::iterator::operator*().second);
//		}
	};
	virtual const_iterator begin() const {
		const_iterator b(_d.begin());
		return b;
	}
	const_iterator end() const {
		return const_iterator(_d.end());
	}
	iterator begin() { untested(); return _d.begin(); }
	iterator end() { untested(); return _d.end(); }
	size_t size() const {
		return _d.size();
	}
//	const CommonData* operator[](const_iterator& x){
//		if(!x->second){
//			x->second = resolve(x->first);
//		}else{
//		}
//		return x->second;
//	}
protected:
	void push_back(CommonData*);

protected:
	mutable /*why?*/ container_t _d;
}; // SimOutputDir
/* -------------------------------------------------------------------------------- */
// the root directory. here,
// output plugins register as directories
// obsolete.
class SimOutputRoot : public SimOutputDir {
private:
	SimOutputRoot(const SimOutputRoot& s) : SimOutputDir(s){
		for(auto i : _d){ untested();
			i.second=i.second->clone();
		}
	}
public:
	CommonData* clone(){return new SimOutputRoot(*this);}

public:
	SimOutputRoot(){ }
	virtual void set_var(std::string n, std::string v) { untested();
		for(auto i = begin(); i!=end(); ++i){ untested();
			assert(i.dir());
			i.dir()->set_var(n,v);
		}
	}
	void install(const std::string& s, CommonData* p){
		assert(p);
		_d[s] = p;
		trace2("installing ", s, _d.size());
		trace1("installing ", intptr_t(this));
	}
	void uninstall(CommonData*){ /* incomplete */ }
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
	// std::map<std::string, CommonData*> *_d;
};
/* -------------------------------------------------------------------------------- */
// (dictionary) data from a simulator, maybe later.
class SimOutputParams : public CommonData{
};
/* -------------------------------------------------------------------------------- */
// (tabular) data from a simulator.
class SimOutputData : public CommonData{
public:
	typedef std::pair<double, std::complex<double> > valuetype;
	class const_iterator{
		friend class SimOutputData; // need to access constructor.
		friend class SimOutputDat; // bug.
	protected:
	public:
		const_iterator(double const* x, double const* y) : seekx(x), seeky(y) {};
	public:
		const_iterator& operator++(){ ++seekx; ++seeky; ++seeky; return *this;}
		valuetype operator*(){
			return valuetype(*seekx,std::complex<double>(*seeky,seeky[1]));
		}
		const valuetype* operator->() const{
			_v = valuetype(*seekx,std::complex<double>(*seeky,seeky[1]));
			return &_v;
		}
		bool operator==(const const_iterator& p)const { return seekx==p.seekx; }
		bool operator!=(const const_iterator& p)const { return seekx!=p.seekx; }
	private:
		double const* seekx;
		double const* seeky;
		static valuetype _v; // bit of a hack. lets see...
	};
public:
	SimOutputData() : CommonData() {}
	virtual ~SimOutputData(){}

public: // obsolete interface. don't use.
  virtual DataX const* axis(uint ) const { return nullptr; } // if (i<axis_count) return CPointsX.at(i); return NULL;}
//	  double *cPointsY() const { return CPointsY; }

public:
	virtual bool isEmpty() {return true;}
	size_t size() const{incomplete(); return 0;}

	virtual const_iterator begin() const = 0; //  {return const_iterator(CPointsX.getFirst()->Points, CPointsY);}
	virtual const_iterator end() const = 0; //  {return const_iterator(CPointsX.getFirst()->end(), NULL);}
	virtual SimOutputData const* refresh() {return nullptr;}

public:
	const double& min()const {return Min;}
	const double& max()const {return Max;}

protected:
	double Min;
	double Max;
}; // SimOutputRoot
/* -------------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------------- */
#endif
