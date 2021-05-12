// VS::printSubckt main
module main();
OpAmp #(.G(1e6), .Umax(15 V), .$ty(-26), .$tx(-26), .$xposition(370), .$yposition(160), .$vflip(1), .$hflip(1), .$angle(0)) OP1(net_340_180, net_340_140, net_410_160);
GND #(.$xposition(300), .$yposition(180), .$vflip(1), .$hflip(1), .$angle(0)) anonymous_gnd_hack_0(net_300_180);
GND #(.$xposition(180), .$yposition(260), .$vflip(1), .$hflip(1), .$angle(0)) anonymous_gnd_hack_1(net_180_260);
GND #(.$xposition(60), .$yposition(260), .$vflip(1), .$hflip(1), .$angle(0)) anonymous_gnd_hack_2(net_60_260);
Vac #(.U(1 V), .f(1 GHz), .Phase(0), .Theta(0), .$ty(18), .$tx(18), .$xposition(60), .$yposition(230), .$vflip(1), .$hflip(1), .$angle(450)) V1(net_60_200, net_60_260);
R #(.R(390), .Temp(26.85), .Tc1(0.0), .Tc2(0.0), .Tnom(26.85), .Symbol(european), .$ty(-26), .$tx(-26), .$xposition(130), .$yposition(140), .$vflip(1), .$hflip(1), .$angle(0)) R3(net_100_140, net_160_140);
R #(.R(5.6k), .Temp(26.85), .Tc1(0.0), .Tc2(0.0), .Tnom(26.85), .Symbol(european), .$ty(15), .$tx(15), .$xposition(180), .$yposition(230), .$vflip(1), .$hflip(1), .$angle(90)) R2(net_180_260, net_180_200);
C #(.C(47n), .V(), .Symbol(neutral), .$ty(-26), .$tx(-26), .$xposition(230), .$yposition(140), .$vflip(1), .$hflip(1), .$angle(0)) C1(net_200_140, net_260_140);
C #(.C(47n), .V(), .Symbol(neutral), .$ty(17), .$tx(17), .$xposition(180), .$yposition(90), .$vflip(1), .$hflip(1), .$angle(90)) C2(net_180_120, net_180_60);
R #(.R(3.9k), .Temp(26.85), .Tc1(0.0), .Tc2(0.0), .Tnom(26.85), .Symbol(european), .$ty(15), .$tx(15), .$xposition(300), .$yposition(90), .$vflip(1), .$hflip(1), .$angle(90)) R1(net_300_120, net_300_60);
//AC1
//DC1
wire #(.$vscale(20), .$xposition(160), .$yposition(140), .$vflip(1), .$hflip(1), .$angle(0)) noname(net_160_140, net_180_140);
wire #(.$vscale(20), .$xposition(180), .$yposition(140), .$vflip(1), .$hflip(1), .$angle(0)) noname(net_180_140, net_200_140);
wire #(.$vscale(20), .$xposition(180), .$yposition(120), .$vflip(1), .$hflip(1), .$angle(270)) noname(net_180_120, net_180_140);
wire #(.$vscale(60), .$xposition(180), .$yposition(140), .$vflip(1), .$hflip(1), .$angle(270)) noname(net_180_140, net_180_200);
wire #(.$vscale(40), .$xposition(260), .$yposition(140), .$vflip(1), .$hflip(1), .$angle(0)) noname(net_260_140, net_300_140);
wire #(.$vscale(40), .$xposition(300), .$yposition(140), .$vflip(1), .$hflip(1), .$angle(0)) noname(net_300_140, net_340_140);
wire #(.$vscale(20), .$xposition(300), .$yposition(120), .$vflip(1), .$hflip(1), .$angle(270)) noname(net_300_120, net_300_140);
wire #(.$vscale(40), .$xposition(300), .$yposition(180), .$vflip(1), .$hflip(1), .$angle(0)) noname(net_300_180, net_340_180);
wire #(.$vscale(40), .$xposition(60), .$yposition(140), .$vflip(1), .$hflip(1), .$angle(0)) noname(net_60_140, net_100_140);
wire #(.$vscale(60), .$xposition(60), .$yposition(140), .$vflip(1), .$hflip(1), .$angle(270)) noname(net_60_140, net_60_200);
wire #(.$vscale(110), .$xposition(300), .$yposition(50), .$vflip(1), .$hflip(1), .$angle(0)) noname(net_300_50, net_410_50);
wire #(.$vscale(10), .$xposition(300), .$yposition(50), .$vflip(1), .$hflip(1), .$angle(270)) noname(net_300_50, net_300_60);
wire #(.$tx(40), .$ty(0), .netname(Output), .$vscale(110), .$xposition(410), .$yposition(50), .$vflip(1), .$hflip(1), .$angle(270)) noname(net_410_50, net_410_160);
wire #(.$vscale(120), .$xposition(180), .$yposition(50), .$vflip(1), .$hflip(1), .$angle(0)) noname(net_180_50, net_300_50);
wire #(.$vscale(10), .$xposition(180), .$yposition(50), .$vflip(1), .$hflip(1), .$angle(270)) noname(net_180_50, net_180_60);
place #(.$xposition(340), .$yposition(180)) net_340_180(net_340_180);
place #(.$xposition(340), .$yposition(140)) net_340_140(net_340_140);
place #(.$xposition(410), .$yposition(160)) net_410_160(net_410_160);
place #(.$xposition(300), .$yposition(180)) net_300_180(net_300_180);
place #(.$xposition(180), .$yposition(260)) net_180_260(net_180_260);
place #(.$xposition(60), .$yposition(260)) net_60_260(net_60_260);
place #(.$xposition(60), .$yposition(200)) net_60_200(net_60_200);
place #(.$xposition(100), .$yposition(140)) net_100_140(net_100_140);
place #(.$xposition(160), .$yposition(140)) net_160_140(net_160_140);
place #(.$xposition(180), .$yposition(200)) net_180_200(net_180_200);
place #(.$xposition(200), .$yposition(140)) net_200_140(net_200_140);
place #(.$xposition(260), .$yposition(140)) net_260_140(net_260_140);
place #(.$xposition(180), .$yposition(120)) net_180_120(net_180_120);
place #(.$xposition(180), .$yposition(60)) net_180_60(net_180_60);
place #(.$xposition(300), .$yposition(120)) net_300_120(net_300_120);
place #(.$xposition(300), .$yposition(60)) net_300_60(net_300_60);
place #(.$xposition(180), .$yposition(140)) net_180_140(net_180_140);
place #(.$xposition(300), .$yposition(140)) net_300_140(net_300_140);
place #(.$xposition(60), .$yposition(140)) net_60_140(net_60_140);
place #(.$xposition(300), .$yposition(50)) net_300_50(net_300_50);
place #(.$xposition(410), .$yposition(50)) net_410_50(net_410_50);
place #(.$xposition(180), .$yposition(50)) net_180_50(net_180_50);
endmodule // main

