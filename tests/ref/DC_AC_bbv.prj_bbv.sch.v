module main();
R #(.R(12), .Temp(26.85), .Tc1(0.0), .Tc2(0.0), .Tnom(26.85), .Symbol(european)) RE1(gnd, _net6);
R #(.R(650), .Temp(26.85), .Tc1(0.0), .Tc2(0.0), .Tnom(26.85), .Symbol(european)) R1(_net2, _net15);
_BJT #(.Type(npn), .Is(2e-15), .Nf(1), .Nr(1), .Ikf(0.01), .Ikr(0.10), .Vaf(100), .Var(10), .Ise(0), .Ne(2.0), .Isc(0), .Nc(2.0), .Bf(300), .Br(5.67), .Rbm(0), .Irb(0), .Rc(0), .Re(0), .Rb(0), .Cje(0), .Vje(0.75), .Mje(0.33), .Cjc(0), .Vjc(0.75), .Mjc(0.33), .Xcjc(1.0), .Cjs(0), .Vjs(0.75), .Mjs(0), .Fc(0.5), .Tf(0.0), .Xtf(0.0), .Vtf(0.0), .Itf(0.0), .Tr(0.0), .Temp(26.85), .Kf(0.0), .Af(1.0), .Ffe(1.0), .Kb(0.0), .Ab(1.0), .Fb(1.0), .Ptf(0.0), .Xtb(0.0), .Xti(3.0), .Eg(1.11), .Tnom(26.85), .Area(1.0)) btgn1(_net8, _net2, _net6, _net2);
L #(.L(3 nH), .I()) L1(_net7, _net8);
R #(.R(140), .Temp(26.85), .Tc1(0.0), .Tc2(0.0), .Tnom(26.85), .Symbol(european)) RF1(_net13, _net6);
_BJT #(.Type(npn), .Is(1e-15), .Nf(1), .Nr(1), .Ikf(0.01), .Ikr(0.10), .Vaf(100), .Var(10), .Ise(0), .Ne(2.0), .Isc(0), .Nc(2.0), .Bf(300), .Br(5.67), .Rbm(0), .Irb(0), .Rc(0), .Re(0), .Rb(0), .Cje(0), .Vje(0.75), .Mje(0.33), .Cjc(0), .Vjc(0.75), .Mjc(0.33), .Xcjc(1.0), .Cjs(0), .Vjs(0.75), .Mjs(0), .Fc(0.5), .Tf(0.0), .Xtf(0.0), .Vtf(0.0), .Itf(0.0), .Tr(0.0), .Temp(26.85), .Kf(0.0), .Af(1.0), .Ffe(1.0), .Kb(0.0), .Ab(1.0), .Fb(1.0), .Ptf(0.0), .Xtb(0.0), .Xti(3.0), .Eg(1.11), .Tnom(26.85), .Area(1.0)) btgn4(_net43, _net43, _net13, _net43);
_BJT #(.Type(npn), .Is(1e-15), .Nf(1), .Nr(1), .Ikf(0.01), .Ikr(0.10), .Vaf(100), .Var(10), .Ise(0), .Ne(2.0), .Isc(0), .Nc(2.0), .Bf(300), .Br(5.67), .Rbm(0), .Irb(0), .Rc(0), .Re(0), .Rb(0), .Cje(0), .Vje(0.75), .Mje(0.33), .Cjc(0), .Vjc(0.75), .Mjc(0.33), .Xcjc(1.0), .Cjs(0), .Vjs(0.75), .Mjs(0), .Fc(0.5), .Tf(0.0), .Xtf(0.0), .Vtf(0.0), .Itf(0.0), .Tr(0.0), .Temp(26.85), .Kf(0.0), .Af(1.0), .Ffe(1.0), .Kb(0.0), .Ab(1.0), .Fb(1.0), .Ptf(0.0), .Xtb(0.0), .Xti(3.0), .Eg(1.11), .Tnom(26.85), .Area(1.0)) btgn3(_net22, _net15, _net43, _net15);
GND #() *(gnd);
R #(.R(200), .Temp(26.85), .Tc1(0.0), .Tc2(0.0), .Tnom(26.85), .Symbol(european)) RF2(_net24, _net8);
Eqn #(.Gain(abs(Out.v/In.v)), .Export(yes)) Eqn1();
GND #() *(gnd);
Vdc #(.U(6 V)) VCC(_net15, gnd);
GND #() *(gnd);
Vac #(.U(50u), .f(1 GHz), .Phase(0), .Theta(0)) V1(In, gnd);
//DC1
//AC1
R #(.R(140), .Temp(26.85), .Tc1(0.0), .Tc2(0.0), .Tnom(26.85), .Symbol(european)) R3(_net24, _net27);
_BJT #(.Type(npn), .Is(1e-15), .Nf(1), .Nr(1), .Ikf(0.01), .Ikr(0.10), .Vaf(100), .Var(10), .Ise(0), .Ne(2.0), .Isc(0), .Nc(2.0), .Bf(300), .Br(5.67), .Rbm(0), .Irb(0), .Rc(0), .Re(0), .Rb(0), .Cje(0), .Vje(0.75), .Mje(0.33), .Cjc(0), .Vjc(0.75), .Mjc(0.33), .Xcjc(1.0), .Cjs(0), .Vjs(0.75), .Mjs(0), .Fc(0.5), .Tf(0.0), .Xtf(0.0), .Vtf(0.0), .Itf(0.0), .Tr(0.0), .Temp(26.85), .Kf(0.0), .Af(1.0), .Ffe(1.0), .Kb(0.0), .Ab(1.0), .Fb(1.0), .Ptf(0.0), .Xtb(0.0), .Xti(3.0), .Eg(1.11), .Tnom(26.85), .Area(1.0)) btgn6(_net2, _net22, _net27, _net22);
R #(.R(12), .Temp(26.85), .Tc1(0.0), .Tc2(0.0), .Tnom(26.85), .Symbol(european)) RE2(_net28, _net24);
_BJT #(.Type(npn), .Is(1e-15), .Nf(1), .Nr(1), .Ikf(0.01), .Ikr(0.10), .Vaf(100), .Var(10), .Ise(0), .Ne(2.0), .Isc(0), .Nc(2.0), .Bf(300), .Br(5.67), .Rbm(0), .Irb(0), .Rc(0), .Re(0), .Rb(0), .Cje(0), .Vje(0.75), .Mje(0.33), .Cjc(0), .Vjc(0.75), .Mjc(0.33), .Xcjc(1.0), .Cjs(0), .Vjs(0.75), .Mjs(0), .Fc(0.5), .Tf(0.0), .Xtf(0.0), .Vtf(0.0), .Itf(0.0), .Tr(0.0), .Temp(26.85), .Kf(0.0), .Af(1.0), .Ffe(1.0), .Kb(0.0), .Ab(1.0), .Fb(1.0), .Ptf(0.0), .Xtb(0.0), .Xti(3.0), .Eg(1.11), .Tnom(26.85), .Area(1.0)) btgn5(_net28, _net28, gnd, _net28);
_BJT #(.Type(npn), .Is(1e-15), .Nf(1), .Nr(1), .Ikf(0.01), .Ikr(0.10), .Vaf(100), .Var(10), .Ise(0), .Ne(2.0), .Isc(0), .Nc(2.0), .Bf(300), .Br(5.67), .Rbm(0), .Irb(0), .Rc(0), .Re(0), .Rb(0), .Cje(0), .Vje(0.75), .Mje(0.33), .Cjc(0), .Vjc(0.75), .Mjc(0.33), .Xcjc(1.0), .Cjs(0), .Vjs(0.75), .Mjs(0), .Fc(0.5), .Tf(0.0), .Xtf(0.0), .Vtf(0.0), .Itf(0.0), .Tr(0.0), .Temp(26.85), .Kf(0.0), .Af(1.0), .Ffe(1.0), .Kb(0.0), .Ab(1.0), .Fb(1.0), .Ptf(0.0), .Xtb(0.0), .Xti(3.0), .Eg(1.11), .Tnom(26.85), .Area(1.0)) btgn2(_net27, _net22, _net24, _net22);
R #(.R(225), .Temp(26.85), .Tc1(0.0), .Tc2(0.0), .Tnom(26.85), .Symbol(european)) R2(_net22, _net15);
GND #() *(gnd);
R #(.R(10), .Temp(26.85), .Tc1(0.0), .Tc2(0.0), .Tnom(26.85), .Symbol(european)) R0(_net38, _net22);
L #(.L(3 nH), .I()) L2(_net38, Out);
GND #() *(gnd);
R #(.R(1k), .Temp(26.85), .Tc1(0.0), .Tc2(0.0), .Tnom(26.85), .Symbol(european)) RL(gnd, _net39);
endmodule // main

paramset Sub ModelFactory;
endparmset //Sub

paramset Verilog ModelFactory;
endparmset //Verilog

module Symbol();
endmodule // Symbol

