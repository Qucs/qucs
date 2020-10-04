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
	protected:
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
	SimOutputData() : attach_count(0){}
	virtual ~SimOutputData(){}

public: // obsolete interface. don't use.
  virtual DataX const* axis(uint ) const { return nullptr; } // if (i<axis_count) return CPointsX.at(i); return NULL;}
//	  double *cPointsY() const { return CPointsY; }

public:
	virtual bool isEmpty() {return true;}
	std::string label() const{return _label;}
	size_t size() const{incomplete(); return 0;}

	virtual const_iterator begin() const = 0; //  {return const_iterator(CPointsX.getFirst()->Points, CPointsY);}
	virtual const_iterator end() const = 0; //  {return const_iterator(CPointsX.getFirst()->end(), NULL);}
	virtual SimOutputData const* refresh() {return nullptr;}

	static void attach(SimOutputData*, SimOutputData**);
	static void detach(SimOutputData**);

public:
	const double& min()const {return Min;}
	const double& max()const {return Max;}

private:
	unsigned attach_count;
	std::string _label;

protected:
	double Min;
	double Max;
};

// "dat" file, legacy code rearranged.
// just one var from dat file?
class SimOutputDat : public SimOutputData {
public:
	SimOutputDat(std::string const& filename, std::string const& varname);
	int loadIndepVarData(std::string const&, char* datfilecontent, DataX*);

	bool isEmpty() const { return !numAxes(); }
	unsigned numAxes() const { return CPointsX.size();}
	int countY() const { return CountY; }

public: // obsolete interface. don't use.
	DataX const* axis(uint i) const override { if (i<axis_count) return CPointsX[i]; return NULL;}
	double *cPointsY() const { return CPointsY; }
	virtual SimOutputData const* refresh();
private:
	unsigned axis_count;
	std::vector<DataX*>  CPointsX;
	double *CPointsY;
	int CountY;    // number of curves
	QString Var;
	std::string _fileName;
	QDateTime lastLoaded;  // when it was loaded into memory
	unsigned attach_count;

private:
	void clear();
public:
	void setLimit(const double& x){
		if (Min>x) Min=x;
		if (Max<x) Max=x;
	}
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

inline void SimOutputData::attach(SimOutputData* what, SimOutputData** where)
{
	assert(!*where); // for now.
	*where = what;
	++(what->attach_count);
}

#endif
