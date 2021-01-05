// VS::printSubckt main
module main();
GND #(.$xposition(800), .$yposition(220), .$tx(0), .$ty(0)) anonymous_gnd_hack_0(net_800_220);
Pac #(.$xposition(800), .$yposition(170), .$tx(18), .$ty(18), .Num(2), .Z(50 Ohm), .P(0 W), .f(1 GHz), .Temp(-273.15)) P1(net_800_140, net_800_200);
GND #(.$xposition(310), .$yposition(220), .$tx(0), .$ty(0)) anonymous_gnd_hack_1(net_310_220);
//DC1
Eqn #(.$xposition(450), .$yposition(320), .$tx(-28), .$ty(-28), .S21_dB(dB(S[2,1])), .S11_dB(dB(S[1,1])), .Export(yes)) Eqn2();
Eqn #(.$xposition(450), .$yposition(440), .$tx(-31), .$ty(-31), .Ql(200), .Qc(5000), .f0(14e6), .Export(yes)) Eqn1();
//SP1
//AC1
GND #(.$xposition(100), .$yposition(220), .$tx(0), .$ty(0)) anonymous_gnd_hack_2(net_100_220);
Pac #(.$xposition(100), .$yposition(150), .$tx(-122), .$ty(-122), .Num(1), .Z(50 Ohm), .P(0 dBm), .f(1 GHz), .Temp(-273.15)) P2(net_100_120, net_100_180);
GND #(.$xposition(630), .$yposition(290), .$tx(0), .$ty(0)) anonymous_gnd_hack_3(net_630_290);
Vdc #(.$xposition(630), .$yposition(250), .$tx(-75), .$ty(-75), .U(1.1 V)) V1(net_630_220, net_630_280);
wire #(.$xposition(630), .$yposition(90)) noname(net_630_90, net_630_140);
wire #(.$xposition(590), .$yposition(140)) noname(net_590_140, net_630_140);
wire #(.$xposition(590), .$yposition(200)) noname(net_590_200, net_630_200);
wire #(.$xposition(630), .$yposition(200)) noname(net_630_200, net_630_220);
wire #(.$xposition(580), .$yposition(90)) noname(net_580_90, net_630_90);
wire #(.$xposition(630), .$yposition(90)) noname(net_630_90, net_800_90);
wire #(.$xposition(800), .$yposition(90)) noname(net_800_90, net_800_140);
wire #(.$xposition(800), .$yposition(200)) noname(net_800_200, net_800_220);
wire #(.$xposition(440), .$yposition(90)) noname(net_440_90, net_520_90);
wire #(.$xposition(310), .$yposition(90)) noname(net_310_90, net_310_140);
wire #(.$xposition(270), .$yposition(140)) noname(net_270_140, net_310_140);
wire #(.$xposition(270), .$yposition(200)) noname(net_270_200, net_310_200);
wire #(.$xposition(310), .$yposition(90)) noname(net_310_90, net_380_90);
wire #(.$xposition(310), .$yposition(200)) noname(net_310_200, net_310_220);
wire #(.$xposition(100), .$yposition(90)) noname(net_100_90, net_310_90);
wire #(.$xposition(100), .$yposition(90)) noname(net_100_90, net_100_120);
wire #(.$xposition(100), .$yposition(180)) noname(net_100_180, net_100_220);
wire #(.$xposition(630), .$yposition(280)) noname(net_630_280, net_630_290);
wire #(.$xposition(800), .$yposition(90)) noname(net_800_90, net_800_90);
place #(.$xposition(800), .$yposition(220)) net_800_220(net_800_220);
place #(.$xposition(800), .$yposition(140)) net_800_140(net_800_140);
place #(.$xposition(800), .$yposition(200)) net_800_200(net_800_200);
place #(.$xposition(310), .$yposition(220)) net_310_220(net_310_220);
place #(.$xposition(100), .$yposition(220)) net_100_220(net_100_220);
place #(.$xposition(100), .$yposition(120)) net_100_120(net_100_120);
place #(.$xposition(100), .$yposition(180)) net_100_180(net_100_180);
place #(.$xposition(630), .$yposition(290)) net_630_290(net_630_290);
place #(.$xposition(630), .$yposition(220)) net_630_220(net_630_220);
place #(.$xposition(630), .$yposition(280)) net_630_280(net_630_280);
place #(.$xposition(630), .$yposition(90)) net_630_90(net_630_90);
place #(.$xposition(630), .$yposition(140)) net_630_140(net_630_140);
place #(.$xposition(590), .$yposition(140)) net_590_140(net_590_140);
place #(.$xposition(590), .$yposition(200)) net_590_200(net_590_200);
place #(.$xposition(630), .$yposition(200)) net_630_200(net_630_200);
place #(.$xposition(580), .$yposition(90)) net_580_90(net_580_90);
place #(.$xposition(800), .$yposition(90)) net_800_90(net_800_90);
place #(.$xposition(440), .$yposition(90)) net_440_90(net_440_90);
place #(.$xposition(520), .$yposition(90)) net_520_90(net_520_90);
place #(.$xposition(310), .$yposition(90)) net_310_90(net_310_90);
place #(.$xposition(310), .$yposition(140)) net_310_140(net_310_140);
place #(.$xposition(270), .$yposition(140)) net_270_140(net_270_140);
place #(.$xposition(270), .$yposition(200)) net_270_200(net_270_200);
place #(.$xposition(310), .$yposition(200)) net_310_200(net_310_200);
place #(.$xposition(380), .$yposition(90)) net_380_90(net_380_90);
place #(.$xposition(100), .$yposition(90)) net_100_90(net_100_90);
endmodule // main

