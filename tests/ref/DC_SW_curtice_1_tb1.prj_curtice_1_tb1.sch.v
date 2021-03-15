module main();
Vdc #(.U(Vgs)) V2(_net7, gnd);
GND #() *(gnd);
//DC1
Eqn #(.Id(-V1.I), .Export(yes)) Eqn1();
GND #() *(gnd);
Vdc #(.U(Vds)) V1(_net2, gnd);
GND #() *(gnd);
IProbe #() Pr1(_net2, _net3);
Sub$curtice_1 #(.File(curtice_1.sch)) Curtice1(gnd, _net7, _net3);
endmodule // main

module :SymbolSection:();
painting incomplete
painting incomplete
painting incomplete
painting incomplete
painting incomplete
endmodule // :SymbolSection:

module Sub:curtice_1(_net2, _net0, _net1);
module :SymbolSection:(_net2, _net0, _net1);
// .port_ x=-30 y=0 n=1 label=:port2
painting incomplete
painting incomplete
painting incomplete
painting incomplete
painting incomplete
// parameter RG 1m
// parameter RD 1m
// parameter RS 1m
// parameter VBR 10e10
// parameter LG 1e-15
// parameter LD 1e-15
// parameter LS 1e-15
// parameter IS 10f
// parameter N 1
// parameter XTI 0
// parameter EG 1.11
// parameter Beta 3m
// parameter Lambda 40m
// parameter VT0 -6
// parameter Temp 27
// parameter Alpha 0.8
// parameter CDS 300f
// parameter TAU 10p
// parameter RIN 1m
// parameter CGS 300f
// parameter CGD 300f
// parameter Tnom 27
painting incomplete
// .port_ x=0 y=-40 n=2 label=:port3
painting incomplete
// .port_ x=0 y=40 n=0 label=:port1
painting incomplete
painting incomplete
painting incomplete
painting incomplete
endmodule // :SymbolSection:

module main(Source, Gate, Drain);
R #(.R(RS), .Temp(26.85), .Tc1(0.0), .Tc2(0.0), .Tnom(26.85), .Symbol(european)) Rs(_net3, _net24);
L #(.L(LS), .I()) Ls(Source, _net3);
Port #(.Num(1), .Type(analog)) Source(Source, Source);
R #(.R(RIN), .Temp(26.85), .Tc1(0.0), .Tc2(0.0), .Tnom(26.85), .Symbol(european)) Rin(_net26, _net24);
EDD #(.Type(explicit), .Branches(1), .I1(0), .Q1(CGD*V1)) D2(_net22, _net8);
Port #(.Num(3), .Type(analog)) Drain(Drain, Drain);
R #(.R(RD), .Temp(26.85), .Tc1(0.0), .Tc2(0.0), .Tnom(26.85), .Symbol(european)) Rd(_net14, _net8);
L #(.L(LD), .I()) Ld(Drain, _net14);
R #(.R(RG), .Temp(26.85), .Tc1(0.0), .Tc2(0.0), .Tnom(26.85), .Symbol(european)) Rg(_net22, _net17);
L #(.L(LG), .I()) Lg(_net17, Gate);
Port #(.Num(2), .Type(analog)) Gate(Gate, Gate);
Eqn #(.Vt(kB/q*TK), .GMIN(1e-12), .TK(Temp+273.15), .TnK(Tnom+273.15), .Export(yes)) Eqn1();
Eqn #(.TR(TK/TnK), .IsT(IS*exp(XTI/N*ln(TR) - EG/N/Vt*(1-TR))), .Export(yes)) Eqn2();
EDD #(.Type(explicit), .Branches(4), .I1(V1<-VBR+50*Vt ? -IsT*(1+exp(-(VBR+V1)/Vt)) + GMIN*V1 : 0), .Q1(0), .I2(V1>=-VBR+50*Vt && V1<-5*Vt ? -IsT+GMIN*V1 : V1>=-5*Vt ? IsT*(exp(V1/(N*Vt))-1) + GMIN*V1 : 0), .Q2(0), .I3(V1-VT0>0 ? Beta*(V1-VT0)^2*(1+Lambda*V3)*tanh(Alpha*V3) : 0), .Q3(CDS*V3 + TAU*I3), .I4(0), .Q4(CGS*V4)) D1(_net22, _net24, _net22, _net24, _net8, _net24, _net22, _net26);
endmodule // main

module :Paintings:();
endmodule // :Paintings:

endmodule // Sub:curtice_1

module :Paintings:();
endmodule // :Paintings:

