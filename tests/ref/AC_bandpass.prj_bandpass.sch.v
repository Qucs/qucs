module main();
GND #() *(gnd);
GND #() *(gnd);
GND #() *(gnd);
GND #() *(gnd);
C #(.C(0.1 uF), .V(), .Symbol(neutral)) C1(Vin, _net5);
R #(.R(10 kOhm), .Temp(26.85), .Tc1(0.0), .Tc2(0.0), .Tnom(26.85), .Symbol(european)) R2(gnd, _net7);
R #(.R(10 kOhm), .Temp(26.85), .Tc1(0.0), .Tc2(0.0), .Tnom(26.85), .Symbol(european)) R5(gnd, _net9);
Sub$opa227 #(.File(opa227.sch)) OPA227_1(_net5, _net7, _net12);
Sub$opa227 #(.File(opa227.sch)) OPA227_2(_net26, _net9, Vout);
//AC1
GND #() *(gnd);
Vac #(.U(1 V), .f(1 kHz), .Phase(0), .Theta(0)) V1(Vin, gnd);
Eqn #(.y1(dB(Vout.v)), .y2(phase(Vout.v)), .Export(yes)) Eqn1();
R #(.R(10 kOhm), .Temp(26.85), .Tc1(0.0), .Tc2(0.0), .Tnom(26.85), .Symbol(european)) R1(_net7, _net12);
R #(.R(10 kOhm), .Temp(26.85), .Tc1(0.0), .Tc2(0.0), .Tnom(26.85), .Symbol(european)) R4(_net9, Vout);
C #(.C(0.1 uF), .V(), .Symbol(neutral)) C2(gnd, _net26);
R #(.R(1591 Ohm), .Temp(26.85), .Tc1(0.0), .Tc2(0.0), .Tnom(26.85), .Symbol(european)) R3(_net5, gnd);
R #(.R(1.59 Ohm), .Temp(26.85), .Tc1(0.0), .Tc2(0.0), .Tnom(26.85), .Symbol(european)) R6(_net12, _net26);
endmodule // main

module Symbol();
endmodule // Symbol

module Sub$opa227(_net2, _net1, _net0);
module main(P1, P2, P3);
VCVS #(.G(1), .T(0)) SRC2(_net6, P3, gnd, gnd);
GND #() *(gnd);
R #(.R(1 kOhm), .Temp(26.85), .Tc1(0.0), .Tc2(0.0), .Tnom(26.85), .Symbol(european)) R1(_net10, _net6);
C #(.C(1.98944 mF), .V(), .Symbol(neutral)) C1(gnd, _net6);
VCVS #(.G(1e8), .T(0)) SRC1(P1, _net10, gnd, P2);
Port #(.Num(1), .Type(analog)) P1(P1, P1);
Port #(.Num(3), .Type(analog)) P3(P3, P3);
Port #(.Num(2), .Type(analog)) P2(P2, P2);
GND #() *(gnd);
endmodule // main

module Symbol(_net2, _net1, _net0);
painting incomplete
painting incomplete
painting incomplete
painting incomplete
painting incomplete
// .port_ x=30 y=0 n=2 label=:port3
painting incomplete
painting incomplete
// .port_ x=-30 y=-30 n=1 label=:port2
painting incomplete
painting incomplete
painting incomplete
// .port_ x=-30 y=30 n=0 label=:port1
painting incomplete
painting incomplete
painting incomplete
endmodule // Symbol

module $Paintings$();
endmodule // :Paintings:

endmodule // Sub:opa227

