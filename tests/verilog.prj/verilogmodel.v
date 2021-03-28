
module something ( X, Y, Z );

   // Inputs and outputs
   input [2:0] in;
   output [7:0] out;

   // Data types
   reg [7:0] out;

   integer n;
   always @ (in) begin
      for (n = 0; n < 8; n = n+1) begin
         if (in == n) begin
            out[n] = 1;
         end
         else begin
            out[n] = 0;
         end
      end
   end

endmodule
