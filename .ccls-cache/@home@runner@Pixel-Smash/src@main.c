#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "screen.h"
#include "keyboard.h"
#include "timer.h"

#define WIDTH_BORDER (MAXX - MINX + 1)
#define HEIGHT_BORDER (MAXY - MINY + 1)
#define WIDTH_BAR 15
#define SYMBOL_BALL 'o'
#define SYMBOL_TARGET 'X'
#define SCORE_FILE "best_score.txt"

typedef struct TargetNode {
    int x, y;
    struct TargetNode *next;
} TargetNode;

typedef struct {
    int x, y;
    int width;
} Bar;

typedef struct {
    int x, y;
    int speedX, speedY;
} Ball;

typedef struct {
    int x, y;
    int width, height;
} Target;

int targetCount = 0;
int bestScore = 0;

Bar *bar;
Ball *ball;
Target *target;
TargetNode *headDestroyedTargets = NULL;
int ballInMotion = 0;

void addTargetToList(int x, int y) {
    TargetNode *newNode = (TargetNode *)malloc(sizeof(TargetNode));
    if (newNode == NULL) {
        fprintf(stderr, "Error allocating memory for the new node in the linked list.\n");
        exit(EXIT_FAILURE);
    }
    newNode->x = x;
    newNode->y = y;
    newNode->next = headDestroyedTargets;
    headDestroyedTargets = newNode;
}

void generateRandomTargetPosition() {
    target->x = rand() % (WIDTH_BORDER - target->width - 4) + 2;
    target->y = rand() % (HEIGHT_BORDER / 2 - target->height - 3) + 2;
}

void readBestScore() {
    FILE *file = fopen(SCORE_FILE, "r");
    if (file != NULL) {
        fscanf(file, "%d", &bestScore);
        fclose(file);
    }
}

void writeBestScore() {
    FILE *file = fopen(SCORE_FILE, "w");
    if (file != NULL) {
        fprintf(file, "%d", bestScore);
        fclose(file);
    }
}

void drawScreenBorder() {
    screenSetColor(WHITE, BLACK);
    screenDrawBorders();
}

void drawBar() {
    screenSetColor(CYAN, BLACK);
    int adjustedBarX = bar->x;
    if (bar->x + bar->width >= WIDTH_BORDER - 2) {
        adjustedBarX = WIDTH_BORDER - bar->width - 2;
    }
    for (int i = bar->y - 1; i < bar->y; i++) {
        screenSetColor(WHITE, BLACK);
        screenGotoxy(adjustedBarX, i);
        for (int j = 0; j < bar->width; j++)
            printf("=");
    }
}

void drawBall() {
    screenSetColor(YELLOW, DARKGRAY);
    screenGotoxy(ball->x, ball->y);
    printf("%c", SYMBOL_BALL);
}

void drawTarget() {
    screenSetColor(RED, BLACK);
    for (int i = target->y; i < target->y + target->height; i++) {
        screenGotoxy(target->x, i);
        for (int j = 0; j < target->width; j++)
            printf("%c", SYMBOL_TARGET);
    }
}

void moveBar(char direction) {
    if (direction == 'a' && bar->x > 1)
        bar->x--;
    else if (direction == 'd' && bar->x < WIDTH_BORDER - bar->width - 2)
        bar->x++;
}

void moveBall() {
    if (ballInMotion) {
        ball->x += ball->speedX;
        ball->y += ball->speedY;

        if (ball->x <= 1 || ball->x >= WIDTH_BORDER - 2 - 1) {
            ball->speedX = -ball->speedX;
        }

        if (ball->y <= 1) {
            ball->speedY = -ball->speedY;
        }

        if (ball->y >= target->y && ball->y < target->y + target->height &&
            ball->x >= target->x && ball->x < target->x + target->width) {
            targetCount++;
            addTargetToList(target->x, target->y);
            generateRandomTargetPosition();
        }
    }
}

void handleCollision() {
    if (ball->y == bar->y - 1 && ball->x >= bar->x && ball->x <= bar->x + bar->width) {
        int collisionPoint = ball->x - bar->x;
        if (collisionPoint < bar->width / 2)
            ball->speedX = -1;
        else if (collisionPoint > bar->width / 2)
            ball->speedX = 1;
        else
            ball->speedX = 0;
        ball->speedY = -ball->speedY;
    }
}

int main() {
    screenInit(1);
    keyboardInit();
    timerInit(75);
    srand(time(NULL));

    readBestScore();

    bar = (Bar *)malloc(sizeof(Bar));
    ball = (Ball *)malloc(sizeof(Ball));
    target = (Target *)malloc(sizeof(Target));

    bar->x = WIDTH_BORDER / 2 - WIDTH_BAR / 2;
    bar->y = HEIGHT_BORDER - 4;
    bar->width = WIDTH_BAR;

    ball->x = WIDTH_BORDER / 2;
    ball->y = HEIGHT_BORDER / 2;
    ball->speedX = 0;
    ball->speedY = 1;

    target->width = 6;
    target->height = 4;

    drawScreenBorder();
    drawBar();
    drawBall();
    generateRandomTargetPosition();
    drawTarget();

    screenGotoxy(5, HEIGHT_BORDER - 1);
    screenSetColor(WHITE, BLACK);
    printf("Score: ");
    screenSetColor(YELLOW, BLACK);
    printf("%d", targetCount);
    screenSetColor(WHITE, BLACK);
    screenGotoxy(WIDTH_BORDER - 15, HEIGHT_BORDER - 1);
    printf("Best Score: ");
    screenSetColor(YELLOW, BLACK);
    printf("%d", bestScore);

    screenUpdate();

    int messageDisplayed = 1;

    while (1) {
        if (messageDisplayed) {
            screenGotoxy(WIDTH_BORDER / 2 - 8, HEIGHT_BORDER / 2);
            screenSetColor(WHITE, BLACK);
            printf("Press A or D");
            screenUpdate();
        }

        if (keyhit()) {
            char key = readch();
            if (!ballInMotion && (key == 'a' || key == 'd')) {
                ballInMotion = 1;
                messageDisplayed = 0;

                screenGotoxy(WIDTH_BORDER / 2 - 8, HEIGHT_BORDER / 2);
                printf("               ");
                screenUpdate();
            }
            moveBar(key);
        }

        if (timerTimeOver() == 1) {
            moveBall();
            handleCollision();

            screenClear();
            drawScreenBorder();
            drawBar();
            drawBall();
            drawTarget();
            TargetNode *current = headDestroyedTargets;
            while (current != NULL) {
                drawTarget(current->x, current->y);
                current = current->next;
            }

            screenGotoxy(5, HEIGHT_BORDER - 1);
            screenSetColor(WHITE, BLACK);
            printf("Score: ");
            screenSetColor(WHITE, BLACK);
            printf("%d", targetCount);
            screenGotoxy(WIDTH_BORDER - 15, HEIGHT_BORDER - 1);
            screenSetColor(WHITE, BLACK);
            printf("Best Score: ");
            screenSetColor(WHITE, BLACK);
            printf("%d", bestScore);

            screenUpdate();

            if (ball->y >= HEIGHT_BORDER - 2) {
                screenSetColor(YELLOW, DARKGRAY);
                screenGotoxy(WIDTH_BORDER / 2 - 4, HEIGHT_BORDER / 2);
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

    while (headDestroyedTargets != NULL) {
        TargetNode *temp = headDestroyedTargets;
        headDestroyedTargets = headDestroyedTargets->next;
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
