# Qucs 0.1.0  ../../tests/DC_SP_LPF-Balun2.prj/LPF-Balun2.sch

L #(.L(1.2uH), .I()) L1(_net40, _net41);
L #(.L(470nH), .I()) L2(_net41, _net3);
C #(.C(270pF), .V(), .Symbol(neutral)) C1(_net4, _net40);
C #(.C(270pF), .V(), .Symbol(neutral)) C2(_net6, _net41);
L #(.L(470nH), .I()) L3(_net44, _net40);
L #(.L(470nH), .I()) L4(_net45, _net4);
L #(.L(1.2uH), .I()) L5(_net4, _net6);
L #(.L(470nH), .I()) L6(_net6, _net52);
GND #() *(_net47);
Pac #(.Num(1), .Z(100 Ohm), .P(0 dBm), .f(1 GHz), .Temp(26.85)) P1(_net46, _net18);
GND #() *(_net18);
GND #() *(_net49);
IdealBalun #(.File(IdealBalun.sch)) Balun1(_net46, _net22, _net23, _net24, _net25);
IdealBalun #(.File(IdealBalun.sch)) Balun2(_net26, _net27, _net28, _net29, _net30);
GND #() *(_net33);
Vdc #(.U(1V)) V1(_net32, _net33);
VProbe #() Pr1(_net58, _net35);
GND #() *(_net35);
Pac #(.Num(2), .Z(100 Ohm), .P(0 dBm), .f(1 GHz), .Temp(26.85)) P2(_net27, _net38);
GND #() *(_net38);
Eqn #(.dBS21(dB(S[2,1])), .dBS11(dB(S[1,1])), .Export(yes)) Eqn1();
