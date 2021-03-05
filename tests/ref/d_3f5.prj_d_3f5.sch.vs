// VS::printSubckt main
module main();
GND #(.$xposition(490), .$yposition(380), .$vflip(1), .$hflip(1), .$tx(0), .$ty(0)) anonymous_gnd_hack_0(net_490_380);
Lib #(.$xposition(490), .$yposition(340), .$vflip(1), .$hflip(1), .$tx(0), .$ty(0), .Lib(1), .Component(0)) d_3f52();
Lib #(.$xposition(400), .$yposition(300), .$vflip(1), .$hflip(1), .$tx(0), .$ty(0), .Lib(), .Component()) ammeter1();
GND #(.$xposition(330), .$yposition(360), .$vflip(1), .$hflip(1), .$tx(0), .$ty(0)) anonymous_gnd_hack_1(net_330_360);
Lib #(.$xposition(350), .$yposition(350), .$vflip(1), .$hflip(1), .$tx(0), .$ty(0), .Lib(dcsweep), .Component()) v_dc1();
wire #(.$xposition(440), .$yposition(300), .$vflip(1), .$hflip(1)) noname(net_440_300, net_490_300);
wire #(.$xposition(330), .$yposition(300), .$vflip(1), .$hflip(1)) noname(net_330_300, net_380_300);
wire #(.$xposition(330), .$yposition(300), .$vflip(1), .$hflip(1)) noname(net_330_300, net_330_310);
place #(.$xposition(490), .$yposition(380), .$vflip(1), .$hflip(1)) net_490_380(net_490_380);
place #(.$xposition(330), .$yposition(360), .$vflip(1), .$hflip(1)) net_330_360(net_330_360);
place #(.$xposition(440), .$yposition(300), .$vflip(1), .$hflip(1)) net_440_300(net_440_300);
place #(.$xposition(490), .$yposition(300), .$vflip(1), .$hflip(1)) net_490_300(net_490_300);
place #(.$xposition(330), .$yposition(300), .$vflip(1), .$hflip(1)) net_330_300(net_330_300);
place #(.$xposition(380), .$yposition(300), .$vflip(1), .$hflip(1)) net_380_300(net_380_300);
place #(.$xposition(330), .$yposition(310), .$vflip(1), .$hflip(1)) net_330_310(net_330_310);
endmodule // main

