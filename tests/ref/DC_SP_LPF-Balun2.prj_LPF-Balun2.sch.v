module main();
L #(.L(1.2uH), .I()) L1(_net40, _net5);
L #(.L(470nH), .I()) L2(_net5, _net3);
C #(.C(270pF), .V(), .Symbol(neutral)) C1(_net4, _net40);
C #(.C(270pF), .V(), .Symbol(neutral)) C2(_net6, _net5);
L #(.L(470nH), .I()) L3(_net14, _net40);
L #(.L(470nH), .I()) L4(_net8, _net4);
L #(.L(1.2uH), .I()) L5(_net4, _net6);
L #(.L(470nH), .I()) L6(_net6, _net20);
GND #() *(gnd);
Pac #(.Num(1), .Z(100 Ohm), .P(0 dBm), .f(1 GHz), .Temp(26.85)) P1(_net19, gnd);
GND #() *(gnd);
GND #() *(gnd);
Sub$IdealBalun #(.File(IdealBalun.sch)) Balun1(_net19, _net22, _net23, _net24, _net25);
Sub$IdealBalun #(.File(IdealBalun.sch)) Balun2(_net26, _net27, _net28, _net29, _net30);
GND #() *(gnd);
Vdc #(.U(1V)) V1(_net32, gnd);
VProbe #() Pr1(_net34, gnd);
GND #() *(gnd);
Pac #(.Num(2), .Z(100 Ohm), .P(0 dBm), .f(1 GHz), .Temp(26.85)) P2(_net26, gnd);
GND #() *(gnd);
Eqn #(.dBS21(dB(S[2,1])), .dBS11(dB(S[1,1])), .Export(yes)) Eqn1();
//DC1
endmodule // main

module :SymbolSection:();
endmodule // :SymbolSection:

module Sub:IdealBalun();
module :SymbolSection:();
endmodule // :SymbolSection:

module main(P1, P2, P3, P4, P5);
sTr #(.T1(0.5), .T2(0.5)) Tr1(P4, P1, P5, P3, P2, P2);
Port #(.Num(3), .Type(analog)) P3(P3, P3);
Port #(.Num(4), .Type(analog)) P4(P4, P4);
Port #(.Num(5), .Type(analog)) P5(P5, P5);
Port #(.Num(2), .Type(analog)) P2(P2, P2);
Port #(.Num(1), .Type(analog)) P1(P1, P1);
endmodule // main

module :Paintings:();
endmodule // :Paintings:

endmodule // Sub:IdealBalun

module :Paintings:();
endmodule // :Paintings:

