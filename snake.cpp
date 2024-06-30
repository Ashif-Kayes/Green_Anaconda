#include <SDL.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <math.h>
#include<ctype.h>// Required for mathematical operations

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
#define GRID_SIZE 10
#define INITIAL_SPEED 150  // Initial speed in milliseconds
#define MAX_OBSTACLES 50
typedef struct {
    int x, y;
} Point;

typedef struct {
    Point body[100];
    int length;
    int dx, dy;
} Snake;

typedef struct {
    Point pos;
} Obstacle;

typedef enum {
    START,
    RUNNING,
    GAME_OVER,
    QUIT_OR_CONTINUE
} GameState;
typedef enum {
    EASY,
    MEDIUM,
    HARD
} Difficulty;
Difficulty gameDifficulty = EASY;

Snake snake;
Point food;
Obstacle obstacles[MAX_OBSTACLES];
int score = 0;
int running = 1;
int speed = INITIAL_SPEED;  // Initialize speed
int numObstacles = 20;  // Number of obstacles
GameState gameState = START;
SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;

// Function prototypes
void generateFood();
void initObstacles();
int initSDL();
void closeSDL();
void initGame();
void handleInput(SDL_Event e);
void moveSnake();
void drawDigit(int digit, int posX, int posY);
void renderScore(int score, int posX, int posY);
void drawCharacter(char c, int posX, int posY);
void renderStartScreen1(int posX, int posY);
void renderStartScreen2(int posX, int posY);
void renderStartScreen3(int posX, int posY);
void renderEasyScreen(int posX, int posY);
void renderMediumScreen(int posX, int posY);
void renderHardScreen(int posX, int posY);
void renderLevel(int posX, int posY);
void renderGameOver(int posX, int posY);
void renderFinalScore(int score, int posX, int posY);
void renderQuitOrContinueScreen(int posX, int posY);
void render();
void waitForMilliseconds(int milliseconds);
void setGameParameters(Difficulty difficulty);

void generateFood() {
    // Generate food within the red box borders
    int minX = GRID_SIZE;
    int maxX = SCREEN_WIDTH - 2 * GRID_SIZE - GRID_SIZE;
    int minY = GRID_SIZE + 50;
    int maxY = SCREEN_HEIGHT - 10 * GRID_SIZE - GRID_SIZE;
    food.x = (rand() % ((maxX - minX) / GRID_SIZE + 1)) * GRID_SIZE + minX;
    food.y = (rand() % ((maxY - minY) / GRID_SIZE + 1)) * GRID_SIZE + minY;
}


void initObstacles() {
    for (int i = 0; i < numObstacles; i++) {
        int valid = 0;
        while (!valid) {
            obstacles[i].pos.x = (rand() % ((SCREEN_WIDTH - 2 * GRID_SIZE) / GRID_SIZE)) * GRID_SIZE + GRID_SIZE;
            obstacles[i].pos.y = (rand() % ((SCREEN_HEIGHT - 10 * GRID_SIZE) / GRID_SIZE)) * GRID_SIZE + GRID_SIZE + 50;
            valid = 1;
            // Check for overlap with snake or food
            for (int j = 0; j < snake.length; j++) {
                if (obstacles[i].pos.x == snake.body[j].x && obstacles[i].pos.y == snake.body[j].y) {
                    valid = 0;
                    break;
                }
            }
            if (obstacles[i].pos.x == food.x && obstacles[i].pos.y == food.y) {
                valid = 0;
            }
            for (int k = 0; k < i; k++) {
                if (obstacles[i].pos.x == obstacles[k].pos.x && obstacles[i].pos.y == obstacles[k].pos.y) {
                    valid = 0;
                    break;
                }
            }
        }
    }
}


int initSDL() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return 0;
    }

    window = SDL_CreateWindow("Snake Game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == NULL) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 0;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        printf("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 0;
    }

    return 1;
}

void closeSDL() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void initGame() {
    snake.length = 1;
    snake.body[0].x = (SCREEN_WIDTH / 2 / GRID_SIZE) * GRID_SIZE;
    snake.body[0].y = (SCREEN_HEIGHT / 2 / GRID_SIZE) * GRID_SIZE;
    snake.dx = GRID_SIZE;
    snake.dy = 0;
    score = 0;
    srand((unsigned int)time(NULL));
    generateFood();
    initObstacles();
}
void setGameParameters(Difficulty difficulty) {
    switch (difficulty) {
    case EASY:
        speed = 150;
        numObstacles = 10;
        break;
    case MEDIUM:
        speed = 50;
        numObstacles = 20;
        break;
    case HARD:
        speed = 10;
        numObstacles = 30;
        break;
    }
}

void handleInput(SDL_Event e) {
    if (e.type == SDL_KEYDOWN) {
        switch (e.key.keysym.sym) {
        case SDLK_UP:
            if (snake.dy == 0) {
                snake.dx = 0;
                snake.dy = -GRID_SIZE;
            }
            break;
        case SDLK_DOWN:
            if (snake.dy == 0) {
                snake.dx = 0;
                snake.dy = GRID_SIZE;
            }
            break;
        case SDLK_LEFT:
            if (snake.dx == 0) {
                snake.dx = -GRID_SIZE;
                snake.dy = 0;
            }
            break;
        case SDLK_RIGHT:
            if (snake.dx == 0) {
                snake.dx = GRID_SIZE;
                snake.dy = 0;
            }
            break;
        case SDLK_e:
            if (gameState == START) {
                gameDifficulty = EASY;
                setGameParameters(EASY);
                gameState = RUNNING;
            }
            break;
        case SDLK_m:
            if (gameState == START) {
                gameDifficulty = MEDIUM;
                setGameParameters(MEDIUM);
                gameState = RUNNING;
            }
            break;
        case SDLK_h:
            if (gameState == START) {
                gameDifficulty = HARD;
                setGameParameters(HARD);
                gameState = RUNNING;
            }
            break;
        case SDLK_RETURN:
            if (gameState == START) {
                gameState = RUNNING;
            }
            else if (gameState == QUIT_OR_CONTINUE) {
                initGame();
                gameState = START;
            }
            break;
        case SDLK_ESCAPE:
            running = 0;
            break;
        }
    }
}

void moveSnake() {
    for (int i = snake.length - 1; i > 0; --i) {
        snake.body[i] = snake.body[i - 1];
    }
    snake.body[0].x += snake.dx;
    snake.body[0].y += snake.dy;

    // Check for collision with food
    if (snake.body[0].x == food.x && snake.body[0].y == food.y) {
        snake.length++;
        score++;
        speed++;
        numObstacles++;
        initObstacles();
        generateFood();

        // Increase speed as score increases, but cap the speed to prevent it from becoming too fast
        if (speed > 50) {
            speed -= 5;  // Decrease delay time to increase speed
        }
    }

    // Check for collision with walls
    if (snake.body[0].x < GRID_SIZE || snake.body[0].x >= SCREEN_WIDTH - GRID_SIZE || snake.body[0].y < GRID_SIZE + 20 || snake.body[0].y >= SCREEN_HEIGHT - GRID_SIZE) {
        gameState = GAME_OVER;
    }

    // Check for self-collision
    for (int i = 1; i < snake.length; ++i) {
        if (snake.body[0].x == snake.body[i].x && snake.body[0].y == snake.body[i].y) {
            gameState = GAME_OVER;
            break;
        }
    }

    // Check for collision with obstacles
    for (int i = 0; i < numObstacles; i++) {
        if (snake.body[0].x == obstacles[i].pos.x && snake.body[0].y == obstacles[i].pos.y) {
            gameState = GAME_OVER;
            break;
        }
    }
}

void drawDigit(int digit, int posX, int posY) {
    const int digits[10][15] = {
        {1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1}, // 0
        {0, 1, 0, 1, 1, 0, 0, 1, 0, 0, 1, 0, 1, 1, 1}, // 1
        {1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1}, // 2
        {1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1}, // 3
        {1, 0, 1, 1, 0, 1, 1, 1, 1, 0, 0, 1, 0, 0, 1}, // 4
        {1, 1, 1, 1, 0, 0, 1, 1, 1, 0, 0, 1, 1, 1, 1}, // 5
        {1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 1, 1, 1, 1}, // 6
        {1, 1, 1, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 1, 0}, // 7
        {1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1}, // 8
        {1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1}  // 9
    };

    int cellWidth = 5;
    int cellHeight = 5;

    for (int row = 0; row < 5; ++row) {
        for (int col = 0; col < 3; ++col) {
            if (digits[digit][row * 3 + col]) {
                SDL_Rect fillRect = { posX + col * cellWidth, posY + row * cellHeight, cellWidth, cellHeight };
                SDL_RenderFillRect(renderer, &fillRect);
            }
        }
    }
}

void renderScore(int score, int posX, int posY) {
    int digits[10];
    int numDigits = 0;

    if (score == 0) {
        drawDigit(0, posX, posY);
        return;
    }

    while (score > 0) {
        digits[numDigits++] = score % 10;
        score /= 10;
    }

    for (int i = numDigits - 1; i >= 0; --i) {
        drawDigit(digits[i], posX, posY);
        posX += 2 * GRID_SIZE; // Space between digits
    }
}
void renderLevel(int posX, int posY) {
    switch (gameDifficulty) {
    case EASY:
        renderEasyScreen(posX, posY);
        break;
    case MEDIUM:
        renderMediumScreen(posX, posY);
        break;
    case HARD:
        renderHardScreen(posX, posY);
        break;
    }
}

void drawCharacter(char c, int posX, int posY) {
    const int chars[26][15] = {
        {1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1}, // A
        {1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1}, // B
        {1, 1, 1, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 1, 1}, // C
        {1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1}, // D
        {1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1}, // E
        {1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 0, 0}, // F
        {1, 1, 1, 1, 0, 0, 1, 0, 1, 1, 0, 1, 1, 1, 1}, // G
        {1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1}, // H
        {1, 1, 1, 0, 1, 0, 0, 1, 0, 0, 1, 0, 1, 1, 1}, // I
        {0, 0, 1, 0, 0, 1, 0, 0, 1, 1, 0, 1, 1, 1, 1}, // J
        {1, 0, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 0, 1}, // K
        {1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 1, 1}, // L
        {1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1}, // M
        {1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1}, // N
        {1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1}, // O
        {1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 0, 1, 0, 0}, // P
        {1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1}, // Q
        {1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1}, // R
        {1, 1, 1, 1, 0, 0, 1, 1, 1, 0, 0, 1, 1, 1, 1}, // S
        {1, 1, 1, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0}, // T
        {1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1}, // U
        {1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 0, 1, 0}, // V
        {1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1}, // W
        {1, 0, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 0, 1}, // X
        {1, 0, 1, 1, 0, 1, 1, 1, 0, 0, 1, 0, 0, 1, 0}, // Y
        {1, 1, 1, 0, 0, 1, 0, 1, 0, 1, 0, 0, 1, 1, 1}  // Z
    };

    int index = toupper(c) - 'A';
    if (index < 0 || index >= 26) {
        return; // Character out of range
    }

    int blockSize = GRID_SIZE / 2; // Reduce block size for characters
    for (int i = 0; i < 15; i++) {
        int x = posX + (i % 3) * blockSize;
        int y = posY + (i / 3) * blockSize;
        if (chars[index][i]) {
            SDL_Rect fillRect = { x, y, blockSize, blockSize };
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // White color for characters
            SDL_RenderFillRect(renderer, &fillRect);
        }
    }
}

void renderStartScreen1(int posX, int posY) {
    const char* message = "Press E TO EASY LEVEL";
    while (*message) {
        drawCharacter(*message, posX, posY);
        posX += 4 * (GRID_SIZE / 2); // Space between characters
        message++;
    }
}void renderStartScreen2(int posX, int posY) {
    const char* message = "Press M TO MEDIUM LEVEL";
    while (*message) {
        drawCharacter(*message, posX, posY);
        posX += 4 * (GRID_SIZE / 2); // Space between characters
        message++;
    }
}void renderStartScreen3(int posX, int posY) {
    const char* message = "Press H to HARD LEVEL";
    while (*message) {
        drawCharacter(*message, posX, posY);
        posX += 4 * (GRID_SIZE / 2); // Space between characters
        message++;
    }
}
void renderInstructionScreen(int posX, int posY) {
    const char* message = "Use ARROW KEYS to move";
    while (*message) {
        drawCharacter(*message, posX, posY);
        posX += 4 * (GRID_SIZE / 2); // Space between characters
        message++;
    }
}


