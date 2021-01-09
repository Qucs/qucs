module main();
GND #() anonymous_gnd_hack_0(net_660_370);
//TR1
R #(.R(50 Ohm), .Temp(26.85), .Tc1(0.0), .Tc2(0.0), .Tnom(26.85), .Symbol(european)) R2(net_580_270, net_640_270);
R #(.R(50 Ohm), .Temp(26.85), .Tc1(0.0), .Tc2(0.0), .Tnom(26.85), .Symbol(european)) R1(net_500_270, net_560_270);
Vac #(.U(10 V), .f(1kHz), .Phase(0), .Theta(0)) V1(net_450_290, net_450_350);
wire #() noname(net_450_350, net_450_360);
wire #() noname(net_450_360, net_660_360);
wire #() noname(net_660_360, net_660_370);
wire #() noname(net_660_270, net_660_360);
wire #() noname(net_450_270, net_450_290);
wire #() noname(net_450_270, net_500_270);
wire #() noname(net_560_270, net_580_270);
wire #() noname(net_640_270, net_660_270);
place #(.$xposition(450),.$yposition(270)) place_450_270(net_450_270);
place #(.$xposition(450),.$yposition(290)) place_450_290(net_450_290);
place #(.$xposition(450),.$yposition(350)) place_450_350(net_450_350);
place #(.$xposition(450),.$yposition(360)) place_450_360(net_450_360);
place #(.$xposition(500),.$yposition(270)) place_500_270(net_500_270);
place #(.$xposition(560),.$yposition(270)) place_560_270(net_560_270);
place #(.$xposition(580),.$yposition(270)) place_580_270(net_580_270);
place #(.$xposition(640),.$yposition(270)) place_640_270(net_640_270);
place #(.$xposition(660),.$yposition(270)) place_660_270(net_660_270);
place #(.$xposition(660),.$yposition(360)) place_660_360(net_660_360);
place #(.$xposition(660),.$yposition(370)) place_660_370(net_660_370);
endmodule // main

Sub #(.File()) Sub(net_0_0);
// skip sckt :SymbolSection:
// skip sckt :Paintings:
