module main(P1, P2, P3, P4, P5);
sTr #(.T1(0.5), .T2(0.5)) Tr1(P4, P1, P5, P3, P2, P2);
Port #(.Num(3), .Type(analog)) P3(P3, P3);
Port #(.Num(4), .Type(analog)) P4(P4, P4);
Port #(.Num(5), .Type(analog)) P5(P5, P5);
Port #(.Num(2), .Type(analog)) P2(P2, P2);
Port #(.Num(1), .Type(analog)) P1(P1, P1);
endmodule // main

paramset Sub ModelFactory;
endparmset //Sub

paramset Verilog ModelFactory;
endparmset //Verilog

module Symbol();
endmodule // Symbol

