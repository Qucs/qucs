# Qucs 0.1.0  ../../tests/AC_bandpass.prj/bandpass.sch

GND #() *(gnd);
GND #() *(gnd);
GND #() *(gnd);
GND #() *(gnd);
C #(.C(0.1 uF), .V(), .Symbol(neutral)) C1(Vin, _net5);
R #(.R(10 kOhm), .Temp(26.85), .Tc1(0.0), .Tc2(0.0), .Tnom(26.85), .Symbol(european)) R2(gnd, _net7);
R #(.R(10 kOhm), .Temp(26.85), .Tc1(0.0), .Tc2(0.0), .Tnom(26.85), .Symbol(european)) R5(gnd, _net9);
opa227 #(.File(opa227.sch)) OPA227_1(_net5, _net7, _net12);
opa227 #(.File(opa227.sch)) OPA227_2(_net26, _net9, Vout);
GND #() *(gnd);
Vac #(.U(1 V), .f(1 kHz), .Phase(0), .Theta(0)) V1(Vin, gnd);
Eqn #(.y1(dB(Vout.v)), .y2(phase(Vout.v)), .Export(yes)) Eqn1();
R #(.R(10 kOhm), .Temp(26.85), .Tc1(0.0), .Tc2(0.0), .Tnom(26.85), .Symbol(european)) R1(_net7, _net12);
R #(.R(10 kOhm), .Temp(26.85), .Tc1(0.0), .Tc2(0.0), .Tnom(26.85), .Symbol(european)) R4(_net9, Vout);
C #(.C(0.1 uF), .V(), .Symbol(neutral)) C2(gnd, _net26);
R #(.R(1591 Ohm), .Temp(26.85), .Tc1(0.0), .Tc2(0.0), .Tnom(26.85), .Symbol(european)) R3(_net5, gnd);
R #(.R(1.59 Ohm), .Temp(26.85), .Tc1(0.0), .Tc2(0.0), .Tnom(26.85), .Symbol(european)) R6(_net12, _net26);