void renderGameOver(int posX, int posY) {
    const char* message = "GAME OVER!";
    while (*message) {
        drawCharacter(*message, posX, posY);
        posX += 4 * (GRID_SIZE / 2); // Space between characters
        message++;
    }

}
void renderEasyScreen(int posX, int posY) {
    const char* message = "EASY";
    while (*message) {
        drawCharacter(*message, posX, posY);
        posX += 4 * (GRID_SIZE / 2); // Space between characters
        message++;
    }
}
void renderHardScreen(int posX, int posY) {
    const char* message = "HARD";
    while (*message) {
        drawCharacter(*message, posX, posY);
        posX += 4 * (GRID_SIZE / 2); // Space between characters
        message++;
    }
}
void renderMediumScreen(int posX, int posY) {
    const char* message = "MEDIUM";
    while (*message) {
        drawCharacter(*message, posX, posY);
        posX += 4 * (GRID_SIZE / 2); // Space between characters
        message++;
    }
}
void renderFinalScore(int score, int posX, int posY) {
    const char* message = "SCORE:";
    while (*message) {
        drawCharacter(*message, posX, posY);
        posX += 4 * (GRID_SIZE / 2); // Space between characters
        message++;
    }
    posX += GRID_SIZE; // Space between "SCORE:" and the actual score
    renderScore(score, posX, posY);
}
void renderQuitOrContinueScreen(int posX, int posY) {
    const char* quitOrContinueText = "PRESS ENTER TO CONTINUE OR ESC TO QUIT";;
    while (*quitOrContinueText) {
        drawCharacter(*quitOrContinueText, posX, posY);

        quitOrContinueText;
    }
}

void render() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Black background
    SDL_RenderClear(renderer);

    if (gameState == START) {
        renderStartScreen1((SCREEN_WIDTH - 5 * 4) / 6, SCREEN_HEIGHT / 3);
        renderStartScreen2((SCREEN_WIDTH - 5 * 4) / 6, SCREEN_HEIGHT / 2.5);
        renderStartScreen3((SCREEN_WIDTH - 5 * 4) / 6, SCREEN_HEIGHT / 2);
        renderInstructionScreen((SCREEN_WIDTH - 5 * 4) / 6, SCREEN_HEIGHT / 10);

    }
    else if (gameState == RUNNING) {
        // Draw snake
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); // Green color for snake
        for (int i = 0; i < snake.length; ++i) {
            SDL_Rect fillRect = { snake.body[i].x, snake.body[i].y, GRID_SIZE, GRID_SIZE };
            SDL_RenderFillRect(renderer, &fillRect);
        }

        // Draw food
        SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255); // Yellow color for food
        SDL_Rect foodRect = { food.x, food.y, GRID_SIZE, GRID_SIZE };
        SDL_RenderFillRect(renderer, &foodRect);

        // Draw obstacles
        for (int i = 0; i < numObstacles; i++) {
            SDL_Rect obstacleRect = { obstacles[i].pos.x, obstacles[i].pos.y, GRID_SIZE, GRID_SIZE };
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // Red color for obstacles
            SDL_RenderFillRect(renderer, &obstacleRect);
        }

        // Draw borders for red box
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // Red border
        SDL_Rect borderRect = { GRID_SIZE, GRID_SIZE + 20, SCREEN_WIDTH - 2 * GRID_SIZE, SCREEN_HEIGHT - 2 * GRID_SIZE - 20 };
        SDL_RenderDrawRect(renderer, &borderRect);

        // Render score
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // White color for score
        renderScore(score, GRID_SIZE, GRID_SIZE);
        renderLevel(SCREEN_WIDTH / 1.5 + 6 * GRID_SIZE, GRID_SIZE);
    }
    else if (gameState == GAME_OVER) {
        renderGameOver((SCREEN_WIDTH - 10 * GRID_SIZE) / 2, SCREEN_HEIGHT / 2 - 20);
        renderFinalScore(score, SCREEN_WIDTH / 2 - 40, SCREEN_HEIGHT / 2 + 20);
        gameState = QUIT_OR_CONTINUE;
    }
    else if (gameState == QUIT_OR_CONTINUE) {
        renderQuitOrContinueScreen((SCREEN_WIDTH - 20 * GRID_SIZE) / 4, SCREEN_HEIGHT / 2 + 50);
    }

    SDL_RenderPresent(renderer);
}

void waitForMilliseconds(int milliseconds) {
    Uint32 start = SDL_GetTicks();
    while (SDL_GetTicks() - start < (Uint32)milliseconds) {
        SDL_PollEvent(NULL);
    }
}

int main(int argc, char* args[]) {
    if (!initSDL()) {
        printf("Failed to initialize!\n");
        return 0;
    }

    initGame();

    SDL_Event e;
    while (running) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                running = 0;
            }
            handleInput(e);
        }

        if (gameState == RUNNING) {
            moveSnake();
            render();
            SDL_Delay(180);
        }
        else if (gameState == START || gameState == GAME_OVER) {
            render();
        }
    }
    closeSDL();
    return 0;
}