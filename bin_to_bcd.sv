module bin_to_bcd (
  input logic clk, rst, start,
  input logic [7:0] bin,          // Entrada binária de 8 bits
  output logic ready, done_tick,  // Sinais de controle
  output logic [11:0] bcd         // Saída BCD de 12 bits (3 dígitos)
);

  typedef enum {idle, shift, adjust, done} state_type;
  state_type state_reg, state_next;

  logic [7:0] bin_reg, bin_next;  // Registrador para armazenar a entrada binária
  logic [11:0] bcd_reg, bcd_next; // Registrador para armazenar a saída BCD
  logic [3:0] count_reg, count_next; // Contador para acompanhar as iterações

  // Registradores da FSM
  always_ff @(posedge clk or posedge rst) begin
    if (rst) begin
      state_reg <= idle;
      bin_reg <= 0;
      bcd_reg <= 0;
      count_reg <= 0;
    end else begin
      state_reg <= state_next;
      bin_reg <= bin_next;
      bcd_reg <= bcd_next;
      count_reg <= count_next;
    end
  end

  // Lógica do próximo estado
  always_comb begin
    // Inicializa valores
    state_next = state_reg;
    ready = 1'b0;
    done_tick = 1'b0;
    bin_next = bin_reg;
    bcd_next = bcd_reg;
    count_next = count_reg;

    case (state_reg)
      idle: begin
        ready = 1'b1;
        if (start) begin
          bin_next = bin;         // Carrega o valor binário de entrada
          bcd_next = 12'd0;       // Zera o valor BCD
          count_next = 4'd8;      // Configura para 8 ciclos (8 bits da entrada binária)
          state_next = shift;     // Próximo estado: deslocar e ajustar
        end
      end

      shift: begin
        if (count_reg == 0) begin
          state_next = done;  // Termina a conversão quando os bits foram processados
        end
        else if (count_reg == 1) begin
          // Desloca o BCD e adiciona o bit mais significativo do binário
          bcd_next = {bcd_reg[10:0], bin_reg[7]}; 
          bin_next = bin_reg << 1;                // Desloca o binário à esquerda
          count_next = count_reg - 1;             // Decrementa o contador
        end
        else begin
          // Desloca o BCD e adiciona o bit mais significativo do binário
          bcd_next = {bcd_reg[10:0], bin_reg[7]}; 
          bin_next = bin_reg << 1;                // Desloca o binário à esquerda
          count_next = count_reg - 1;             // Decrementa o contador
          state_next = adjust;                    // Próximo estado: ajustar os dígitos BCD
        end
      end

      adjust: begin
        // Ajusta os dígitos do BCD se forem maiores que 4, para garantir a conversão correta
        if (bcd_reg[3:0] > 4)
          bcd_next[3:0] = bcd_reg[3:0] + 3;
        if (bcd_reg[7:4] > 4)
          bcd_next[7:4] = bcd_reg[7:4] + 3;
        if (bcd_reg[11:8] > 4)
          bcd_next[11:8] = bcd_reg[11:8] + 3;

        state_next = shift;  // Volta para o estado de deslocamento
      end

      done: begin
        done_tick = 1'b1; // Sinaliza que a conversão terminou
        state_next = idle;
      end

      default: state_next = idle;
    endcase
  end

  assign bcd = bcd_reg; // Atribui a saída BCD

endmodule
