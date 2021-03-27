module main();
Pac #(.Num(1), .Z(50 Ohm), .P(0 dBm), .f(1 GHz), .Temp(26.85)) P1(_net0, gnd);
GND #() *(gnd);
TLIN #(.Z(50 Ohm), .L(100 mm), .Alpha(0 dB), .Temp(26.85)) Line1(_net0, _net14);
TLIN #(.Z(70.7 Ohm), .L(75 mm), .Alpha(0 dB), .Temp(26.85)) Line2(_net14, _net5);
TLIN #(.Z(70.7 Ohm), .L(75 mm), .Alpha(0 dB), .Temp(26.85)) Line3(_net14, _net7);
Pac #(.Num(2), .Z(50 Ohm), .P(0 dBm), .f(1 GHz), .Temp(26.85)) P2(_net5, gnd);
Pac #(.Num(3), .Z(50 Ohm), .P(0 dBm), .f(1 GHz), .Temp(26.85)) P3(_net7, gnd);
GND #() *(gnd);
GND #() *(gnd);
R #(.R(100 Ohm), .Temp(26.85), .Tc1(0.0), .Tc2(0.0), .Tnom(26.85), .Symbol(european)) R1(_net7, _net5);
Eqn #(.Attenuation2(dB(S[2,1])), .Reflect(dB(S[1,1])), .Attenuation3(dB(S[3,1])), .Decoupling(dB(S[3,2])), .Export(yes)) Eqn1();
endmodule // main

module Symbol();
endmodule // Symbol

