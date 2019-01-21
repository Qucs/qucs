# Qucs 0.1.0  ../../tests/AC_groupdelay_ac.prj/groupdelay_ac.sch

C #(.C(39.01pF), .V(), .Symbol(neutral)) C1(0, P1);
GND #() *(0);
L #(.L(280.9nH), .I()) L1(P1, P2);
C #(.C(39.01pF), .V(), .Symbol(neutral)) C2(0, P2);
GND #() *(0);
GND #() *(0);
Vac #(.U(1 V), .f(1 GHz), .Phase(0), .Theta(0)) V1(_net0, 0);
Vac #(.U(0 V), .f(1 GHz), .Phase(0), .Theta(0)) V2(_net1, 0);
GND #() *(0);
R #(.R(Z0), .Temp(26.85), .Tc1(0.0), .Tc2(0.0), .Tnom(26.85), .Symbol(european)) R2(P2, _net1);
//AC1
Eqn #(.dBS21_3rd(dB(S21)), .S21(b2/a1), .S11(b1/a1), .a1((P1.v+Z0*-V1.i)/(2*sqrt(Z0))), .Z0(60), .b1((P1.v-Z0*-V1.i)/(2*sqrt(Z0))), .b2((P2.v-Z0*-V2.i)/(2*sqrt(Z0))), .gain_phase(unwrap(angle(S21))), .group_delay(-diff(gain_phase,2*pi*acfrequency)), .Export(yes)) Eqn1();
R #(.R(Z0), .Temp(26.85), .Tc1(0.0), .Tc2(0.0), .Tnom(26.85), .Symbol(european)) R1(_net0, P1);
