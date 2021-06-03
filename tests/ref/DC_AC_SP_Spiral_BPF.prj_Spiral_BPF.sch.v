module main();
C #(.C(195 fF), .V(), .Symbol(neutral)) C1(gnd, _net20);
C #(.C(155 fF), .V(), .Symbol(neutral)) C2(_net20, _net3);
SUBST #(.er(4.5), .h(0.8 mm), .t(35 um), .tand(2e-4), .rho(0.022e-6), .D(0.15e-6)) Subst1();
GND #() *(gnd);
GND #() *(gnd);
GND #() *(gnd);
GND #() *(gnd);
C #(.C(320 fF), .V(), .Symbol(neutral)) C3(gnd, _net3);
C #(.C(82 fF), .V(), .Symbol(neutral)) C5(gnd, out);
C #(.C(600 fF), .V(), .Symbol(neutral)) C4(_net3, out);
//AC1
//DC1
Eqn #(.S21_dB(dB(S[2,1])), .S11_dB(dB(S[1,1])), .Export(yes)) Eqn1();
Pac #(.Num(2), .Z(50 Ohm), .P(0 W), .f(1 GHz), .Temp(-273.15)) P2(out, gnd);
Pac #(.Num(1), .Z(50 Ohm), .P(0 dBm), .f(1 GHz), .Temp(-273.15)) P1(_net20, _net17);
Vdc #(.U(1.1 V)) V1(_net17, gnd);
GND #() *(gnd);
endmodule // main

paramset Sub ModelFactory;
endparmset //Sub

paramset Verilog ModelFactory;
endparmset //Verilog

module Symbol();
endmodule // Symbol

