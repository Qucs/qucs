/*
 * ccoupled_shunt_resonators.cpp - Capacitively coupled shunt resonators filter filter implementation
 *
 * copyright (C) 2015 Andres Martinez-Mera <andresmartinezmera@gmail.com>
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
 *
 */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include "ccoupled_shunt_resonators.h"

#include <QString>
#include <QMessageBox>


/*
References:
Microwave engineering. David M Pozar. John Wiley and Sons. 4th Edition. Pages 443-448
*/

CCoupled_Shunt_Resonator_Filter::CCoupled_Shunt_Resonator_Filter()
{
}

// -----------------------------------------------------------------------
// This function generates the schematic
QString* CCoupled_Shunt_Resonator_Filter::createSchematic(tFilter *Filter)
{
//Design equations
double f0 = Filter->Frequency+0.5*(Filter->Frequency2-Filter->Frequency);//Central frequency
double delta = (Filter->Frequency2-Filter->Frequency)/f0;
double w0 = 2*pi*f0;
double lambda0 = 3e8/f0;
int N=Filter->Order;
double Z0 = Filter->Impedance;//Reference impedance
double J[N+1], C[N+1], g[N+1], deltaC[N], l[N];
double fstart = Filter->Frequency-1e9;
double fstop =  Filter->Frequency2+1e9;
if (fstart < 0) fstart = 0;

int x = 90;//Index for drawing the schematic
QString *wirestr= new QString("<0 -30 60 -30 "" 0 0 0 "">\n");
QString *s = new QString("<Qucs Schematic " PACKAGE_VERSION ">\n");
  *s += "<Components>\n";
  *s += QString("<Pac P1 1 0 0 -92 -31 0 1 \"1\" 1 \"%1 Ohm\" 1 \"0 dBm\" 0 \"1 GHz\" 0>\n").arg(Filter->Impedance);
  *s += QString("<GND * 1 0 30 0 0 0 0>\n");
  
  for(int i = 0; i < N; i++)//Component calculation 
  {
     g[i] = getNormValue(i, Filter);
     if (i == 0)//First element
     {
       J[i]= sqrt(pi*delta/(4*g[i]))/Z0;
       C[i] = J[i]/(w0*sqrt(1-Z0*Z0*J[i]*J[i]));
       *s += QString("<C C1 1 %1 -30 -26 17 0 0 \"%2\" 1 \"\" 0 \"neutral\" 0>\n").arg(x).arg(num2str(C[i], 3, QString("F")));
       x+=60;
       continue;
     }

     J[i]= (0.25*pi*delta/sqrt(g[i]*g[i-1]))/Z0;
     C[i] = J[i]/w0;
     deltaC[i-1] = -C[i-1]-C[i];
     l[i-1] = (Z0*w0*deltaC[i-1]/(2*pi))*lambda0;
     if (l[i-1] < 0) l[i-1] += lambda0/4;
     *wirestr += QString("<%1 -30 %2 -30 "" 0 0 0 "">\n").arg(x-30).arg(x+30);
     *s += QString("<TLIN Line1 1 %1 0 -41 46 0 1 \"%2 Ohm\" 1 \"%3\" 1 \"0 dB\" 0 \"26.85\" 0>\n").arg(x).arg(Filter->Impedance).arg(num2str(l[i-1]));
     *s += QString("<GND * 1 %1 30 0 0 0 0>\n").arg(x);
     x += 60;
     *s += QString("<C C1 1 %1 -30 -26 17 0 0 \"%2\" 1 \"\" 0 \"neutral\" 0>\n").arg(x).arg(num2str(C[i], 3, QString("F")));
     x += 60;
  }

g[N] = getNormValue(N, Filter);
J[N] = sqrt(pi*delta/(4*g[N-1]*g[N]))/Z0;
C[N] = J[N]/(w0*sqrt(1-Z0*Z0*J[N]*J[N]));
deltaC[N-1] = -C[N]-C[N-1];
l[N-1] = (Z0*w0*deltaC[N-1]/(2*pi))*lambda0;
if (l[N-1] < 0) l[N-1] +=lambda0/4;

  //Wiring
  *wirestr += QString("<%1 -30 %2 -30 "" 0 0 0 "">\n").arg(x-30).arg(x+30);
  *s += QString("<TLIN Line1 1 %1 0 -41 46 0 1 \"%2 Ohm\" 1 \"%3\" 1 \"0 dB\" 0 \"26.85\" 0>\n").arg(x).arg(Filter->Impedance).arg(num2str(l[N-1]));
  *s += QString("<GND * 1 %1 30 0 0 0 0>\n").arg(x);
  x += 60;
  *s += QString("<C C1 1 %1 -30 -26 17 0 0 \"%2\" 1 \"\" 0 \"neutral\" 0>\n	").arg(x).arg(num2str(C[N], 3, QString("F")));
  x += 60;  
  *wirestr += QString("<%1 -30 %2 -30 "" 0 0 0 "">\n").arg(x-30).arg(x+30);
  *s += QString("<Pac P2 1 %1 0 18 -26 0 1 \"2\" 1 \"%2 Ohm\" 1 \"0 dBm\" 0 \"1 GHz\" 0>\n").arg(x+30).arg(Filter->Impedance);
  *s += QString("<GND * 1 %1 30 0 0 0 0 0>\n").arg(x+30);

  *s += QString("<.SP SP1 1 70 150 0 67 0 0 \"lin\" 1 \"%1\" 1 \"%2\" 1 \"300\" 1 \"no\" 0 \"1\" 0 \"2\" 0 \"no\" 0 \"no\" 0>\n").arg(num2str(fstart, 3, QString("Hz"))).arg(num2str(fstop, 3, QString("Hz")));
  *s += QString("<Eqn Eqn1 1 300 150 -28 15 0 0 \"S21_dB=dB(S[2,1])\" 1 \"S11_dB=dB(S[1,1])\" 1 \"yes\" 0>\n");
  *s += "</Components>\n";

  *s += "<Wires>\n";
  *s += wirestr;
  *s += "</Wires>\n" ;
  return s;
}
