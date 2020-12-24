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

#ifndef QUCS_SIM_DATA_H__
#define QUCS_SIM_DATA_H__
#include <assert.h>
#include <complex>
#include <list>
#include <QDateTime>
#include "output.h"

struct DataX {
	DataX(std::string const& Var_, double *Points_=0, int count_=0)
		: Var(Var_), Points(Points_), count(count_), Min(INFINITY), Max(-INFINITY) {};
	~DataX() { if(Points) delete[] Points; };
	const double* end() const{return Points+count;}

public:
	const double& min()const {return Min;}
	const double& max()const {return Max;}
public: // only called from Graph. cleanup later.
	void setLimit(const double& x){
		if (Min>x) Min=x;
		if (Max<x) Max=x;
	}
public: // ??
	std::string Var;
	double *Points;
	int     count;

private:
	double Min;
	double Max;
};

// (tabular) data from a simulator.
class SimOutputData : public QucsData{
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
	SimOutputData() : QucsData() {}
	virtual ~SimOutputData(){}

	Object::Type type() const override {return Object::Type::SimOutputData;}

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
};


#if 0 // not yet?
// a sequence of doubles (with a name on it)
class SimOutputSeq : public SimOutputData{
public:
	class const_iterator{
	};

public:
	const_iterator begin() const;
	const_iterator end() const;
};

// a sequence of double pairs (with a name on it)
class SimOutputWave : public SimOutputData{
private:
	SimOutputSeq* dep;
};
#endif


#endif
