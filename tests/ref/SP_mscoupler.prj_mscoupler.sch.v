module main();
Pac #(.Num(1), .Z(50 Ohm), .P(0 dBm), .f(1 GHz), .Temp(26.85)) P1(_net13, gnd);
Pac #(.Num(2), .Z(50 Ohm), .P(0 dBm), .f(1 GHz), .Temp(26.85)) P2(_net6, gnd);
Pac #(.Num(3), .Z(50 Ohm), .P(0 dBm), .f(1 GHz), .Temp(26.85)) P3(_net2, gnd);
Pac #(.Num(4), .Z(50 Ohm), .P(0 dBm), .f(1 GHz), .Temp(26.85)) P4(_net8, gnd);
GND #() *(gnd);
GND #() *(gnd);
GND #() *(gnd);
GND #() *(gnd);
SUBST #(.er(9.8), .h(0.635 mm), .t(17.5 um), .tand(0.0001), .rho(2.43902e-08), .D(1.5e-07)) SubstTC1();
//SPTC1
MCOUPLED #(.Subst(SubstTC1), .W(0.518 mm), .L(14.94 mm), .S(0.185 mm), .Model(Kirschning), .DispModel(Kirschning), .Temp(26.85)) MSTC1(_net13, _net6, _net2, _net8);
Eqn #(.reflect(dB(S[1,1])), .isolated(dB(S[3,1])), .through(dB(S[2,1])), .coupled(dB(S[4,1])), .Export(yes)) Eqn1();
endmodule // main

Sub #(.File()) Sub((null));
