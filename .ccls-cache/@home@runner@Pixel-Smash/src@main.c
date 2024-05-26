#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h> // Incluindo a biblioteca time.h para utilizar a função srand()

#include "screen.h"
#include "keyboard.h"
#include "timer.h"

#define BORDER_WIDTH (MAXX - MINX + 1)
#define BORDER_HEIGHT (MAXY - MINY + 1)
#define BAR_WIDTH 15
#define BALL_SYMBOL 'o'
#define TARGET_SYMBOL 'X'
#define SCORE_FILE "best_score.txt" // Nome do arquivo para armazenar a melhor pontuação

// Declaração da variável global para contar os alvos destruídos
int targetCount = 0;
int bestScore = 0; // Variável para armazenar a melhor pontuação

int barX = BORDER_WIDTH / 2 - BAR_WIDTH / 2, barY = BORDER_HEIGHT - 4; // Posição inicial da barra
int ballX = BORDER_WIDTH / 2, ballY = BORDER_HEIGHT / 2; // Posição inicial da bola
int ballSpeedX = 1, ballSpeedY = -1; // Velocidade da bola
int targetX = BORDER_WIDTH / 2, targetY = 5; // Posição inicial do alvo
int targetWidth = 4, targetHeight = 2; // Tamanho do alvo

// Função para gerar uma posição aleatória para o alvo
void generateRandomTargetPosition() {
    targetX = rand() % (BORDER_WIDTH - targetWidth - 2) + 1;
    targetY = rand() % (BORDER_HEIGHT / 2 - targetHeight) + 1; // Garante que o alvo seja gerado dentro da tela
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
    int adjustedBarX = barX;
    // Verifica se a posição final da barra ultrapassa a borda direita
    if (barX + BAR_WIDTH >= BORDER_WIDTH - 2) {
        adjustedBarX = BORDER_WIDTH - BAR_WIDTH - 2;
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
    else if (direction == 'd' && barX < BORDER_WIDTH - BAR_WIDTH - 2)
        barX++;
}

void moveBall() {
    ballX += ballSpeedX;
    ballY += ballSpeedY;

    // Verifica colisão com a parede esquerda ou direita
    if (ballX <= 1 || ballX >= BORDER_WIDTH - 2 - 1) {
        ballSpeedX = -ballSpeedX; // Reflete a bola na parede
    }

    // Verifica colisão com o teto
    if (ballY <= 1) {
        ballSpeedY = -ballSpeedY;
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

    readBestScore(); // Lê a melhor pontuação do arquivo

    drawScreenBorder();
    drawBar();
    drawBall();
    generateRandomTargetPosition(); // Gera a posição aleatória do primeiro alvo
    drawTarget(); // Desenha o alvo inicial

    // Mostra o placar dentro dos limites da tela
    screenGotoxy(5, BORDER_HEIGHT - 1); // Posição do placar
    printf("Score: %d", targetCount); // Atualiza o placar

    // Mostra a melhor pontuação na tela
    screenGotoxy(BORDER_WIDTH - 15, BORDER_HEIGHT - 1); // Posição da melhor pontuação
    printf("Best Score: %d", bestScore);

    screenUpdate();

    while (1) // Loop infinito
    {
        // Handle user input
        if (keyhit()) 
        {
            char ch = readch();
            moveBar(ch);
        }

        // Update game state (move elements, verify collision,
        if (timerTimeOver() == 1)
                {
                    moveBall();
                    handleCollision();

                    screenClear(); // Limpa a tela antes de redesenhar
                    drawScreenBorder();
                    drawBar();
                    drawBall();
                    drawTarget(); // Redesenha o alvo

                    // Atualiza e exibe o placar dentro dos limites da tela
                    screenGotoxy(5, BORDER_HEIGHT - 1);
                    printf("Score: %d", targetCount);
                    screenGotoxy(BORDER_WIDTH - 15, BORDER_HEIGHT - 1);
                    printf("Best Score: %d", bestScore);

                    screenUpdate();

                    // Verifica colisão com o chão após desenhar a bola na próxima posição
                    if (ballY >= BORDER_HEIGHT - 2) {
                        screenSetColor(YELLOW, DARKGRAY);
                        screenGotoxy(BORDER_WIDTH / 2 - 4, BORDER_HEIGHT / 2);
                        printf("Game Over!");

                        // Atualiza a melhor pontuação se necessário
                        if (targetCount > bestScore) {
                            bestScore = targetCount;
                            writeBestScore(); // Grava a melhor pontuação no arquivo
                        }

                        screenUpdate();
                        timerDestroy();
                        exit(0);
                    }
                }
            }

            keyboardDestroy();
            screenDestroy();
            timerDestroy();

            return 0;
        }