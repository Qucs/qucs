// VS::printSubckt main
module main();
L #(.L(47uH), .I(0), .$ty(-26), .$tx(-26), .$xposition(170), .$yposition(150), .$vflip(1), .$hflip(1), .$angle(0)) L1(net_140_150, net_200_150);
Vdc #(.U(12V), .$ty(18), .$tx(18), .$xposition(70), .$yposition(200), .$vflip(1), .$hflip(1), .$angle(450)) V2(net_70_170, net_70_230);
GND #(.$xposition(70), .$yposition(250), .$vflip(1), .$hflip(1), .$angle(0)) anonymous_gnd_hack_0(net_70_250);
Eqn #(.Tmax(Bperiod*20), .Tstep(Bperiod/1000), .Export(yes), .$ty(-35), .$tx(-35), .$xposition(400), .$yposition(300), .$vflip(1), .$hflip(1), .$angle(0)) Eqn1();
//TR1
Eqn #(.Bfreq(40k), .Bperiod(1/Bfreq), .Bduty(50), .Ton(Bperiod*Bduty/100), .Toff(Bperiod-Ton), .Export(yes), .$ty(-35), .$tx(-35), .$xposition(580), .$yposition(520), .$vflip(1), .$hflip(1), .$angle(0)) Eqn2();
Relais #(.Vt(0.5 V), .Vh(0.1 V), .Ron(1), .Roff(1e12), .Temp(26.85), .$ty(49), .$tx(49), .$xposition(350), .$yposition(200), .$vflip(1), .$hflip(1), .$angle(0)) S1(net_320_170, net_380_170, net_380_230, net_320_230);
Vrect #(.U(1V), .TH(Ton), .TL(Toff), .Tr(1 ns), .Tf(1 ns), .Td(0 ns), .$ty(18), .$tx(18), .$xposition(220), .$yposition(300), .$vflip(1), .$hflip(1), .$angle(450)) V1(net_220_270, net_220_330);
GND #(.$xposition(220), .$yposition(370), .$vflip(1), .$hflip(1), .$angle(0)) anonymous_gnd_hack_1(net_220_370);
Diode #(.Is(1e-12 A), .N(1), .Cj0(10 fF), .M(0.5), .Vj(0.7 V), .Fc(0.5), .Cp(0.0 fF), .Isr(0.0), .Nr(2.0), .Rs(0.0 Ohm), .Tt(0.0 ps), .Ikf(0), .Kf(0.0), .Af(1.0), .Ffe(1.0), .Bv(0), .Ibv(1 mA), .Temp(26.85), .Xti(3.0), .Eg(1.11), .Tbv(0.0), .Trs(0.0), .Ttt1(0.0), .Ttt2(0.0), .Tm1(0.0), .Tm2(0.0), .Tnom(26.85), .Area(1.0), .Symbol(normal), .$ty(-26), .$tx(-26), .$xposition(490), .$yposition(150), .$vflip(1), .$hflip(1), .$angle(180)) D1(net_520_150, net_460_150);
GND #(.$xposition(660), .$yposition(250), .$vflip(1), .$hflip(1), .$angle(0)) anonymous_gnd_hack_2(net_660_250);
C #(.C(100u), .V(0), .Symbol(neutral), .$ty(17), .$tx(17), .$xposition(560), .$yposition(200), .$vflip(-1), .$hflip(1), .$angle(270)) C1(net_560_170, net_560_230);
R #(.R(5), .Temp(26.85), .Tc1(0.0), .Tc2(0.0), .Tnom(26.85), .Symbol(european), .$ty(15), .$tx(15), .$xposition(660), .$yposition(200), .$vflip(1), .$hflip(1), .$angle(90)) R1(net_660_230, net_660_170);
wire #(.$vscale(20), .$xposition(70), .$yposition(230), .$vflip(1), .$hflip(1), .$angle(270)) noname(net_70_230, net_70_250);
wire #(.$vscale(20), .$xposition(70), .$yposition(150), .$vflip(1), .$hflip(1), .$angle(270)) noname(net_70_150, net_70_170);
wire #(.$vscale(70), .$xposition(70), .$yposition(150), .$vflip(1), .$hflip(1), .$angle(0)) noname(net_70_150, net_140_150);
wire #(.$vscale(20), .$xposition(380), .$yposition(150), .$vflip(1), .$hflip(1), .$angle(270)) noname(net_380_150, net_380_170);
wire #(.$vscale(140), .$xposition(320), .$yposition(230), .$vflip(1), .$hflip(1), .$angle(270)) noname(net_320_230, net_320_370);
wire #(.$vscale(100), .$xposition(220), .$yposition(370), .$vflip(1), .$hflip(1), .$angle(0)) noname(net_220_370, net_320_370);
wire #(.$vscale(40), .$xposition(220), .$yposition(330), .$vflip(1), .$hflip(1), .$angle(270)) noname(net_220_330, net_220_370);
wire #(.$vscale(180), .$xposition(200), .$yposition(150), .$vflip(1), .$hflip(1), .$angle(0)) noname(net_200_150, net_380_150);
wire #(.$vscale(100), .$xposition(220), .$yposition(170), .$vflip(1), .$hflip(1), .$angle(0)) noname(net_220_170, net_320_170);
wire #(.$tx(-50), .$ty(90), .netname(ctrl), .$vscale(100), .$xposition(220), .$yposition(170), .$vflip(1), .$hflip(1), .$angle(270)) noname(net_220_170, net_220_270);
wire #(.$tx(30), .$ty(-30), .netname(dio), .$vscale(80), .$xposition(380), .$yposition(150), .$vflip(1), .$hflip(1), .$angle(0)) noname(net_380_150, net_460_150);
wire #(.$vscale(20), .$xposition(380), .$yposition(230), .$vflip(1), .$hflip(1), .$angle(270)) noname(net_380_230, net_380_250);
wire #(.$vscale(20), .$xposition(660), .$yposition(150), .$vflip(1), .$hflip(1), .$angle(270)) noname(net_660_150, net_660_170);
wire #(.$vscale(20), .$xposition(660), .$yposition(230), .$vflip(1), .$hflip(1), .$angle(270)) noname(net_660_230, net_660_250);
wire #(.$vscale(40), .$xposition(520), .$yposition(150), .$vflip(1), .$hflip(1), .$angle(0)) noname(net_520_150, net_560_150);
wire #(.$tx(80), .$ty(-30), .netname(out), .$vscale(100), .$xposition(560), .$yposition(150), .$vflip(1), .$hflip(1), .$angle(0)) noname(net_560_150, net_660_150);
wire #(.$vscale(20), .$xposition(560), .$yposition(150), .$vflip(1), .$hflip(1), .$angle(270)) noname(net_560_150, net_560_170);
wire #(.$vscale(180), .$xposition(380), .$yposition(250), .$vflip(1), .$hflip(1), .$angle(0)) noname(net_380_250, net_560_250);
wire #(.$vscale(100), .$xposition(560), .$yposition(250), .$vflip(1), .$hflip(1), .$angle(0)) noname(net_560_250, net_660_250);
wire #(.$vscale(20), .$xposition(560), .$yposition(230), .$vflip(1), .$hflip(1), .$angle(270)) noname(net_560_230, net_560_250);
// `ifdef QUCS_DIAGRAMS
// Rect
// Rect
// `endif
place #(.$xposition(140), .$yposition(150)) net_140_150(net_140_150);
place #(.$xposition(200), .$yposition(150)) net_200_150(net_200_150);
place #(.$xposition(70), .$yposition(170)) net_70_170(net_70_170);
place #(.$xposition(70), .$yposition(230)) net_70_230(net_70_230);
place #(.$xposition(70), .$yposition(250)) net_70_250(net_70_250);
place #(.$xposition(320), .$yposition(170)) net_320_170(net_320_170);
place #(.$xposition(380), .$yposition(170)) net_380_170(net_380_170);
place #(.$xposition(380), .$yposition(230)) net_380_230(net_380_230);
place #(.$xposition(320), .$yposition(230)) net_320_230(net_320_230);
place #(.$xposition(220), .$yposition(270)) net_220_270(net_220_270);
place #(.$xposition(220), .$yposition(330)) net_220_330(net_220_330);
place #(.$xposition(220), .$yposition(370)) net_220_370(net_220_370);
place #(.$xposition(520), .$yposition(150)) net_520_150(net_520_150);
place #(.$xposition(460), .$yposition(150)) net_460_150(net_460_150);
place #(.$xposition(660), .$yposition(250)) net_660_250(net_660_250);
place #(.$xposition(560), .$yposition(170)) net_560_170(net_560_170);
place #(.$xposition(560), .$yposition(230)) net_560_230(net_560_230);
place #(.$xposition(660), .$yposition(230)) net_660_230(net_660_230);
place #(.$xposition(660), .$yposition(170)) net_660_170(net_660_170);
place #(.$xposition(70), .$yposition(150)) net_70_150(net_70_150);
place #(.$xposition(380), .$yposition(150)) net_380_150(net_380_150);
place #(.$xposition(320), .$yposition(370)) net_320_370(net_320_370);
place #(.$xposition(220), .$yposition(170)) net_220_170(net_220_170);
place #(.$xposition(380), .$yposition(250)) net_380_250(net_380_250);
place #(.$xposition(660), .$yposition(150)) net_660_150(net_660_150);
place #(.$xposition(560), .$yposition(150)) net_560_150(net_560_150);
place #(.$xposition(560), .$yposition(250)) net_560_250(net_560_250);
endmodule // main

