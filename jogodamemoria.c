#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"

// --- Mapeamento de Hardware ---
#define LED_RED 14      // LED Vermelho no GPIO 14
#define LED_BLUE 10     // LED Azul no GPIO 10
#define LED_GREEN 12    // LED Verde no GPIO 12
#define LED_YELLOW 8    // LED Amarelo no GPIO 8
#define BUTTON_RED 15   // Botão Vermelho no GPIO 15
#define BUTTON_BLUE 11  // Botão Azul no GPIO 11
#define BUTTON_GREEN 13 // Botão Verde no GPIO 13
#define BUTTON_YELLOW 9 // Botão Amarelo no GPIO 9
#define PIEZO 16        // Buzzer no GPIO 16

// --- Frequências das notas ---
#define NOTE_C4 262  // Nota Dó  262Hz
#define NOTE_E4 330  // Nota Mi  330Hz
#define NOTE_G4 392  // Nota Sol 392Hz
#define NOTE_A4 440  // Nota Lá  440Hz

// --- Constantes ---
#define MAX_MEMORY 20 // Tamanho máximo da sequência de memória

// --- Variáveis globais ---
uint8_t memory[MAX_MEMORY]; // Vetor que armazena a sequência do jogo
uint8_t count = 0;          // Contador de botões pressionados
int level = 1;              // Nível do jogo
bool wait = false;          // Variável para aguardar entrada do jogador

// --- Protótipos ---
void output_red();
void output_blue();
void output_green();
void output_yellow();
void memory_clear();
void start();
void game_over();
void complete();
void check(uint8_t number);

// --- Funções Auxiliares ---
void setup() {
    // Configura os LEDs como saída
    gpio_init(LED_RED); gpio_set_dir(LED_RED, GPIO_OUT);
    gpio_init(LED_BLUE); gpio_set_dir(LED_BLUE, GPIO_OUT);
    gpio_init(LED_GREEN); gpio_set_dir(LED_GREEN, GPIO_OUT);
    gpio_init(LED_YELLOW); gpio_set_dir(LED_YELLOW, GPIO_OUT);

    // Configura os botões como entrada com pull-up interno
    gpio_init(BUTTON_RED); gpio_set_dir(BUTTON_RED, GPIO_IN); //gpio_pull_up(BUTTON_RED);
    gpio_init(BUTTON_BLUE); gpio_set_dir(BUTTON_BLUE, GPIO_IN); //gpio_pull_up(BUTTON_BLUE);
    gpio_init(BUTTON_GREEN); gpio_set_dir(BUTTON_GREEN, GPIO_IN); //gpio_pull_up(BUTTON_GREEN);
    gpio_init(BUTTON_YELLOW); gpio_set_dir(BUTTON_YELLOW, GPIO_IN); //gpio_pull_up(BUTTON_YELLOW);

    // Configura o buzzer como saída
    gpio_init(PIEZO); gpio_set_dir(PIEZO, GPIO_OUT);

    memory_clear(); // Limpa a memória
}

// Função para tocar o buzzer com uma frequência específica
void play_tone(uint16_t frequency, uint16_t duration) {
    gpio_set_function(PIEZO, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(PIEZO);  // Obtém o slice correspondente ao pino
    pwm_set_wrap(slice_num, 125000000 / frequency - 1);  // Ajuste para a frequência
    pwm_set_chan_level(slice_num, PWM_CHAN_A, (125000000 / frequency) / 2);  // 50% de ciclo
    pwm_set_enabled(slice_num, true);  // Habilita o PWM
    sleep_ms(duration);  // Duração do tom
    pwm_set_enabled(slice_num, false);  // Desabilita o PWM após a duração
}

// Saídas correspondentes aos LEDs e sons
void output_red() {
    gpio_put(LED_RED, 1);
    play_tone(NOTE_C4, 300);
    gpio_put(LED_RED, 0);
    sleep_ms(300);
}

void output_blue() {
    gpio_put(LED_BLUE, 1);
    play_tone(NOTE_E4, 300);
    gpio_put(LED_BLUE, 0);
    sleep_ms(300);
}

void output_green() {
    gpio_put(LED_GREEN, 1);
    play_tone(NOTE_G4, 300);
    gpio_put(LED_GREEN, 0);
    sleep_ms(300);
}

void output_yellow() {
    gpio_put(LED_YELLOW, 1);
    play_tone(NOTE_A4, 300);
    gpio_put(LED_YELLOW, 0);
    sleep_ms(300);
}

// Limpa a memória
void memory_clear() {
    for (int i = 0; i < MAX_MEMORY; i++) {
        memory[i] = 0;
    }
}

// Função para sinalizar o início do jogo
void start() {
    for (int i = 0; i < 5; i++) {
        gpio_put(LED_RED, 1); sleep_ms(100); gpio_put(LED_RED, 0);
        gpio_put(LED_BLUE, 1); sleep_ms(100); gpio_put(LED_BLUE, 0);
        gpio_put(LED_GREEN, 1); sleep_ms(100); gpio_put(LED_GREEN, 0);
        gpio_put(LED_YELLOW, 1); sleep_ms(100); gpio_put(LED_YELLOW, 0);
    }
    sleep_ms(500);
}

// Função de Game Over
void game_over() {
    // Indica o Game Over piscando todos os LEDs
    for (int i = 0; i < 5; i++) {
        gpio_put(LED_RED, 1);
        gpio_put(LED_BLUE, 1);
        gpio_put(LED_GREEN, 1);
        gpio_put(LED_YELLOW, 1);
        sleep_ms(100);
        gpio_put(LED_RED, 0);
        gpio_put(LED_BLUE, 0);
        gpio_put(LED_GREEN, 0);
        gpio_put(LED_YELLOW, 0);
        sleep_ms(100);
    }

    // Reinicia o jogo
    level = 1;
    count = 0;
    wait = false;
    memory_clear();
    sleep_ms(1000); // Pequena pausa antes de reiniciar
}

// Indica conclusão do nível
void complete() {
    if (level == count) {
        wait = false;
        count = 0;
        memory_clear();
        level++;
    }
}

void check(uint8_t number) {
    // Exibe o LED correspondente ao botão pressionado
    switch (number) {
        case 1: output_red(); break;
        case 2: output_blue(); break;
        case 3: output_green(); break;
        case 4: output_yellow(); break;
    }

    // Verifica se o botão pressionado está correto
    if (memory[count] == number) {
        // Botão correto, avança para o próximo da sequência
        count++;

        // Se o jogador completou o nível
        if (count == level) {
            wait = false; // Sai do loop de espera
            count = 0;    // Reseta o contador para o próximo nível
            level++;      // Avança para o próximo nível
        }
    } else {
        // Botão errado, sinaliza erro
        for (int i = 0; i < 3; i++) { // Pisca todos os LEDs para indicar erro
            gpio_put(LED_RED, 1);
            gpio_put(LED_BLUE, 1);
            gpio_put(LED_GREEN, 1);
            gpio_put(LED_YELLOW, 1);
            sleep_ms(200);
            gpio_put(LED_RED, 0);
            gpio_put(LED_BLUE, 0);
            gpio_put(LED_GREEN, 0);
            gpio_put(LED_YELLOW, 0);
            sleep_ms(200);
        }

        // Reinicia o jogo
        game_over();
    }
}


void wait_for_correct_button(uint8_t expected_button) {
    bool button_pressed = false;

    while (!button_pressed) {
        // Verifica o botão pressionado e acende o LED correspondente
        if ((expected_button == 1 && gpio_get(BUTTON_RED)) ||  // Verifica o botão vermelho
            (expected_button == 2 && gpio_get(BUTTON_BLUE)) || // Verifica o botão azul
            (expected_button == 3 && gpio_get(BUTTON_GREEN)) || // Verifica o botão verde
            (expected_button == 4 && gpio_get(BUTTON_YELLOW))) { // Verifica o botão amarelo
            button_pressed = true; // O botão correto foi pressionado

            // Acende o LED correspondente ao botão pressionado
            switch (expected_button) {
                case 1: gpio_put(LED_RED, 1); break;
                case 2: gpio_put(LED_BLUE, 1); break;
                case 3: gpio_put(LED_GREEN, 1); break;
                case 4: gpio_put(LED_YELLOW, 1); break;
            }

            // Espera um momento para manter o LED aceso antes de desligá-lo
            sleep_ms(300);

            // Apaga o LED correspondente ao botão pressionado
            switch (expected_button) {
                case 1: gpio_put(LED_RED, 0); break;
                case 2: gpio_put(LED_BLUE, 0); break;
                case 3: gpio_put(LED_GREEN, 0); break;
                case 4: gpio_put(LED_YELLOW, 0); break;
            }
        }
    }
}


// Loop principal
void loop() {
    if (!wait) {
        // Começa um novo nível
        start();

        // Gera a sequência até o nível atual
        for (int i = 0; i < level; i++) {
            uint8_t rand_value = (rand() % 4) + 1;
            memory[i] = rand_value;

            // Mostra o LED correspondente na sequência
            switch (rand_value) {
                case 1: output_red(); break;
                case 2: output_blue(); break;
                case 3: output_green(); break;
                case 4: output_yellow(); break;
            }
        }

        wait = true; // Habilita a espera pela interação do jogador
    }

    // Aguarda o jogador pressionar os botões
    while (wait) {
        if (gpio_get(BUTTON_RED)) {
            check(1);
            sleep_ms(300); // Evita múltiplos registros rápidos
        } else if (gpio_get(BUTTON_BLUE)) {
            check(2);
            sleep_ms(300);
        } else if (gpio_get(BUTTON_GREEN)) {
            check(3);
            sleep_ms(300);
        } else if (gpio_get(BUTTON_YELLOW)) {
            check(4);
            sleep_ms(300);
        }
    }
}


int main() {
    stdio_init_all();
    setup();

    while (true) {
        loop();
    }
}
