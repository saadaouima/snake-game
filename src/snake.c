#include "snake.h"
#include <stdlib.h>

Snake* createSnake(int initialLength) {
    Snake* snake = (Snake*) malloc(sizeof(Snake));
    if (!snake) {
        return NULL;
    }

    snake->body = (Point*) malloc(initialLength * sizeof(Point));
    if (!snake->body) {
        free(snake);
        return NULL;
    }

    snake->length = initialLength;
    snake->direction = RIGHT;  // Assuming RIGHT is defined in snake.h
    snake->alive = true;

    // Initialize snake body
    for (int i = 0; i < initialLength; i++) {
        snake->body[i].x = i;
        snake->body[i].y = 0;
    }

    return snake;
}

void moveSnake(Snake* snake) {
    // Move the snake by shifting the body elements
    for (int i = snake->length - 1; i > 0; i--) {
        snake->body[i] = snake->body[i - 1];
    }

    // Move the head of the snake based on its direction
    switch (snake->direction) {
        case UP:
            snake->body[0].y--;
            break;
        case DOWN:
            snake->body[0].y++;
            break;
        case LEFT:
            snake->body[0].x--;
            break;
        case RIGHT:
            snake->body[0].x++;
            break;
    }
}

void changeDirection(Snake* snake, Direction newDirection) {
    // Ensure the snake cannot reverse its direction
    if ((snake->direction == UP && newDirection != DOWN) ||
        (snake->direction == DOWN && newDirection != UP) ||
        (snake->direction == LEFT && newDirection != RIGHT) ||
        (snake->direction == RIGHT && newDirection != LEFT)) {
        
        snake->direction = newDirection;
    }
}

void growSnake(Snake* snake) {
    snake->length++;
    snake->body = realloc(snake->body, snake->length * sizeof(Point));
    if (!snake->body) {
        // Handle memory allocation failure
        snake->alive = false;
        return;
    }

    // The new segment is added to the tail of the snake
    snake->body[snake->length - 1] = snake->body[snake->length - 2];
}

void destroySnake(Snake* snake) {
    free(snake->body);
    free(snake);
}
