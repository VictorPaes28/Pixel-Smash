#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "screen.h"
#include "keyboard.h"
#include "timer.h"

#define BORDER_WIDTH (MAXX - MINX + 1)
#define BORDER_HEIGHT (MAXY - MINY + 1)
#define BAR_WIDTH 15
#define BALL_SYMBOL 'o'
#define TARGET_SYMBOL 'X'
#define SCORE_FILE "best_score.txt"

// Struct para representar um nó da lista encadeada de alvos destruídos
typedef struct TargetNode {
    int x, y; // Posição do alvo
    struct TargetNode *next; // Ponteiro para o próximo nó da lista
} TargetNode;

// Struct para representar a barra
typedef struct {
    int x, y; // Posição da barra
    int width; // Largura da barra
} Bar;

// Struct para representar a bola
typedef struct {
    int x, y; // Posição da bola
    int speedX, speedY; // Velocidade da bola
} Ball;

// Struct para representar os alvos
typedef struct {
    int x, y; // Posição do alvo
    int width, height; // Tamanho do alvo
} Target;

int targetCount = 0;
int bestScore = 0;

Bar *bar;
Ball *ball;
Target *target;

// Cabeça da lista encadeada de alvos destruídos
TargetNode *destroyedTargetsHead = NULL;

// Função para adicionar um alvo destruído à lista encadeada
void addTargetToList(int x, int y) {
    TargetNode *newNode = (TargetNode *)malloc(sizeof(TargetNode));
    if (newNode == NULL) {
        // Tratamento de erro, se a alocação de memória falhar
        fprintf(stderr, "Erro ao alocar memória para o novo nó da lista encadeada.\n");
        exit(EXIT_FAILURE);
    }
    newNode->x = x;
    newNode->y = y;
    newNode->next = destroyedTargetsHead;
    destroyedTargetsHead = newNode;
}

// Função para gerar uma posição aleatória para o alvo
void generateRandomTargetPosition() {
    target->x = rand() % (BORDER_WIDTH - target->width - 2) + 1;
    target->y = rand() % (BORDER_HEIGHT / 2 - target->height) + 1;
}

// Função para ler a melhor pontuação do arquivo
void readBestScore() {
    FILE *file = fopen(SCORE_FILE, "r");
    if (file != NULL) {
        fscanf(file, "%d", &bestScore);
        fclose(file);
    }
}

// Função para gravar a melhor pontuação no arquivo
void writeBestScore() {
    FILE *file = fopen(SCORE_FILE, "w");
    if (file != NULL) {
        fprintf(file, "%d", bestScore);
        fclose(file);
    }
}

void drawScreenBorder() {
    screenDrawBorders();
}

void drawBar() {
    screenSetColor(CYAN, DARKGRAY);
    int adjustedBarX = bar->x;
    if (bar->x + bar->width >= BORDER_WIDTH - 2) {
        adjustedBarX = BORDER_WIDTH - bar->width - 2;
    }
    for (int i = bar->y - 1; i < bar->y; i++) {
        screenGotoxy(adjustedBarX, i);
        for (int j = 0; j < bar->width; j++)
            printf("="); // Desenha a barra
    }
}

void drawBall() {
    screenSetColor(YELLOW, DARKGRAY);
    screenGotoxy(ball->x, ball->y);
    printf("%c", BALL_SYMBOL); // Desenha a bola
}

void drawTarget() {
    screenSetColor(RED, DARKGRAY);
    for (int i = target->y; i < target->y + target->height; i++) {
        screenGotoxy(target->x, i);
        for (int j = 0; j < target->width; j++)
            printf("%c", TARGET_SYMBOL); // Desenha o alvo
    }
}

void moveBar(char direction) {
    if (direction == 'a' && bar->x > 1)
        bar->x--;
    else if (direction == 'd' && bar->x < BORDER_WIDTH - bar->width - 2)
        bar->x++;
}

void moveBall() {
    ball->x += ball->speedX;
    ball->y += ball->speedY;

    if (ball->x <= 1 || ball->x >= BORDER_WIDTH - 2 - 1) {
        ball->speedX = -ball->speedX;
    }

    if (ball->y <= 1) {
        ball->speedY = -ball->speedY;
    }

    if (ball->y == target->y + target->height - 1 && ball->x >= target->x && ball->x <= target->x + target->width) {
        targetCount++;
        addTargetToList(target->x, target->y); // Adiciona o alvo à lista encadeada
        generateRandomTargetPosition();
    }
}

void handleCollision() {
    if (ball->y == bar->y - 1 && ball->x >= bar->x && ball->x <= bar->x + bar->width) {
        int collisionPoint = ball->x - bar->x;
        if (collisionPoint < bar->width / 2)
            ball->speedX = -1;
        else if (collisionPoint > bar->width / 2)
            ball->speedX  = 1;
        else
            ball->speedX = 0;
        ball->speedY = -ball->speedY;
    }
}

int main() {
    screenInit(1);
    keyboardInit();
    timerInit(50);
    srand(time(NULL));

    readBestScore();

    bar = (Bar *)malloc(sizeof(Bar));
    ball = (Ball *)malloc(sizeof(Ball));
    target = (Target *)malloc(sizeof(Target));

    bar->x = BORDER_WIDTH / 2 - BAR_WIDTH / 2;
    bar->y = BORDER_HEIGHT - 4;
    bar->width = BAR_WIDTH;

    ball->x = BORDER_WIDTH / 2;
    ball->y = BORDER_HEIGHT / 2;
    ball->speedX = 1;
    ball->speedY = -1;

    target->width = 4;
    target->height = 2;

    drawScreenBorder();
    drawBar();
    drawBall();
    generateRandomTargetPosition();
    drawTarget();

    screenGotoxy(5, BORDER_HEIGHT - 1);
    printf("Score: %d", targetCount);
    screenGotoxy(BORDER_WIDTH - 15, BORDER_HEIGHT - 1);
    printf("Best Score: %d", bestScore);

    screenUpdate();

    while (1) {
        if (keyhit()) {
            char ch = readch();
            moveBar(ch);
        }

        if (timerTimeOver() == 1) {
            moveBall();
            handleCollision();

            screenClear();
            drawScreenBorder();
            drawBar();
            drawBall();
            drawTarget();

            // Atualiza a lista encadeada de alvos destruídos
            TargetNode *current = destroyedTargetsHead;
            while (current != NULL) {
                drawTarget(current->x, current->y);
                current = current->next;
            }

            screenGotoxy(5, BORDER_HEIGHT - 1);
            printf("Score: %d", targetCount);
            screenGotoxy(BORDER_WIDTH - 15, BORDER_HEIGHT - 1);
            printf("Best Score: %d", bestScore);

            screenUpdate();

            if (ball->y >= BORDER_HEIGHT - 2) {
                screenSetColor(YELLOW, DARKGRAY);
                screenGotoxy(BORDER_WIDTH / 2 - 4, BORDER_HEIGHT / 2);
                printf("Game Over!");

                if (targetCount > bestScore) {
                    bestScore = targetCount;
                    writeBestScore();
                }

                screenUpdate();
                timerDestroy();
                exit(0);
            }
        }
        }

        // Libera a memória alocada para a lista encadeada
        while (destroyedTargetsHead != NULL) {
        TargetNode *temp = destroyedTargetsHead;
        destroyedTargetsHead = destroyedTargetsHead->next;
        free(temp);
        }

        free(bar);
        free(ball);
        free(target);

        keyboardDestroy();
        screenDestroy();
        timerDestroy();

        return 0;
        }
