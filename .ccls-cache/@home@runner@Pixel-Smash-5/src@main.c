#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "screen.h"
#include "keyboard.h"
#include "timer.h"

#define LARGURA_BORDA (MAXX - MINX + 1)
#define ALTURA_BORDA (MAXY - MINY + 1)
#define LARGURA_BARRA 15
#define SIMBOLO_BOLA 'o'
#define SIMBOLO_ALVO 'X'
#define ARQUIVO_PONTUACAO "melhor_pontuacao.txt"

typedef struct NoAlvo {
    int x, y;
    struct NoAlvo *proximo;
} NoAlvo;

typedef struct {
    int x, y;
    int largura;
} Barra;

typedef struct {
    int x, y;
    int velocidadeX, velocidadeY;
} Bola;

typedef struct {
    int x, y;
    int largura, altura;
} Alvo;

int contagemAlvos = 0;
int melhorPontuacao = 0;

Barra *barra;
Bola *bola;
Alvo *alvo;
NoAlvo *cabecaAlvosDestruidos = NULL;
int bolaEmMovimento = 0;

void adicionarAlvoNaLista(int x, int y) {
    NoAlvo *novoNo = (NoAlvo *)malloc(sizeof(NoAlvo));
    if (novoNo == NULL) {
        fprintf(stderr, "Erro ao alocar memória para o novo nó da lista encadeada.\n");
        exit(EXIT_FAILURE);
    }
    novoNo->x = x;
    novoNo->y = y;
    novoNo->proximo = cabecaAlvosDestruidos;
    cabecaAlvosDestruidos = novoNo;
}

void gerarPosicaoAleatoriaAlvo() {
    alvo->x = rand() % (LARGURA_BORDA - alvo->largura - 4) + 2;
    alvo->y = rand() % (ALTURA_BORDA / 2 - alvo->altura - 3) + 2;
}

void lerMelhorPontuacao() {
    FILE *arquivo = fopen(ARQUIVO_PONTUACAO, "r");
    if (arquivo != NULL) {
        fscanf(arquivo, "%d", &melhorPontuacao);
        fclose(arquivo);
    }
}

void escreverMelhorPontuacao() {
    FILE *arquivo = fopen(ARQUIVO_PONTUACAO, "w");
    if (arquivo != NULL) {
        fprintf(arquivo, "%d", melhorPontuacao);
        fclose(arquivo);
    }
}

void desenharBordaTela() {
    screenSetColor(WHITE, BLACK);
    screenDrawBorders();
}

void desenharBarra() {
    screenSetColor(CYAN, BLACK);
    int barraXajustada = barra->x;
    if (barra->x + barra->largura >= LARGURA_BORDA - 2) {
        barraXajustada = LARGURA_BORDA - barra->largura - 2;
    }
    for (int i = barra->y - 1; i < barra->y; i++) {
        screenSetColor(WHITE, BLACK);
        screenGotoxy(barraXajustada, i);
        for (int j = 0; j < barra->largura; j++)
            printf("=");
    }
}

void desenharBola() {
    screenSetColor(YELLOW, DARKGRAY);
    screenGotoxy(bola->x, bola->y);
    printf("%c", SIMBOLO_BOLA);
}

void desenharAlvo() {
    screenSetColor(RED, BLACK);
    for (int i = alvo->y; i < alvo->y + alvo->altura; i++) {
        screenGotoxy(alvo->x, i);
        for (int j = 0; j < alvo->largura; j++)
            printf("%c", SIMBOLO_ALVO);
    }
}

void moverBarra(char direcao) {
    if (direcao == 'a' && barra->x > 1)
        barra->x--;
    else if (direcao == 'd' && barra->x < LARGURA_BORDA - barra->largura - 2)
        barra->x++;
}

void moverBola() {
    if (bolaEmMovimento) {
        bola->x += bola->velocidadeX;
        bola->y += bola->velocidadeY;

        if (bola->x <= 1 || bola->x >= LARGURA_BORDA - 2 - 1) {
            bola->velocidadeX = -bola->velocidadeX;
        }

        if (bola->y <= 1) {
            bola->velocidadeY = -bola->velocidadeY;
        }

        if (bola->y >= alvo->y && bola->y < alvo->y + alvo->altura &&
            bola->x >= alvo->x && bola->x < alvo->x + alvo->largura) {
            contagemAlvos++;
            adicionarAlvoNaLista(alvo->x, alvo->y);
            gerarPosicaoAleatoriaAlvo();
        }
    }
}

void lidarColisao() {
    if (bola->y == barra->y - 1 && bola->x >= barra->x && bola->x <= barra->x + barra->largura) {
        int pontoColisao = bola->x - barra->x;
        if (pontoColisao < barra->largura / 2)
            bola->velocidadeX = -1;
        else if (pontoColisao > barra->largura / 2)
            bola->velocidadeX = 1;
        else
            bola->velocidadeX = 0;
        bola->velocidadeY = -bola->velocidadeY;
    }
}

int main() {
    screenInit(1);
    keyboardInit();
    timerInit(75);
    srand(time(NULL));

    lerMelhorPontuacao();

    barra = (Barra *)malloc(sizeof(Barra));
    bola = (Bola *)malloc(sizeof(Bola));
    alvo = (Alvo *)malloc(sizeof(Alvo));

    barra->x = LARGURA_BORDA / 2 - LARGURA_BARRA / 2;
    barra->y = ALTURA_BORDA - 4;
    barra->largura = LARGURA_BARRA;

    bola->x = LARGURA_BORDA / 2;
    bola->y = ALTURA_BORDA / 2;
    bola->velocidadeX = 0;
    bola->velocidadeY = 1;

    alvo->largura = 6;
    alvo->altura = 4;

    desenharBordaTela();
    desenharBarra();
    desenharBola();
    gerarPosicaoAleatoriaAlvo();
    desenharAlvo();

    screenGotoxy(5, ALTURA_BORDA - 1);
    screenSetColor(WHITE, BLACK);
    printf("Pontuação: ");
    screenSetColor(YELLOW, BLACK);
    printf("%d", contagemAlvos);
    screenSetColor(WHITE, BLACK);
    screenGotoxy(LARGURA_BORDA - 15, ALTURA_BORDA - 1);
    printf("Melhor Pontuação: ");
    screenSetColor(YELLOW, BLACK);
    printf("%d", melhorPontuacao);

    screenUpdate();

    int mensagemExibida = 1;

    while (1) {
        if (mensagemExibida) {
            screenGotoxy(LARGURA_BORDA / 2 - 8, ALTURA_BORDA / 2);
            screenSetColor(WHITE, BLACK);
            printf("Pressione A ou D");
            screenUpdate();
        }

        if (keyhit()) {
            char tecla = readch();
            if (!bolaEmMovimento && (tecla == 'a' || tecla == 'd')) {
                bolaEmMovimento = 1;
                mensagemExibida = 0;
                // Limpa a mensagem "Pressione A ou D" ao iniciar o movimento
                screenGotoxy(LARGURA_BORDA / 2 - 8, ALTURA_BORDA / 2);
                printf("               ");
                screenUpdate();
            }
            moverBarra(tecla);
        }

        if (timerTimeOver() == 1) {
            moverBola();
            lidarColisao();

            screenClear();
            desenharBordaTela();
            desenharBarra();
            desenharBola();
            desenharAlvo();
            NoAlvo *atual = cabecaAlvosDestruidos;
            while (atual != NULL) {
                desenharAlvo(atual->x, atual->y);
                atual = atual->proximo;
            }

            screenGotoxy(5, ALTURA_BORDA - 1);
            screenSetColor(WHITE, BLACK);
            printf("Pontuação: ");
            screenSetColor(WHITE, BLACK);
            printf("%d", contagemAlvos);
            screenGotoxy(LARGURA_BORDA - 15, ALTURA_BORDA - 1);
            screenSetColor(WHITE, BLACK);
            printf("Melhor Pontuação: ");
            screenSetColor(WHITE, BLACK);
            printf("%d", melhorPontuacao);

            screenUpdate();

            if (bola->y >= ALTURA_BORDA - 2) {
                screenSetColor(YELLOW, DARKGRAY);
                screenGotoxy(LARGURA_BORDA / 2 - 4, ALTURA_BORDA / 2);
                printf("Game Over!");

                if (contagemAlvos > melhorPontuacao) {
                    melhorPontuacao = contagemAlvos;
                    escreverMelhorPontuacao();
                }

                screenUpdate();
                timerDestroy();
                exit(0);
            }
        }
    }

    while (cabecaAlvosDestruidos != NULL) {
        NoAlvo *temp = cabecaAlvosDestruidos;
        cabecaAlvosDestruidos = cabecaAlvosDestruidos->proximo;
        free(temp);
    }

    free(barra);
    free(bola);
    free(alvo);

    keyboardDestroy();
    screenDestroy();
    timerDestroy();

    return 0;
}
