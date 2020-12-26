module main();
C #(.C(195 fF), .V(), .Symbol(neutral)) C1(net_180_370, net_180_310);
C #(.C(155 fF), .V(), .Symbol(neutral)) C2(net_260_260, net_320_260);
SUBST #(.er(4.5), .h(0.8 mm), .t(35 um), .tand(2e-4), .rho(0.022e-6), .D(0.15e-6)) Subst1();
GND #() anonymous_gnd_hack_0(net_180_410);
GND #() anonymous_gnd_hack_1(net_400_410);
GND #() anonymous_gnd_hack_2(net_620_410);
GND #() anonymous_gnd_hack_3(net_730_410);
C #(.C(320 fF), .V(), .Symbol(neutral)) C3(net_400_370, net_400_310);
C #(.C(82 fF), .V(), .Symbol(neutral)) C5(net_620_370, net_620_310);
C #(.C(600 fF), .V(), .Symbol(neutral)) C4(net_480_260, net_540_260);
//SP1
//AC1
//DC1
Eqn #(.S21_dB(dB(S[2,1])), .S11_dB(dB(S[1,1])), .Export(yes)) Eqn1();
Pac #(.Num(2), .Z(50 Ohm), .P(0 W), .f(1 GHz), .Temp(-273.15)) P2(net_730_310, net_730_370);
Pac #(.Num(1), .Z(50 Ohm), .P(0 dBm), .f(1 GHz), .Temp(-273.15)) P1(net_30_270, net_30_330);
Vdc #(.U(1.1 V)) V1(net_30_350, net_30_410);
GND #() anonymous_gnd_hack_4(net_30_410);
wire #() noname(net_180_260, net_180_310);
wire #() noname(net_180_260, net_260_260);
wire #() noname(net_180_200, net_180_260);
wire #() noname(net_180_200, net_260_200);
wire #() noname(net_320_200, net_400_200);
wire #() noname(net_320_260, net_400_260);
wire #() noname(net_400_260, net_400_310);
wire #() noname(net_400_200, net_400_260);
wire #() noname(net_400_260, net_480_260);
wire #() noname(net_540_260, net_620_260);
wire #() noname(net_620_260, net_620_310);
wire #() noname(net_620_370, net_620_410);
wire #() noname(net_400_370, net_400_410);
wire #() noname(net_180_370, net_180_410);
wire #() noname(net_400_200, net_480_200);
wire #() noname(net_540_200, net_620_200);
wire #() noname(net_620_200, net_620_260);
wire #() noname(net_730_370, net_730_410);
wire #() noname(net_620_260, net_730_260);
wire #() noname(net_730_260, net_730_310);
wire #() noname(net_30_260, net_180_260);
wire #() noname(net_30_260, net_30_270);
wire #() noname(net_30_330, net_30_350);
wire #() noname(net_730_260, net_730_260);
place #(.$xposition(30),.$yposition(260)) place_30_260(net_30_260);
place #(.$xposition(30),.$yposition(270)) place_30_270(net_30_270);
place #(.$xposition(30),.$yposition(330)) place_30_330(net_30_330);
place #(.$xposition(30),.$yposition(350)) place_30_350(net_30_350);
place #(.$xposition(30),.$yposition(410)) place_30_410(net_30_410);
place #(.$xposition(180),.$yposition(200)) place_180_200(net_180_200);
place #(.$xposition(180),.$yposition(260)) place_180_260(net_180_260);
place #(.$xposition(180),.$yposition(310)) place_180_310(net_180_310);
place #(.$xposition(180),.$yposition(370)) place_180_370(net_180_370);
place #(.$xposition(180),.$yposition(410)) place_180_410(net_180_410);
place #(.$xposition(260),.$yposition(200)) place_260_200(net_260_200);
place #(.$xposition(260),.$yposition(260)) place_260_260(net_260_260);
place #(.$xposition(320),.$yposition(200)) place_320_200(net_320_200);
place #(.$xposition(320),.$yposition(260)) place_320_260(net_320_260);
place #(.$xposition(400),.$yposition(200)) place_400_200(net_400_200);
place #(.$xposition(400),.$yposition(260)) place_400_260(net_400_260);
place #(.$xposition(400),.$yposition(310)) place_400_310(net_400_310);
place #(.$xposition(400),.$yposition(370)) place_400_370(net_400_370);
place #(.$xposition(400),.$yposition(410)) place_400_410(net_400_410);
place #(.$xposition(480),.$yposition(200)) place_480_200(net_480_200);
place #(.$xposition(480),.$yposition(260)) place_480_260(net_480_260);
place #(.$xposition(540),.$yposition(200)) place_540_200(net_540_200);
place #(.$xposition(540),.$yposition(260)) place_540_260(net_540_260);
place #(.$xposition(620),.$yposition(200)) place_620_200(net_620_200);
place #(.$xposition(620),.$yposition(260)) place_620_260(net_620_260);
place #(.$xposition(620),.$yposition(310)) place_620_310(net_620_310);
place #(.$xposition(620),.$yposition(370)) place_620_370(net_620_370);
place #(.$xposition(620),.$yposition(410)) place_620_410(net_620_410);
place #(.$xposition(730),.$yposition(260)) place_730_260(net_730_260);
place #(.$xposition(730),.$yposition(310)) place_730_310(net_730_310);
place #(.$xposition(730),.$yposition(370)) place_730_370(net_730_370);
place #(.$xposition(730),.$yposition(410)) place_730_410(net_730_410);
endmodule // main

Sub #(.File()) Sub(net_0_0);
// skip sckt :SymbolSection:
// skip sckt :Diagrams:
// skip sckt :Paintings:
