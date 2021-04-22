// VS::printSubckt main
module main();
GND #(.$xposition(110), .$yposition(260), .$vflip(1), .$hflip(1), .$tx(0), .$ty(0)) anonymous_gnd_hack_0(net_110_260);
GND #(.$xposition(330), .$yposition(300), .$vflip(1), .$hflip(1), .$tx(0), .$ty(0)) anonymous_gnd_hack_1(net_330_300);
GND #(.$xposition(220), .$yposition(280), .$vflip(1), .$hflip(1), .$tx(0), .$ty(0)) anonymous_gnd_hack_2(net_220_280);
C #(.$xposition(220), .$yposition(250), .$vflip(1), .$hflip(1), .$tx(17), .$ty(17), .C(10 pF), .V(), .Symbol(neutral)) C1(net_220_280, net_220_220);
R #(.$xposition(350), .$yposition(160), .$vflip(1), .$hflip(1), .$tx(15), .$ty(15), .R(10k), .Temp(26.85), .Tc1(0.0), .Tc2(0.0), .Tnom(26.85), .Symbol(european)) R1(net_350_190, net_350_130);
Vac #(.$xposition(110), .$yposition(230), .$vflip(-1), .$hflip(1), .$tx(-72), .$ty(-72), .U(5 V), .f(10 MHz), .Phase(0), .Theta(0)) V1(net_110_200, net_110_260);
Vac #(.$xposition(330), .$yposition(270), .$vflip(1), .$hflip(1), .$tx(18), .$ty(18), .U(-5 V), .f(10 MHz), .Phase(0), .Theta(0)) V3(net_330_240, net_330_300);
Vac #(.$xposition(350), .$yposition(80), .$vflip(1), .$hflip(1), .$tx(18), .$ty(18), .U(5 V), .f(10 MHz), .Phase(0), .Theta(0)) V2(net_350_50, net_350_110);
//TR1
wire #(.$xposition(350), .$yposition(190), .$vflip(1), .$hflip(1)) noname(net_350_190, net_350_220);
wire #(.$xposition(220), .$yposition(220), .$vflip(1), .$hflip(1)) noname(net_220_220, net_350_220);
wire #(.$xposition(350), .$yposition(110), .$vflip(1), .$hflip(1)) noname(net_350_110, net_350_130);
wire #(.$xposition(110), .$yposition(190), .$vflip(1), .$hflip(1)) noname(net_110_190, net_110_200);
wire #(.$xposition(110), .$yposition(190), .$vflip(1), .$hflip(1), .netname(Gate), .$ty(-30), .$tx(0)) noname(net_110_190, net_190_190);
wire #(.$xposition(330), .$yposition(190), .$vflip(1), .$hflip(1)) noname(net_330_190, net_330_240);
wire #(.$xposition(240), .$yposition(190), .$vflip(1), .$hflip(1)) noname(net_240_190, net_330_190);
wire #(.$xposition(220), .$yposition(40), .$vflip(1), .$hflip(1)) noname(net_220_40, net_350_40);
wire #(.$xposition(350), .$yposition(40), .$vflip(1), .$hflip(1)) noname(net_350_40, net_350_50);
wire #(.$xposition(220), .$yposition(40), .$vflip(1), .$hflip(1), .netname(Drain), .$ty(30), .$tx(30)) noname(net_220_40, net_220_160);
wire #(.$xposition(220), .$yposition(220), .$vflip(1), .$hflip(1), .netname(Source), .$ty(20), .$tx(-80)) noname(net_220_220, net_220_220);
place #(.$xposition(110), .$yposition(260)) net_110_260(net_110_260);
place #(.$xposition(330), .$yposition(300)) net_330_300(net_330_300);
place #(.$xposition(220), .$yposition(280)) net_220_280(net_220_280);
place #(.$xposition(220), .$yposition(220)) net_220_220(net_220_220);
place #(.$xposition(350), .$yposition(190)) net_350_190(net_350_190);
place #(.$xposition(350), .$yposition(130)) net_350_130(net_350_130);
place #(.$xposition(110), .$yposition(200)) net_110_200(net_110_200);
place #(.$xposition(330), .$yposition(240)) net_330_240(net_330_240);
place #(.$xposition(350), .$yposition(50)) net_350_50(net_350_50);
place #(.$xposition(350), .$yposition(110)) net_350_110(net_350_110);
place #(.$xposition(350), .$yposition(220)) net_350_220(net_350_220);
place #(.$xposition(110), .$yposition(190)) net_110_190(net_110_190);
place #(.$xposition(190), .$yposition(190)) net_190_190(net_190_190);
place #(.$xposition(330), .$yposition(190)) net_330_190(net_330_190);
place #(.$xposition(240), .$yposition(190)) net_240_190(net_240_190);
place #(.$xposition(220), .$yposition(40)) net_220_40(net_220_40);
place #(.$xposition(350), .$yposition(40)) net_350_40(net_350_40);
place #(.$xposition(220), .$yposition(160)) net_220_160(net_220_160);
endmodule // main

