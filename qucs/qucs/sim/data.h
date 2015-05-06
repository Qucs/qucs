/***************************************************************************
                                data.h
                              ----------
    begin                : yes
    copyright            : (C) 2015 by Felix Salfelder
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

#ifndef SIM_DATA_H__
#define SIM_DATA_H__
#include <assert.h>
#include <complex>
#include "output.h"

// (tabular) data from a simulator.
class SimOutputData : public SimOutputBase{
public:
  typedef std::pair<double,std::complex<double> > valuetype;
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
private:
  unsigned attach_count;
public:
  virtual bool isEmpty() {return true;}
  std::string label() const{return _label;}
  size_t size() const{incomplete(); return 0;}

  virtual const_iterator begin() const {return const_iterator(CPointsX.getFirst()->Points, CPointsY);}
  virtual const_iterator end() const {return const_iterator(CPointsX.getFirst()->end(), NULL);}
  virtual SimOutputData const* refresh();

  static void attach(SimOutputData*, SimOutputData**);
  static void detach(SimOutputData**);
private:
  std::string _label;
public:
  const double& min()const {return Min;}
  const double& max()const {return Max;}
private: // limits on "y"-axis (ordinate)
  double Min;
  double Max;
};

class SimOutputDat : public SimOutputData {
public:

  SimOutputData(const QString& filename, const QString& varname);
  int loadIndepVarData(const QString&, char* datfilecontent);

  bool isEmpty() const { return !numAxes(); }
  unsigned numAxes() const { return CPointsX.count();}
  int countY() const { return CountY; }

public: // obsolete interface. don't use.
  DataX* axis(uint i) { if (i<axis_count) return CPointsX.at(i); return NULL;}
  double *cPointsY() const { return CPointsY; }
private:
  unsigned axis_count;
  Q3PtrList<DataX>  CPointsX;
  double *CPointsY;
  int CountY;    // number of curves
  QString Var;
  QString FileName;
  QDateTime lastLoaded;  // when it was loaded into memory
  unsigned attach_count;
public:
  void setLimit(const double& x){
	  if (Min>x) Min=x;
	  if (Max<x) Max=x;
  }
};


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
>>>>>>> 339e5c8... move plugin related code into place. add some more infrastructure.
};

inline void SimOutputData::attach(SimOutputData* what, SimOutputData** where)
{
  assert(!*where); // for now.
  *where = what;
  ++(what->attach_count);
}

#endif
// vim:ts=8:sw=2:noet
