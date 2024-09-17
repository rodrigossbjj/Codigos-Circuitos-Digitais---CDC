module SomadorSerial(
  input logic A,B,clk,rst,
  output logic S,Cout
);

logic carry;

somadorCompleto somacom(
    .a(A),
    .b(B),
    .cin(carry),
    .s(S),
    .cout(Cout)
);

always_ff @(posedge clk or posedge rst)
  begin
    if(rst)
      begin
carry<= 1'b0;
      end
    else
      begin
        
        carry<=Cout;
      end
  end
  
endmodule

module somadorCompleto(
    input logic a,b,cin,
    output logic s,cout 
);
  assign s = a^b^cin;
  assign cout = (b&cin) | (a&b) | (a&cin);

endmodule