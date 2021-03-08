module main(P1, P2);
Port #(.Num(2), .Type(analog)) P2(P2, P2);
Port #(.Num(1), .Type(analog)) P1(P1, P1);
R #(.R(0 Ohm), .Temp(26.85), .Tc1(0.0), .Tc2(0.0), .Tnom(26.85), .Symbol(US)) R1(P1, P2);
GND #() *(gnd);
C #(.C(1 pF), .V(), .Symbol(neutral)) C1(gnd, P2);
endmodule // main

module :SymbolSection:(_net1, _net0);
painting incomplete
// .port_ x=-30 y=0 n=1 label=:port2
painting incomplete
painting incomplete
painting incomplete
painting incomplete
painting incomplete
painting incomplete
painting incomplete
// .port_ x=30 y=0 n=0 label=:port1
painting incomplete
painting incomplete
endmodule // :SymbolSection:

module :Paintings:();
endmodule // :Paintings:

