/***************************************************************************
    copyright            : (C) 2001, 2014 Albert Davis
                               2021 Felix Salfelder
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
 // base class for models
#ifndef QUCS_MODEL_H
#define QUCS_MODEL_H
#include "parameter.h"
#include "element.h"
/*--------------------------------------------------------------------------*/
namespace qucs{
/*--------------------------------------------------------------------------*/
// external
class Component;
class CommonComponent;
/*--------------------------------------------------------------------------*/
class Model : public Element{
private:
  explicit	Model() {unreachable();}
protected:
  explicit	Model(const Model& p);
public:
  explicit	Model(const Component* p);
		~Model();

public: // override virtuals
  char	id_letter()const	{untested();return '\0';}
  Element* clone_instance()const{
    return (_component_proto) ? _component_proto->clone() : NULL;
  }
  void precalc_first();
  void set_param_by_index(index_t, std::string const&); // (, int);
  bool param_is_printable(index_t)const;
//  std::string value_name()const {untested();return "";}
  std::string param_name(index_t)const override;
  //std::string param_name(int,int)const;
  std::string param_value(index_t)const override;
  index_t param_count()const {return (1 + Element::param_count());}
public:
  virtual void	tr_eval(Component*)const{unreachable();}
  virtual void	ac_eval(Component*)const{unreachable();}
  virtual CommonComponent* new_common()const {return nullptr;}
//  virtual SDP_Element* new_sdp(CommonComponent*)const {unreachable();return 0;};
  virtual bool parse_params_obsolete_callback(CS&) {unreachable(); return false;}
  virtual bool is_valid(Component const*)const {return true;}
  const Element* component_proto()const { return _component_proto;}
protected:
  const Element* _component_proto;
public:
  Parameter<double> _tnom_c;
};
/*--------------------------------------------------------------------------*/
} // qucs
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
