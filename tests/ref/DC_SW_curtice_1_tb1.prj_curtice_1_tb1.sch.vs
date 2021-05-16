// VS::printSubckt main
module main();
Vdc #(.U(Vgs), .$ty(18), .$tx(18), .$xposition(80), .$yposition(160), .$vflip(1), .$hflip(1), .$angle(450)) V2(net_80_130, net_80_190);
GND #(.$xposition(80), .$yposition(190), .$vflip(1), .$hflip(1), .$angle(0)) anonymous_gnd_hack_0(net_80_190);
//DC1
Eqn #(.Id(-V1.I), .Export(yes), .$ty(-32), .$tx(-32), .$xposition(70), .$yposition(390), .$vflip(1), .$hflip(1), .$angle(0)) Eqn1();
GND #(.$xposition(380), .$yposition(160), .$vflip(1), .$hflip(1), .$angle(0)) anonymous_gnd_hack_1(net_380_160);
Vdc #(.U(Vds), .$ty(18), .$tx(18), .$xposition(380), .$yposition(120), .$vflip(1), .$hflip(1), .$angle(450)) V1(net_380_90, net_380_150);
GND #(.$xposition(210), .$yposition(160), .$vflip(1), .$hflip(1), .$angle(0)) anonymous_gnd_hack_2(net_210_160);
IProbe #(.$ty(-26), .$tx(-26), .$xposition(320), .$yposition(60), .$vflip(-1), .$hflip(1), .$angle(180)) Pr1(net_350_60, net_290_60);
Sub #(.$tx(20), .$ty(-46), .File(curtice_1.sch), .RG(1m), .RD(1m), .RS(1m), .VBR(10e10), .LG(1e-15), .LD(1e-15), .LS(1e-15), .IS(10f), .N(1), .XTI(0), .EG(1.11), .Beta(3m), .Lambda(40m), .VT0(-6), .Temp(27), .Alpha(0.8), .CDS(300f), .TAU(10p), .RIN(1m), .CGS(300f), .CGD(300f), .Tnom(27), .$xposition(210), .$yposition(120), .$vflip(1), .$hflip(1), .$angle(0)) Curtice1(net_210_160, net_180_120, net_210_80);
wire #(.$vscale(10), .$xposition(380), .$yposition(150), .$vflip(1), .$hflip(1), .$angle(270)) noname(net_380_150, net_380_160);
wire #(.$vscale(30), .$xposition(380), .$yposition(60), .$vflip(1), .$hflip(1), .$angle(270)) noname(net_380_60, net_380_90);
wire #(.$vscale(80), .$xposition(210), .$yposition(60), .$vflip(1), .$hflip(1), .$angle(0)) noname(net_210_60, net_290_60);
wire #(.$vscale(10), .$xposition(80), .$yposition(120), .$vflip(1), .$hflip(1), .$angle(270)) noname(net_80_120, net_80_130);
wire #(.$vscale(100), .$xposition(80), .$yposition(120), .$vflip(1), .$hflip(1), .$angle(0)) noname(net_80_120, net_180_120);
wire #(.$vscale(20), .$xposition(210), .$yposition(60), .$vflip(1), .$hflip(1), .$angle(270)) noname(net_210_60, net_210_80);
wire #(.$vscale(30), .$xposition(350), .$yposition(60), .$vflip(1), .$hflip(1), .$angle(0)) noname(net_350_60, net_380_60);
// `ifdef QUCS_DIAGRAMS
// Rect
// Rect
// `endif
place #(.$xposition(80), .$yposition(130)) net_80_130(net_80_130);
place #(.$xposition(80), .$yposition(190)) net_80_190(net_80_190);
place #(.$xposition(380), .$yposition(160)) net_380_160(net_380_160);
place #(.$xposition(380), .$yposition(90)) net_380_90(net_380_90);
place #(.$xposition(380), .$yposition(150)) net_380_150(net_380_150);
place #(.$xposition(210), .$yposition(160)) net_210_160(net_210_160);
place #(.$xposition(350), .$yposition(60)) net_350_60(net_350_60);
place #(.$xposition(290), .$yposition(60)) net_290_60(net_290_60);
place #(.$xposition(180), .$yposition(120)) net_180_120(net_180_120);
place #(.$xposition(210), .$yposition(80)) net_210_80(net_210_80);
place #(.$xposition(380), .$yposition(60)) net_380_60(net_380_60);
place #(.$xposition(210), .$yposition(60)) net_210_60(net_210_60);
place #(.$xposition(80), .$yposition(120)) net_80_120(net_80_120);
endmodule // main

