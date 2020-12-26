module main();
C #(.C(39.01pF), .V(), .Symbol(neutral)) C1(net_240_250, net_240_190);
GND #() anonymous_gnd_hack_0(net_240_250);
L #(.L(280.9nH), .I()) L1(net_280_140, net_340_140);
C #(.C(39.01pF), .V(), .Symbol(neutral)) C2(net_380_250, net_380_190);
GND #() anonymous_gnd_hack_1(net_380_250);
GND #() anonymous_gnd_hack_2(net_60_250);
Vac #(.U(1 V), .f(1 GHz), .Phase(0), .Theta(0)) V1(net_60_170, net_60_230);
Vac #(.U(0 V), .f(1 GHz), .Phase(0), .Theta(0)) V2(net_550_170, net_550_230);
GND #() anonymous_gnd_hack_3(net_550_250);
R #(.R(Z0), .Temp(26.85), .Tc1(0.0), .Tc2(0.0), .Tnom(26.85), .Symbol(european)) R2(net_450_140, net_510_140);
//AC1
Eqn #(.dBS21_3rd(dB(S21)), .S21(b2/a1), .S11(b1/a1), .a1((P1.v+Z0*-V1.i)/(2*sqrt(Z0))), .Z0(60), .b1((P1.v-Z0*-V1.i)/(2*sqrt(Z0))), .b2((P2.v-Z0*-V2.i)/(2*sqrt(Z0))), .gain_phase(unwrap(angle(S21))), .group_delay(-diff(gain_phase,2*pi*acfrequency)), .Export(yes)) Eqn1();
R #(.R(Z0), .Temp(26.85), .Tc1(0.0), .Tc2(0.0), .Tnom(26.85), .Symbol(european)) R1(net_120_140, net_180_140);
wire #() noname(net_240_140, net_240_190);
wire #() noname(net_380_140, net_380_190);
wire #() noname(net_240_140, net_280_140);
wire #() noname(net_340_140, net_380_140);
wire #() noname(net_60_230, net_60_250);
wire #() noname(net_380_140, net_450_140);
wire #() noname(net_510_140, net_550_140);
wire #() noname(net_550_140, net_550_170);
wire #() noname(net_550_230, net_550_250);
wire #() noname(net_180_140, net_240_140);
wire #() noname(net_60_140, net_60_170);
wire #() noname(net_60_140, net_120_140);
place #(.$xposition(60),.$yposition(140)) place_60_140(net_60_140);
place #(.$xposition(60),.$yposition(170)) place_60_170(net_60_170);
place #(.$xposition(60),.$yposition(230)) place_60_230(net_60_230);
place #(.$xposition(60),.$yposition(250)) place_60_250(net_60_250);
place #(.$xposition(120),.$yposition(140)) place_120_140(net_120_140);
place #(.$xposition(180),.$yposition(140)) place_180_140(net_180_140);
place #(.$xposition(240),.$yposition(140)) place_240_140(net_240_140);
place #(.$xposition(240),.$yposition(190)) place_240_190(net_240_190);
place #(.$xposition(240),.$yposition(250)) place_240_250(net_240_250);
place #(.$xposition(280),.$yposition(140)) place_280_140(net_280_140);
place #(.$xposition(340),.$yposition(140)) place_340_140(net_340_140);
place #(.$xposition(380),.$yposition(140)) place_380_140(net_380_140);
place #(.$xposition(380),.$yposition(190)) place_380_190(net_380_190);
place #(.$xposition(380),.$yposition(250)) place_380_250(net_380_250);
place #(.$xposition(450),.$yposition(140)) place_450_140(net_450_140);
place #(.$xposition(510),.$yposition(140)) place_510_140(net_510_140);
place #(.$xposition(550),.$yposition(140)) place_550_140(net_550_140);
place #(.$xposition(550),.$yposition(170)) place_550_170(net_550_170);
place #(.$xposition(550),.$yposition(230)) place_550_230(net_550_230);
place #(.$xposition(550),.$yposition(250)) place_550_250(net_550_250);
endmodule // main

Sub #(.File()) Sub(net_0_0);
// skip sckt :SymbolSection:
// skip sckt :Diagrams:
// skip sckt :Paintings:
