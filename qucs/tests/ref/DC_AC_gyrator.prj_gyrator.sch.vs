module main();
GND #() anonymous_gnd_hack_0(net_120_320);
Vdc #(.U(10 V)) V1(net_120_260, net_120_320);
Vac #(.U(1 V), .f(1 GHz), .Phase(0), .Theta(0)) V2(net_120_200, net_120_260);
GND #() anonymous_gnd_hack_1(net_570_320);
R #(.R(1000 Ohm), .Temp(26.85), .Tc1(0.0), .Tc2(0.0), .Tnom(26.85), .Symbol(european)) Load(net_570_320, net_570_260);
//DC1
//AC1
_BJT #(.Type(npn), .Is(1e-16), .Nf(1), .Nr(1), .Ikf(0), .Ikr(0), .Vaf(0), .Var(0), .Ise(0), .Ne(1.5), .Isc(0), .Nc(2), .Bf(250), .Br(1), .Rbm(0), .Irb(0), .Rc(0), .Re(0), .Rb(0), .Cje(0), .Vje(0.75), .Mje(0.33), .Cjc(0), .Vjc(0.75), .Mjc(0.33), .Xcjc(1.0), .Cjs(0), .Vjs(0.75), .Mjs(0), .Fc(0.5), .Tf(0.0), .Xtf(0.0), .Vtf(0.0), .Itf(0.0), .Tr(0.0), .Temp(26.85), .Kf(0.0), .Af(1.0), .Ffe(1.0), .Kb(0.0), .Ab(1.0), .Fb(1.0), .Ptf(0.0), .Xtb(0.0), .Xti(3.0), .Eg(1.11), .Tnom(26.85), .Area(1.0)) T1(net_360_160, net_330_130, net_390_130, net_330_130);
R #(.R(20 kOhm), .Temp(26.85), .Tc1(0.0), .Tc2(0.0), .Tnom(26.85), .Symbol(european)) R1(net_280_160, net_340_160);
C #(.C(200 uF), .V(), .Symbol(neutral)) C1(net_380_160, net_440_160);
wire #() noname(net_280_130, net_330_130);
wire #() noname(net_280_130, net_280_160);
wire #() noname(net_340_160, net_360_160);
wire #() noname(net_440_130, net_440_160);
wire #() noname(net_390_130, net_440_130);
wire #() noname(net_440_160, net_570_160);
wire #() noname(net_570_160, net_570_260);
wire #() noname(net_120_160, net_280_160);
wire #() noname(net_120_160, net_120_200);
wire #() noname(net_360_160, net_380_160);
wire #() noname(net_570_160, net_570_160);
wire #() noname(net_120_160, net_120_160);
place #(.$xposition(120),.$yposition(160)) place_120_160(net_120_160);
place #(.$xposition(120),.$yposition(200)) place_120_200(net_120_200);
place #(.$xposition(120),.$yposition(260)) place_120_260(net_120_260);
place #(.$xposition(120),.$yposition(320)) place_120_320(net_120_320);
place #(.$xposition(280),.$yposition(130)) place_280_130(net_280_130);
place #(.$xposition(280),.$yposition(160)) place_280_160(net_280_160);
place #(.$xposition(330),.$yposition(130)) place_330_130(net_330_130);
place #(.$xposition(340),.$yposition(160)) place_340_160(net_340_160);
place #(.$xposition(360),.$yposition(160)) place_360_160(net_360_160);
place #(.$xposition(380),.$yposition(160)) place_380_160(net_380_160);
place #(.$xposition(390),.$yposition(130)) place_390_130(net_390_130);
place #(.$xposition(440),.$yposition(130)) place_440_130(net_440_130);
place #(.$xposition(440),.$yposition(160)) place_440_160(net_440_160);
place #(.$xposition(570),.$yposition(160)) place_570_160(net_570_160);
place #(.$xposition(570),.$yposition(260)) place_570_260(net_570_260);
place #(.$xposition(570),.$yposition(320)) place_570_320(net_570_320);
endmodule // main

Sub #(.File()) Sub(net_0_0);
// skip sckt :SymbolSection:
// skip sckt :Diagrams:
// skip sckt :Paintings:
