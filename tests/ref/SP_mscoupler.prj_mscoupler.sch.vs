// VS::printSubckt main
module main();
Pac #(.$xposition(80), .$yposition(90), .$tx(-74), .$ty(-74), .Num(1), .Z(50 Ohm), .P(0 dBm), .f(1 GHz), .Temp(26.85)) P1(net_80_60, net_80_120);
Pac #(.$xposition(300), .$yposition(90), .$tx(18), .$ty(18), .Num(2), .Z(50 Ohm), .P(0 dBm), .f(1 GHz), .Temp(26.85)) P2(net_300_60, net_300_120);
Pac #(.$xposition(260), .$yposition(180), .$tx(18), .$ty(18), .Num(3), .Z(50 Ohm), .P(0 dBm), .f(1 GHz), .Temp(26.85)) P3(net_260_150, net_260_210);
Pac #(.$xposition(120), .$yposition(160), .$tx(-74), .$ty(-74), .Num(4), .Z(50 Ohm), .P(0 dBm), .f(1 GHz), .Temp(26.85)) P4(net_120_130, net_120_190);
GND #(.$xposition(80), .$yposition(120), .$tx(0), .$ty(0)) anonymous_gnd_hack_0(net_80_120);
GND #(.$xposition(120), .$yposition(190), .$tx(0), .$ty(0)) anonymous_gnd_hack_1(net_120_190);
GND #(.$xposition(300), .$yposition(120), .$tx(0), .$ty(0)) anonymous_gnd_hack_2(net_300_120);
GND #(.$xposition(260), .$yposition(210), .$tx(0), .$ty(0)) anonymous_gnd_hack_3(net_260_210);
SUBST #(.$xposition(390), .$yposition(180), .$tx(-30), .$ty(-30), .er(9.8), .h(0.635 mm), .t(17.5 um), .tand(0.0001), .rho(2.43902e-08), .D(1.5e-07)) SubstTC1();
MCOUPLED #(.$xposition(170), .$yposition(70), .$tx(-26), .$ty(-26), .Subst(SubstTC1), .W(0.518 mm), .L(14.94 mm), .S(0.185 mm), .Model(Kirschning), .DispModel(Kirschning), .Temp(26.85)) MSTC1(net_140_40, net_200_40, net_200_100, net_140_100);
Eqn #(.$xposition(250), .$yposition(300), .$tx(-23), .$ty(-23), .reflect(dB(S[1,1])), .isolated(dB(S[3,1])), .through(dB(S[2,1])), .coupled(dB(S[4,1])), .Export(yes)) Eqn1();
wire #(.$xposition(80), .$yposition(40)) noname(net_80_40, net_140_40);
wire #(.$xposition(80), .$yposition(40)) noname(net_80_40, net_80_60);
wire #(.$xposition(120), .$yposition(100)) noname(net_120_100, net_120_130);
wire #(.$xposition(120), .$yposition(100)) noname(net_120_100, net_140_100);
wire #(.$xposition(300), .$yposition(40)) noname(net_300_40, net_300_60);
wire #(.$xposition(200), .$yposition(40)) noname(net_200_40, net_300_40);
wire #(.$xposition(260), .$yposition(100)) noname(net_260_100, net_260_150);
wire #(.$xposition(200), .$yposition(100)) noname(net_200_100, net_260_100);
place #(.$xposition(80), .$yposition(60)) net_80_60(net_80_60);
place #(.$xposition(80), .$yposition(120)) net_80_120(net_80_120);
place #(.$xposition(300), .$yposition(60)) net_300_60(net_300_60);
place #(.$xposition(300), .$yposition(120)) net_300_120(net_300_120);
place #(.$xposition(260), .$yposition(150)) net_260_150(net_260_150);
place #(.$xposition(260), .$yposition(210)) net_260_210(net_260_210);
place #(.$xposition(120), .$yposition(130)) net_120_130(net_120_130);
place #(.$xposition(120), .$yposition(190)) net_120_190(net_120_190);
place #(.$xposition(140), .$yposition(40)) net_140_40(net_140_40);
place #(.$xposition(200), .$yposition(40)) net_200_40(net_200_40);
place #(.$xposition(200), .$yposition(100)) net_200_100(net_200_100);
place #(.$xposition(140), .$yposition(100)) net_140_100(net_140_100);
place #(.$xposition(80), .$yposition(40)) net_80_40(net_80_40);
place #(.$xposition(120), .$yposition(100)) net_120_100(net_120_100);
place #(.$xposition(300), .$yposition(40)) net_300_40(net_300_40);
place #(.$xposition(260), .$yposition(100)) net_260_100(net_260_100);
endmodule // main

