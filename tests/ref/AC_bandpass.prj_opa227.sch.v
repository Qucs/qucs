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

//else?
module :SymbolSection:();
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
endmodule // :SymbolSection:

//else?
module :Paintings:();
endmodule // :Paintings:

