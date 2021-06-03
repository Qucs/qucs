module main();
Vdc #(.U(1 V)) V1(in, gnd);
GND #() *(gnd);
R #(.R(50 Ohm), .Temp(26.85), .Tc1(0.0), .Tc2(0.0), .Tnom(26.85), .Symbol(US)) R1(out, gnd);
//DC1
Sub$short_circuit #(.File(short_circuit.sch)) SC1(out, in);
// `ifdef QUCS_DIAGRAMS
// Tab
// Tab
// `endif
endmodule // main

paramset Sub ModelFactory;
endparmset //Sub

paramset Verilog ModelFactory;
endparmset //Verilog

module Symbol();
endmodule // Symbol

module Sub$short_circuit(_net1, _net0);
module main(P1, P1);
Port #(.Num(2), .Type(analog)) P2(P1, P1);
Port #(.Num(1), .Type(analog)) P1(P1, P1);
R #(.R(0 Ohm), .Temp(26.85), .Tc1(0.0), .Tc2(0.0), .Tnom(26.85), .Symbol(US)) R1(P1, P1);
GND #() *(gnd);
C #(.C(1 pF), .V(), .Symbol(neutral)) C1(gnd, P1);
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

endmodule // Sub:short_circuit

