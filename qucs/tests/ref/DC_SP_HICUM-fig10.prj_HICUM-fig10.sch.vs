module main();
Vdc #(.U(0.9 V)) V1(net_70_200, net_70_260);
GND #() anonymous_gnd_hack_0(net_400_270);
IProbe #() Ib(net_270_170, net_330_170);
IProbe #() Ic(net_480_110, net_420_110);
//DC1
Pac #(.Num(1), .Z(50 Ohm), .P(0 dBm), .f(1 GHz), .Temp(26.85)) P1(net_150_200, net_150_260);
Pac #(.Num(2), .Z(50 Ohm), .P(0 dBm), .f(1 GHz), .Temp(26.85)) P2(net_550_180, net_550_240);
Vdc #(.U(2 V)) V2(net_630_190, net_630_250);
Eqn #(.NFmin(10*log10(Fmin)), .NF(10*log10(F)), .Export(yes)) Eqn1();
//SP1
wire #() noname(net_70_110, net_70_200);
wire #() noname(net_520_60, net_520_80);
wire #() noname(net_70_260, net_70_270);
wire #() noname(net_70_270, net_150_270);
wire #() noname(net_150_270, net_400_270);
wire #() noname(net_150_260, net_150_270);
wire #() noname(net_70_110, net_210_110);
wire #() noname(net_210_110, net_210_140);
wire #() noname(net_150_170, net_150_200);
wire #() noname(net_150_170, net_180_170);
wire #() noname(net_400_270, net_460_270);
wire #() noname(net_460_270, net_550_270);
wire #() noname(net_460_170, net_460_270);
wire #() noname(net_430_170, net_460_170);
wire #() noname(net_400_200, net_400_270);
wire #() noname(net_400_110, net_400_140);
wire #() noname(net_400_110, net_420_110);
wire #() noname(net_480_110, net_490_110);
wire #() noname(net_330_170, net_370_170);
wire #() noname(net_240_170, net_270_170);
wire #() noname(net_550_240, net_550_270);
wire #() noname(net_550_110, net_550_180);
wire #() noname(net_550_270, net_630_270);
wire #() noname(net_630_250, net_630_270);
wire #() noname(net_520_60, net_630_60);
wire #() noname(net_630_60, net_630_190);
place #(.$xposition(70),.$yposition(110)) place_70_110(net_70_110);
place #(.$xposition(70),.$yposition(200)) place_70_200(net_70_200);
place #(.$xposition(70),.$yposition(260)) place_70_260(net_70_260);
place #(.$xposition(70),.$yposition(270)) place_70_270(net_70_270);
place #(.$xposition(150),.$yposition(170)) place_150_170(net_150_170);
place #(.$xposition(150),.$yposition(200)) place_150_200(net_150_200);
place #(.$xposition(150),.$yposition(260)) place_150_260(net_150_260);
place #(.$xposition(150),.$yposition(270)) place_150_270(net_150_270);
place #(.$xposition(180),.$yposition(170)) place_180_170(net_180_170);
place #(.$xposition(210),.$yposition(110)) place_210_110(net_210_110);
place #(.$xposition(210),.$yposition(140)) place_210_140(net_210_140);
place #(.$xposition(240),.$yposition(170)) place_240_170(net_240_170);
place #(.$xposition(270),.$yposition(170)) place_270_170(net_270_170);
place #(.$xposition(330),.$yposition(170)) place_330_170(net_330_170);
place #(.$xposition(370),.$yposition(170)) place_370_170(net_370_170);
place #(.$xposition(400),.$yposition(110)) place_400_110(net_400_110);
place #(.$xposition(400),.$yposition(140)) place_400_140(net_400_140);
place #(.$xposition(400),.$yposition(200)) place_400_200(net_400_200);
place #(.$xposition(400),.$yposition(270)) place_400_270(net_400_270);
place #(.$xposition(420),.$yposition(110)) place_420_110(net_420_110);
place #(.$xposition(430),.$yposition(170)) place_430_170(net_430_170);
place #(.$xposition(460),.$yposition(170)) place_460_170(net_460_170);
place #(.$xposition(460),.$yposition(270)) place_460_270(net_460_270);
place #(.$xposition(480),.$yposition(110)) place_480_110(net_480_110);
place #(.$xposition(490),.$yposition(110)) place_490_110(net_490_110);
place #(.$xposition(520),.$yposition(60)) place_520_60(net_520_60);
place #(.$xposition(520),.$yposition(80)) place_520_80(net_520_80);
place #(.$xposition(550),.$yposition(110)) place_550_110(net_550_110);
place #(.$xposition(550),.$yposition(180)) place_550_180(net_550_180);
place #(.$xposition(550),.$yposition(240)) place_550_240(net_550_240);
place #(.$xposition(550),.$yposition(270)) place_550_270(net_550_270);
place #(.$xposition(630),.$yposition(60)) place_630_60(net_630_60);
place #(.$xposition(630),.$yposition(190)) place_630_190(net_630_190);
place #(.$xposition(630),.$yposition(250)) place_630_250(net_630_250);
place #(.$xposition(630),.$yposition(270)) place_630_270(net_630_270);
endmodule // main

Sub #(.File()) Sub(net_0_0);
// skip sckt :SymbolSection:
// skip sckt :Diagrams:
// skip sckt :Paintings:
