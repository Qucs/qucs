/*
 * transline.h - base for a transmission line class definition
 *
 * Copyright (C) 2005 Stefan Jahn <stefan@lkcc.org>
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307,
 * USA.
 *
 * $Id: transline.h,v 1.1 2005/03/13 14:46:35 margraf Exp $
 *
 */

#ifndef __TRANSLINE_H
#define __TRANSLINE_H

#include "qucstrans.h"

class transline {
 public:
  transline ();
  virtual ~transline ();

  void   setApplication (QucsTranscalc *);
  void   setProperty (char *, double);
  void   setProperty (char *, double, int, int);
  double getProperty (char *);
  double getProperty (char *, int, int);
  double convertProperty (char *, double, int, int);
  void   setResult (int, double, char *);
  void   setResult (int, char *);
  int    translateUnit (char *);
  char * getUnit (char *);
  bool   isSelected (char *);

  virtual void synthesize () { };
  virtual void analyze () { };

 private:
  QucsTranscalc * app;
};

#endif /* __TRANSLINE_H */
