module main();
Vdc #(.U(1 V)) V1(in, gnd);
GND #() *(gnd);
R #(.R(50 Ohm), .Temp(26.85), .Tc1(0.0), .Tc2(0.0), .Tnom(26.85), .Symbol(US)) R1(out, gnd);
//DC1
Sub$nest1 #(.File(nest1.sch)) SC1(in, out);
endmodule // main

//else?
module :SymbolSection:();
endmodule // :SymbolSection:

//else?
module Sub:nest1(_net0, _net1);
module :SymbolSection:(_net0, _net1);
painting incomplete
// .port_ x=-30 y=0 n=0 label=:port1
painting incomplete
// .port_ x=30 y=0 n=1 label=:port2
painting incomplete
endmodule // :SymbolSection:

module main(P1, P2);
Port #(.Num(2), .Type(analog)) P2(P2, P2);
Port #(.Num(1), .Type(analog)) P1(P1, P1);
GND #() *(gnd);
C #(.C(1 pF), .V(), .Symbol(neutral)) C1(gnd, P2);
R #(.R(0 Ohm), .Temp(26.85), .Tc1(0.0), .Tc2(0.0), .Tnom(26.85), .Symbol(US)) R1(P1, _net7);
endmodule // main

module :Paintings:();
endmodule // :Paintings:

endmodule // Sub:nest1

//else?
module :Paintings:();
endmodule // :Paintings:

