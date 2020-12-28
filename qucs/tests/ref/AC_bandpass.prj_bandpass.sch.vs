module main();
GND #() anonymous_gnd_hack_0(net_280_310);
GND #() anonymous_gnd_hack_1(net_570_310);
GND #() anonymous_gnd_hack_2(net_170_150);
GND #() anonymous_gnd_hack_3(net_480_150);
C #(.C(0.1 uF), .V(), .Symbol(neutral)) C1(net_180_210, net_240_210);
R #(.R(10 kOhm), .Temp(26.85), .Tc1(0.0), .Tc2(0.0), .Tnom(26.85), .Symbol(european)) R2(net_180_150, net_240_150);
R #(.R(10 kOhm), .Temp(26.85), .Tc1(0.0), .Tc2(0.0), .Tnom(26.85), .Symbol(european)) R5(net_490_150, net_550_150);
Sub$opa227 #(.File(opa227.sch)) OPA227_1(net_310_210, net_310_150, net_370_180);
Sub$opa227 #(.File(opa227.sch)) OPA227_2(net_590_210, net_590_150, net_650_180);
//AC1
GND #() anonymous_gnd_hack_4(net_100_310);
Vac #(.U(1 V), .f(1 kHz), .Phase(0), .Theta(0)) V1(net_100_230, net_100_290);
Eqn #(.y1(dB(Vout.v)), .y2(phase(Vout.v)), .Export(yes)) Eqn1();
R #(.R(10 kOhm), .Temp(26.85), .Tc1(0.0), .Tc2(0.0), .Tnom(26.85), .Symbol(european)) R1(net_310_70, net_370_70);
R #(.R(10 kOhm), .Temp(26.85), .Tc1(0.0), .Tc2(0.0), .Tnom(26.85), .Symbol(european)) R4(net_590_70, net_650_70);
C #(.C(0.1 uF), .V(), .Symbol(neutral)) C2(net_570_310, net_570_250);
R #(.R(1591 Ohm), .Temp(26.85), .Tc1(0.0), .Tc2(0.0), .Tnom(26.85), .Symbol(european)) R3(net_280_250, net_280_310);
R #(.R(1.59 Ohm), .Temp(26.85), .Tc1(0.0), .Tc2(0.0), .Tnom(26.85), .Symbol(european)) R6(net_490_210, net_550_210);
wire #() noname(net_240_150, net_280_150);
wire #() noname(net_240_210, net_280_210);
wire #() noname(net_280_70, net_310_70);
wire #() noname(net_280_150, net_310_150);
wire #() noname(net_280_70, net_280_150);
wire #() noname(net_370_180, net_410_180);
wire #() noname(net_410_70, net_410_180);
wire #() noname(net_370_70, net_410_70);
wire #() noname(net_550_150, net_560_150);
wire #() noname(net_560_70, net_590_70);
wire #() noname(net_560_150, net_590_150);
wire #() noname(net_560_70, net_560_150);
wire #() noname(net_650_180, net_700_180);
wire #() noname(net_700_70, net_700_180);
wire #() noname(net_650_70, net_700_70);
wire #() noname(net_700_180, net_730_180);
wire #() noname(net_550_210, net_570_210);
wire #() noname(net_570_210, net_590_210);
wire #() noname(net_480_150, net_490_150);
wire #() noname(net_170_150, net_180_150);
wire #() noname(net_410_180, net_410_210);
wire #() noname(net_410_210, net_490_210);
wire #() noname(net_280_210, net_310_210);
wire #() noname(net_280_210, net_280_250);
wire #() noname(net_100_290, net_100_310);
wire #() noname(net_100_210, net_180_210);
wire #() noname(net_100_210, net_100_230);
wire #() noname(net_570_210, net_570_250);
place #(.$xposition(100),.$yposition(210)) place_100_210(net_100_210);
place #(.$xposition(100),.$yposition(230)) place_100_230(net_100_230);
place #(.$xposition(100),.$yposition(290)) place_100_290(net_100_290);
place #(.$xposition(100),.$yposition(310)) place_100_310(net_100_310);
place #(.$xposition(170),.$yposition(150)) place_170_150(net_170_150);
place #(.$xposition(180),.$yposition(150)) place_180_150(net_180_150);
place #(.$xposition(180),.$yposition(210)) place_180_210(net_180_210);
place #(.$xposition(240),.$yposition(150)) place_240_150(net_240_150);
place #(.$xposition(240),.$yposition(210)) place_240_210(net_240_210);
place #(.$xposition(280),.$yposition(70)) place_280_70(net_280_70);
place #(.$xposition(280),.$yposition(150)) place_280_150(net_280_150);
place #(.$xposition(280),.$yposition(210)) place_280_210(net_280_210);
place #(.$xposition(280),.$yposition(250)) place_280_250(net_280_250);
place #(.$xposition(280),.$yposition(310)) place_280_310(net_280_310);
place #(.$xposition(310),.$yposition(70)) place_310_70(net_310_70);
place #(.$xposition(310),.$yposition(150)) place_310_150(net_310_150);
place #(.$xposition(310),.$yposition(210)) place_310_210(net_310_210);
place #(.$xposition(370),.$yposition(70)) place_370_70(net_370_70);
place #(.$xposition(370),.$yposition(180)) place_370_180(net_370_180);
place #(.$xposition(410),.$yposition(70)) place_410_70(net_410_70);
place #(.$xposition(410),.$yposition(180)) place_410_180(net_410_180);
place #(.$xposition(410),.$yposition(210)) place_410_210(net_410_210);
place #(.$xposition(480),.$yposition(150)) place_480_150(net_480_150);
place #(.$xposition(490),.$yposition(150)) place_490_150(net_490_150);
place #(.$xposition(490),.$yposition(210)) place_490_210(net_490_210);
place #(.$xposition(550),.$yposition(150)) place_550_150(net_550_150);
place #(.$xposition(550),.$yposition(210)) place_550_210(net_550_210);
place #(.$xposition(560),.$yposition(70)) place_560_70(net_560_70);
place #(.$xposition(560),.$yposition(150)) place_560_150(net_560_150);
place #(.$xposition(570),.$yposition(210)) place_570_210(net_570_210);
place #(.$xposition(570),.$yposition(250)) place_570_250(net_570_250);
place #(.$xposition(570),.$yposition(310)) place_570_310(net_570_310);
place #(.$xposition(590),.$yposition(70)) place_590_70(net_590_70);
place #(.$xposition(590),.$yposition(150)) place_590_150(net_590_150);
place #(.$xposition(590),.$yposition(210)) place_590_210(net_590_210);
place #(.$xposition(650),.$yposition(70)) place_650_70(net_650_70);
place #(.$xposition(650),.$yposition(180)) place_650_180(net_650_180);
place #(.$xposition(700),.$yposition(70)) place_700_70(net_700_70);
place #(.$xposition(700),.$yposition(180)) place_700_180(net_700_180);
place #(.$xposition(730),.$yposition(180)) place_730_180(net_730_180);
endmodule // main

