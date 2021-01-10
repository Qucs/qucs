// VS::printSubckt main
module main();
OpAmp #(.G(1e6), .Umax(15 V)) OP1(net_340_180, net_340_140, net_410_160);
GND #() anonymous_gnd_hack_0(net_300_180);
GND #() anonymous_gnd_hack_1(net_180_260);
GND #() anonymous_gnd_hack_2(net_60_260);
Vac #(.U(1 V), .f(1 GHz), .Phase(0), .Theta(0)) V1(net_60_200, net_60_260);
R #(.R(390), .Temp(26.85), .Tc1(0.0), .Tc2(0.0), .Tnom(26.85), .Symbol(european)) R3(net_100_140, net_160_140);
R #(.R(5.6k), .Temp(26.85), .Tc1(0.0), .Tc2(0.0), .Tnom(26.85), .Symbol(european)) R2(net_180_260, net_180_200);
C #(.C(47n), .V(), .Symbol(neutral)) C1(net_200_140, net_260_140);
C #(.C(47n), .V(), .Symbol(neutral)) C2(net_180_120, net_180_60);
R #(.R(3.9k), .Temp(26.85), .Tc1(0.0), .Tc2(0.0), .Tnom(26.85), .Symbol(european)) R1(net_300_120, net_300_60);
//AC1
//DC1
wire #() noname(net_160_140, net_180_140);
wire #() noname(net_180_140, net_200_140);
wire #() noname(net_180_120, net_180_140);
wire #() noname(net_180_140, net_180_200);
wire #() noname(net_260_140, net_300_140);
wire #() noname(net_300_140, net_340_140);
wire #() noname(net_300_120, net_300_140);
wire #() noname(net_300_180, net_340_180);
wire #() noname(net_60_140, net_100_140);
wire #() noname(net_60_140, net_60_200);
wire #() noname(net_300_50, net_410_50);
wire #() noname(net_300_50, net_300_60);
wire #() noname(net_410_50, net_410_160);
wire #() noname(net_180_50, net_300_50);
wire #() noname(net_180_50, net_180_60);
place #(.$xposition(60),.$yposition(140)) place_60_140(net_60_140);
place #(.$xposition(60),.$yposition(200)) place_60_200(net_60_200);
place #(.$xposition(60),.$yposition(260)) place_60_260(net_60_260);
place #(.$xposition(100),.$yposition(140)) place_100_140(net_100_140);
place #(.$xposition(160),.$yposition(140)) place_160_140(net_160_140);
place #(.$xposition(180),.$yposition(50)) place_180_50(net_180_50);
place #(.$xposition(180),.$yposition(60)) place_180_60(net_180_60);
place #(.$xposition(180),.$yposition(120)) place_180_120(net_180_120);
place #(.$xposition(180),.$yposition(140)) place_180_140(net_180_140);
place #(.$xposition(180),.$yposition(200)) place_180_200(net_180_200);
place #(.$xposition(180),.$yposition(260)) place_180_260(net_180_260);
place #(.$xposition(200),.$yposition(140)) place_200_140(net_200_140);
place #(.$xposition(260),.$yposition(140)) place_260_140(net_260_140);
place #(.$xposition(300),.$yposition(50)) place_300_50(net_300_50);
place #(.$xposition(300),.$yposition(60)) place_300_60(net_300_60);
place #(.$xposition(300),.$yposition(120)) place_300_120(net_300_120);
place #(.$xposition(300),.$yposition(140)) place_300_140(net_300_140);
place #(.$xposition(300),.$yposition(180)) place_300_180(net_300_180);
place #(.$xposition(340),.$yposition(140)) place_340_140(net_340_140);
place #(.$xposition(340),.$yposition(180)) place_340_180(net_340_180);
place #(.$xposition(410),.$yposition(50)) place_410_50(net_410_50);
place #(.$xposition(410),.$yposition(160)) place_410_160(net_410_160);
endmodule // main

