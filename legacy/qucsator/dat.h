/***************************************************************************
    copyright            : (C) 20?? QUCS
                               2015, 2020 by Felix Salfelder
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

// TODO: remove.
/* -------------------------------------------------------------------------------- */
#include "output.h"
#include <QString>
#include <QDateTime>

#ifndef QUCS_LEGACY_DAT_H
#define QUCS_LEGACY_DAT_H

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
// "dat" file, some legacy code rearranged.
// was just one var from dat file. (fixed)
class SimOutputDat : public SimOutputDir {
public:
	SimOutputDat(std::string const& filename, std::string const& varname);
//	int loadIndepVarData(std::string const&, char* datfilecontent, DataX*);

//	bool isEmpty() const { return !numAxes(); }
//	unsigned numAxes() const { return CPointsX.size();}
//	int countY() const { return CountY; }

public: // obsolete interface. don't use.
	SimOutputData const* refresh() override;

//	virtual const_iterator begin() const {return const_iterator(CPointsX.front()->Points, CPointsY);}
//	virtual const_iterator end() const {return const_iterator(CPointsX.back()->end(), NULL);}

private:
	void clear();

public:
//	void setLimit(const double& x){
//		if (Min>x) Min=x;
//		if (Max<x) Max=x;
//	}
private:
	unsigned axis_count;
//	std::vector<DataX*>  CPointsX;
	double *CPointsY;
//	int CountY;    // number of curves
	QString Var;
	std::string _fileName;
	QDateTime lastLoaded;  // when it was loaded into memory
	unsigned _attach_count;

//	std::map<std::string, SimOutputData*> _map; -> dir
};

#endif
