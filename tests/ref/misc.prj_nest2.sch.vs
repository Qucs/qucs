module main(net_180_180, net_400_180);
Port #(.Num(2), .Type(analog)) P2(net_400_180);
Port #(.Num(1), .Type(analog)) P1(net_180_180);
R #(.R(0 Ohm), .Temp(26.85), .Tc1(0.0), .Tc2(0.0), .Tnom(26.85), .Symbol(US)) R1(net_260_260, net_320_260);
GND #() anonymous_gnd_hack_0(net_380_360);
C #(.C(1 pF), .V(), .Symbol(neutral)) C1(net_380_310, net_380_250);
wire #() noname(net_240_260, net_260_260);
wire #() noname(net_320_260, net_340_260);
wire #() noname(net_380_180, net_400_180);
wire #() noname(net_380_180, net_380_250);
wire #() noname(net_380_310, net_380_360);
wire #() noname(net_340_180, net_380_180);
wire #() noname(net_340_180, net_340_260);
wire #() noname(net_180_180, net_240_180);
wire #() noname(net_240_180, net_240_260);
place #(.$xposition(180),.$yposition(180)) place_180_180(net_180_180);
place #(.$xposition(240),.$yposition(180)) place_240_180(net_240_180);
place #(.$xposition(240),.$yposition(260)) place_240_260(net_240_260);
place #(.$xposition(260),.$yposition(260)) place_260_260(net_260_260);
place #(.$xposition(320),.$yposition(260)) place_320_260(net_320_260);
place #(.$xposition(340),.$yposition(180)) place_340_180(net_340_180);
place #(.$xposition(340),.$yposition(260)) place_340_260(net_340_260);
place #(.$xposition(380),.$yposition(180)) place_380_180(net_380_180);
place #(.$xposition(380),.$yposition(250)) place_380_250(net_380_250);
place #(.$xposition(380),.$yposition(310)) place_380_310(net_380_310);
place #(.$xposition(380),.$yposition(360)) place_380_360(net_380_360);
place #(.$xposition(400),.$yposition(180)) place_400_180(net_400_180);
endmodule // main

Sub #(.File()) Sub(net_0_0);
// skip sckt :SymbolSection:
// skip sckt :Paintings:
