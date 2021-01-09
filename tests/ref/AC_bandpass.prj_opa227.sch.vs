module main(net_160_160, net_160_220, net_610_160);
VCVS #(.G(1), .T(0)) SRC2(net_500_160, net_560_160, net_560_220, net_500_220);
GND #() anonymous_gnd_hack_0(net_410_220);
R #(.R(1 kOhm), .Temp(26.85), .Tc1(0.0), .Tc2(0.0), .Tnom(26.85), .Symbol(european)) R1(net_330_160, net_390_160);
C #(.C(1.98944 mF), .V(), .Symbol(neutral)) C1(net_440_220, net_440_160);
VCVS #(.G(1e8), .T(0)) SRC1(net_230_160, net_290_160, net_290_220, net_230_220);
Port #(.Num(1), .Type(analog)) P1(net_160_160);
Port #(.Num(3), .Type(analog)) P3(net_610_160);
Port #(.Num(2), .Type(analog)) P2(net_160_220);
GND #() anonymous_gnd_hack_1(net_600_220);
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
endmodule // main

Sub #(.File()) Sub(net_0_0);
// skip sckt :SymbolSection:
// skip sckt :Paintings:
