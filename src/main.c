#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "screen.h"
#include "keyboard.h"
#include "timer.h"

#define SCREEN_WIDTH 80
#define SCREEN_HEIGHT 25
#define BAR_WIDTH 10
#define BALL_SYMBOL 'o'
#define BORDER_SYMBOL '-'
#define ALTURA 20
#define LARGURA 50

typedef struct {
    int x, y;
    int dirX, dirY;
} Bola;

typedef struct {
    int x;
} Barra;

typedef struct Bloco {
    int x, y;
    struct Bloco* prox;
} Bloco;

typedef struct {
    char** matriz;
} Tela;

Bola* bola;
Barra* barra;
Bloco* blocos;
Tela* tela;

void inicializar() {
    bola = (Bola*)malloc(sizeof(Bola));
    barra = (Barra*)malloc(sizeof(Barra));

    // Inicializa a barra
    barra->x = SCREEN_WIDTH / 2 - BAR_WIDTH / 2;

    // Inicializa a bola na posição inicial da barra
    bola->x = barra->x + BAR_WIDTH / 2;
    bola->y = SCREEN_HEIGHT - 3; // Posição acima da barra
    bola->dirX = 1;
    bola->dirY = -1; // Movimento para cima

    tela = (Tela*)malloc(sizeof(Tela));
    tela->matriz = (char**)malloc(ALTURA * sizeof(char*));
    for (int i = 0; i < ALTURA; i++) {
        tela->matriz[i] = (char*)malloc(LARGURA * sizeof(char));
    }

    // Inicializa a lista encadeada de blocos
    Bloco* atual = NULL;
    for (int i = 0; i < LARGURA; i++) {
        for (int j = 0; j < 3; j++) {
            Bloco* novoBloco = (Bloco*)malloc(sizeof(Bloco));
            novoBloco->x = i;
            novoBloco->y = j + 1;
            novoBloco->prox = NULL;
            if (atual == NULL) {
                blocos = novoBloco;
            } else {
                atual->prox = novoBloco;
            }
            atual = novoBloco;
        }
    }
}

void desenhar() {
    screenClear(); // Limpa a tela

    // Preenche a tela com espaços em branco
    for (int i = 0; i < ALTURA; i++) {
        for (int j = 0; j < LARGURA; j++) {
            if (i == 0 || i == ALTURA - 1 || j == 0 || j == LARGURA - 1) {
                tela->matriz[i][j] = BORDER_SYMBOL; // Define bordas ao redor da tela
            } else {
                tela->matriz[i][j] = ' ';
            }
        }
    }

    // Desenha a bola
    tela->matriz[bola->y][bola->x] = BALL_SYMBOL;

    // Desenha a barra
    for (int i = barra->x; i < barra->x + BAR_WIDTH; i++) {
        tela->matriz[SCREEN_HEIGHT - 2][i] = BORDER_SYMBOL;
    }

    // Desenha os blocos
    Bloco* atual = blocos;
    while (atual != NULL) {
        tela->matriz[atual->y][atual->x] = '@';
        atual = atual->prox;
    }

    // Desenha a tela
    for (int i = 0; i < ALTURA; i++) {
        for (int j = 0; j < LARGURA; j++) {
            screenGotoxy(j, i);
            printf("%c", tela->matriz[i][j]);
        }
    }
}

void atualizar() {
    // Move a bola
    bola->x += bola->dirX;
    bola->y += bola->dirY;

    // Bate nas paredes
    if (bola->x <= 0 || bola->x >= LARGURA - 1) {
        bola->dirX = -bola->dirX;
    }
    if (bola->y <= 0) {
        bola->dirY = -bola->dirY;
    }

    // Verifica se a bola caiu no chão (game over)
    if (bola->y >= ALTURA - 1) {
        screenGotoxy(30, 12);
        printf("Game Over\n");
        screenUpdate();
        exit(0);
    }

    // Verifica colisão com a barra
    if (bola->y == SCREEN_HEIGHT - 2 && bola->x >= barra->x && bola->x < barra->x + BAR_WIDTH) {
        bola->dirY = -bola->dirY;
    }

    // Verifica colisão com os blocos
    Bloco* atual = blocos;
    Bloco* anterior = NULL;
    while (atual != NULL) {
        if (bola->x == atual->x && bola->y == atual->y) {
            bola->dirY = -bola->dirY;
            if (anterior == NULL) {
                blocos = atual->prox;
            } else {
                anterior->prox = atual->prox;
            }
            free(atual);
            break;
        }
        anterior = atual;
        atual = atual->prox;
    }
}

void entrada() {
    // Verifica se alguma tecla foi pressionada
    if (keyhit()) {
        int ch = readch();

        // Move a barra
        if (ch == 'a' && barra->x > 0) {
            barra->x--;
        } else if (ch == 'd' && barra->x < SCREEN_WIDTH - BAR_WIDTH) {
            barra->x++;
        }
    }
}

int main() {
    screenInit(1);
    keyboardInit();
    timerInit(300);

    inicializar();
    while (1) {
        entrada();
        atualizar();
                desenhar();
                timerUpdateTimer(100); // Ajustando o valor para 100 ms
            }

            screenDestroy();
            keyboardDestroy();
            timerDestroy();

            // Liberação de memória
            free(bola);
            free(barra);
            for (int i = 0; i < ALTURA; i++) {
                free(tela->matriz[i]);
            }
            free(tela->matriz);
            free(tela);

            Bloco* atual = blocos;
            while (atual != NULL) {
                Bloco* temp = atual;
                atual = atual->prox;
                free(temp);
            }

            return 0;
        }
