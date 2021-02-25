module main()
C #(.C(39.01pF), .V(), .Symbol(neutral)) C1(net_240_250, net_240_190);
GND #() anonymous_gnd_hack_0(net_240_250);
L #(.L(280.9nH), .I()) L1(net_280_140, net_340_140);
C #(.C(39.01pF), .V(), .Symbol(neutral)) C2(net_380_250, net_380_190);
GND #() anonymous_gnd_hack_1(net_380_250);
GND #() anonymous_gnd_hack_2(net_60_250);
Vac #(.U(1 V), .f(1 GHz), .Phase(0), .Theta(0)) V1(net_60_170, net_60_230);
Vac #(.U(0 V), .f(1 GHz), .Phase(0), .Theta(0)) V2(net_550_170, net_550_230);
GND #() anonymous_gnd_hack_3(net_550_250);
R #(.R(Z0), .Temp(26.85), .Tc1(0.0), .Tc2(0.0), .Tnom(26.85), .Symbol(european)) R2(net_450_140, net_510_140);
Eqn #(.dBS21_3rd(dB(S21)), .S21(b2/a1), .S11(b1/a1), .a1((P1.v+Z0*-V1.i)/(2*sqrt(Z0))), .Z0(60), .b1((P1.v-Z0*-V1.i)/(2*sqrt(Z0))), .b2((P2.v-Z0*-V2.i)/(2*sqrt(Z0))), .gain_phase(unwrap(angle(S21))), .group_delay(-diff(gain_phase,2*pi*acfrequency)), .Export(yes)) Eqn1();
R #(.R(Z0), .Temp(26.85), .Tc1(0.0), .Tc2(0.0), .Tnom(26.85), .Symbol(european)) R1(net_120_140, net_180_140);
endmodule
