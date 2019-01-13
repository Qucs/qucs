/*
 * qw_coupled_ring_filter.cpp - Quarter wavelength side coupled ring filter implementation
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

#include "qw_coupled_ring_filter.h"

#include <QString>
#include <QMessageBox>


/*
References:
Quarter Wavelength Side-Coupled Ring Resonator for Bandpass filters. Mohd Khairul Mohd Salleh, Gaëtan Prigent, Olivier Pigaglio, and Raymond Crampagne. IEEE TRANSACTIONS ON MICROWAVE THEORY AND TECHNIQUES. VOL. 56, NO. 1, JANUARY 2008
*/

QW_Coupled_Ring_Filter::QW_Coupled_Ring_Filter()
{
}

// -----------------------------------------------------------------------
QString* QW_Coupled_Ring_Filter::createSchematic(tFilter *Filter, tSubstrate *Substrate, bool isMicrostrip)
{
  // Design equations
  double f0 = Filter->Frequency+0.5*(Filter->Frequency2-Filter->Frequency);//Central frequency
  double ftz = Filter->Frequency;//First transmission zero
  double Z0 = Filter->Impedance;//Reference impedance
  double Ze = 60;//Even mode impedance (fixed term)
  double lambda0 = LIGHTSPEED/f0;
  double y = 0.01; // Bandpass ripple (dB)
  double Ye = 1/Ze;
  double x = -pow(10,(-y/20));
  double TZ = (sin(pi*ftz/(2*f0))*sin(pi*ftz/(2*f0)))/(1 + cos(pi*ftz/(2*f0))*cos(pi*ftz/(2*f0)));
  double SQ = sqrt((1-x*x)*(TZ*TZ-1)*(TZ*TZ-1)*Z0*Z0);
  double R = -Ye*Z0*(Z0*(x*x-2)+2*SQ);
  double Q = 2*TZ*TZ*Ye*Z0*(Z0*(x*x-2)+SQ);
  double P = 4*x*Z0*TZ*TZ*TZ -Ye*Z0*Z0*(TZ*TZ*TZ*TZ)*(x*x-2) + 4*TZ*x*(SQ-Z0);
  double Zr = (2*TZ*x + TZ*TZ*Ye*Z0 + Ye*(-Z0+SQ-x*sqrt((P+Q+R)/(Ye*x*x))))/(2*x*Ye);
  double Yue = TZ/Zr;//Unit element capacitance
  double Yo = Yue*Ye/(2*Ye-Yue);
  double Zo = 1/Yo; //Impedance of the odd mode
  double l4 = lambda0/4;
  double fstart = Filter->Frequency-1e9;
  double fstop =  Filter->Frequency2+1e9;
  double width, er_eff, gap;

  if (Zo < 1)
  {
  	 QMessageBox::warning(0, QObject::tr("Error"),
         QObject::tr("Please reduce the bandwidth between the attenuation poles\n"));
         return NULL;
  }

  if (fstart < 0) fstart = 0;
  // create the Qucs schematic
  QString *s = new QString("<Qucs Schematic " PACKAGE_VERSION ">\n");
  *s += QString("<Components>\n");
  *s += QString("<Pac P1 1 -100 70 18 -26 0 1 \"1\" 1 \"%1 Ohm\" 1 \"0 dBm\" 0 \"1 GHz\" 0 \"26.85\" 0>\n").arg(Z0);
  *s += QString("<GND * 1 -100 100 0 0 0 0>\n");
   *s += QString("<Eqn Eqn2 1 230 310 -31 19 0 0 \"S21_dB=dB(S[2,1])\" 1 \"S11_dB=dB(S[1,1])\" 1 \"yes\" 0>\n");
  *s += QString("<.SP SP1 1 -50 310 0 80 0 0 \"lin\" 1 \"%1\" 1 \"%2\" 1 \"300\" 1 \"no\" 0 \"1\" 0 \"2\" 0 \"no\" 0 \"no\" 0>\n").arg(num2str(fstart, 3, QString("Hz"))).arg(num2str(fstop, 3, QString("Hz")));

  if (isMicrostrip)//Microstrip implementation
  {
   TL_Filter::getMicrostrip(Filter->Impedance, f0, Substrate, width, er_eff);
   *s += QString("<MLIN MS1 1 190 -40 -26 20 0 0 \"Sub1\" 0 \"%1\" 0 \"%2\" 0 \"Hammerstad\" 0 \"Kirschning\" 0 \"26.85\" 0>\n").arg(num2str(width)).arg(num2str(l4/sqrt(er_eff)));
   *s += QString("<MLIN MS1 1 200 140 -26 20 0 0 \"Sub1\" 0 \"%1\" 0 \"%2\" 0 \"Hammerstad\" 0 \"Kirschning\" 0 \"26.85\" 0>\n").arg(num2str(width)).arg(num2str(l4/sqrt(er_eff)));
   
    sythesizeCoupledMicrostrip(Zo, Ze, f0, Substrate, width, gap, er_eff);
    /*if((width < 1e-7) || (gap < 1e-7)) 
    {
     QMessageBox::critical(0, "Error", "Filter can't be created.");
     delete s;
     return NULL;
    }*/
    //Coupled lines
   *s += QString("<MCOUPLED MS1 1 310 60 16 -26 0 1 \"Sub1\" 0 \"%1\" 0 \"%2\" 0 \"%3\" 0 \"Kirschning\" 0 \"Kirschning\" 0 \"26.85\" 0>\n").arg(num2str(width)).arg(num2str(l4/sqrt(er_eff))).arg(num2str(gap));
   *s += QString("<MCOUPLED MS1 1 70 60 -63 -26 0 3 \"Sub1\" 0 \"%1\" 0 \"%2\" 0 \"%3\" 0 \"Kirschning\" 0 \"Kirschning\" 0 \"26.85\" 0>\n").arg(num2str(width)).arg(num2str(l4/sqrt(er_eff))).arg(num2str(gap));
    
    //Microstrip open end
      *s += QString("<MOPEN MS1 1 340 120 -26 -81 0 -1 \"Sub1\" 0 \"%3\" 0 \"Hammerstad\" 0 \"Kirschning\" 0 \"Kirschning\" 0>\n").arg(num2str(width));
      *s += QString("<MOPEN MS1 1 40 120 -26 -81 0 -1 \"Sub1\" 0 \"%3\" 0 \"Hammerstad\" 0 \"Kirschning\" 0 \"Kirschning\" 0>\n").arg(num2str(width));

    *s += QString("<SUBST Sub1 1 450 400 50 0 80 0 \"%1\" 1 \"%2\" 1 \"%3\" 1 \"%4\" 1 \"%5\" 1 \"%6\" 1>\n").arg(Substrate->er).arg(num2str(Substrate->height)).arg(num2str(Substrate->thickness)).arg(Substrate->tand).arg(Substrate->resistivity).arg(Substrate->roughness);
  }
  else//Ideal transmission line implementation
  {
  *s += QString("<TLIN Line1 1 190 -40 -26 20 0 0 \"%1 Ohm\" 1 \"%2\" 1 \"0 dB\" 0 \"26.85\" 0>\n").arg(Zr).arg(num2str(l4));
  *s += QString("<TLIN Line2 1 200 140 -26 20 0 0 \"%1 Ohm\" 1 \"%2\" 1 \"0 dB\" 0 \"26.85\" 0>\n").arg(Zr).arg(num2str(l4));
  *s += QString("<CTLIN Line3 1 290 60 16 -26 0 1 \"%1 Ohm\" 1 \"%2 Ohm\" 1 \"%3\" 1 \"1\" 0 \"1\" 0 \"0 dB\" 0 \"0 dB\" 0 \"26.85\" 0>\n").arg(Ze).arg(Zo).arg(num2str(l4));
  *s += QString("<CTLIN Line4 1 90 60 -63 -26 0 3 \"%1 Ohm\" 1 \"%2 Ohm\" 1 \"%3\" 1 \"1\" 0 \"1\" 0 \"0 dB\" 0 \"0 dB\" 0 \"26.85\" 0>\n").arg(Ze).arg(Zo).arg(num2str(l4));


  }
  *s += QString("<Pac P2 1 470 60 18 -26 0 1 \"2\" 1 \"50 Ohm\" 1 \"0 dBm\" 0 \"1 GHz\" 0 \"26.85\" 0>\n");
  *s += QString("<GND * 1 470 90 0 0 0 0>\n");
  *s += QString("</Components>\n");


  *s += QString("<Wires>\n");
  *s += QString("<-100 30 -100 40 "" 0 0 0 "">\n");
  *s += QString("<-100 30 40 30 "" 0 0 0 "">\n");
  *s += QString("<100 -40 100 30 "" 0 0 0 "">\n");
  *s += QString("<100 -40 160 -40 "" 0 0 0 "">\n");
  *s += QString("<220 -40 280 -40 "" 0 0 0 "">\n");
  *s += QString("<280 -40 280 30 "" 0 0 0 "">\n");
  *s += QString("<280 90 280 140 "" 0 0 0 "">\n");
  *s += QString("<230 140 280 140 "" 0 0 0 "">\n");
  *s += QString("<100 140 170 140 "" 0 0 0 "">\n");
  *s += QString("<100 90 100 140 "" 0 0 0 "">\n");
  *s += QString("<340 30 470 30 "" 0 0 0 "">\n");
  
  if (!isMicrostrip)//The microstrip coupled lines component is wider than the ideal coupled lines, so additional wires are required
  {
    *s += QString("<-100 30 80 30 "" 0 0 0 "">\n");
    *s += QString("<300 30 470 30 "" 0 0 0 "">\n");
  }
  *s += QString("</Wires>\n");


  *s += "<Paintings>\n";
  *s += QString("<Text 300 500 12 #000000 0 \"Quarter wave side coupled ring bandpass filter\\n");
  *s += QString("%1...%2 ").arg(num2str(Filter->Frequency, 3, QString("Hz"))).arg(num2str(Filter->Frequency2, 3, QString("Hz")));
  *s += QString("Impedance matching %3 Ohm\">\n").arg(Filter->Impedance);
  *s += "</Paintings>\n";

  return s;
}
