module RegisterBank (
    input logic clock, wr_en, reset,
    input logic [1:0] add_rd0, add_rd1,add_wr,    
    input logic [7:0] wr_data,    
    output logic [7:0] rd0,       
    output logic [7:0] rd1        
);

  logic [7:0] registradores [3:0];

    always_ff @(posedge clock or posedge reset) begin
      if (reset == 1'b1) begin
            registradores[0] <= 8'b0;
            registradores[1] <= 8'b0;
            registradores[2] <= 8'b0;
            registradores[3] <= 8'b0;
        end 
      else if (wr_en == 1'b0) 
        begin
            registradores[add_wr] <= wr_data;
        end
    end

    assign rd0 = registradores[add_rd0];
    assign rd1 = registradores[add_rd1];

endmodule
