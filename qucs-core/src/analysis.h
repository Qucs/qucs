/*
 * analysis.h - analysis class definitions
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
 * $Id: analysis.h,v 1.1 2003-12-21 13:25:37 ela Exp $
 *
 */

#ifndef __ANALYSIS_H__
#define __ANALYSIS_H__

class dataset;
class net;

class analysis : public object
{
 public:
  analysis ();
  analysis (char *);
  analysis (analysis &);
  ~analysis ();
  virtual void solve (void) { };
  dataset * getData (void) { return data; }
  void setData (dataset * d) { data = d; }
  net * getNet (void) { return subnet; }
  void setNet (net * netlist) { subnet = netlist; }

 protected:
  net * subnet;
  dataset * data;
};

#endif /* __ANALYSIS_H__ */
