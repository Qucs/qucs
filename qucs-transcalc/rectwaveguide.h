/*
 * rectwaveguide.h - rectangular waveguide  class definition
 *
 * Copyright (C) 2001 Gopal Narayanan <gopal@astro.umass.edu>
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
 */

#ifndef __RECTWAVEGUIDE_H
#define __RECTWAVEGUIDE_H

class rectwaveguide : public transline {
 public:
  rectwaveguide();
  ~rectwaveguide();

 private:
  double er;               /* dielectric constant */
  double mur;              /* mag. permeability */
  double sigma;            /* Conductivity of the metal */
  double tand;             /* Dielectric Loss Tangent */
  double tanm;             /* Magnetic Loss Tangent */  
  double f;                /* Frequency of operation */
  double a;                /* width of waveguide */
  double b;                /* height of waveguide */
  double l;                /* length of waveguide */
  double Z0;               /* characteristic impedance */
  double ang_l;            /* Electrical length in angle */
  double er_eff;           /* Effective dielectric constant */
  double mur_eff;          /* Effective mag. permeability */
  double atten_dielectric; /* Loss in dielectric (dB) */
  double atten_cond;       /* Loss in conductors (dB) */
  double skindepth;        /* Skin depth in mils */
  double fc10;             /* Cutoff frequency for TE10 mode */

 public:
  void analyze ();
  void synthesize ();

 private:
  double kval ();
  double kc (int, int);
  double fc (int, int);
  double alphac ();
  double alphac_cutoff ();
  double alphad ();
  void get_rectwaveguide_sub ();
  void get_rectwaveguide_comp ();
  void get_rectwaveguide_phys ();
  void get_rectwaveguide_elec ();
  void show_results ();
};

#endif /* __RECTWAVEGUIDE_H */
