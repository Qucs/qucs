module main(_net6, _net7, _net16, _net10, _net20);
R #(.R(4k), .Temp(26.85), .Tc1(0.0), .Tc2(0.0), .Tnom(26.85), .Symbol(european)) R1(_net13, _net1);
C #(.C(4p), .V(), .Symbol(neutral)) C1(_net1, _net20);
Port #(.Num(3), .Type()) P3(_net16);
Port #(.Num(4), .Type()) P4(_net10);
Port #(.Num(5), .Type()) P6(_net20);
Port #(.Num(1), .Type()) P1(_net6);
Port #(.Num(2), .Type()) P2(_net7);
Idc #(.I(5 uA)) I1(_net17, _net16);
endmodule // main

Sub #(.File()) Sub(_net0);
