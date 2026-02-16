#include <stdlib.h>

#include "raylib.h"

#define NAME "Snaker"

#define SCREEN_WIDTH  1024
#define SCREEN_HEIGHT 768
#define SCREEN_CENTER \
    (Vector2) { .x = 0.5f * SCREEN_WIDTH, .y = 0.5f * SCREEN_HEIGHT }
#define FONT_SIZE  30
#define TARGET_FPS 60

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

typedef struct Segment {
    Vector2         pos;
    struct Segment* prev;
    struct Segment* next;
} Segment;

typedef struct Snake {
    Segment* head;
    Segment* tail;
    int      direction;
    float    speed;
    int      score;
    int      lives;
} Snake;

typedef struct Food {
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

Snake SnakeNew() {
    Segment* head = malloc(sizeof(* head));
    head->pos = SCREEN_CENTER;
    head->prev = 0;
    head->next = 0;

    Snake snake = {0};
    snake.score = 0;
    snake.lives = 3;
    snake.direction = RandomDirection();
    snake.speed = 2.0f;
    snake.head = head;
    snake.tail = head;

    return snake;
}

bool IsOutOfBounds(Vector2 pos) {
    bool outOfBounds = false;
    if (pos.x - TILE_OFFSET < 0)
        outOfBounds = true;
    else if (pos.x + TILE_OFFSET > SCREEN_WIDTH)
        outOfBounds = true;
    else if (pos.y - TILE_OFFSET < 0)
        outOfBounds = true;
    else if (pos.y + TILE_OFFSET > SCREEN_HEIGHT)
        outOfBounds = true;
    return outOfBounds;
}

bool IsCollision(Vector2 pos1, Vector2 pos2) {
    return CheckCollisionRecs(
        (Rectangle){pos1.x - TILE_OFFSET, pos1.y - TILE_OFFSET, TILE_SIZE, TILE_SIZE},
        (Rectangle){pos2.x - TILE_OFFSET, pos2.y - TILE_OFFSET, TILE_SIZE, TILE_SIZE});
}

void Update(Snake* snake, Food* food) {
    if (IsKeyDown(KEY_RIGHT) && (snake->direction == UP || snake->direction == DOWN))
        snake->direction = RIGHT;
    else if (IsKeyDown(KEY_LEFT) && (snake->direction == UP || snake->direction == DOWN))
        snake->direction = LEFT;
    else if (IsKeyDown(KEY_UP) && (snake->direction == RIGHT || snake->direction == LEFT))
        snake->direction = UP;
    else if (IsKeyDown(KEY_DOWN) && (snake->direction == RIGHT || snake->direction == LEFT))
        snake->direction = DOWN;

    for (Segment* cur = snake->tail; cur && cur->prev; cur = cur->prev)
        cur->pos = cur->prev->pos;

    if (snake->direction == RIGHT) {
        snake->head->pos.x += snake->speed;
    } else if (snake->direction == LEFT) {
        snake->head->pos.x -= snake->speed;
    } else if (snake->direction == UP) {
        snake->head->pos.y -= snake->speed;
    } else if (snake->direction == DOWN) {
        snake->head->pos.y += snake->speed;
    }

    if (IsOutOfBounds(snake->head->pos)) {
        // TODO(Per): Shrink snake, reduce to 1 segment?
        // TODO(Per): Memory managment head and tail.
        snake->lives--;
        snake->head->pos = SCREEN_CENTER;
        return;
    }

    // TODO(Per): Check collision with snake's self.

    if (IsCollision(snake->head->pos, food->pos)) {
        Segment* seg = malloc(sizeof(* seg));
        seg->pos = snake->head->pos;
        snake->tail->next = seg;
        seg->prev = snake->tail;
        snake->tail = seg;
        seg->next = 0;
        snake->score++;

        // TODO(Per): Fine tune speed: absolute values, percentage, ...
        //            Maybe back off the acceleration when speeds gets higher?
        //            Time based instead of score based?  Both?
        // TODO(Per): If we check multiple things to determine if and by how much we should
        //            increase the speed, then this check needs to be taken out of the collision
        //            detection and we should set a state(boolean) here.
        if (snake->score % 3 == 0) {
            snake->speed += .12f;
        }

        food->pos = RandomPosition();
    }
}

void DrawFrame(Snake snake, Food food) {
    const char* s = TextFormat("Score: %4d - Lives: %d - Speed: %.2f - %3d FPS", snake.score,
                               snake.lives, snake.speed, GetFPS());
    const int   y = SCREEN_HEIGHT - FONT_SIZE - 5;
    DrawText(s, 10, y, FONT_SIZE, GRAY);

    DrawRectangleV(TOPLEFT(snake.head->pos), TILE_SIZE_V, ORANGE);
    for (Segment* cur = snake.head; cur != 0; cur = cur->next) {
        DrawRectangleV(TOPLEFT(cur->pos), TILE_SIZE_V, ORANGE);
    }

    DrawRectangleV(TOPLEFT(food.pos), TILE_SIZE_V, BLUE);
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
        DrawFrame(snake, food);
        EndDrawing();

        if (snake.lives == 0) continue;

        Update(&snake, &food);
    }
    CloseWindow();
    return 0;
}
