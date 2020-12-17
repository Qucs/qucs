module main(_net10, _net5, _net3, _net0, _net6);
sTr #(.T1(0.5), .T2(0.5)) Tr1(_net0, _net10, _net6, _net3, _net5, _net5);
Port #(.Num(3), .Type(analog)) P3(_net3);
Port #(.Num(4), .Type(analog)) P4(_net0);
Port #(.Num(5), .Type(analog)) P5(_net6);
Port #(.Num(2), .Type(analog)) P2(_net5);
Port #(.Num(1), .Type(analog)) P1(_net10);
endmodule // main

Sub #(.File()) Sub(_net0);
