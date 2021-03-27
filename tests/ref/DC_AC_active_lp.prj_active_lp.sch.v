module main();
R #(.R(10k), .Temp(26.85), .Tc1(0.0), .Tc2(0.0), .Tnom(26.85), .Symbol(european)) R2(_net3, _net5);
R #(.R(10k), .Temp(26.85), .Tc1(0.0), .Tc2(0.0), .Tnom(26.85), .Symbol(european)) R1(_net5, _net2);
OpAmp #(.G(1e6), .Umax(15 V)) OP1(_net9, _net2, _net9);
C #(.C(29.62n), .V(), .Symbol(neutral)) C4(gnd, _net2);
GND #() *(gnd);
C #(.C(68.6n), .V(), .Symbol(neutral)) C1(_net5, _net9);
Vac #(.U(1 V), .f(1 GHz), .Phase(0), .Theta(0)) V1(_net3, gnd);
GND #() *(gnd);
R #(.R(10k), .Temp(26.85), .Tc1(0.0), .Tc2(0.0), .Tnom(26.85), .Symbol(european)) R3(_net9, _net16);
R #(.R(10k), .Temp(26.85), .Tc1(0.0), .Tc2(0.0), .Tnom(26.85), .Symbol(european)) R4(_net16, _net13);
OpAmp #(.G(1e6), .Umax(15 V)) OP2(_net20, _net13, _net20);
C #(.C(4.85n), .V(), .Symbol(neutral)) C5(gnd, _net13);
GND #() *(gnd);
C #(.C(93.7n), .V(), .Symbol(neutral)) C2(_net16, _net20);
R #(.R(10k), .Temp(26.85), .Tc1(0.0), .Tc2(0.0), .Tnom(26.85), .Symbol(european)) R5(_net20, _net25);
R #(.R(10k), .Temp(26.85), .Tc1(0.0), .Tc2(0.0), .Tnom(26.85), .Symbol(european)) R6(_net25, _net22);
OpAmp #(.G(1e6), .Umax(15 V)) OP3(Output, _net22, Output);
C #(.C(1n), .V(), .Symbol(neutral)) C6(gnd, _net22);
GND #() *(gnd);
C #(.C(256n), .V(), .Symbol(neutral)) C3(_net25, Output);
//AC1
Eqn #(.Ampl(dB(Output.v)), .Phase(phase(Output.v)), .Export(yes)) Eqn1();
//DC1
endmodule // main

module Symbol();
endmodule // Symbol

