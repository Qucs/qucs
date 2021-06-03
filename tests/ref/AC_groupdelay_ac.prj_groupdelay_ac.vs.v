paramset Sub ModelFactory;
endparmset //Sub

paramset Verilog ModelFactory;
endparmset //Verilog

module main();
C #(.C(39.01pF), .V(), .Symbol(neutral)) C1(gnd, _net1);
GND #() anonymous_gnd_hack_0(gnd);
L #(.L(280.9nH), .I()) L1(_net2, _net3);
C #(.C(39.01pF), .V(), .Symbol(neutral)) C2(gnd, _net5);
GND #() anonymous_gnd_hack_1(gnd);
GND #() anonymous_gnd_hack_2(gnd);
Vac #(.U(1 V), .f(1 GHz), .Phase(0), .Theta(0)) V1(_net7, _net8);
Vac #(.U(0 V), .f(1 GHz), .Phase(0), .Theta(0)) V2(_net9, _net10);
GND #() anonymous_gnd_hack_3(gnd);
R #(.R(Z0), .Temp(26.85), .Tc1(0.0), .Tc2(0.0), .Tnom(26.85), .Symbol(european)) R2(_net12, _net13);
R #(.R(Z0), .Temp(26.85), .Tc1(0.0), .Tc2(0.0), .Tnom(26.85), .Symbol(european)) R1(_net14, _net15);
endmodule // main

