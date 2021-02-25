module main();
Vac #(.U(1 V), .f(1 GHz), .Phase(0), .Theta(0)) V1(_net0, _net1);
GND #() anonymous_gnd_hack_0(gnd);
GND #() anonymous_gnd_hack_1(gnd);
R #(.R(R_par), .Temp(26.85), .Tc1(0.0), .Tc2(0.0), .Tnom(26.85), .Symbol(european)) R1(_net4, _net5);
C #(.C(2 pF), .V(), .Symbol(neutral)) C1(gnd, _net6);
L #(.L(13 nH), .I()) L1(_net6, _net4);
endmodule // main

