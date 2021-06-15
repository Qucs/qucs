module main();
R #(.R(200m), .Temp(26.85), .Tc1(0.0), .Tc2(0.0), .Tnom(26.85), .Symbol(european)) RD(_net15, _net1);
GND #() *(gnd);
Vdc #(.U(13.5V)) VDC(_net18, gnd);
Vdc #(.U(200V)) VGEN(_net7, _net18);
C #(.C(40nF), .V(), .Symbol(neutral)) CS(_net18, _net17);
R #(.R(400m), .Temp(26.85), .Tc1(0.0), .Tc2(0.0), .Tnom(26.85), .Symbol(european)) RE(_net18, _net15);
C #(.C(40nF), .V(), .Symbol(neutral)) CL(_net18, _net1);
//TR1
R #(.R(50), .Temp(26.85), .Tc1(0.0), .Tc2(0.0), .Tnom(26.85), .Symbol(european)) RI(_net1, Puls3b);
Switch #(.init(off), .time(10ns), .Ron(0), .Roff(1e9), .Temp(26.85), .MaxDuration(1e-6), .Transition(spline)) S2(_net17, _net15);
Switch #(.init(on), .time(9ns), .Ron(0), .Roff(1e9), .Temp(26.85), .MaxDuration(1e-6), .Transition(spline)) S1(_net7, _net17);
// `ifdef QUCS_DIAGRAMS
// Rect
// Rect
// `endif
endmodule // main

paramset Sub ModelFactory;
endparmset //Sub

paramset Verilog ModelFactory;
endparmset //Verilog

module Symbol();
endmodule // Symbol

