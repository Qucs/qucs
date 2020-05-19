# Qucs 0.1.0  ../../tests/AC_groupdelay_ac.prj/groupdelay_ac.sch

C #(.C(39.01pF), .V(), .Symbol(neutral)) C1(gnd, _net16);
GND #() *(gnd);
L #(.L(280.9nH), .I()) L1(_net16, _net17);
C #(.C(39.01pF), .V(), .Symbol(neutral)) C2(gnd, _net17);
GND #() *(gnd);
GND #() *(_net8);
Vac #(.U(1 V), .f(1 GHz), .Phase(0), .Theta(0)) V1(_net19, _net8);
Vac #(.U(0 V), .f(1 GHz), .Phase(0), .Theta(0)) V2(_net13, _net10);
GND #() *(_net10);
R #(.R(Z0), .Temp(26.85), .Tc1(0.0), .Tc2(0.0), .Tnom(26.85), .Symbol(european)) R2(_net17, _net13);
Eqn #(.dBS21_3rd(dB(S21)), .S21(b2/a1), .S11(b1/a1), .a1((P1.v+Z0*-V1.i)/(2*sqrt(Z0))), .Z0(60), .b1((P1.v-Z0*-V1.i)/(2*sqrt(Z0))), .b2((P2.v-Z0*-V2.i)/(2*sqrt(Z0))), .gain_phase(unwrap(angle(S21))), .group_delay(-diff(gain_phase,2*pi*acfrequency)), .Export(yes)) Eqn1();
R #(.R(Z0), .Temp(26.85), .Tc1(0.0), .Tc2(0.0), .Tnom(26.85), .Symbol(european)) R1(_net19, _net16);
