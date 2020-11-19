// Qucs

GND #() *(gnd);
Vdc #(.U(15 V)) V1(_net11, gnd);
IProbe #() Collector(output, _net25);
R #(.R(20 Ohm), .Temp(26.85), .Tc1(0.0), .Tc2(0.0), .Tnom(26.85), .Symbol(european)) R2(output, _net11);
Vac #(.U(1 V), .f(1 GHz), .Phase(0), .Theta(0)) V2(input, output);
Diode #(.Is(1e-15 A), .N(1), .Cj0(10 pF), .M(0.5), .Vj(0.7 V), .Fc(0.5), .Cp(0.0 fF), .Isr(0.0), .Nr(2.0), .Rs(0.0 Ohm), .Tt(0.0 ps), .Ikf(0), .Kf(0.0), .Af(1.0), .Ffe(1.0), .Bv(0), .Ibv(1 mA), .Temp(26.85), .Xti(3.0), .Eg(1.11), .Tbv(0.0), .Trs(0.0), .Ttt1(0.0), .Ttt2(0.0), .Tm1(0.0), .Tm2(0.0), .Tnom(26.85), .Area(1.0), .Symbol(normal)) D2(_net8, _net11);
Diode #(.Is(1e-15 A), .N(1), .Cj0(10 pF), .M(0.5), .Vj(0.7 V), .Fc(0.5), .Cp(0.0 fF), .Isr(0.0), .Nr(2.0), .Rs(0.0 Ohm), .Tt(0.0 ps), .Ikf(0), .Kf(0.0), .Af(1.0), .Ffe(1.0), .Bv(0), .Ibv(1 mA), .Temp(26.85), .Xti(3.0), .Eg(1.11), .Tbv(0.0), .Trs(0.0), .Ttt1(0.0), .Ttt2(0.0), .Tm1(0.0), .Tm2(0.0), .Tnom(26.85), .Area(1.0), .Symbol(normal)) D1(_net10, _net8);
_BJT #(.Type(pnp), .Is(1e-16), .Nf(1), .Nr(1), .Ikf(0), .Ikr(0), .Vaf(0), .Var(0), .Ise(0), .Ne(1.5), .Isc(0), .Nc(2), .Bf(300), .Br(1), .Rbm(0), .Irb(0), .Rc(0), .Re(0), .Rb(0), .Cje(30 pF), .Vje(0.75), .Mje(0.33), .Cjc(0), .Vjc(0.75), .Mjc(0.33), .Xcjc(1.0), .Cjs(0), .Vjs(0.75), .Mjs(0), .Fc(0.5), .Tf(0.0), .Xtf(0.0), .Vtf(0.0), .Itf(0.0), .Tr(0.0), .Temp(26.85), .Kf(0.0), .Af(1.0), .Ffe(1.0), .Kb(0.0), .Ab(1.0), .Fb(1.0), .Ptf(0.0), .Xtb(0.0), .Xti(3.0), .Eg(1.11), .Tnom(26.85), .Area(1.0)) T2(_net10, _net12, input, _net12);
GND #() *(gnd);
C #(.C(1 nF), .V(), .Symbol(neutral)) C4(_net24, _net16);
GND #() *(gnd);
GND #() *(gnd);
GND #() *(gnd);
C #(.C(1 nF), .V(), .Symbol(neutral)) C3(_net22, _net27);
L #(.L(100 nH), .I()) L1(_net22, _net25);
Pac #(.Num(1), .Z(50 Ohm), .P(0), .f(1 GHz), .Temp(26.85)) P1(_net24, gnd);
Pac #(.Num(2), .Z(50 Ohm), .P(0), .f(1 GHz), .Temp(26.85)) P2(_net27, gnd);
_BJT #(.Type(npn), .Is(1e-16), .Nf(1), .Nr(1), .Ikf(0), .Ikr(0), .Vaf(0), .Var(0), .Ise(0), .Ne(1.5), .Isc(0), .Nc(2), .Bf(150), .Br(1), .Rbm(0), .Irb(0), .Rc(0), .Re(0), .Rb(0), .Cje(10 pF), .Vje(0.75), .Mje(0.33), .Cjc(0), .Vjc(0.75), .Mjc(0.33), .Xcjc(1.0), .Cjs(0), .Vjs(0.75), .Mjs(0), .Fc(0.5), .Tf(0.0), .Xtf(0.0), .Vtf(0.0), .Itf(0.0), .Tr(0.0), .Temp(26.85), .Kf(0.0), .Af(1.0), .Ffe(1.0), .Kb(0.0), .Ab(1.0), .Fb(1.0), .Ptf(0.0), .Xtb(0.0), .Xti(3.0), .Eg(1.11), .Tnom(26.85), .Area(1.0)) T1(_net26, _net22, gnd, _net22);
IProbe #() Basis(_net16, _net26);
L #(.L(100 nH), .I()) L2(_net16, _net30);
GND #() *(gnd);
R #(.R(10 Ohm), .Temp(26.85), .Tc1(0.0), .Tc2(0.0), .Tnom(26.85), .Symbol(european)) R3(_net30, _net12);
GND #() *(gnd);
R #(.R(20 kOhm), .Temp(26.85), .Tc1(0.0), .Tc2(0.0), .Tnom(26.85), .Symbol(european)) R1(gnd, _net10);
Eqn #(.LoopGain(output.v / input.v), .LoopPhase(phase(output.v / input.v)), .Export(yes)) Eqn1();
C #(.C(0.1 uF), .V(), .Symbol(neutral)) C1(_net25, gnd);
C #(.C(5 uF), .V(), .Symbol(neutral)) C2(_net12, gnd);
