#include <stdlib.h>

#include "raylib.h"

#define NAME "Snaker"

#define SCREEN_WIDTH  1024
#define SCREEN_HEIGHT 768
#define SCREEN_CENTER                                       \
    (Vector2) {                                             \
        .x = 0.5f * SCREEN_WIDTH, .y = 0.5f * SCREEN_HEIGHT \
    }
#define FONT_SIZE  30
#define TARGET_FPS 60

#define TILE_SIZE   20.0f
#define TILE_OFFSET 0.5f * TILE_SIZE
#define TILE_SIZE_V                    \
    (Vector2) {                        \
        .x = TILE_SIZE, .y = TILE_SIZE \
    }

#define TOPLEFT(position)                                            \
    (Vector2) {                                                      \
        .x = position.x - TILE_OFFSET, .y = position.y - TILE_OFFSET \
    }
#define TOPRIGHT(position)                                           \
    (Vector2) {                                                      \
        .x = position.x + TILE_OFFSET, .y = position.y - TILE_OFFSET \
    }
#define BOTLEFT(position)                                            \
    (Vector2) {                                                      \
        .x = position.x - TILE_OFFSET, .y = position.y + TILE_OFFSET \
    }
#define BOTRIGHT(position)                                           \
    (Vector2) {                                                      \
        .x = position.x + TILE_OFFSET, .y = position.y + TILE_OFFSET \
    }

#define UP    0
#define RIGHT 1
#define DOWN  2
#define LEFT  3

typedef struct Segment {
    Vector2         position;
    Vector2         previous_position;
    struct Segment* prev;
    struct Segment* next;
} Segment;

// TODO(Per): Store length instead of segment counts.
//            We can make the snake grow smoothly and at the length of whatever
//            the food it ate.
typedef struct Snake {
    Segment* head;
    Segment* tail;
    int      direction;
    float    speed;
    int      score;
    int      lives;
} Snake;

// TODO(Per): Bad food: Snake shrinks
// TODO(Per): Super food: Big growth
// TODO(Per): Power food: Temporal, fi. going out of bounds
// TODO(Per): Food shelf life: Food disappears after a while
typedef struct Food {
    Vector2 position;
} Food;

int random_direction() {
    return GetRandomValue(0, 3);
}

Vector2 random_position() {
    Vector2 v;
    v.x = GetRandomValue(TILE_OFFSET, SCREEN_WIDTH - TILE_OFFSET);
    v.y = GetRandomValue(TILE_OFFSET, SCREEN_HEIGHT - TILE_OFFSET);
    return v;
}

Food food_new() {
    Food f;
    // TODO(Per): Check validity, ie. not on snake!
    f.position = random_position();
    return f;
}

Snake snake_new() {
    Segment* head = malloc(sizeof(*head));
    head->position = SCREEN_CENTER;
    head->prev = 0;
    head->next = 0;

    Snake snake = {0};
    snake.score = 0;
    snake.lives = 3;
    snake.direction = random_direction();
    snake.speed = 2.0f;
    snake.head = head;
    snake.tail = head;

    return snake;
}

bool out_of_bounds(Vector2 position) {
    bool oobounds = false;
    if (position.x - TILE_OFFSET < 0)
        oobounds = true;
    else if (position.x + TILE_OFFSET > SCREEN_WIDTH)
        oobounds = true;
    else if (position.y - TILE_OFFSET < 0)
        oobounds = true;
    else if (position.y + TILE_OFFSET > SCREEN_HEIGHT)
        oobounds = true;
    return oobounds;
}

Rectangle bounding_rectangle(Vector2 position) {
    return (Rectangle){position.x - TILE_OFFSET, position.y - TILE_OFFSET, TILE_SIZE,
                       TILE_SIZE};
}

bool collides(Vector2 p1, Vector2 p2) {
    return CheckCollisionRecs(bounding_rectangle(p1), bounding_rectangle(p2));
}

void update(Snake* snake, Food* food) {
    if (IsKeyDown(KEY_RIGHT) && (snake->direction == UP || snake->direction == DOWN))
        snake->direction = RIGHT;
    else if (IsKeyDown(KEY_LEFT) && (snake->direction == UP || snake->direction == DOWN))
        snake->direction = LEFT;
    else if (IsKeyDown(KEY_UP) && (snake->direction == RIGHT || snake->direction == LEFT))
        snake->direction = UP;
    else if (IsKeyDown(KEY_DOWN) && (snake->direction == RIGHT || snake->direction == LEFT))
        snake->direction = DOWN;

    for (Segment* cur = snake->tail; cur && cur->prev; cur = cur->prev) {
        cur->previous_position = cur->position;
        cur->position = cur->prev->position;
    }

    if (snake->direction == RIGHT) {
        snake->head->position.x += snake->speed;
    } else if (snake->direction == LEFT) {
        snake->head->position.x -= snake->speed;
    } else if (snake->direction == UP) {
        snake->head->position.y -= snake->speed;
    } else if (snake->direction == DOWN) {
        snake->head->position.y += snake->speed;
    }

    if (out_of_bounds(snake->head->position)) {
        // TODO(Per): Shrink snake, reduce to 1 segment?
        // TODO(Per): Memory management head and tail.
        snake->lives--;
        snake->head->position = SCREEN_CENTER;
        return;
    }

    // TODO(Per): Check collision with snake's self.

    if (collides(snake->head->position, food->position)) {
        Segment* seg = malloc(sizeof(*seg));
        seg->position = snake->tail->previous_position;
        snake->tail->next = seg;
        seg->prev = snake->tail;
        seg->next = 0;
        snake->tail = seg;
        snake->score++;

        // TODO(Per): Fine tune speed: absolute values, percentage, ...
        //            Maybe back off the acceleration when speeds gets higher?
        //            Time based instead of score based?  Both?
        // TODO(Per): If we check multiple things to determine if and by how much we
        // should
        //            increase the speed, then this check needs to be taken out of
        //            the collision detection and we should set a state(boolean)
        //            here.
        if (snake->score % 3 == 0) {
            snake->speed += .12f;
        }

        food->position = random_position();
    }
}

void draw_frame(Snake snake, Food food) {
    ClearBackground(BLACK);

    const char* s = TextFormat("Score: %4d - Lives: %d - Speed: %.2f - %3d FPS", snake.score,
                               snake.lives, snake.speed, GetFPS());
    const int   y = SCREEN_HEIGHT - FONT_SIZE - 5;
    DrawText(s, 10, y, FONT_SIZE, GRAY);

    for (Segment* cur = snake.head; cur != 0; cur = cur->next) {
        DrawRectangleV(TOPLEFT(cur->position), TILE_SIZE_V, ORANGE);
    }

    DrawRectangleV(TOPLEFT(food.position), TILE_SIZE_V, BLUE);
}

// TODO(Per): Save / Load high score
int main(void) {
    const char* title = NAME;
    Snake       snake = snake_new();
    Food        food = food_new();

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, title);
    SetTargetFPS(TARGET_FPS);
    HideCursor();

    while (!WindowShouldClose()) {
        BeginDrawing();
        draw_frame(snake, food);
        EndDrawing();
        if (snake.lives > 0) update(&snake, &food);
    }

    // TODO(Per): cleanup; head, tail, segments, ...

    CloseWindow();

    return 0;
}
