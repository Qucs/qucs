// VS::printSubckt main
module main();
GND #(.$xposition(660), .$yposition(370), .$vflip(1), .$hflip(1), .$tx(0), .$ty(0)) anonymous_gnd_hack_0(net_660_370);
//TR1
R #(.$xposition(610), .$yposition(270), .$vflip(1), .$hflip(1), .$tx(-26), .$ty(-26), .R(50 Ohm), .Temp(26.85), .Tc1(0.0), .Tc2(0.0), .Tnom(26.85), .Symbol(european)) R2(net_580_270, net_640_270);
R #(.$xposition(530), .$yposition(270), .$vflip(1), .$hflip(1), .$tx(-26), .$ty(-26), .R(50 Ohm), .Temp(26.85), .Tc1(0.0), .Tc2(0.0), .Tnom(26.85), .Symbol(european)) R1(net_500_270, net_560_270);
Vac #(.$xposition(450), .$yposition(320), .$vflip(1), .$hflip(1), .$tx(18), .$ty(18), .U(10 V), .f(1kHz), .Phase(0), .Theta(0)) V1(net_450_290, net_450_350);
wire #(.$xposition(450), .$yposition(350), .$vflip(1), .$hflip(1)) noname(net_450_350, net_450_360);
wire #(.$xposition(450), .$yposition(360), .$vflip(1), .$hflip(1)) noname(net_450_360, net_660_360);
wire #(.$xposition(660), .$yposition(360), .$vflip(1), .$hflip(1)) noname(net_660_360, net_660_370);
wire #(.$xposition(660), .$yposition(270), .$vflip(1), .$hflip(1)) noname(net_660_270, net_660_360);
wire #(.$xposition(450), .$yposition(270), .$vflip(1), .$hflip(1)) noname(net_450_270, net_450_290);
wire #(.$xposition(450), .$yposition(270), .$vflip(1), .$hflip(1)) noname(net_450_270, net_500_270);
wire #(.$xposition(560), .$yposition(270), .$vflip(1), .$hflip(1)) noname(net_560_270, net_580_270);
wire #(.$xposition(640), .$yposition(270), .$vflip(1), .$hflip(1)) noname(net_640_270, net_660_270);
place #(.$xposition(660), .$yposition(370), .$vflip(1), .$hflip(1)) net_660_370(net_660_370);
place #(.$xposition(580), .$yposition(270), .$vflip(1), .$hflip(1)) net_580_270(net_580_270);
place #(.$xposition(640), .$yposition(270), .$vflip(1), .$hflip(1)) net_640_270(net_640_270);
place #(.$xposition(500), .$yposition(270), .$vflip(1), .$hflip(1)) net_500_270(net_500_270);
place #(.$xposition(560), .$yposition(270), .$vflip(1), .$hflip(1)) net_560_270(net_560_270);
place #(.$xposition(450), .$yposition(290), .$vflip(1), .$hflip(1)) net_450_290(net_450_290);
place #(.$xposition(450), .$yposition(350), .$vflip(1), .$hflip(1)) net_450_350(net_450_350);
place #(.$xposition(450), .$yposition(360), .$vflip(1), .$hflip(1)) net_450_360(net_450_360);
place #(.$xposition(660), .$yposition(360), .$vflip(1), .$hflip(1)) net_660_360(net_660_360);
place #(.$xposition(660), .$yposition(270), .$vflip(1), .$hflip(1)) net_660_270(net_660_270);
place #(.$xposition(450), .$yposition(270), .$vflip(1), .$hflip(1)) net_450_270(net_450_270);
endmodule // main

