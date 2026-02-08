#include "raylib.h"

#define NAME "Snaker"

#define SCREEN_WIDTH  1024
#define SCREEN_HEIGHT 768
#define FONT_SIZE     30
#define TARGET_FPS    60

#define TILE_SIZE   20.0f
#define TILE_OFFSET 0.5f * TILE_SIZE
#define TILE_SIZE_V \
    (Vector2) { .x = TILE_SIZE, .y = TILE_SIZE }
#define TOPLEFT(pos) \
    (Vector2) { .x = pos.x - TILE_OFFSET, .y = pos.y - TILE_OFFSET }
#define TOPRIGHT(pos) \
    (Vector2) { .x = pos.x + TILE_OFFSET, .y = pos.y - TILE_OFFSET }
#define BOTLEFT(pos) \
    (Vector2) { .x = pos.x - TILE_OFFSET, .y = pos.y + TILE_OFFSET }
#define BOTRIGHT(pos) \
    (Vector2) { .x = pos.x + TILE_OFFSET, .y = pos.y + TILE_OFFSET }

#define UP    0
#define RIGHT 1
#define DOWN  2
#define LEFT  3

#define MAX_SNAKE_SIZE 100000

typedef struct {
    Vector2 pos;
} Tail;

typedef struct {
    int     score;
    int     lives;
    int     facing;
    int     size;
    Vector2 pos;
    float   speed;
    // FIXME(Per): Some form of linked list should work and be cleaner
    Tail tail[MAX_SNAKE_SIZE];
} Snake;

typedef struct {
    Vector2 pos;
} Food;

int RandomDirection() { return GetRandomValue(0, 3); }

Vector2 RandomPosition() {
    Vector2 v;
    v.x = GetRandomValue(TILE_OFFSET, SCREEN_WIDTH - TILE_OFFSET);
    v.y = GetRandomValue(TILE_OFFSET, SCREEN_HEIGHT - TILE_OFFSET);
    return v;
}

Food FoodNew() {
    Food f;
    f.pos = RandomPosition();
    return f;
}

void FoodDraw(Food food) { DrawRectangleV(TOPLEFT(food.pos), TILE_SIZE_V, BLUE); }

Snake SnakeNew() {
    Snake snake;
    snake.speed = 2.0f;
    snake.pos.x = 0.5f * SCREEN_WIDTH;
    snake.pos.y = 0.5f * SCREEN_HEIGHT;
    snake.score = 0;
    snake.lives = 3;
    snake.facing = RandomDirection();
    snake.size = 0;
    return snake;
}

bool SnakeIsOutOfBounds(Snake* snake) {
    bool out_of_bounds = false;
    if (snake->pos.x - TILE_OFFSET < 0)
        out_of_bounds = true;
    else if (snake->pos.x + TILE_OFFSET > SCREEN_WIDTH)
        out_of_bounds = true;
    else if (snake->pos.y - TILE_OFFSET < 0)
        out_of_bounds = true;
    else if (snake->pos.y + TILE_OFFSET > SCREEN_HEIGHT)
        out_of_bounds = true;
    return out_of_bounds;
}

bool SnakeEatsFood(Snake* snake, Food* food) {
    // TODO(Per): Can't this be simplified??
    return (TOPLEFT(snake->pos).x < TOPRIGHT(food->pos).x &&
            TOPLEFT(snake->pos).x > TOPLEFT(food->pos).x &&
            TOPLEFT(snake->pos).y > TOPLEFT(food->pos).y &&
            TOPLEFT(snake->pos).y < BOTLEFT(food->pos).y) ||
           (TOPRIGHT(snake->pos).x > TOPLEFT(food->pos).x &&
            TOPRIGHT(snake->pos).x < TOPRIGHT(food->pos).x &&
            TOPRIGHT(snake->pos).y > TOPLEFT(food->pos).y &&
            TOPRIGHT(snake->pos).y < BOTLEFT(food->pos).y) ||
           (BOTLEFT(snake->pos).x > BOTLEFT(food->pos).x &&
            BOTLEFT(snake->pos).x < BOTRIGHT(food->pos).x &&
            BOTLEFT(snake->pos).y > TOPRIGHT(food->pos).y &&
            BOTLEFT(snake->pos).y < BOTRIGHT(food->pos).y) ||
           (BOTRIGHT(snake->pos).x > BOTLEFT(food->pos).x &&
            BOTRIGHT(snake->pos).x < BOTRIGHT(food->pos).x &&
            BOTRIGHT(snake->pos).y > TOPLEFT(food->pos).y &&
            BOTRIGHT(snake->pos).y < BOTLEFT(food->pos).y);
}

void SnakeDraw(Snake snake) {
    DrawRectangleV(TOPLEFT(snake.pos), TILE_SIZE_V, ORANGE);
    for (int i = snake.size - 1; i >= 0; i--)
        DrawRectangleV(TOPLEFT(snake.tail[i].pos), TILE_SIZE_V, ORANGE);
}

void StatusDraw(Snake snake) {
    const char* s = TextFormat("Score: %4d - Lives: %d - Size: %d - Speed: %.2f - %3d FPS",
                               snake.score, snake.lives, snake.size, snake.speed, GetFPS());
    const int   y = SCREEN_HEIGHT - FONT_SIZE - 5;
    DrawText(s, 10, y, FONT_SIZE, GRAY);
}

void Update(Snake* snake, Food* food) {
    if (IsKeyDown(KEY_RIGHT) && (snake->facing == UP || snake->facing == DOWN))
        snake->facing = RIGHT;
    else if (IsKeyDown(KEY_LEFT) && (snake->facing == UP || snake->facing == DOWN))
        snake->facing = LEFT;
    else if (IsKeyDown(KEY_UP) && (snake->facing == RIGHT || snake->facing == LEFT))
        snake->facing = UP;
    else if (IsKeyDown(KEY_DOWN) && (snake->facing == RIGHT || snake->facing == LEFT))
        snake->facing = DOWN;

    int last = snake->size - 1;
    if (snake->size > 0) {
        for (int i = 0; i < last; i++) {
            snake->tail[i].pos = snake->tail[i + 1].pos;
        }
        snake->tail[last].pos = snake->pos;
    }
    if (snake->facing == RIGHT) {
        snake->pos.x += snake->speed;
    } else if (snake->facing == LEFT) {
        snake->pos.x -= snake->speed;
    } else if (snake->facing == UP) {
        snake->pos.y -= snake->speed;
    } else if (snake->facing == DOWN) {
        snake->pos.y += snake->speed;
    }

    if (SnakeIsOutOfBounds(snake)) {
        snake->lives--;
        snake->pos.x = 0.5f * SCREEN_WIDTH;
        snake->pos.y = 0.5f * SCREEN_HEIGHT;
        snake->facing = RandomDirection();
        snake->size = 0;
        return;
    }

    // TODO(Per): Check collision with snake's self.

    if (SnakeEatsFood(snake, food)) {
        food->pos = RandomPosition();
        Tail t;
        t.pos = snake->pos;
        snake->tail[snake->size++] = t;
        snake->score++;
        if (snake->score % 3 == 0) {
            // TODO(Per): Fine tune speed, .12f is a bit too much.
            snake->speed += .12f;
        }
    }
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
