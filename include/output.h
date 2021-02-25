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
class DataX; // BUG
/* -------------------------------------------------------------------------------- */
// a SimOutputDir provides a named list of CommonData
// these can be lists, or data sets, waves etc.
class SimOutputDir : public CommonData {
protected: // for now.
	explicit SimOutputDir() : CommonData() {}
	SimOutputDir(const SimOutputDir& s);

public:
	// multimap?
	typedef std::map<std::string, CommonData const*> container_t;
	virtual ~SimOutputDir();
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
		SimOutputDir const* dir(){
			return dynamic_cast<SimOutputDir const*>(container_t::iterator::operator*().second);
		}
		container_t::value_type operator*(){ return container_t::iterator::operator*(); }
	};
	class const_iterator : protected container_t::const_iterator{
	public:
		const_iterator(const_iterator const& p) : container_t::const_iterator(p) { untested(); }
		const_iterator(iterator const& p) : container_t::const_iterator(p) { }
//		explicit const_iterator(const container_t::iterator& p) : container_t::const_iterator(p) {}
		explicit const_iterator(const container_t::const_iterator& p) : container_t::const_iterator(p) {}

	public: //ops
		CommonData const* operator->(){incomplete(); return nullptr;}
		CommonData const* operator*(){
			return container_t::const_iterator::operator*().second;
		}
		bool operator==(const_iterator const&x) const {
			return container_t::const_iterator(*this)==(container_t::const_iterator(x));
		}
		bool operator!=(const_iterator const&x) const {
			return container_t::const_iterator(*this)!=(container_t::const_iterator(x));
		}
		const_iterator& operator++(){
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
	const_iterator find(std::string key) const {
		return const_iterator(_d.find(key));
	}
	const_iterator end() const {
		return const_iterator(_d.end());
	}
	iterator begin() { return _d.begin(); }
	iterator end() { return _d.end(); }
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
	void push_back(CommonData const*);

protected:
	mutable /*why?*/ container_t _d;
}; // SimOutputDir
/* -------------------------------------------------------------------------------- */
// (dictionary) data from a simulator, maybe later.
class SimOutputParams : public CommonData {
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

public:
	virtual bool isEmpty() {return true;}
	virtual size_t size() const{incomplete(); return 0;}

	virtual const_iterator begin() const = 0; //  {return const_iterator(CPointsX.getFirst()->Points, CPointsY);}
	virtual const_iterator end() const = 0; //  {return const_iterator(CPointsX.getFirst()->end(), NULL);}
	virtual SimOutputData const* refresh() {return nullptr;}
	virtual index_t numDeps() const {return 0;}
	virtual CommonData const* dep(index_t) const {unreachable(); return nullptr;}

public:
	const double& min()const {return Min;}
	const double& max()const {return Max;}

protected:
	double Min;
	double Max;
}; // SimOutputData
/* -------------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------------- */
#endif
