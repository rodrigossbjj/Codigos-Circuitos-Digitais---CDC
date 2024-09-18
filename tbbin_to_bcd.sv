module testbench;
    localparam int VECTOR_SIZE = 256;  // Número de vetores de teste (0 a 255)

    logic clk, rst, start;
    logic [7:0] bin;          // Entrada binária
    logic ready, done_tick;   // Sinais de controle
    logic [11:0] bcd;         // Saída BCD
    logic [7:0] test_vectors_in [0:VECTOR_SIZE-1];   // Vetores de entrada (binário)
    logic [11:0] test_vectors_out [0:VECTOR_SIZE-1]; // Vetores de saída esperados (BCD)
    int errors;
    int i;

    // Instanciação do módulo bin_to_bcd
    bin_to_bcd uut (
      .clk(clk),
      .rst(rst),
      .start(start),
      .bin(bin),
      .ready(ready),
      .done_tick(done_tick),
      .bcd(bcd)
    );

    // Geração do clock (período de 10 unidades de tempo)
    always #5 clk = ~clk;

    initial begin
        $dumpfile("waveform.vcd");
        $dumpvars(0, testbench);

        // Inicialização dos sinais
        clk = 0;
        rst = 1;
        start = 0;
        errors = 0;

        // Carregar vetores de teste dos arquivos
        $readmemb("entrada.txt", test_vectors_in);   // Arquivo de entrada binária
        $readmemb("saida.txt", test_vectors_out);    // Arquivo de saída esperada (BCD)

        // Desativar o reset após 10 unidades de tempo
        #10 rst = 0;

        // Loop para testar todos os valores de 0 a 255
        for (i = 0; i < VECTOR_SIZE; i++) begin
            // Aguardar até que o módulo esteja pronto
            while (!ready) @(posedge clk);
            
            // Iniciar a conversão
            bin = test_vectors_in[i];
            start = 1;
            @(posedge clk); // Pulso de start
            start = 0;

            // Aguardar a conclusão da conversão
            wait (done_tick == 1);

            // Verificação do resultado
            if (bcd !== test_vectors_out[i]) begin
                $display("Erro na conversão: Entrada = %b, Saída = %b, Esperado = %b", bin, bcd, test_vectors_out[i]);
                errors++;
            end
        end

        // Relatório de erros
        if (errors == 0) begin
            $display("Teste concluído com sucesso, sem erros.");
        end else begin
            $display("Teste concluído com %0d erro(s).", errors);
        end

        // Finalizar a simulação
        #50 $finish;
    end

endmodule
