module main();
Vdc #(.U(1 V)) V1(in, gnd);
GND #() *(gnd);
R #(.R(50 Ohm), .Temp(26.85), .Tc1(0.0), .Tc2(0.0), .Tnom(26.85), .Symbol(US)) R1(out, gnd);
//DC1
Sub$nest1 #(.File(nest1.sch)) SC1(in, out);
endmodule // main

paramset Sub ModelFactory;
endparmset //Sub

paramset Verilog ModelFactory;
endparmset //Verilog

module Symbol();
endmodule // Symbol

module Sub$nest2(_net1, _net0);
module main(P1, P2);
Port #(.Num(2), .Type(analog)) P2(P2, P2);
Port #(.Num(1), .Type(analog)) P1(P1, P1);
R #(.R(0 Ohm), .Temp(26.85), .Tc1(0.0), .Tc2(0.0), .Tnom(26.85), .Symbol(US)) R1(P1, P2);
GND #() *(gnd);
C #(.C(1 pF), .V(), .Symbol(neutral)) C1(gnd, P2);
endmodule // main

module Symbol(_net1, _net0);
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
endmodule // Symbol

module $Paintings$();
endmodule // :Paintings:

endmodule // Sub:nest2

module Sub$nest1(_net0, _net1);
module main(P1, P2);
Port #(.Num(2), .Type(analog)) P2(P2, P2);
Port #(.Num(1), .Type(analog)) P1(P1, P1);
GND #() *(gnd);
C #(.C(1 pF), .V(), .Symbol(neutral)) C1(gnd, P2);
Sub$nest2 #(.File(nest2.sch)) SC2(P2, _net9);
R #(.R(0 Ohm), .Temp(26.85), .Tc1(0.0), .Tc2(0.0), .Tnom(26.85), .Symbol(US)) R1(P1, _net9);
endmodule // main

module Symbol(_net0, _net1);
painting incomplete
// .port_ x=-30 y=0 n=0 label=:port1
painting incomplete
// .port_ x=30 y=0 n=1 label=:port2
painting incomplete
endmodule // Symbol

module $Paintings$();
endmodule // :Paintings:

endmodule // Sub:nest1