Sub #(.File()) Sub(net_0_0);
// skip sckt :SymbolSection:
// just main scope
module Sub:opa227();
VCVS #(.G(1), .T(0)) SRC2(net_500_160, net_560_160, net_560_220, net_500_220);
GND #() anonymous_gnd_hack_5(net_410_220);
R #(.R(1 kOhm), .Temp(26.85), .Tc1(0.0), .Tc2(0.0), .Tnom(26.85), .Symbol(european)) R1(net_330_160, net_390_160);
C #(.C(1.98944 mF), .V(), .Symbol(neutral)) C1(net_440_220, net_440_160);
VCVS #(.G(1e8), .T(0)) SRC1(net_230_160, net_290_160, net_290_220, net_230_220);
Port #(.Num(1), .Type(analog)) P1(net_160_160);
Port #(.Num(3), .Type(analog)) P3(net_610_160);
Port #(.Num(2), .Type(analog)) P2(net_160_220);
GND #() anonymous_gnd_hack_6(net_600_220);
wire #() noname(net_390_160, net_440_160);
wire #() noname(net_410_220, net_440_220);
wire #() noname(net_560_160, net_610_160);
wire #() noname(net_290_220, net_410_220);
wire #() noname(net_290_160, net_330_160);
wire #() noname(net_440_220, net_500_220);
wire #() noname(net_440_160, net_500_160);
wire #() noname(net_160_160, net_230_160);
wire #() noname(net_160_220, net_230_220);
wire #() noname(net_560_220, net_600_220);
place #(.$xposition(160),.$yposition(160)) place_160_160(net_160_160);
place #(.$xposition(160),.$yposition(220)) place_160_220(net_160_220);
place #(.$xposition(230),.$yposition(160)) place_230_160(net_230_160);
place #(.$xposition(230),.$yposition(220)) place_230_220(net_230_220);
place #(.$xposition(290),.$yposition(160)) place_290_160(net_290_160);
place #(.$xposition(290),.$yposition(220)) place_290_220(net_290_220);
place #(.$xposition(330),.$yposition(160)) place_330_160(net_330_160);
place #(.$xposition(390),.$yposition(160)) place_390_160(net_390_160);
place #(.$xposition(410),.$yposition(220)) place_410_220(net_410_220);
place #(.$xposition(440),.$yposition(160)) place_440_160(net_440_160);
place #(.$xposition(440),.$yposition(220)) place_440_220(net_440_220);
place #(.$xposition(500),.$yposition(160)) place_500_160(net_500_160);
place #(.$xposition(500),.$yposition(220)) place_500_220(net_500_220);
place #(.$xposition(560),.$yposition(160)) place_560_160(net_560_160);
place #(.$xposition(560),.$yposition(220)) place_560_220(net_560_220);
place #(.$xposition(600),.$yposition(220)) place_600_220(net_600_220);
place #(.$xposition(610),.$yposition(160)) place_610_160(net_610_160);
endmodule // Sub:opa227

// skip sckt :Diagrams:
// skip sckt :Paintings:
