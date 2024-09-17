module lfsr_16bit(
    input  logic clk,rst,en, //Entrada
    output logic [15:0] q  //Saída de 16 bits
);

logic [15:0] lfsr_reg; //Registrador de 16 bits
logic feedback; //Variável que vai armazenar a XOR
assign feedback = lfsr_reg[0] ^ lfsr_reg[7] ^ lfsr_reg[15]; //XOR com bits 0, 7 e 15
                       
always_ff @(posedge clk,posedge rst) 
begin
    if (rst==1) //reset = 1 o estado atual recebe 1
        lfsr_reg <= 16'b1;  //Não recebe 0 pois o LSFR utiliza do estado atual para o próximo valor
    else if (en==1)
        lfsr_reg <= {lfsr_reg[30:0], feedback}; //Desloca os bits a esquerda adicionando o feedback LSB
end

assign q = lfsr_reg; //Saída q recebe estado atual
endmodule