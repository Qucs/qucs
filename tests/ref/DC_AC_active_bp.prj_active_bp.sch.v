module main();
OpAmp #(.G(1e6), .Umax(15 V)) OP1(gnd, _net11, Output);
GND #() *(gnd);
GND #() *(gnd);
GND #() *(gnd);
Vac #(.U(1 V), .f(1 GHz), .Phase(0), .Theta(0)) V1(_net0, gnd);
R #(.R(390), .Temp(26.85), .Tc1(0.0), .Tc2(0.0), .Tnom(26.85), .Symbol(european)) R3(_net0, _net8);
R #(.R(5.6k), .Temp(26.85), .Tc1(0.0), .Tc2(0.0), .Tnom(26.85), .Symbol(european)) R2(gnd, _net8);
C #(.C(47n), .V(), .Symbol(neutral)) C1(_net8, _net11);
C #(.C(47n), .V(), .Symbol(neutral)) C2(_net8, Output);
R #(.R(3.9k), .Temp(26.85), .Tc1(0.0), .Tc2(0.0), .Tnom(26.85), .Symbol(european)) R1(_net11, Output);
//AC1
//DC1
endmodule // main

paramset Sub ModelFactory;
endparmset //Sub

paramset Verilog ModelFactory;
endparmset //Verilog

module Symbol();
endmodule // Symbol

