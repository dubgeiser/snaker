#include "raylib.h"

#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 768
#define FONT_SIZE 30
#define TARGET_FPS 60

typedef unsigned int uint;

typedef struct {
    uint score;
    uint lives;
    Vector2 pos;
    float speed;
} snake_t;

snake_t SnakeNew() {
    snake_t snake;
    snake.speed = 2.0f;
    snake.pos.x = (float)SCREEN_WIDTH / 2;
    snake.pos.y = (float)SCREEN_HEIGHT / 2;
    snake.score = 0;
    snake.lives = 3;
    return snake;
}

void SnakeUpdate(snake_t* snake) {
    if (IsKeyDown(KEY_RIGHT))
        snake->pos.x += snake->speed;
    else if (IsKeyDown(KEY_LEFT))
        snake->pos.x -= snake->speed;
    else if (IsKeyDown(KEY_UP))
        snake->pos.y -= snake->speed;
    else if (IsKeyDown(KEY_DOWN))
        snake->pos.y += snake->speed;
}

void SnakeDraw(snake_t snake) { DrawCircleV(snake.pos, 10, ORANGE); }

void StatusDraw(snake_t snake) {
    const char* s = TextFormat("Score: %4d - Lives: %d - (%.f, %.f)", snake.score, snake.lives,
                               snake.pos.x, snake.pos.y);
    const int y = SCREEN_HEIGHT - FONT_SIZE - 5;
    DrawText(s, 10, y, FONT_SIZE, BLACK);
}

int main(void) {
    const char* title = "Snaker (Arrow keys to move)";
    snake_t snake = SnakeNew();

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, title);
    SetTargetFPS(TARGET_FPS);

    while (!WindowShouldClose()) {
        SnakeUpdate(&snake);

        BeginDrawing();

        ClearBackground(RAYWHITE);
        StatusDraw(snake);
        SnakeDraw(snake);

        EndDrawing();
    }
    CloseWindow();
    return 0;
}
