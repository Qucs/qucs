/*
 * environment.h - variable environment class definitions
 *
 * Copyright (C) 2004 Stefan Jahn <stefan@lkcc.org>
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this package; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street - Fifth Floor,
 * Boston, MA 02110-1301, USA.  
 *
 * $Id: environment.h,v 1.2 2005/06/02 18:17:49 raimi Exp $
 *
 */

#ifndef __ENVIRONMENT_H__
#define __ENVIRONMENT_H__

class variable;

class environment
{
 public:
  environment ();
  environment (char *);
  environment (const environment &);
  virtual ~environment ();
  void setName (char *);
  char * getName (void);
  void copyVariables (variable *);
  void deleteVariables (void);
  void addVariable (variable *);
  variable * getVariable (char *);

 private:
  char * name;
  variable * root;
};

#endif /* __ENVIRONMENT_H__ */
