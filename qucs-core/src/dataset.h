/*
 * dataset.h - dataset class definitions
 *
 * Copyright (C) 2003 Stefan Jahn <stefan@lkcc.org>
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
 * the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.  
 *
 * $Id: dataset.h,v 1.1.1.1 2003-12-20 19:03:25 ela Exp $
 *
 */

#ifndef __DATASET_H__
#define __DATASET_H__

class vector;

class dataset : public object
{
 public:
  dataset ();
  dataset (char *);
  dataset (const dataset &);
  ~dataset ();
  void addDependency (vector *);
  void appendDependency (vector *);
  void addVariable (vector *);
  void appendVariable (vector *);
  char * getFile (void);
  void setFile (char *);
  void print (void);
  void printData (vector *, FILE *);
  vector * findDependency (char *);
  vector * findVariable (char *);
  vector * getDependencies (void) { return dependencies; }
  vector * getVariables (void) { return variables; }
  static dataset * load (char *);
  static dataset * load_touchstone (char *);

 private:
  char * file;
  vector * dependencies;
  vector * variables;
};

#endif /* __DATASET_H__ */
