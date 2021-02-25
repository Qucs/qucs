module main()
GND #() anonymous_gnd_hack_0(net_660_370);
R #(.R(50 Ohm), .Temp(26.85), .Tc1(0.0), .Tc2(0.0), .Tnom(26.85), .Symbol(european)) R2(net_580_270, net_640_270);
R #(.R(50 Ohm), .Temp(26.85), .Tc1(0.0), .Tc2(0.0), .Tnom(26.85), .Symbol(european)) R1(net_500_270, net_560_270);
Vac #(.U(10 V), .f(1kHz), .Phase(0), .Theta(0)) V1(net_450_290, net_450_350);
endmodule
