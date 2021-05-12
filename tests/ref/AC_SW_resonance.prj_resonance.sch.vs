// VS::printSubckt main
module main();
Vac #(.U(1 V), .f(1 GHz), .Phase(0), .Theta(0), .$ty(18), .$tx(18), .$xposition(120), .$yposition(150), .$vflip(1), .$hflip(1), .$angle(450)) V1(net_120_120, net_120_180);
GND #(.$xposition(320), .$yposition(240), .$vflip(1), .$hflip(1), .$angle(0)) anonymous_gnd_hack_0(net_320_240);
GND #(.$xposition(120), .$yposition(240), .$vflip(1), .$hflip(1), .$angle(0)) anonymous_gnd_hack_1(net_120_240);
R #(.R(R_par), .Temp(26.85), .Tc1(0.0), .Tc2(0.0), .Tnom(26.85), .Symbol(european), .$ty(15), .$tx(15), .$xposition(320), .$yposition(90), .$vflip(1), .$hflip(1), .$angle(90)) R1(net_320_120, net_320_60);
C #(.C(2 pF), .V(), .Symbol(neutral), .$ty(17), .$tx(17), .$xposition(320), .$yposition(210), .$vflip(1), .$hflip(1), .$angle(90)) C1(net_320_240, net_320_180);
L #(.L(13 nH), .I(), .$ty(10), .$tx(10), .$xposition(320), .$yposition(150), .$vflip(1), .$hflip(1), .$angle(90)) L1(net_320_180, net_320_120);
//AC1
wire #(.$xposition(120), .$yposition(180), .$vflip(1), .$hflip(1), .$angle(270)) noname(net_120_180, net_120_240);
wire #(.$xposition(120), .$yposition(60), .$vflip(1), .$hflip(1), .$angle(0)) noname(net_120_60, net_320_60);
wire #(.$xposition(120), .$yposition(60), .$vflip(1), .$hflip(1), .$angle(270)) noname(net_120_60, net_120_120);
wire #(.$tx(-80), .$ty(-40), .netname(Voltage), .$xposition(320), .$yposition(180), .$vflip(1), .$hflip(1), .$angle(0)) noname(net_320_180, net_320_180);
place #(.$xposition(120), .$yposition(120)) net_120_120(net_120_120);
place #(.$xposition(120), .$yposition(180)) net_120_180(net_120_180);
place #(.$xposition(320), .$yposition(240)) net_320_240(net_320_240);
place #(.$xposition(120), .$yposition(240)) net_120_240(net_120_240);
place #(.$xposition(320), .$yposition(120)) net_320_120(net_320_120);
place #(.$xposition(320), .$yposition(60)) net_320_60(net_320_60);
place #(.$xposition(320), .$yposition(180)) net_320_180(net_320_180);
place #(.$xposition(120), .$yposition(60)) net_120_60(net_120_60);
endmodule // main

