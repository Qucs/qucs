/*
 * attenuatorfunc.cpp - Attenuator synthesis and schematic generation
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "attenuatorfunc.h"

#include <QString>

QUCS_Att::QUCS_Att(){}
QUCS_Att::~QUCS_Att(){}

int QUCS_Att::Calc(tagATT *ATT)
{
  double Lmin, L, A;
  L = pow(10, ATT->Attenuation / 10);
  A = (L + 1) / (L - 1);

  //Check minumum attenuation for Pi and Tee type attenuators
  if ((ATT->Topology == PI_TYPE) || (ATT->Topology == TEE_TYPE))
  {
  if(ATT->Zin > ATT->Zout)
    {
      Lmin = (2 * ATT->Zin / ATT->Zout) - 1 + 2 * 
	sqrt(ATT->Zin / ATT->Zout * (ATT->Zin / ATT->Zout - 1));
    }
  else
    {
      Lmin = (2 * ATT->Zout / ATT->Zin) - 1 + 2 * 
	sqrt(ATT->Zout / ATT->Zin * (ATT->Zout / ATT->Zin - 1));
    }
  ATT->MinimumATT = 10 * log10(Lmin);  

  if(ATT->MinimumATT > ATT->Attenuation)
    {
      return -1;
    }
  }

    switch(ATT->Topology)
	{
	case PI_TYPE:
	  {
        //Design equations
	    ATT->R2 = ((L - 1) / 2) * sqrt(ATT->Zin * ATT->Zout / L);
	    ATT->R1 = 1 / (((A / ATT->Zin)) - (1 / ATT->R2));
	    ATT->R3 = 1 / (((A / ATT->Zout)) - (1 / ATT->R2));
        //Power dissipation
        ATT->PR1 = ATT->Pin*(ATT->Zin/ATT->R1);//[W]
        ATT->PR2 = ATT->Pin*(ATT->R2*pow(ATT->R1-ATT->Zin,2)/(ATT->R1*ATT->R1*ATT->Zin));//[W]
        ATT->PR3 = ATT->Pin*(pow(ATT->R1*ATT->R2-ATT->Zin*(ATT->R1+ATT->R2),2))/(ATT->R1*ATT->R1*ATT->R3*ATT->Zin);//[W]
	    break;
	  }
	case TEE_TYPE:
	  {
        //Design equations
	    ATT->R2 = (2 * sqrt(L * ATT->Zin * ATT->Zout)) / (L - 1);
	    ATT->R1 = ATT->Zin * A - ATT->R2;
	    ATT->R3 = ATT->Zout * A - ATT->R2;
        //Power dissipation
        ATT->PR1 = ATT->Pin*(ATT->R1/ATT->Zin);//[W]
        ATT->PR2 = ATT->Pin*(pow(ATT->R1-ATT->Zin,2))/(ATT->R2*ATT->Zin);//[W]
        ATT->PR2 = ATT->Pin*(ATT->R3*pow(ATT->R1+ATT->R2-ATT->Zin,2)/(ATT->Zin*ATT->R2*ATT->R2));//[W]
	    break;
	  }
	case BRIDGE_TYPE:
	  {
        //Design equations
        L = pow(10, 0.05*ATT->Attenuation);
	    ATT->R1 = ATT->Zin * (L - 1);
        ATT->R2 = ATT->Zin / (L - 1);
        ATT->R3 = ATT->Zin;//Z01
        ATT->R4 = ATT->Zin;//Z02
        //R3 = Z0
        //Power dissipation
        ATT->PR1 = ATT->Pin*((4*ATT->R1*ATT->R2*ATT->R2*ATT->Zin)/(pow(ATT->R1*ATT->R2+ATT->Zin*(2*ATT->R2+ATT->Zin),2)));//[W]
        ATT->PR2 = ATT->Pin*(4*ATT->R2*ATT->Zin*ATT->Zin*ATT->Zin)/pow(ATT->R1*ATT->R2+ATT->Zin*(2*ATT->R2+ATT->Zin),2);//[W]
        ATT->PR3 = ATT->Pin*(pow(ATT->R1*ATT->R2+ATT->Zin*ATT->Zin,2)/pow(ATT->R1*ATT->R2+ATT->Zin*(2*ATT->R2+ATT->Zin),2));//[W]
        ATT->PR4 = 0;//Z02 dissipates no power.
	    break;
	  }
      case REFLECTION_TYPE:
      {
         //Design equations
         L = pow(10, 0.05*ATT->Attenuation);
         if (ATT->minR)
            ATT->R1 = ATT->Zin*(L + 1)/(L - 1);
         else
            ATT->R1 = ATT->Zin*(L - 1)/(L + 1);
         ATT->R2 = ATT->R1;
         //Power dissipation. Both resistors dissipate the same power
         ATT->PR1 = 0.5*ATT->Pin*(1-pow(abs((ATT->Zin-ATT->R1)/(ATT->Zin+ATT->R1)),2));
         ATT->PR2 = ATT->PR1;
         break;
      }
      case QW_SERIES_TYPE:
      {
         //Design equations
         L = pow(10, 0.05*ATT->Attenuation);
         ATT->R1  = ATT->Zin/(L-1);
         ATT->R2 = ATT->Zin;
         ATT->R3 = ATT->R1;
         ATT->R4 = 0.25*C0/ATT->freq;//lambda/4
         //Power dissipation.
         ATT->PR1 = ATT->Pin*ATT->R1*ATT->Zin/pow(ATT->R1 + ATT->Zin,2);
         ATT->PR2 = ATT->Pin*ATT->Zin*ATT->Zin/pow(ATT->R1 + ATT->Zin,2);
         ATT->PR3 = ATT->PR1;
         break;
      }
      case QW_SHUNT_TYPE:
      {
         //Design equations
         L = pow(10, 0.05*ATT->Attenuation);
         ATT->R1  = ATT->Zin*(L-1);
         ATT->R2 = ATT->Zin;
         ATT->R3 = ATT->R1;
         ATT->R4 = 0.25*C0/ATT->freq;//lambda/4
         //Power dissipation.
         ATT->PR1 = ATT->Pin*ATT->R1*ATT->Zin/pow(ATT->R1 + ATT->Zin,2);
         ATT->PR2 = ATT->Pin*ATT->Zin*ATT->Zin/pow(ATT->R1 + ATT->Zin,2);
         ATT->PR3 = ATT->PR1;
         break;
      }
	}
    return 0;
}

//This function creates the schematic. It receives the attenuator resistor values (tagATT * ATT) and bool flag to include a S-parameter box in the schematic
QString* QUCS_Att::createSchematic(tagATT *ATT, bool SP_box)
{
  // create the Qucs schematic
  QString *s = new QString("<Qucs Schematic ");
  *s += PACKAGE_VERSION;
  *s += ">\n";
  *s += "<Components>\n";
  
  switch(ATT->Topology)
    {
    case PI_TYPE:
      *s += QString("<R R1 1 180 200 -15 50 0 1 \"%1 Ohm\" 1 \"26.85\" 0 \"0.0\" 0 \"0.0\" 0 \"26.85\" 0 \"US\" 0>\n").arg(ATT->R1);
      *s += QString("<R R2 1 255 130 -60 10 0 0 \"%1 Ohm\" 1 \"26.85\" 0 \"0.0\" 0 \"0.0\" 0 \"26.85\" 0 \"US\" 0>\n").arg(ATT->R2);
      *s += QString("<R R3 1 330 200 -15 50 0 1 \"%1 Ohm\" 1 \"26.85\" 0 \"0.0\" 0 \"0.0\" 0 \"26.85\" 0 \"US\" 0>\n").arg(ATT->R3);
      *s += "<GND * 1 180 230 0 0 0 0>\n";
      *s += "<GND * 1 330 230 0 0 0 0>\n";
      if (SP_box)
      {
        // S-parameter simulation block
        //-----------------------------
        // Resistor attenuators are broadband ckts, so it's pointless to ask the user to input the analysis freq sweep. Let's do a wideband
        // sweep and then the user can modify that in the schematic
        *s += "<.SP SP1 1 140 350 0 83 0 0 \"lin\" 1 \"50 MHz\" 1 \"3 GHz\" 1 \"200\" 1 \"no\" 0 \"1\" 0 \"2\" 0 \"no\" 0 \"no\" 0>\n";

        // Equations
        *s += "<Eqn Eqn1 1 360 350 -32 19 0 0 \"S21_dB=dB(S[2,1])\" 1 \"S11_dB=dB(S[1,1])\" 1 \"S22_dB=dB(S[2,2])\" 1 \"yes\" 0>\n";

        // Input term
        *s += QString("<Pac P1 1 50 200 18 -26 0 1 \"1\" 1 \"%1 Ohm\" 1 \"0 dBm\" 0 \"1 GHz\" 0 \"26.85\" 0>\n").arg(ATT->Zin);
        *s += "<GND * 1 50 230 0 0 0 0>\n";

        // Output term
        *s += QString("<Pac P1 1 460 200 18 -26 0 1 \"1\" 1 \"%1 Ohm\" 1 \"0 dBm\" 0 \"1 GHz\" 0 \"26.85\" 0>\n").arg(ATT->Zout);
        *s += "<GND * 1 460 230 0 0 0 0>\n";
      }
      *s += "</Components>\n";

      *s += "<Wires>\n";
      *s += "<285 130 350 130 \"\" 0 0 0 \"\">\n";
      *s += "<140 130 225 130 \"\" 0 0 0 \"\">\n";
      *s += "<180 130 180 170 \"\" 0 0 0 \"\">\n";
      *s += "<330 130 330 170 \"\" 0 0 0 \"\">\n";
      if (SP_box)
      {   // Additional wiring because of the input/output ports

          // Input port
          *s += "<50 130 50 170 \"\" 0 0 0 \"\">\n";
          *s += "<50 130 140 130 \"\" 0 0 0 \"\">\n";

          // Output port
          *s += "<460 130 460 170 \"\" 0 0 0 \"\">\n";
          *s += "<460 130 350 130 \"\" 0 0 0 \"\">\n";
      }
      *s += "</Wires>\n";

      *s += "<Diagrams>\n";
      *s += "</Diagrams>\n";

      *s += "<Paintings>\n";
      *s += QString("<Text 160 60 12 #000000 0 \"%1 dB Pi-Type Attenuator\">\n").arg(ATT->Attenuation);
      if (!SP_box)
      {// If the SP simulation box option is activated, then the input and output ports are attached.
       // Thus, it doesn't make sense to have a text field indicating the input/output impedance
         *s += QString("<Text 50 122 10 #000000 0 \"Z1: %1 Ohm\">\n").arg(ATT->Zin);
         *s += QString("<Text 360 122 10 #000000 0 \"Z2: %1 Ohm\">\n").arg(ATT->Zout);
      }
      *s += "</Paintings>\n";
      break;

    case TEE_TYPE:
      *s += QString("<R R1 1 180 130 -40 20 0 2 \"%1 Ohm\" 1 \"26.85\" 0 \"0.0\" 0 \"0.0\" 0 \"26.85\" 0 \"US\" 0>\n").arg(ATT->R1);
      *s += QString("<R R2 1 270 200 -20 60 0 3 \"%1 Ohm\" 1 \"26.85\" 0 \"0.0\" 0 \"0.0\" 0 \"26.85\" 0 \"US\" 0>\n").arg(ATT->R2);
      *s += QString("<R R3 1 350 130 -40 20 0 2 \"%1 Ohm\" 1 \"26.85\" 0 \"0.0\" 0 \"0.0\" 0 \"26.85\" 0 \"US\" 0>\n").arg(ATT->R3);
      *s += "<GND * 1 270 230 0 0 0 0>\n";
      if (SP_box)
      {
        // S-parameter simulation block
        //-----------------------------
        // Resistor attenuators are broadband ckts, so it's pointless to ask the user to input the analysis freq sweep. Let's do a wideband
        // sweep and then the user can modify that in the schematic
        *s += "<.SP SP1 1 140 350 0 83 0 0 \"lin\" 1 \"50 MHz\" 1 \"3 GHz\" 1 \"200\" 1 \"no\" 0 \"1\" 0 \"2\" 0 \"no\" 0 \"no\" 0>\n";

        // Equations
        *s += "<Eqn Eqn1 1 360 350 -32 19 0 0 \"S21_dB=dB(S[2,1])\" 1 \"S11_dB=dB(S[1,1])\" 1 \"S22_dB=dB(S[2,2])\" 1 \"yes\" 0>\n";

        // Input term
        *s += QString("<Pac P1 1 70 200 18 -26 0 1 \"1\" 1 \"%1 Ohm\" 1 \"0 dBm\" 0 \"1 GHz\" 0 \"26.85\" 0>\n").arg(ATT->Zin);
        *s += "<GND * 1 70 230 0 0 0 0>\n";

        // Output term
        *s += QString("<Pac P1 1 460 200 18 -26 0 1 \"1\" 1 \"%1 Ohm\" 1 \"0 dBm\" 0 \"1 GHz\" 0 \"26.85\" 0>\n").arg(ATT->Zout);
        *s += "<GND * 1 460 230 0 0 0 0>\n";
      }
      *s +="</Components>\n";

      *s += "<Wires>\n";
      *s += "<140 130 150 130 \"\" 0 0 0 \"\">\n";
      *s += "<210 130 320 130 \"\" 0 0 0 \"\">\n";
      *s += "<270 130 270 170 \"\" 0 0 0 \"\">\n";
      if (SP_box)
      {   // Additional wiring because of the input/output ports

          // Input port
          *s += "<70 130 70 170 \"\" 0 0 0 \"\">\n";
          *s += "<70 130 140 130 \"\" 0 0 0 \"\">\n";

          // Output port
          *s += "<460 130 460 170 \"\" 0 0 0 \"\">\n";
          *s += "<460 130 380 130 \"\" 0 0 0 \"\">\n";
      }
      *s += "</Wires>\n";

      *s += "<Diagrams>\n";
      *s += "</Diagrams>\n";
      *s += "<Paintings>\n";
      *s += QString("<Text 170 60 12 #000000 0 \"%1 dB Tee-Type Attenuator\">\n").arg(ATT->Attenuation);
      if (!SP_box)
      {// If the SP simulation box option is activated, then the input and output ports are attached.
       // Thus, it doesn't make sense to have a text field indicating the input/output impedance
          *s += QString("<Text 50 122 10 #000000 0 \"Z1: %1 Ohm\">\n").arg(ATT->Zin);
          *s += QString("<Text 390 122 10 #000000 0 \"Z2: %1 Ohm\">\n").arg(ATT->Zout);
      }
      *s += "</Paintings>\n";
      break;

    case BRIDGE_TYPE:
      *s += QString("<R R1 1 260 130 -60 0 0 0 \"%1 Ohm\" 1 \"26.85\" 0 \"0.0\" 0 \"0.0\" 0 \"26.85\" 0 \"US\" 0>\n").arg(ATT->R1);
      *s += QString("<R R2 1 180 200 -100 -30 0 1 \"%1 Ohm\" 1 \"26.85\" 0 \"0.0\" 0 \"0.0\" 0 \"26.85\" 0 \"US\" 0>\n").arg(ATT->Zin);
      *s += QString("<R R3 1 340 200 11 -30 0 1 \"%1 Ohm\" 1 \"26.85\" 0 \"0.0\" 0 \"0.0\" 0 \"26.85\" 0 \"US\" 0>\n").arg(ATT->Zout);
      *s += QString("<R R4 1 260 260 11 -14 0 1 \"%1 Ohm\" 1 \"26.85\" 0 \"0.0\" 0 \"0.0\" 0 \"26.85\" 0 \"US\" 0>\n").arg(ATT->R2);
      *s += "<GND * 1 260 290 0 0 0 0>\n";
      if (SP_box)
      {
        // S-parameter simulation block
        //-----------------------------
        // Resistor attenuators are broadband ckts, so it's pointless to ask the user to input the analysis freq sweep. Let's do a wideband
        // sweep and then the user can modify that in the schematic
        *s += "<.SP SP1 1 140 350 0 83 0 0 \"lin\" 1 \"50 MHz\" 1 \"3 GHz\" 1 \"200\" 1 \"no\" 0 \"1\" 0 \"2\" 0 \"no\" 0 \"no\" 0>\n";

        // Equations
        *s += "<Eqn Eqn1 1 360 350 -32 19 0 0 \"S21_dB=dB(S[2,1])\" 1 \"S11_dB=dB(S[1,1])\" 1 \"S22_dB=dB(S[2,2])\" 1 \"yes\" 0>\n";

        // Input term
        *s += QString("<Pac P1 1 50 200 -100 -26 0 1 \"1\" 1 \"%1 Ohm\" 1 \"0 dBm\" 0 \"1 GHz\" 0 \"26.85\" 0>\n").arg(ATT->Zin);
        *s += "<GND * 1 50 230 0 0 0 0>\n";

        // Output term
        *s += QString("<Pac P1 1 460 200 18 -26 0 1 \"1\" 1 \"%1 Ohm\" 1 \"0 dBm\" 0 \"1 GHz\" 0 \"26.85\" 0>\n").arg(ATT->Zout);
        *s += "<GND * 1 460 230 0 0 0 0>\n";
      }
      *s += "</Components>\n";

      *s += "<Wires>\n";
      *s += "<130 130 230 130 \"\" 0 0 0 \"\">\n";
      *s += "<290 130 380 130 \"\" 0 0 0 \"\">\n";
      *s += "<180 230 340 230 \"\" 0 0 0 \"\">\n";
      *s += "<180 130 180 170 \"\" 0 0 0 \"\">\n";
      *s += "<340 130 340 170 \"\" 0 0 0 \"\">\n";
      if (SP_box)
      {   // Additional wiring because of the input/output ports

          // Input port
          *s += "<50 130 50 170 \"\" 0 0 0 \"\">\n";
          *s += "<50 130 140 130 \"\" 0 0 0 \"\">\n";

          // Output port
          *s += "<460 130 460 170 \"\" 0 0 0 \"\">\n";
          *s += "<460 130 380 130 \"\" 0 0 0 \"\">\n";
      }
      *s += "</Wires>\n";

      *s += "<Diagrams>\n";
      *s += "</Diagrams>\n";
      *s += "<Paintings>\n";
      *s += QString("<Text 140 60 12 #000000 0 \"%1 dB Bridged-Tee-Type Attenuator\">\n").arg(ATT->Attenuation);
      if (!SP_box)
      {// If the SP simulation box option is activated, then the input and output ports are attached.
       // Thus, it doesn't make sense to have a text field indicating the input/output impedance
          *s += QString("<Text 50 122 10 #000000 0 \"Z1: %1 Ohm\">\n").arg(ATT->Zin);
          *s += QString("<Text 400 122 10 #000000 0 \"Z2: %1 Ohm\">\n").arg(ATT->Zout);
      }
      *s += "</Paintings>\n";
      break;

    case REFLECTION_TYPE:
      *s += QString("<R R1 1 130 300 15 -26 0 1 \"%1\" 1 \"26.85\" 0 \"0.0\" 0 \"0.0\" 0 \"26.85\" 0 \"US\" 0>\n").arg(ATT->R1);
      *s += "<GND * 1 130 330 0 0 0 0>\n";
      *s += QString("<Coupler X1 5 200 200 29 -26 0 1 \"0.7071\" 0 \"90\" 0 \"%1\" 0>\n").arg(ATT->Zin);
      *s += QString("<R R1 1 270 300 15 -26 0 1 \"%1\" 1 \"26.85\" 0 \"0.0\" 0 \"0.0\" 0 \"26.85\" 0 \"US\" 0>\n").arg(ATT->R1);
      *s += "<GND * 1 270 330 0 0 0 0>\n";

      if (SP_box)
      {
        // S-parameter simulation block
        //-----------------------------
        // Resistor attenuators are broadband ckts, so it's pointless to ask the user to input the analysis freq sweep. Let's do a wideband
        // sweep and then the user can modify that in the schematic
        *s += QString("<.SP SP1 1 80 400 0 83 0 0 \"lin\" 1 \"50 MHz\" 1 \"3 GHz\" 1 \"200\" 1 \"no\" 0 \"1\" 0 \"2\" 0 \"no\" 0 \"no\" 0>\n");

        // Equations
        *s += "<Eqn Eqn1 1 300 400 -32 19 0 0 \"S21_dB=dB(S[2,1])\" 1 \"S11_dB=dB(S[1,1])\" 1 \"S22_dB=dB(S[2,2])\" 1 \"yes\" 0>\n";

        // Input term
        *s += QString("<Pac P1 1 50 200 -100 -26 0 1 \"1\" 1 \"%1 Ohm\" 1 \"0 dBm\" 0 \"1 GHz\" 0 \"26.85\" 0>\n").arg(ATT->Zin);
        *s += "<GND * 1 50 230 0 0 0 0>\n";

        // Output term
        *s += QString("<Pac P1 1 350 200 18 -26 0 1 \"1\" 1 \"%1 Ohm\" 1 \"0 dBm\" 0 \"1 GHz\" 0 \"26.85\" 0>\n").arg(ATT->Zout);
        *s += "<GND * 1 350 230 0 0 0 0>\n";
      }
      *s += "</Components>\n";

      *s += "<Wires>\n";
      //First resistor to the coupler
      *s += "<130 270 130 250 \"\" 0 0 0 \"\">\n";
      *s += "<130 250 180 250 \"\" 0 0 0 \"\">\n";
      *s += "<180 230 180 250 \"\" 0 0 0 \"\">\n";

      //Second resistor to the coupler
      *s += "<270 270 270 250 \"\" 0 0 0 \"\">\n";
      *s += "<270 250 220 250 \"\" 0 0 0 \"\">\n";
      *s += "<220 230 220 250 \"\" 0 0 0 \"\">\n";

      *s += "<100 170 180 170 \"\" 0 0 0 \"\">\n";
      *s += "<220 170 300 170 \"\" 0 0 0 \"\">\n";

      if (SP_box)
      {   // Additional wiring because of the input/output ports

          // Input port
          *s += "<50 170 100 170 \"\" 0 0 0 \"\">\n";

          // Output port
          *s += "<300 170 350 170 \"\" 0 0 0 \"\">\n";
      }
      *s += "</Wires>\n";

      *s += "<Diagrams>\n";
      *s += "</Diagrams>\n";
      *s += "<Paintings>\n";
      *s += QString("<Text 100 100 12 #000000 0 \"%1 dB Reflection Attenuator\">\n").arg(ATT->Attenuation);
      if (!SP_box)
      {// If the SP simulation box option is activated, then the input and output ports are attached.
       // Thus, it doesn't make sense to have a text field indicating the input/output impedance
          *s += QString("<Text 70 135 10 #000000 0 \"Z0: %1 Ohm\">\n").arg(ATT->Zin);
          *s += QString("<Text 270 135 10 #000000 0 \"Z0: %1 Ohm\">\n").arg(ATT->Zout);
      }
      *s += "</Paintings>\n";

      break;

     case QW_SERIES_TYPE:

      if (ATT->useLumped)
      {
        double w = 2*PI*ATT->freq;
        *s += QString("<L L1 1 250 0 -40 -60 0 0 \"%1H\" 1 \"26.85\" 0 \"0.0\" 0 \"0.0\" 0 \"26.85\" 0 \"US\" 0>\n").arg(num2str(ATT->Zin/w));
        *s += QString("<C C1 1 180 -60 -80 -20 0 1 \"%1F\" 1 \"\" 0 \"neutral\" 0>\n").arg(num2str(1/(ATT->Zin*w)));
        *s += "<GND * 1 180 -90 0 0 1 0>\n";
        *s += QString("<C C1 1 320 -60 20 -20 0 1 \"%1F\" 1 \"\" 0 \"neutral\" 0>\n").arg(num2str(1/(ATT->Zin*w)));
        *s += "<GND * 1 320 -90 0 0 1 0>\n";
      }
      else
      {
          *s += QString("<TLIN Line1 1 250 0 -38 -75 0 0 \"%1 Ohm\" 1 \"%2\" 1 \"0 dB\" 0 \"26.85\" 0>\n").arg(ATT->Zin).arg(ConvertLengthFromM(ATT->R4));
      }
      *s += QString("<R R1 1 100 50 15 -26 0 1 \"%1 Ohm\" 1 \"26.85\" 0 \"0.0\" 0 \"0.0\" 0 \"26.85\" 0 \"US\" 0>\n").arg(RoundVariablePrecision(ATT->R1));
      *s += QString("<R R1 1 100 150 15 -26 0 1 \"%1 Ohm\" 1 \"26.85\" 0 \"0.0\" 0 \"0.0\" 0 \"26.85\" 0 \"US\" 0>\n").arg(RoundVariablePrecision(ATT->Zin));
      *s += "<GND * 1 100 180 0 0 0 0>\n";
      *s += QString("<R R1 1 400 150 -100 -15 0 1 \"%1 Ohm\" 1 \"26.85\" 0 \"0.0\" 0 \"0.0\" 0 \"26.85\" 0 \"US\" 0>\n").arg(RoundVariablePrecision(ATT->R1));
      *s += "<GND * 1 400 180 0 0 0 0>\n";

      if (SP_box)
      {
        // S-parameter simulation block
        //-----------------------------
        // The quarter-wave line is a narrowband device... so let's set the SP sweep from f0/2 to 3*f0/2
        QString freq_start = QString("%1").arg(0.5*ATT->freq*1e-6);//MHz
        QString freq_stop = QString("%1").arg(1.5*ATT->freq*1e-6);//MHz
        *s += QString("<.SP SP1 1 100 270 0 83 0 0 \"lin\" 1 \"%1 MHz\" 1 \"%2 MHz\" 1 \"200\" 1 \"no\" 0 \"1\" 0 \"2\" 0 \"no\" 0 \"no\" 0>\n").arg(freq_start).arg(freq_stop);

        // Equations
        *s += "<Eqn Eqn1 1 320 270 -32 19 0 0 \"S21_dB=dB(S[2,1])\" 1 \"S11_dB=dB(S[1,1])\" 1 \"S22_dB=dB(S[2,2])\" 1 \"yes\" 0>\n";

        // Input term
        *s += QString("<Pac P1 1 0 150 -100 -26 0 1 \"1\" 1 \"%1 Ohm\" 1 \"0 dBm\" 0 \"1 GHz\" 0 \"26.85\" 0>\n").arg(ATT->Zin);
        *s += "<GND * 1 0 180 0 0 0 0>\n";

        // Output term
        *s += QString("<Pac P1 1 500 150 18 -26 0 1 \"1\" 1 \"%1 Ohm\" 1 \"0 dBm\" 0 \"1 GHz\" 0 \"26.85\" 0>\n").arg(ATT->Zout);
        *s += "<GND * 1 500 180 0 0 0 0>\n";
      }
      *s += "</Components>\n";

      *s += "<Wires>\n";
      *s += "<100 20 100 0 \"\" 0 0 0 \"\">\n";
      *s += "<50 0 220 0 \"\" 0 0 0 \"\">\n";
      *s += "<100 80 100 120 \"\" 0 0 0 \"\">\n";
      *s += "<400 120 400 0 \"\" 0 0 0 \"\">\n";
      *s += "<280 0 450 0 \"\" 0 0 0 \"\">\n";

      if (SP_box)
      {
          //Term 1 to input port
          *s += "<0 120 0 0 \"\" 0 0 0 \"\">\n";
          *s += "<0 0 50 0 \"\" 0 0 0 \"\">\n";

          //Term 2 to output port
          *s += "<500 120 500 0 \"\" 0 0 0 \"\">\n";
          *s += "<450 0 500 0 \"\" 0 0 0 \"\">\n";
      }
      if (ATT->useLumped)
      {//Add extra wiring to connect the shunt capacitors to the main line
          *s += "<180 -30 180 0 \"\" 0 0 0 \"\">\n";
          *s += "<320 -30 320 0 \"\" 0 0 0 \"\">\n";
      }

      *s += "</Wires>\n";
      *s += "<Diagrams>\n";
      *s += "</Diagrams>\n";
      *s += "<Paintings>\n";

      //In the case of the Pi-equivalent of the quarter wavelength line it is needed to put the title slighly higher.
      if (ATT->useLumped) *s += QString("<Text 120 -140 12 #000000 0 \"%1 dB Quarter-Wave series attenuator\">\n").arg(ATT->Attenuation);
      else *s += QString("<Text 120 -120 12 #000000 0 \"%1 dB Quarter-Wave series attenuator\">\n").arg(ATT->Attenuation);

      if (!SP_box)
      {// If the SP simulation box option is activated, then the input and output ports are attached.
       // Thus, it doesn't make sense to have a text field indicating the input/output impedance
          *s += QString("<Text 50 -30 10 #000000 0 \"Z1: %1 Ohm\">\n").arg(ATT->Zin);
          *s += QString("<Text 390 -30 10 #000000 0 \"Z2: %1 Ohm\">\n").arg(ATT->Zout);
      }
      *s += "</Paintings>\n";
      break;

  case QW_SHUNT_TYPE:
      if (ATT->useLumped)
      {
        double w = 2*PI*ATT->freq;
        *s += QString("<L L1 1 200 60 20 -35 0 1 \"%1H\" 1 \"26.85\" 0 \"0.0\" 0 \"0.0\" 0 \"26.85\" 0 \"US\" 0>\n").arg(num2str(ATT->Zin/w));
        *s += QString("<C C1 1 200 -60 -80 -20 0 1 \"%1F\" 1 \"\" 0 \"neutral\" 0>\n").arg(num2str(1/(ATT->Zin*w)));
        *s += "<GND * 1 200 -90 0 0 1 0>\n";
        *s += QString("<C C1 1 320 150 0 60 0 1 \"%1F\" 1 \"\" 0 \"neutral\" 0>\n").arg(num2str(1/(ATT->Zin*w)));
        *s += "<GND * 1 320 180 0 0 0 0>\n";
      }
      else
      {
        *s += QString("<TLIN Line1 1 200 60 20 -35 0 1 \"%1 Ohm\" 1 \"%2\" 1 \"0 dB\" 0 \"26.85\" 0>\n").arg(ATT->Zin).arg(ConvertLengthFromM(ATT->R4));
      }
      *s += QString("<R R1 1 160 150 -40 60 0 1 \"%1 Ohm\" 1 \"26.85\" 0 \"0.0\" 0 \"0.0\" 0 \"26.85\" 0 \"US\" 0>\n").arg(RoundVariablePrecision(ATT->R1));
      *s += "<GND * 1 160 180 0 0 0 0>\n";
      *s += QString("<R R1 1 240 150 -20 60 0 1 \"%1 Ohm\" 1 \"26.85\" 0 \"0.0\" 0 \"0.0\" 0 \"26.85\" 0 \"US\" 0>\n").arg(RoundVariablePrecision(ATT->Zin));
      *s += "<GND * 1 240 180 0 0 0 0>\n";
      *s += QString("<R R1 1 300 0 -30 -60 0 0 \"%1 Ohm\" 1 \"26.85\" 0 \"0.0\" 0 \"0.0\" 0 \"26.85\" 0 \"US\" 0>\n").arg(RoundVariablePrecision(ATT->R1));

      if (SP_box)
      {
      // S-parameter simulation block
      //-----------------------------
      // The quarter-wave line is a narrowband device... so let's set the SP sweep from f0/2 to 3*f0/2
      QString freq_start = QString("%1").arg(0.5*ATT->freq*1e-6);//MHz
      QString freq_stop = QString("%1").arg(1.5*ATT->freq*1e-6);//MHz
      *s += QString("<.SP SP1 1 100 270 0 83 0 0 \"lin\" 1 \"%1 MHz\" 1 \"%2 MHz\" 1 \"200\" 1 \"no\" 0 \"1\" 0 \"2\" 0 \"no\" 0 \"no\" 0>\n").arg(freq_start).arg(freq_stop);

      // Equations
      *s += "<Eqn Eqn1 1 320 270 -32 19 0 0 \"S21_dB=dB(S[2,1])\" 1 \"S11_dB=dB(S[1,1])\" 1 \"S22_dB=dB(S[2,2])\" 1 \"yes\" 0>\n";

      // Input term
      *s += QString("<Pac P1 1 0 150 -100 -26 0 1 \"1\" 1 \"%1 Ohm\" 1 \"0 dBm\" 0 \"1 GHz\" 0 \"26.85\" 0>\n").arg(ATT->Zin);
      *s += "<GND * 1 0 180 0 0 0 0>\n";

      // Output term
      *s += QString("<Pac P1 1 500 150 18 -26 0 1 \"1\" 1 \"%1 Ohm\" 1 \"0 dBm\" 0 \"1 GHz\" 0 \"26.85\" 0>\n").arg(ATT->Zout);
      *s += "<GND * 1 500 180 0 0 0 0>\n";
   }
   *s += "</Components>\n";

   *s += "<Wires>\n";
   *s += "<160 120 160 105 \"\" 0 0 0 \"\">\n";//First resistor to qw line
   *s += "<160 105 200 105 \"\" 0 0 0 \"\">\n";//First resistor to qw line

   *s += "<240 120 240 105 \"\" 0 0 0 \"\">\n";//Second resistor to qw line
   *s += "<240 105 200 105 \"\" 0 0 0 \"\">\n";//First resistor to qw line

   *s += "<200 105 200 90 \"\" 0 0 0 \"\">\n";//Connect the previous wires to the line

   *s += "<200 30 200 0 \"\" 0 0 0 \"\">\n";//Connect qw line to main branch
   *s += "<200 0 270 0 \"\" 0 0 0 \"\">\n";//qw line to series resistor

   *s += "<120 0 200 0 \"\" 0 0 0 \"\">\n";//Input port
   *s += "<330 0 410 0 \"\" 0 0 0 \"\">\n";//Output port


   if (SP_box)
   {
       //Term 1 to input port
       *s += "<0 120 0 0 \"\" 0 0 0 \"\">\n";
       *s += "<0 0 120 0 \"\" 0 0 0 \"\">\n";

       //Term 2 to output port
       *s += "<500 120 500 0 \"\" 0 0 0 \"\">\n";
       *s += "<410 0 500 0 \"\" 0 0 0 \"\">\n";
   }

   if (ATT->useLumped)
   {//Add extra wiring to connect the shunt capacitors
       *s += "<320 120 320 105 \"\" 0 0 0 \"\">\n";
       *s += "<320 105 180 105 \"\" 0 0 0 \"\">\n";

       *s += "<200 -30 200 0 \"\" 0 0 0 \"\">\n";
   }
   *s += "</Wires>\n";
   *s += "<Diagrams>\n";
   *s += "</Diagrams>\n";
   *s += "<Paintings>\n";

   //Put the title a little bit higher because of the shunt cpa
   if (ATT->useLumped) *s += QString("<Text 120 -140 12 #000000 0 \"%1 dB Quarter-Wave shunt attenuator\">\n").arg(ATT->Attenuation);
   else *s += QString("<Text 120 -120 12 #000000 0 \"%1 dB Quarter-Wave shunt attenuator\">\n").arg(ATT->Attenuation);

   if (!SP_box)
   {// If the SP simulation box option is activated, then the input and output ports are attached.
    // Thus, it doesn't make sense to have a text field indicating the input/output impedance
       *s += QString("<Text 50 -30 10 #000000 0 \"Z1: %1 Ohm\">\n").arg(ATT->Zin);
       *s += QString("<Text 390 -30 10 #000000 0 \"Z2: %1 Ohm\">\n").arg(ATT->Zout);
   }
   *s += "</Paintings>\n";
   break;
    }

  return s;
}


//This function creates a string for the transmission line length and automatically changes the unit length if the value lies outside [1,999.99]
//COPIED FROM QUCS-POWERCOMBINING TOOL
QString ConvertLengthFromM(double len)
{
  int index = -6;
  double conv;

  do{
  conv=len;
  switch (index)
  {
    case 1: //mils
          conv *= 39370.1;
          if (conv > 999.99)
          {
            index = 4;//inches
            break;
          }
          if(conv < 1)
          {
            index = 2;//microns
            break;
          }
          return QString("%1 mil").arg(RoundVariablePrecision(conv));
    case 2: //microns
          conv *= 1e6;
          if (conv > 999.99)
          {
            index = 0;//milimeters
            break;
          }
          if(conv < 1)
          {
            index = 3;//nanometers
            break;
          }
          return QString("%1 um").arg(RoundVariablePrecision(conv));
    case 3: //nanometers
          conv *= 1e9;
          if (conv > 999.99)
          {
            index = 2;//microns
            break;
          }
          return QString("%1 nm").arg(RoundVariablePrecision(conv));
    case 4: //inch
          conv *= 39.3701;
          if (conv > 999.99)
          {
            index = 5;//feets
            break;
          }
          if(conv < 1)
          {
            index = 1;//mils
            break;
          }
          return QString("%1 in").arg(RoundVariablePrecision(conv));
    case 5: //ft
          conv *= 3.280841666667;
          if (conv > 999.99)
          {
            index = 6;//meters
            break;
          }
          if(conv < 1)
          {
            index = 4;//inches
            break;
          }
          return QString("%1 ft").arg(RoundVariablePrecision(conv));
    case 6: //m
          if(conv < 1)
          {
            index = 0;//mm
            break;
          }
          return QString("%1").arg(RoundVariablePrecision(len));
    default: //milimeters
          conv *=1e3;
          if (conv > 999.99)
          {
            index = 6;//meters
            break;
          }
          if(conv < 1)
          {
            index = 2;//microns
            break;
          }
          return QString("%1 mm").arg(RoundVariablePrecision(conv));
  }
  }while(true);
  return QString("");
}

//Rounds a double number using the minimum number of decimal places.
//COPIED FROM QUCS-POWERCOMBINING TOOL
QString RoundVariablePrecision(double val)
{
  int precision = 0;//By default, it takes 2 decimal places
  while (val*pow(10, precision) < 100) precision++;//Adds another decimal place if the conversion is less than 0.1, 0.01, etc
  return QString::number(val, 'F', precision);// Round to 'precision' decimals.
}


//COPIED FROM QUCS-POWERCOMBINING TOOL
// Converts a double number into string adding the corresponding prefix
QString num2str(double Num)
{
  char c = 0;
  double cal = fabs(Num);
  if(cal > 1e-20) {
    cal = log10(cal) / 3.0;
    if(cal < -0.2)  cal -= 0.98;
    int Expo = int(cal);

    if(Expo >= -5) if(Expo <= 4)
      switch(Expo) {
        case -5: c = 'f'; break;
        case -4: c = 'p'; break;
        case -3: c = 'n'; break;
        case -2: c = 'u'; break;
        case -1: c = 'm'; break;
        case  1: c = 'k'; break;
        case  2: c = 'M'; break;
        case  3: c = 'G'; break;
        case  4: c = 'T'; break;
      }

    if(c)  Num /= pow(10.0, double(3*Expo));
  }

  QString Str = RoundVariablePrecision(Num);
  if(c)  Str += c;

  return Str;
}
