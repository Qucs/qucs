// VS::printSubckt main
module main();
C #(.C(39.01pF), .V(), .Symbol(neutral), .$ty(17), .$tx(17), .$xposition(240), .$yposition(220), .$vflip(1), .$hflip(1)) C1(net_240_250, net_240_190);
GND #(.$xposition(240), .$yposition(250), .$vflip(1), .$hflip(1)) anonymous_gnd_hack_0(net_240_250);
L #(.L(280.9nH), .I(), .$ty(-26), .$tx(-26), .$xposition(310), .$yposition(140), .$vflip(1), .$hflip(1)) L1(net_280_140, net_340_140);
C #(.C(39.01pF), .V(), .Symbol(neutral), .$ty(17), .$tx(17), .$xposition(380), .$yposition(220), .$vflip(1), .$hflip(1)) C2(net_380_250, net_380_190);
GND #(.$xposition(380), .$yposition(250), .$vflip(1), .$hflip(1)) anonymous_gnd_hack_1(net_380_250);
GND #(.$xposition(60), .$yposition(250), .$vflip(1), .$hflip(1)) anonymous_gnd_hack_2(net_60_250);
Vac #(.U(1 V), .f(1 GHz), .Phase(0), .Theta(0), .$ty(18), .$tx(18), .$xposition(60), .$yposition(200), .$vflip(1), .$hflip(1)) V1(net_60_170, net_60_230);
Vac #(.U(0 V), .f(1 GHz), .Phase(0), .Theta(0), .$ty(18), .$tx(18), .$xposition(550), .$yposition(200), .$vflip(1), .$hflip(1)) V2(net_550_170, net_550_230);
GND #(.$xposition(550), .$yposition(250), .$vflip(1), .$hflip(1)) anonymous_gnd_hack_3(net_550_250);
R #(.R(Z0), .Temp(26.85), .Tc1(0.0), .Tc2(0.0), .Tnom(26.85), .Symbol(european), .$ty(-26), .$tx(-26), .$xposition(480), .$yposition(140), .$vflip(1), .$hflip(1)) R2(net_450_140, net_510_140);
//AC1
Eqn #(.dBS21_3rd(dB(S21)), .S21(b2/a1), .S11(b1/a1), .a1((P1.v+Z0*-V1.i)/(2*sqrt(Z0))), .Z0(60), .b1((P1.v-Z0*-V1.i)/(2*sqrt(Z0))), .b2((P2.v-Z0*-V2.i)/(2*sqrt(Z0))), .gain_phase(unwrap(angle(S21))), .group_delay(-diff(gain_phase,2*pi*acfrequency)), .Export(yes), .$ty(-28), .$tx(-28), .$xposition(270), .$yposition(340), .$vflip(1), .$hflip(1)) Eqn1();
R #(.R(Z0), .Temp(26.85), .Tc1(0.0), .Tc2(0.0), .Tnom(26.85), .Symbol(european), .$ty(-26), .$tx(-26), .$xposition(150), .$yposition(140), .$vflip(1), .$hflip(1)) R1(net_120_140, net_180_140);
wire #(.$xposition(240), .$yposition(140), .$vflip(1), .$hflip(1)) noname(net_240_140, net_240_190);
wire #(.$xposition(380), .$yposition(140), .$vflip(1), .$hflip(1)) noname(net_380_140, net_380_190);
wire #(.$xposition(240), .$yposition(140), .$vflip(1), .$hflip(1)) noname(net_240_140, net_280_140);
wire #(.$xposition(340), .$yposition(140), .$vflip(1), .$hflip(1)) noname(net_340_140, net_380_140);
wire #(.$xposition(60), .$yposition(230), .$vflip(1), .$hflip(1)) noname(net_60_230, net_60_250);
wire #(.$tx(50), .$ty(-40), .netname(P2), .$xposition(380), .$yposition(140), .$vflip(1), .$hflip(1)) noname(net_380_140, net_450_140);
wire #(.$xposition(510), .$yposition(140), .$vflip(1), .$hflip(1)) noname(net_510_140, net_550_140);
wire #(.$xposition(550), .$yposition(140), .$vflip(1), .$hflip(1)) noname(net_550_140, net_550_170);
wire #(.$xposition(550), .$yposition(230), .$vflip(1), .$hflip(1)) noname(net_550_230, net_550_250);
wire #(.$tx(50), .$ty(-40), .netname(P1), .$xposition(180), .$yposition(140), .$vflip(1), .$hflip(1)) noname(net_180_140, net_240_140);
wire #(.$xposition(60), .$yposition(140), .$vflip(1), .$hflip(1)) noname(net_60_140, net_60_170);
wire #(.$xposition(60), .$yposition(140), .$vflip(1), .$hflip(1)) noname(net_60_140, net_120_140);
place #(.$xposition(240), .$yposition(250)) net_240_250(net_240_250);
place #(.$xposition(240), .$yposition(190)) net_240_190(net_240_190);
place #(.$xposition(280), .$yposition(140)) net_280_140(net_280_140);
place #(.$xposition(340), .$yposition(140)) net_340_140(net_340_140);
place #(.$xposition(380), .$yposition(250)) net_380_250(net_380_250);
place #(.$xposition(380), .$yposition(190)) net_380_190(net_380_190);
place #(.$xposition(60), .$yposition(250)) net_60_250(net_60_250);
place #(.$xposition(60), .$yposition(170)) net_60_170(net_60_170);
place #(.$xposition(60), .$yposition(230)) net_60_230(net_60_230);
place #(.$xposition(550), .$yposition(170)) net_550_170(net_550_170);
place #(.$xposition(550), .$yposition(230)) net_550_230(net_550_230);
place #(.$xposition(550), .$yposition(250)) net_550_250(net_550_250);
place #(.$xposition(450), .$yposition(140)) net_450_140(net_450_140);
place #(.$xposition(510), .$yposition(140)) net_510_140(net_510_140);
place #(.$xposition(120), .$yposition(140)) net_120_140(net_120_140);
place #(.$xposition(180), .$yposition(140)) net_180_140(net_180_140);
place #(.$xposition(240), .$yposition(140)) net_240_140(net_240_140);
place #(.$xposition(380), .$yposition(140)) net_380_140(net_380_140);
place #(.$xposition(550), .$yposition(140)) net_550_140(net_550_140);
place #(.$xposition(60), .$yposition(140)) net_60_140(net_60_140);
endmodule // main

