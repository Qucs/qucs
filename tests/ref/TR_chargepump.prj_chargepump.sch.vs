// VS::printSubckt main
module main();
GND #(.$xposition(110), .$yposition(260), .$vflip(1), .$hflip(1), .$angle(0)) anonymous_gnd_hack_0(net_110_260);
GND #(.$xposition(330), .$yposition(300), .$vflip(1), .$hflip(1), .$angle(0)) anonymous_gnd_hack_1(net_330_300);
GND #(.$xposition(220), .$yposition(280), .$vflip(1), .$hflip(1), .$angle(0)) anonymous_gnd_hack_2(net_220_280);
C #(.C(10 pF), .V(), .Symbol(neutral), .$ty(17), .$tx(17), .$xposition(220), .$yposition(250), .$vflip(1), .$hflip(1), .$angle(90)) C1(net_220_280, net_220_220);
R #(.R(10k), .Temp(26.85), .Tc1(0.0), .Tc2(0.0), .Tnom(26.85), .Symbol(european), .$ty(15), .$tx(15), .$xposition(350), .$yposition(160), .$vflip(1), .$hflip(1), .$angle(90)) R1(net_350_190, net_350_130);
Vac #(.U(5 V), .f(10 MHz), .Phase(0), .Theta(0), .$ty(-72), .$tx(-72), .$xposition(110), .$yposition(230), .$vflip(-1), .$hflip(1), .$angle(450)) V1(net_110_200, net_110_260);
Vac #(.U(-5 V), .f(10 MHz), .Phase(0), .Theta(0), .$ty(18), .$tx(18), .$xposition(330), .$yposition(270), .$vflip(1), .$hflip(1), .$angle(450)) V3(net_330_240, net_330_300);
Vac #(.U(5 V), .f(10 MHz), .Phase(0), .Theta(0), .$ty(18), .$tx(18), .$xposition(350), .$yposition(80), .$vflip(1), .$hflip(1), .$angle(450)) V2(net_350_50, net_350_110);
//TR1
wire #(.$vscale(30), .$xposition(350), .$yposition(190), .$vflip(1), .$hflip(1), .$angle(270)) noname(net_350_190, net_350_220);
wire #(.$vscale(130), .$xposition(220), .$yposition(220), .$vflip(1), .$hflip(1), .$angle(0)) noname(net_220_220, net_350_220);
wire #(.$vscale(20), .$xposition(350), .$yposition(110), .$vflip(1), .$hflip(1), .$angle(270)) noname(net_350_110, net_350_130);
wire #(.$vscale(10), .$xposition(110), .$yposition(190), .$vflip(1), .$hflip(1), .$angle(270)) noname(net_110_190, net_110_200);
wire #(.$tx(0), .$ty(-30), .netname(Gate), .$vscale(80), .$xposition(110), .$yposition(190), .$vflip(1), .$hflip(1), .$angle(0)) noname(net_110_190, net_190_190);
wire #(.$vscale(50), .$xposition(330), .$yposition(190), .$vflip(1), .$hflip(1), .$angle(270)) noname(net_330_190, net_330_240);
wire #(.$vscale(90), .$xposition(240), .$yposition(190), .$vflip(1), .$hflip(1), .$angle(0)) noname(net_240_190, net_330_190);
wire #(.$vscale(130), .$xposition(220), .$yposition(40), .$vflip(1), .$hflip(1), .$angle(0)) noname(net_220_40, net_350_40);
wire #(.$vscale(10), .$xposition(350), .$yposition(40), .$vflip(1), .$hflip(1), .$angle(270)) noname(net_350_40, net_350_50);
wire #(.$tx(30), .$ty(30), .netname(Drain), .$vscale(120), .$xposition(220), .$yposition(40), .$vflip(1), .$hflip(1), .$angle(270)) noname(net_220_40, net_220_160);
wire #(.$tx(-80), .$ty(20), .netname(Source), .$vscale(0), .$xposition(220), .$yposition(220), .$vflip(1), .$hflip(1), .$angle(0)) noname(net_220_220, net_220_220);
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

