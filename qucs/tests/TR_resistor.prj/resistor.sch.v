# Qucs 0.1.0  ../../tests/TR_resistor.prj/resistor.sch

GND #() *(0);
//TR1
R #(.R(50 Ohm), .Temp(26.85), .Tc1(0.0), .Tc2(0.0), .Tnom(26.85), .Symbol(european)) R2(Vout, 0);
R #(.R(50 Ohm), .Temp(26.85), .Tc1(0.0), .Tc2(0.0), .Tnom(26.85), .Symbol(european)) R1(Vin, Vout);
Vac #(.U(10 V), .f(1kHz), .Phase(0), .Theta(0)) V1(Vin, 0);
