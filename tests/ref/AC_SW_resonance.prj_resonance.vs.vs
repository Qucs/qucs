// VS::printSubckt main
module main();
Vac #(.$xposition(0), .$yposition(0), .$vflip(1), .$hflip(1), .$tx(-26), .$ty(-26), .U(1 V), .f(1 GHz), .Phase(0), .Theta(0)) V1(net_120_120, net_120_180);
GND #(.$xposition(0), .$yposition(0), .$vflip(1), .$hflip(1), .$tx(0), .$ty(0)) anonymous_gnd_hack_0(net_320_240);
GND #(.$xposition(0), .$yposition(0), .$vflip(1), .$hflip(1), .$tx(0), .$ty(0)) anonymous_gnd_hack_1(net_120_240);
R #(.$xposition(0), .$yposition(0), .$vflip(1), .$hflip(1), .$tx(-26), .$ty(-26), .R(R_par), .Temp(26.85), .Tc1(0.0), .Tc2(0.0), .Tnom(26.85), .Symbol(european)) R1(net_320_120, net_320_60);
C #(.$xposition(0), .$yposition(0), .$vflip(1), .$hflip(1), .$tx(-26), .$ty(-26), .C(2 pF), .V(), .Symbol(neutral)) C1(net_320_240, net_320_180);
L #(.$xposition(0), .$yposition(0), .$vflip(1), .$hflip(1), .$tx(-26), .$ty(-26), .L(13 nH), .I()) L1(net_320_180, net_320_120);
endmodule // main

