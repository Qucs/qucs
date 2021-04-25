// VS::printSubckt main
module main();
Vac #(.$xposition(40), .$yposition(230), .$vflip(1), .$hflip(1), .$tx(18), .$ty(18), .U(1 uV), .f(2 kHz), .Phase(0), .Theta(0)) V4(net_40_200, net_40_260);
GND #(.$xposition(40), .$yposition(270), .$vflip(1), .$hflip(1), .$tx(0), .$ty(0)) anonymous_gnd_hack_0(net_40_270);
R #(.$xposition(260), .$yposition(310), .$vflip(1), .$hflip(1), .$tx(-26), .$ty(-26), .R(50k), .Temp(26.85), .Tc1(0.0), .Tc2(0.0), .Tnom(26.85), .Symbol(european)) R2(net_290_310, net_230_310);
GND #(.$xposition(100), .$yposition(310), .$vflip(1), .$hflip(1), .$tx(0), .$ty(0)) anonymous_gnd_hack_1(net_100_310);
R #(.$xposition(360), .$yposition(270), .$vflip(1), .$hflip(1), .$tx(13), .$ty(13), .R(100k), .Temp(26.85), .Tc1(0.0), .Tc2(0.0), .Tnom(26.85), .Symbol(european)) R1(net_360_300, net_360_240);
GND #(.$xposition(360), .$yposition(300), .$vflip(1), .$hflip(1), .$tx(0), .$ty(0)) anonymous_gnd_hack_2(net_360_300);
GND #(.$xposition(130), .$yposition(150), .$vflip(1), .$hflip(1), .$tx(0), .$ty(0)) anonymous_gnd_hack_3(net_130_150);
Vdc #(.$xposition(130), .$yposition(120), .$vflip(1), .$hflip(1), .$tx(18), .$ty(18), .U(2.5 V)) V3(net_130_90, net_130_150);
GND #(.$xposition(210), .$yposition(400), .$vflip(1), .$hflip(1), .$tx(0), .$ty(0)) anonymous_gnd_hack_4(net_210_400);
Vdc #(.$xposition(210), .$yposition(370), .$vflip(1), .$hflip(1), .$tx(18), .$ty(18), .U(-2.5 V)) V6(net_210_340, net_210_400);
//AC1
//DC1
Eqn #(.$xposition(360), .$yposition(370), .$vflip(1), .$hflip(1), .$tx(-23), .$ty(-23), .Gain(dB(Output.v/Input.v)), .Phase(phase(Output.v/Input.v)), .Export(yes)) Eqn1();
Sub #(.File(singleOPV.sch), .$tx(-51), .$ty(25), .$xposition(200), .$yposition(220), .$vflip(1), .$hflip(1)) OPV1(net_160_240, net_160_200, net_210_170, net_210_270, net_260_220);
R #(.$xposition(130), .$yposition(310), .$vflip(1), .$hflip(1), .$tx(-26), .$ty(-26), .R(1.7k), .Temp(26.85), .Tc1(0.0), .Tc2(0.0), .Tnom(26.85), .Symbol(european)) R3(net_160_310, net_100_310);
wire #(.$xposition(260), .$yposition(220), .$vflip(1), .$hflip(1)) noname(net_260_220, net_290_220);
wire #(.$xposition(40), .$yposition(260), .$vflip(1), .$hflip(1)) noname(net_40_260, net_40_270);
wire #(.$xposition(40), .$yposition(200), .$vflip(1), .$hflip(1)) noname(net_40_200, net_160_200);
wire #(.$xposition(160), .$yposition(240), .$vflip(1), .$hflip(1)) noname(net_160_240, net_160_310);
wire #(.$xposition(160), .$yposition(310), .$vflip(1), .$hflip(1)) noname(net_160_310, net_230_310);
wire #(.$xposition(290), .$yposition(220), .$vflip(1), .$hflip(1)) noname(net_290_220, net_290_310);
wire #(.$xposition(360), .$yposition(220), .$vflip(1), .$hflip(1)) noname(net_360_220, net_360_240);
wire #(.$xposition(290), .$yposition(220), .$vflip(1), .$hflip(1), .netname(Output), .$ty(-50), .$tx(70)) noname(net_290_220, net_360_220);
wire #(.$xposition(130), .$yposition(80), .$vflip(1), .$hflip(1)) noname(net_130_80, net_210_80);
wire #(.$xposition(130), .$yposition(80), .$vflip(1), .$hflip(1)) noname(net_130_80, net_130_90);
wire #(.$xposition(210), .$yposition(80), .$vflip(1), .$hflip(1)) noname(net_210_80, net_210_170);
wire #(.$xposition(210), .$yposition(270), .$vflip(1), .$hflip(1)) noname(net_210_270, net_210_340);
wire #(.$xposition(40), .$yposition(200), .$vflip(1), .$hflip(1), .netname(Input), .$ty(-30), .$tx(-57)) noname(net_40_200, net_40_200);
place #(.$xposition(40), .$yposition(200)) net_40_200(net_40_200);
place #(.$xposition(40), .$yposition(260)) net_40_260(net_40_260);
place #(.$xposition(40), .$yposition(270)) net_40_270(net_40_270);
place #(.$xposition(290), .$yposition(310)) net_290_310(net_290_310);
place #(.$xposition(230), .$yposition(310)) net_230_310(net_230_310);
place #(.$xposition(100), .$yposition(310)) net_100_310(net_100_310);
place #(.$xposition(360), .$yposition(300)) net_360_300(net_360_300);
place #(.$xposition(360), .$yposition(240)) net_360_240(net_360_240);
place #(.$xposition(130), .$yposition(150)) net_130_150(net_130_150);
place #(.$xposition(130), .$yposition(90)) net_130_90(net_130_90);
place #(.$xposition(210), .$yposition(400)) net_210_400(net_210_400);
place #(.$xposition(210), .$yposition(340)) net_210_340(net_210_340);
place #(.$xposition(160), .$yposition(240)) net_160_240(net_160_240);
place #(.$xposition(160), .$yposition(200)) net_160_200(net_160_200);
place #(.$xposition(210), .$yposition(170)) net_210_170(net_210_170);
place #(.$xposition(210), .$yposition(270)) net_210_270(net_210_270);
place #(.$xposition(260), .$yposition(220)) net_260_220(net_260_220);
place #(.$xposition(160), .$yposition(310)) net_160_310(net_160_310);
place #(.$xposition(290), .$yposition(220)) net_290_220(net_290_220);
place #(.$xposition(360), .$yposition(220)) net_360_220(net_360_220);
place #(.$xposition(130), .$yposition(80)) net_130_80(net_130_80);
place #(.$xposition(210), .$yposition(80)) net_210_80(net_210_80);
endmodule // main

