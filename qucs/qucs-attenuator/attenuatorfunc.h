/*
 * attenuatorfunc.h - Attenuator synthesis and schematic generation header
 *
 * Copyright (C) 2018 Qucs Team
 *
 * This program is free software; you can redistribute it and/or modify
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
 */

#ifndef QUCS_ATT_H
#define QUCS_ATT_H

#define PI_TYPE 0
#define TEE_TYPE 1
#define BRIDGE_TYPE 2
#define REFLECTION_TYPE 3
#define QW_SERIES_TYPE 4
#define QW_SHUNT_TYPE 5

#define C0 299792458
#define PI 3.141592653589793238462643383279502884197169399375105820974944592307816406286

#include <cmath>

struct tagATT
{
  int Topology;
  double Zin;
  double Zout;
  double Attenuation;
  double MinimumATT;
  double R1;
  double R2;
  double R3;
  double R4;
  double PR1;//Power dissipated by R1 [W]
  double PR2;//Power dissipated by R2 [W]
  double PR3;//Power dissipated by R3 [W]
  double PR4;//Power dissipated by R4 [W]
  double Pin;//Input power [W]
  bool minR; //The reflection attenuator can be designed using two different resistor values. The first one is such
             //as R < Z0 whereas the other is such as R > Z0. This field is just a flag to indicate what solution
             //qucsattenuator should use.
  bool useLumped;//Used only in the quarter wave attenuators. It indicates that the program must replace the
                 //qw line by its lumped element equivalent (CLC)
  double freq;//Central frequency for quarter-wavelength attenuators
};

class QString;

class QUCS_Att
{
 public:
  QUCS_Att();
  ~QUCS_Att();

  int Calc(tagATT*);
  static QString* createSchematic(tagATT*, bool);
};

QString RoundVariablePrecision(double);
QString ConvertLengthFromM(double);
QString num2str(double);
#endif
