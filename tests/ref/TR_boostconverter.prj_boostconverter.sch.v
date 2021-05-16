module main();
L #(.L(47uH), .I(0)) L1(_net4, dio);
Vdc #(.U(12V)) V2(_net4, gnd);
GND #() *(gnd);
Eqn #(.Tmax(Bperiod*20), .Tstep(Bperiod/1000), .Export(yes)) Eqn1();
//TR1
Eqn #(.Bfreq(40k), .Bperiod(1/Bfreq), .Bduty(50), .Ton(Bperiod*Bduty/100), .Toff(Bperiod-Ton), .Export(yes)) Eqn2();
Relais #(.Vt(0.5 V), .Vh(0.1 V), .Ron(1), .Roff(1e12), .Temp(26.85)) S1(ctrl, dio, gnd, gnd);
Vrect #(.U(1V), .TH(Ton), .TL(Toff), .Tr(1 ns), .Tf(1 ns), .Td(0 ns)) V1(ctrl, gnd);
GND #() *(gnd);
Diode #(.Is(1e-12 A), .N(1), .Cj0(10 fF), .M(0.5), .Vj(0.7 V), .Fc(0.5), .Cp(0.0 fF), .Isr(0.0), .Nr(2.0), .Rs(0.0 Ohm), .Tt(0.0 ps), .Ikf(0), .Kf(0.0), .Af(1.0), .Ffe(1.0), .Bv(0), .Ibv(1 mA), .Temp(26.85), .Xti(3.0), .Eg(1.11), .Tbv(0.0), .Trs(0.0), .Ttt1(0.0), .Ttt2(0.0), .Tm1(0.0), .Tm2(0.0), .Tnom(26.85), .Area(1.0), .Symbol(normal)) D1(out, dio);
GND #() *(gnd);
C #(.C(100u), .V(0), .Symbol(neutral)) C1(out, gnd);
R #(.R(5), .Temp(26.85), .Tc1(0.0), .Tc2(0.0), .Tnom(26.85), .Symbol(european)) R1(gnd, out);
// `ifdef QUCS_DIAGRAMS
// Rect
// Rect
// `endif
endmodule // main

module Symbol();
endmodule // Symbol

