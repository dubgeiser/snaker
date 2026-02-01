#include "raylib.h"

#define NAME "Snaker"

#define SCREEN_WIDTH  1024
#define SCREEN_HEIGHT 768
#define FONT_SIZE     30
#define TARGET_FPS    60

#define SNAKE_SIZE   20.0f
#define SNAKE_OFFSET SNAKE_SIZE / 2.0f

#define NORTH 0
#define EAST  1
#define SOUTH 2
#define WEST  3

int RandomDirection() { return GetRandomValue(0, 3); }

typedef struct {
    int     score;
    int     lives;
    int     facing;
    Vector2 pos;
    float   speed;
} Snake;

typedef struct {
    Vector2 pos;
} Food;

Snake SnakeNew() {
    Snake snake;
    snake.speed = 2.0f;
    snake.pos.x = (float)SCREEN_WIDTH / 2;
    snake.pos.y = (float)SCREEN_HEIGHT / 2;
    snake.score = 0;
    snake.lives = 3;
    snake.facing = RandomDirection();
    return snake;
}

Food FoodNew() {
    Food f;
    f.pos.x = GetRandomValue(0, SCREEN_WIDTH - SNAKE_OFFSET);
    f.pos.y = GetRandomValue(0, SCREEN_HEIGHT - SNAKE_OFFSET);
    return f;
}

bool SnakeEatsFood(Snake* snake, Food* food) {
    return snake->pos.x >= food->pos.x - SNAKE_OFFSET &&
           snake->pos.x <= food->pos.x + SNAKE_OFFSET &&
           snake->pos.y >= food->pos.y - SNAKE_OFFSET &&
           snake->pos.y <= food->pos.y + SNAKE_OFFSET;
}

void Update(Snake* snake, Food* food) {
    if (IsKeyDown(KEY_RIGHT) && (snake->facing == NORTH || snake->facing == SOUTH))
        snake->facing = EAST;
    else if (IsKeyDown(KEY_LEFT) && (snake->facing == NORTH || snake->facing == SOUTH))
        snake->facing = WEST;
    else if (IsKeyDown(KEY_UP) && (snake->facing == EAST || snake->facing == WEST))
        snake->facing = NORTH;
    else if (IsKeyDown(KEY_DOWN) && (snake->facing == EAST || snake->facing == WEST))
        snake->facing = SOUTH;

    if (snake->facing == EAST)
        snake->pos.x += snake->speed;
    else if (snake->facing == WEST)
        snake->pos.x -= snake->speed;
    else if (snake->facing == NORTH)
        snake->pos.y -= snake->speed;
    else if (snake->facing == SOUTH)
        snake->pos.y += snake->speed;

    bool has_collided = false;
    if (snake->pos.x - SNAKE_OFFSET < 0)
        has_collided = true;
    else if (snake->pos.x + SNAKE_OFFSET > SCREEN_WIDTH)
        has_collided = true;
    else if (snake->pos.y - SNAKE_OFFSET < 0)
        has_collided = true;
    else if (snake->pos.y + SNAKE_OFFSET > SCREEN_HEIGHT)
        has_collided = true;
    if (has_collided) {
        snake->lives--;
        snake->pos.x = (float)SCREEN_WIDTH / 2;
        snake->pos.y = (float)SCREEN_HEIGHT / 2;
        snake->facing = RandomDirection();
        return;
    }

    if (SnakeEatsFood(snake, food)) {
        snake->score++;
        food->pos.x = GetRandomValue(0, SCREEN_WIDTH - SNAKE_OFFSET);
        food->pos.y = GetRandomValue(0, SCREEN_HEIGHT - SNAKE_OFFSET);
    }
}

void SnakeDraw(Snake snake) {
    DrawRectangle(snake.pos.x - SNAKE_OFFSET, snake.pos.y - SNAKE_OFFSET, SNAKE_SIZE,
                  SNAKE_SIZE, ORANGE);
}

void StatusDraw(Snake snake) {
    const char* s = TextFormat("Score: %4d - Lives: %d - (%.f, %.f) - %3d FPS", snake.score,
                               snake.lives, snake.pos.x, snake.pos.y, GetFPS());
    const int   y = SCREEN_HEIGHT - FONT_SIZE - 5;
    DrawText(s, 10, y, FONT_SIZE, WHITE);
}

void FoodDraw(Food food) {
    DrawRectangle(food.pos.x - SNAKE_OFFSET, food.pos.y - SNAKE_OFFSET, SNAKE_SIZE, SNAKE_SIZE,
                  BLUE);
}

int main(void) {
    const char* title = NAME;

    Snake snake = SnakeNew();
    Food  food = FoodNew();

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, title);
    SetTargetFPS(TARGET_FPS);

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);
        StatusDraw(snake);
        SnakeDraw(snake);
        FoodDraw(food);
        EndDrawing();

        if (snake.lives == 0) continue;

        Update(&snake, &food);
    }
    CloseWindow();
    return 0;
}
