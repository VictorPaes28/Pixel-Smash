#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h> // Incluindo a biblioteca time.h para utilizar a função srand()

#include "screen.h"
#include "keyboard.h"
#include "timer.h"

#define SCREEN_WIDTH 60
#define SCREEN_HEIGHT 30
#define BAR_WIDTH 10
#define BALL_SYMBOL 'o'
#define TARGET_SYMBOL 'X'
#define BORDER_SYMBOL '-'

// Declaração da variável global para contar os alvos destruídos
int targetCount = 0;

int barX = SCREEN_WIDTH / 2 - BAR_WIDTH / 2, barY = SCREEN_HEIGHT / 2 + 5; // Posição inicial da barra
int ballX = SCREEN_WIDTH / 2, ballY = SCREEN_HEIGHT / 2; // Posição inicial da bola
int ballSpeedX = 1, ballSpeedY = -1; // Velocidade da bola
int targetX = SCREEN_WIDTH / 2, targetY = 5; // Posição inicial do alvo
int targetWidth = 5, targetHeight = 1; // Tamanho do alvo

// Função para gerar uma posição aleatória para o alvo
void generateRandomTargetPosition() {
    targetX = rand() % (SCREEN_WIDTH - targetWidth - 2) + 1;
    targetY = rand() % (SCREEN_HEIGHT / 2 - targetHeight) + 1; // Garante que o alvo seja gerado dentro da tela
}

void drawScreenBorder() {
    // Desenha a borda superior
    screenSetColor(CYAN, DARKGRAY);
    for (int i = 0; i < SCREEN_WIDTH; i++) {
        screenGotoxy(i, 0);
        printf("%c", BORDER_SYMBOL);
    }

    // Desenha a borda inferior
    for (int i = 0; i < SCREEN_WIDTH; i++) {
        screenGotoxy(i, SCREEN_HEIGHT - 1);
        printf("%c", BORDER_SYMBOL);
    }

    // Desenha a borda esquerda e direita
    for (int i = 1; i < SCREEN_HEIGHT - 1; i++) {
        screenGotoxy(0, i);
        printf("|");
        screenGotoxy(SCREEN_WIDTH - 1, i);
        printf("|");
    }
}

void drawBar() {
    screenSetColor(CYAN, DARKGRAY);
    int adjustedBarX = barX;
    // Verifica se a posição final da barra ultrapassa a borda direita
    if (barX + BAR_WIDTH >= SCREEN_WIDTH - 2) {
        adjustedBarX = SCREEN_WIDTH - BAR_WIDTH - 2;
    }
    for (int i = barY - 1; i < barY; i++) {
        screenGotoxy(adjustedBarX, i);
        for (int j = 0; j < BAR_WIDTH; j++)
            printf("="); // Desenha a barra
    }
}

void drawBall() {
    screenSetColor(YELLOW, DARKGRAY);
    screenGotoxy(ballX, ballY);
    printf("%c", BALL_SYMBOL); // Desenha a bola
}

void drawTarget() {
    screenSetColor(RED, DARKGRAY);
    for (int i = targetY; i < targetY + targetHeight; i++) {
        screenGotoxy(targetX, i);
        for (int j = 0; j < targetWidth; j++)
            printf("%c", TARGET_SYMBOL); // Desenha o alvo
    }
}

void moveBar(char direction) {
    if (direction == 'a' && barX > 1)
        barX--;
    else if (direction == 'd' && barX < SCREEN_WIDTH - BAR_WIDTH - 2)
        barX++;
}

void moveBall() {
    ballX += ballSpeedX;
    ballY += ballSpeedY;

    // Verifica colisão com a parede esquerda ou direita
    if (ballX <= 1 || ballX >= SCREEN_WIDTH - 2 - 1) {
        ballSpeedX = -ballSpeedX; // Reflete a bola na parede
    }

    // Verifica colisão com o teto
    if (ballY <= 1) {
        ballSpeedY = -ballSpeedY;
    }

    // Verifica colisão com o chão
    if (ballY >= SCREEN_HEIGHT - 2) {
        screenSetColor(YELLOW, DARKGRAY);
        screenGotoxy(25, 12);
        printf("Game Over!");
        screenUpdate();
        timerDestroy();
        exit(0);
    }

    // Verifica colisão com o alvo
    if (ballY == targetY + targetHeight - 1 && ballX >= targetX && ballX <= targetX + targetWidth) {
        targetCount++; // Incrementa o contador de alvos destruídos
        generateRandomTargetPosition(); // Gera nova posição para o alvo
    }
}

void handleCollision() {
    // Colisão com a barra
    if (ballY == barY - 1 && ballX >= barX && ballX <= barX + BAR_WIDTH) {
        // Determina onde a bola atingiu a barra
        int collisionPoint = ballX - barX;

        // Reflete a bola
        if (collisionPoint < BAR_WIDTH / 2)
            ballSpeedX = -1; // Reflete para a esquerda
        else if (collisionPoint > BAR_WIDTH / 2)
            ballSpeedX  = 1; // Reflete para a direita
                        else
                            ballSpeedX = 0; // Bola atingiu o centro da barra, mantém a mesma direção horizontal
                        ballSpeedY = -ballSpeedY;
                    }
                }

                int main() {
                    screenInit(1);
                    keyboardInit();
                    timerInit(50);
                    srand(time(NULL)); // Inicializa a semente para gerar números aleatórios

                    drawScreenBorder();
                    drawBar();
                    drawBall();
                    generateRandomTargetPosition(); // Gera a posição aleatória do primeiro alvo
                    drawTarget(); // Desenha o alvo inicial

                    // Mostra o placar fora dos limites da tela
                    screenGotoxy(0, SCREEN_HEIGHT);
                    printf("Score: %d", targetCount);

                    screenUpdate();

                    while (1) // Loop infinito
                    {
                        // Handle user input
                        if (keyhit()) 
                        {
                            char ch = readch();
                            moveBar(ch);
                        }

                        // Update game state (move elements, verify collision, etc)
                        if (timerTimeOver() == 1)
                        {
                            moveBall();
                            handleCollision();

                            screenClear(); // Limpa a tela antes de redesenhar
                            drawScreenBorder();
                            drawBar();
                            drawBall();
                            drawTarget(); // Redesenha o alvo
                            screenGotoxy(0, SCREEN_HEIGHT); // Move o cursor para a posição do placar
                            printf("Score: %d", targetCount); // Atualiza o placar
                            screenUpdate();
                        }
                    }

                    keyboardDestroy();
                    screenDestroy();
                    timerDestroy();

                    return 0;
                }

