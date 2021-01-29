module main();
Pac #(.Num(1), .Z(100 Ohm), .P(0 dBm), .f(1 GHz), .Temp(26.85)) P1(_net12, gnd);
L #(.L(1.2uH), .I()) L1(_net41, _net7);
L #(.L(470nH), .I()) L2(_net7, _net5);
C #(.C(270pF), .V(), .Symbol(neutral)) C1(_net6, _net41);
C #(.C(270pF), .V(), .Symbol(neutral)) C2(_net8, _net7);
L #(.L(470nH), .I()) L3(_net22, _net41);
GND #() *(gnd);
Eqn #(.dBS21(dB(S[2,1])), .dBS11(dB(S[1,1])), .Export(yes)) Eqn1();
Pac #(.Num(2), .Z(100 Ohm), .P(0 dBm), .f(1 GHz), .Temp(26.85)) P2(_net10, gnd);
GND #() *(gnd);
L #(.L(470nH), .I()) L4(_net15, _net6);
L #(.L(1.2uH), .I()) L5(_net6, _net8);
L #(.L(470nH), .I()) L6(_net8, _net21);
sTr #(.T1(0.5), .T2(0.5)) Tr5(_net22, _net12, gnd, _net15, _net27, _net27);
sTr #(.T1(0.5), .T2(0.5)) Tr6(_net5, _net10, gnd, _net21, _net29, _net29);
GND #() *(gnd);
GND #() *(gnd);
//DC1
Vdc #(.U(1V)) V1(_net27, gnd);
GND #() *(gnd);
VProbe #() Pr1(_net29, gnd);
GND #() *(gnd);
endmodule // main

Sub #(.File()) Sub(unknown_net);
//else?
module :SymbolSection:();
endmodule // :SymbolSection:

//else?
module :Paintings:();
endmodule // :Paintings:

