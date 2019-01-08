# Qucs 0.1.0  ../../tests/AC_SW_resonance.prj/resonance.sch

Vac #(.U(1 V), .f(1 GHz), .Phase(0), .Theta(0)) V1( /**/,  /**/);
GND #() *(gnd /**/);
GND #() *(gnd /**/);
R #(.R(R_par), .Temp(26.85), .Tc1(0.0), .Tc2(0.0), .Tnom(26.85), .Symbol(european)) R1( /**/,  /**/);
C #(.C(2 pF), .V(), .Symbol(neutral)) C1(gnd /**/,  /**/);
L #(.L(13 nH), .I()) L1( /**/,  /**/);
//SW1
//AC1
