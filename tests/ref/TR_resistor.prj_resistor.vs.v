paramset Sub ModelFactory;
endparmset //Sub

paramset Verilog ModelFactory;
endparmset //Verilog

module main();
GND #() anonymous_gnd_hack_0(gnd);
R #(.R(50 Ohm), .Temp(26.85), .Tc1(0.0), .Tc2(0.0), .Tnom(26.85), .Symbol(european)) R2(_net1, _net2);
R #(.R(50 Ohm), .Temp(26.85), .Tc1(0.0), .Tc2(0.0), .Tnom(26.85), .Symbol(european)) R1(_net3, _net4);
Vac #(.U(10 V), .f(1kHz), .Phase(0), .Theta(0)) V1(_net5, _net6);
endmodule // main

