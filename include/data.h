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
/* -------------------------------------------------------------------------------- */
#ifndef QUCS_SIM_DATA_H__
#define QUCS_SIM_DATA_H__
#include <assert.h>
#include <complex>
#include <list>
#include <QDateTime>
#include "element.h"
/* -------------------------------------------------------------------------------- */
class CommonData : public Object{
private:
	CommonData(CommonData const&) = delete;
public:
	explicit CommonData() : Object(), _attach_count(0){}
	virtual ~CommonData() {}
public:
	virtual CommonData* clone() { return NULL; }
protected:
	virtual CommonData* resolve(const std::string&){assert(false); return nullptr;}

public:
	static void attach(CommonData*, CommonData**);
	static void detach(CommonData**);
	virtual CommonData const* refresh(){unreachable(); return nullptr;}

private:
	unsigned _attach_count;
};
/* -------------------------------------------------------------------------------- */
// borrowed from e_compon
inline void CommonData::attach(CommonData* d, CommonData** to)
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
inline void CommonData::detach(CommonData** from)
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

/* -------------------------------------------------------------------------------- */
class Data : public Element{
protected:
	explicit Data() : Element(), _common(nullptr){}
	~Data();
public:
	CommonData const* common()const{ return _common; }

private:
protected: // hmm
	CommonData* _common;
};
/* -------------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------------- */
#endif
