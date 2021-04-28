// VS::printSubckt main
module main();
Vac #(.U(1 V), .f(1 GHz), .Phase(0), .Theta(0), .$ty(-26), .$tx(-26), .$xposition(0), .$yposition(0), .$vflip(1), .$hflip(1)) V1(net_120_120, net_120_180);
GND #(.$xposition(0), .$yposition(0), .$vflip(1), .$hflip(1)) anonymous_gnd_hack_0(net_320_240);
GND #(.$xposition(0), .$yposition(0), .$vflip(1), .$hflip(1)) anonymous_gnd_hack_1(net_120_240);
R #(.R(R_par), .Temp(26.85), .Tc1(0.0), .Tc2(0.0), .Tnom(26.85), .Symbol(european), .$ty(-26), .$tx(-26), .$xposition(0), .$yposition(0), .$vflip(1), .$hflip(1)) R1(net_320_120, net_320_60);
C #(.C(2 pF), .V(), .Symbol(neutral), .$ty(-26), .$tx(-26), .$xposition(0), .$yposition(0), .$vflip(1), .$hflip(1)) C1(net_320_240, net_320_180);
L #(.L(13 nH), .I(), .$ty(-26), .$tx(-26), .$xposition(0), .$yposition(0), .$vflip(1), .$hflip(1)) L1(net_320_180, net_320_120);
endmodule // main

