module main();
Vrect #(.U(1V), .TH(Ton), .TL(Toff), .Tr(1 ns), .Tf(1 ns), .Td(0 ns)) V2(ctrl, gnd);
GND #() *(gnd);
Vdc #(.U(12V)) V1(_net1, gnd);
GND #() *(gnd);
Relais #(.Vt(0.5 V), .Vh(0.1 V), .Ron(1), .Roff(1e12), .Temp(26.85)) S1(ctrl, _net1, dio, gnd);
GND #() *(gnd);
Diode #(.Is(1e-12 A), .N(1), .Cj0(10 fF), .M(0.5), .Vj(0.7 V), .Fc(0.5), .Cp(0.0 fF), .Isr(0.0), .Nr(2.0), .Rs(0.0 Ohm), .Tt(0.0 ps), .Ikf(0), .Kf(0.0), .Af(1.0), .Ffe(1.0), .Bv(0), .Ibv(1 mA), .Temp(26.85), .Xti(3.0), .Eg(1.11), .Tbv(0.0), .Trs(0.0), .Ttt1(0.0), .Ttt2(0.0), .Tm1(0.0), .Tm2(0.0), .Tnom(26.85), .Area(1.0), .Symbol(normal)) D1(dio, gnd);
L #(.L(47uH), .I(0)) L1(dio, out);
C #(.C(100u), .V(0), .Symbol(neutral)) C1(out, gnd);
R #(.R(5), .Temp(26.85), .Tc1(0.0), .Tc2(0.0), .Tnom(26.85), .Symbol(european)) R1(gnd, out);
Eqn #(.Tmax(Bperiod*20), .Tstep(Bperiod/1000), .Export(yes)) Eqn2();
//TR1
Eqn #(.Bfreq(40k), .Bperiod(1/Bfreq), .Bduty(50), .Ton(Bperiod*Bduty/100), .Toff(Bperiod-Ton), .Export(yes)) Eqn1();
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

