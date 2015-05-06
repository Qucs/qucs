/***************************************************************************
                               output.h
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
#ifndef DATA_H_
#define DATA_H_
#include <map>
#include <assert.h>
#include <iostream>

#define incomplete() (fprintf(stderr, "@@#\n@@@incomplete:%s:%u:%s\n", \
			   __FILE__, __LINE__, __func__))
#define untested() (fprintf(stderr, "@@# untested \n@@@:%s:%u:%s\n", \
			   __FILE__, __LINE__, __func__))

// data structure to cope with what a simulator outputs.
// this is organized as a directory tree with pluggable directories.
// the "files" are waveforms or parameter sets or something like that.

class SimOutputBase;

// base class for all output related classes
class SimOutputBase {
  public:
    SimOutputBase* clone() { return NULL; }
  protected:
  virtual SimOutputBase* resolve(const std::string&){assert(false);}
};

// base class for SimOutputs
// a SimOutputDir provides a named list of SimOutputBase
// these can again be "directories", or data sets
class SimOutputDir : public SimOutputBase {
protected:
  SimOutputDir(const SimOutputDir& s) : _d(s._d) { untested(); }
public:
  virtual SimOutputDir* clone(){return new SimOutputDir(*this);}
  typedef std::map<std::string, SimOutputBase*> container_t;
  SimOutputDir(){}
  virtual ~SimOutputDir(){}
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
  const SimOutputBase* operator[](const_iterator& x){
    if(!x->second){
      x->second = resolve(x->first);
    }
    return x->second;
  }
protected:
  mutable std::map<std::string, SimOutputBase*> _d;
};

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
  virtual SimOutputRoot* clone(){return new SimOutputRoot(*this);}
public:
  SimOutputRoot(){ }
  virtual void set_var(std::string n, std::string v) { untested();
    for(auto i = begin(); i!=end(); ++i){ untested();
      assert(i.dir());
      i.dir()->set_var(n,v);
    }
  }

  void install(const std::string& s, SimOutputBase* p){
    assert(p);
    _d[s] = p;
    std::cerr << "installing " << s << " " << _d.size() << "\n";
    std::cerr << "installing " << intptr_t(this) << "\n";
  }
  void uninstall(SimOutputBase*){ /* incomplete */ }
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
  // std::map<std::string, SimOutputBase*> *_d;
};

// (dictionary) data from a simulator, maybe later.
class SimOutputParams : public SimOutputBase{
};

extern SimOutputRoot simOutputRoot;

// vim:ts=8:sw=2:noet
#endif
