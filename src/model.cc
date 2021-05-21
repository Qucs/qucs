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
#include "component.h"
#include "model.h"
/*--------------------------------------------------------------------------*/
namespace qucs{
/*--------------------------------------------------------------------------*/
Model::Model(const Component* p)
  :Element(),
   _component_proto(p),
   _tnom_c(NOT_INPUT)
{
//  if (_sim) {
//    _sim->uninit();
//  }else{
//  }
}
/*--------------------------------------------------------------------------*/
Model::Model(const Model& p)
  :Element(p),
   _component_proto(p._component_proto),
   _tnom_c(p._tnom_c)
{
//  if (_sim) {
//    _sim->uninit();
//  }else{untested();
//  }
}
/*--------------------------------------------------------------------------*/
Model::~Model()
{
//  if (_sim) {
//    _sim->uninit();
//  }else{
//  }
}
/*--------------------------------------------------------------------------*/
void Model::set_param_by_index(index_t i, std::string const& value) // (, int offset)
{
  switch (Model::param_count() - 1 - i) {
  case 0: _tnom_c = value; break;
  default: Element::set_param_by_index(i, value/*, offset */); break;
  }
}
/*--------------------------------------------------------------------------*/
bool Model::param_is_printable(index_t i)const
{
  switch (Model::param_count() - 1 - i) {
  case 0: return false;
  default: return Element::param_is_printable(i);
  }
}
/*--------------------------------------------------------------------------*/
std::string Model::param_name(index_t i)const
{
  switch (Model::param_count() - 1 - i) {
  case 0: return "tnom\0";
  default: return Element::param_name(i);
  }
}
/*--------------------------------------------------------------------------*/
// std::string Model::param_name(index_t i, int j)const
// {
//   if (j == 0) {
//     return param_name(i);
//   }else if (i >= Element::param_count()) {
//     return "";
//   }else{
//     return Model::param_name(i, j);
//   }
// }
/*--------------------------------------------------------------------------*/
std::string Model::param_value(index_t i)const
{
  switch (Model::param_count() - 1 - i) {
  case 0: return _tnom_c.string();
  default: return Element::param_value(i);
  }
}
/*--------------------------------------------------------------------------*/
//void Model::precalc_first()
//{
//  Element::precalc_first();
//  _tnom_c.e_val(OPT::tnom_c, scope());
//}
/*--------------------------------------------------------------------------*/
} // qucs
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
