// VS::printSubckt main
module main();
R #(.R(200m), .Temp(26.85), .Tc1(0.0), .Tc2(0.0), .Tnom(26.85), .Symbol(european), .$ty(-26), .$tx(-26), .$xposition(340), .$yposition(80), .$vflip(1), .$hflip(1), .$angle(0)) RD(net_310_80, net_370_80);
GND #(.$xposition(280), .$yposition(300), .$vflip(1), .$hflip(1), .$angle(0)) anonymous_gnd_hack_0(net_280_300);
Vdc #(.U(13.5V), .$ty(18), .$tx(18), .$xposition(280), .$yposition(270), .$vflip(1), .$hflip(1), .$angle(450)) VDC(net_280_240, net_280_300);
Vdc #(.U(200V), .$ty(18), .$tx(18), .$xposition(40), .$yposition(170), .$vflip(1), .$hflip(1), .$angle(450)) VGEN(net_40_140, net_40_200);
C #(.C(40nF), .V(), .Symbol(neutral), .$ty(17), .$tx(17), .$xposition(160), .$yposition(170), .$vflip(1), .$hflip(1), .$angle(90)) CS(net_160_200, net_160_140);
R #(.R(400m), .Temp(26.85), .Tc1(0.0), .Tc2(0.0), .Tnom(26.85), .Symbol(european), .$ty(15), .$tx(15), .$xposition(280), .$yposition(170), .$vflip(1), .$hflip(1), .$angle(90)) RE(net_280_200, net_280_140);
C #(.C(40nF), .V(), .Symbol(neutral), .$ty(17), .$tx(17), .$xposition(400), .$yposition(170), .$vflip(1), .$hflip(1), .$angle(90)) CL(net_400_200, net_400_140);
//TR1
R #(.R(50), .Temp(26.85), .Tc1(0.0), .Tc2(0.0), .Tnom(26.85), .Symbol(european), .$ty(-26), .$tx(-26), .$xposition(470), .$yposition(80), .$vflip(1), .$hflip(1), .$angle(0)) RI(net_440_80, net_500_80);
Switch #(.init(off), .time(10ns), .Ron(0), .Roff(1e9), .Temp(26.85), .MaxDuration(1e-6), .Transition(spline), .$ty(-26), .$tx(-26), .$xposition(220), .$yposition(80), .$vflip(1), .$hflip(1), .$angle(0)) S2(net_190_80, net_250_80);
Switch #(.init(on), .time(9ns), .Ron(0), .Roff(1e9), .Temp(26.85), .MaxDuration(1e-6), .Transition(spline), .$ty(-26), .$tx(-26), .$xposition(100), .$yposition(80), .$vflip(1), .$hflip(1), .$angle(0)) S1(net_70_80, net_130_80);
wire #(.$vscale(20), .$xposition(280), .$yposition(220), .$vflip(1), .$hflip(1), .$angle(270)) noname(net_280_220, net_280_240);
wire #(.$vscale(120), .$xposition(160), .$yposition(220), .$vflip(1), .$hflip(1), .$angle(0)) noname(net_160_220, net_280_220);
wire #(.$vscale(20), .$xposition(400), .$yposition(200), .$vflip(1), .$hflip(1), .$angle(270)) noname(net_400_200, net_400_220);
wire #(.$vscale(120), .$xposition(280), .$yposition(220), .$vflip(1), .$hflip(1), .$angle(0)) noname(net_280_220, net_400_220);
wire #(.$vscale(20), .$xposition(280), .$yposition(200), .$vflip(1), .$hflip(1), .$angle(270)) noname(net_280_200, net_280_220);
wire #(.$vscale(30), .$xposition(250), .$yposition(80), .$vflip(1), .$hflip(1), .$angle(0)) noname(net_250_80, net_280_80);
wire #(.$vscale(60), .$xposition(280), .$yposition(80), .$vflip(1), .$hflip(1), .$angle(270)) noname(net_280_80, net_280_140);
wire #(.$vscale(20), .$xposition(160), .$yposition(200), .$vflip(1), .$hflip(1), .$angle(270)) noname(net_160_200, net_160_220);
wire #(.$vscale(30), .$xposition(130), .$yposition(80), .$vflip(1), .$hflip(1), .$angle(0)) noname(net_130_80, net_160_80);
wire #(.$vscale(60), .$xposition(160), .$yposition(80), .$vflip(1), .$hflip(1), .$angle(270)) noname(net_160_80, net_160_140);
wire #(.$vscale(60), .$xposition(40), .$yposition(80), .$vflip(1), .$hflip(1), .$angle(270)) noname(net_40_80, net_40_140);
wire #(.$vscale(30), .$xposition(40), .$yposition(80), .$vflip(1), .$hflip(1), .$angle(0)) noname(net_40_80, net_70_80);
wire #(.$vscale(20), .$xposition(40), .$yposition(200), .$vflip(1), .$hflip(1), .$angle(270)) noname(net_40_200, net_40_220);
wire #(.$vscale(120), .$xposition(40), .$yposition(220), .$vflip(1), .$hflip(1), .$angle(0)) noname(net_40_220, net_160_220);
wire #(.$vscale(30), .$xposition(370), .$yposition(80), .$vflip(1), .$hflip(1), .$angle(0)) noname(net_370_80, net_400_80);
wire #(.$vscale(60), .$xposition(400), .$yposition(80), .$vflip(1), .$hflip(1), .$angle(270)) noname(net_400_80, net_400_140);
wire #(.$vscale(40), .$xposition(400), .$yposition(80), .$vflip(1), .$hflip(1), .$angle(0)) noname(net_400_80, net_440_80);
wire #(.$vscale(30), .$xposition(160), .$yposition(80), .$vflip(1), .$hflip(1), .$angle(0)) noname(net_160_80, net_190_80);
wire #(.$vscale(30), .$xposition(280), .$yposition(80), .$vflip(1), .$hflip(1), .$angle(0)) noname(net_280_80, net_310_80);
wire #(.$vscale(20), .$xposition(500), .$yposition(80), .$vflip(1), .$hflip(1), .$angle(0)) noname(net_500_80, net_520_80);
wire #(.$tx(20), .$ty(-40), .netname(Puls3b), .$vscale(0), .$xposition(500), .$yposition(80), .$vflip(1), .$hflip(1), .$angle(0)) noname(net_500_80, net_500_80);
// `ifdef QUCS_DIAGRAMS
// Rect
// Rect
// `endif
place #(.$xposition(310), .$yposition(80)) net_310_80(net_310_80);
place #(.$xposition(370), .$yposition(80)) net_370_80(net_370_80);
place #(.$xposition(280), .$yposition(300)) net_280_300(net_280_300);
place #(.$xposition(280), .$yposition(240)) net_280_240(net_280_240);
place #(.$xposition(40), .$yposition(140)) net_40_140(net_40_140);
place #(.$xposition(40), .$yposition(200)) net_40_200(net_40_200);
place #(.$xposition(160), .$yposition(200)) net_160_200(net_160_200);
place #(.$xposition(160), .$yposition(140)) net_160_140(net_160_140);
place #(.$xposition(280), .$yposition(200)) net_280_200(net_280_200);
place #(.$xposition(280), .$yposition(140)) net_280_140(net_280_140);
place #(.$xposition(400), .$yposition(200)) net_400_200(net_400_200);
place #(.$xposition(400), .$yposition(140)) net_400_140(net_400_140);
place #(.$xposition(440), .$yposition(80)) net_440_80(net_440_80);
place #(.$xposition(500), .$yposition(80)) net_500_80(net_500_80);
place #(.$xposition(190), .$yposition(80)) net_190_80(net_190_80);
place #(.$xposition(250), .$yposition(80)) net_250_80(net_250_80);
place #(.$xposition(70), .$yposition(80)) net_70_80(net_70_80);
place #(.$xposition(130), .$yposition(80)) net_130_80(net_130_80);
place #(.$xposition(280), .$yposition(220)) net_280_220(net_280_220);
place #(.$xposition(160), .$yposition(220)) net_160_220(net_160_220);
place #(.$xposition(400), .$yposition(220)) net_400_220(net_400_220);
place #(.$xposition(280), .$yposition(80)) net_280_80(net_280_80);
place #(.$xposition(160), .$yposition(80)) net_160_80(net_160_80);
place #(.$xposition(40), .$yposition(80)) net_40_80(net_40_80);
place #(.$xposition(40), .$yposition(220)) net_40_220(net_40_220);
place #(.$xposition(400), .$yposition(80)) net_400_80(net_400_80);
place #(.$xposition(520), .$yposition(80)) net_520_80(net_520_80);
endmodule // main

