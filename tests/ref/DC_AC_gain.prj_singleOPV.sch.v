module main(P1, P2, P3, P4, P6);
R #(.R(4k), .Temp(26.85), .Tc1(0.0), .Tc2(0.0), .Tnom(26.85), .Symbol(european)) R1(_net13, _net1);
C #(.C(4p), .V(), .Symbol(neutral)) C1(_net1, P6);
Port #(.Num(3), .Type()) P3(P3, P3);
Port #(.Num(4), .Type()) P4(P4, P4);
Port #(.Num(5), .Type()) P6(P6, P6);
Port #(.Num(1), .Type()) P1(P1, P1);
Port #(.Num(2), .Type()) P2(P2, P2);
Idc #(.I(5 uA)) I1(_net17, P3);
endmodule // main

module Symbol(_net0, _net1, _net4, _net3, _net2);
painting incomplete
painting incomplete
// .port_ x=-40 y=20 n=0 label=:port1
painting incomplete
painting incomplete
// .port_ x=-40 y=-20 n=1 label=:port2
painting incomplete
painting incomplete
painting incomplete
painting incomplete
painting incomplete
painting incomplete
painting incomplete
// .port_ x=60 y=0 n=4 label=:port5
painting incomplete
painting incomplete
// .port_ x=10 y=50 n=3 label=:port4
painting incomplete
painting incomplete
// .port_ x=10 y=-50 n=2 label=:port3
painting incomplete
painting incomplete
endmodule // Symbol

