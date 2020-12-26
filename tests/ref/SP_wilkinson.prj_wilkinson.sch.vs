module main();
Pac #(.Num(1), .Z(50 Ohm), .P(0 dBm), .f(1 GHz), .Temp(26.85)) P1(net_60_80, net_60_140);
GND #() anonymous_gnd_hack_0(net_60_140);
TLIN #(.Z(50 Ohm), .L(100 mm), .Alpha(0 dB), .Temp(26.85)) Line1(net_100_80, net_160_80);
TLIN #(.Z(70.7 Ohm), .L(75 mm), .Alpha(0 dB), .Temp(26.85)) Line2(net_200_40, net_260_40);
TLIN #(.Z(70.7 Ohm), .L(75 mm), .Alpha(0 dB), .Temp(26.85)) Line3(net_200_140, net_260_140);
Pac #(.Num(2), .Z(50 Ohm), .P(0 dBm), .f(1 GHz), .Temp(26.85)) P2(net_410_40, net_410_100);
Pac #(.Num(3), .Z(50 Ohm), .P(0 dBm), .f(1 GHz), .Temp(26.85)) P3(net_410_140, net_410_200);
GND #() anonymous_gnd_hack_1(net_410_200);
GND #() anonymous_gnd_hack_2(net_410_100);
R #(.R(100 Ohm), .Temp(26.85), .Tc1(0.0), .Tc2(0.0), .Tnom(26.85), .Symbol(european)) R1(net_300_120, net_300_60);
//SP1
Eqn #(.Attenuation2(dB(S[2,1])), .Reflect(dB(S[1,1])), .Attenuation3(dB(S[3,1])), .Decoupling(dB(S[3,2])), .Export(yes)) Eqn1();
wire #() noname(net_180_40, net_200_40);
wire #() noname(net_180_40, net_180_80);
wire #() noname(net_180_140, net_200_140);
wire #() noname(net_260_40, net_300_40);
wire #() noname(net_260_140, net_300_140);
wire #() noname(net_60_80, net_100_80);
wire #() noname(net_180_80, net_180_140);
wire #() noname(net_160_80, net_180_80);
wire #() noname(net_300_140, net_410_140);
wire #() noname(net_300_120, net_300_140);
wire #() noname(net_300_40, net_410_40);
wire #() noname(net_300_40, net_300_60);
place #(.$xposition(60),.$yposition(80)) place_60_80(net_60_80);
place #(.$xposition(60),.$yposition(140)) place_60_140(net_60_140);
place #(.$xposition(100),.$yposition(80)) place_100_80(net_100_80);
place #(.$xposition(160),.$yposition(80)) place_160_80(net_160_80);
place #(.$xposition(180),.$yposition(40)) place_180_40(net_180_40);
place #(.$xposition(180),.$yposition(80)) place_180_80(net_180_80);
place #(.$xposition(180),.$yposition(140)) place_180_140(net_180_140);
place #(.$xposition(200),.$yposition(40)) place_200_40(net_200_40);
place #(.$xposition(200),.$yposition(140)) place_200_140(net_200_140);
place #(.$xposition(260),.$yposition(40)) place_260_40(net_260_40);
place #(.$xposition(260),.$yposition(140)) place_260_140(net_260_140);
place #(.$xposition(300),.$yposition(40)) place_300_40(net_300_40);
place #(.$xposition(300),.$yposition(60)) place_300_60(net_300_60);
place #(.$xposition(300),.$yposition(120)) place_300_120(net_300_120);
place #(.$xposition(300),.$yposition(140)) place_300_140(net_300_140);
place #(.$xposition(410),.$yposition(40)) place_410_40(net_410_40);
place #(.$xposition(410),.$yposition(100)) place_410_100(net_410_100);
place #(.$xposition(410),.$yposition(140)) place_410_140(net_410_140);
place #(.$xposition(410),.$yposition(200)) place_410_200(net_410_200);
endmodule // main

Sub #(.File()) Sub(net_0_0);
// skip sckt :SymbolSection:
// skip sckt :Diagrams:
// skip sckt :Paintings:
