module main();
GND #() *(gnd);
C #(.C(1 uF), .V(), .Symbol(neutral)) C1(_net10, _net2);
Pac #(.Num(1), .Z(50 Ohm), .P(0 dBm), .f(1 GHz), .Temp(26.85)) P1(_net10, gnd);
GND #() *(gnd);
OpAmp #(.G(1e6), .Umax(15 V)) OP1(_net2, _net9, _net9);
R #(.R(10 Ohm), .Temp(26.85), .Tc1(0.0), .Tc2(0.0), .Tnom(26.85), .Symbol(european)) R1(_net10, _net9);
R #(.R(500k), .Temp(26.85), .Tc1(0.0), .Tc2(0.0), .Tnom(26.85), .Symbol(european)) R2(gnd, _net2);
GND #() *(gnd);
GND #() *(gnd);
Pac #(.Num(2), .Z(50 Ohm), .P(0 dBm), .f(1 GHz), .Temp(26.85)) P2(_net8, gnd);
R #(.R(500k), .Temp(26.85), .Tc1(0.0), .Tc2(0.0), .Tnom(26.85), .Symbol(european)) R4(gnd, _net8);
R #(.R(10 Ohm), .Temp(26.85), .Tc1(0.0), .Tc2(0.0), .Tnom(26.85), .Symbol(european)) R3(_net15, _net8);
GND #() *(gnd);
L #(.L(5 H), .I()) L1(gnd, _net15);
//DC1
endmodule // main

//else?
module :SymbolSection:();
endmodule // :SymbolSection:

//else?
module :Paintings:();
endmodule // :Paintings:

