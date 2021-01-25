// VS::printSubckt main
module main();
Vac #(.$xposition(120), .$yposition(150), .$tx(18), .$ty(18), .U(1 V), .f(1 GHz), .Phase(0), .Theta(0)) V1(net_120_120, net_120_180);
GND #(.$xposition(320), .$yposition(240), .$tx(0), .$ty(0)) anonymous_gnd_hack_0(net_320_240);
GND #(.$xposition(120), .$yposition(240), .$tx(0), .$ty(0)) anonymous_gnd_hack_1(net_120_240);
R #(.$xposition(320), .$yposition(90), .$tx(15), .$ty(15), .R(R_par), .Temp(26.85), .Tc1(0.0), .Tc2(0.0), .Tnom(26.85), .Symbol(european)) R1(net_320_120, net_320_60);
C #(.$xposition(320), .$yposition(210), .$tx(17), .$ty(17), .C(2 pF), .V(), .Symbol(neutral)) C1(net_320_240, net_320_180);
L #(.$xposition(320), .$yposition(150), .$tx(10), .$ty(10), .L(13 nH), .I()) L1(net_320_180, net_320_120);
//AC1
wire #(.$xposition(120), .$yposition(180)) noname(net_120_180, net_120_240);
wire #(.$xposition(120), .$yposition(60)) noname(net_120_60, net_320_60);
wire #(.$xposition(120), .$yposition(60)) noname(net_120_60, net_120_120);
wire #(.$xposition(320), .$yposition(180)) noname(net_320_180, net_320_180);
place #(.$xposition(120), .$yposition(120)) net_120_120(net_120_120);
place #(.$xposition(120), .$yposition(180)) net_120_180(net_120_180);
place #(.$xposition(320), .$yposition(240)) net_320_240(net_320_240);
place #(.$xposition(120), .$yposition(240)) net_120_240(net_120_240);
place #(.$xposition(320), .$yposition(120)) net_320_120(net_320_120);
place #(.$xposition(320), .$yposition(60)) net_320_60(net_320_60);
place #(.$xposition(320), .$yposition(180)) net_320_180(net_320_180);
place #(.$xposition(120), .$yposition(60)) net_120_60(net_120_60);
endmodule // main

