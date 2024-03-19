#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <curses.h>

#define WIDTH 30
#define HEIGHT 20
#define START_LENGTH 20

typedef struct {
    int x;
    int y;
} Point;

typedef struct {
    Point body[WIDTH * HEIGHT];
    int length;
    char direction;
    Point food;
} Snake;

void initialize(Snake *snake) {
    snake->length = START_LENGTH;
    snake->direction = 'r'; // initial direction: right

    // Set initial position of snake
    for (int i = 0; i < START_LENGTH; ++i) {
        snake->body[i].x = WIDTH / 2 - i;
        snake->body[i].y = HEIGHT / 2;
    }

    // Place initial food randomly
    srand(time(NULL));
    snake->food.x = rand() % WIDTH;
    snake->food.y = rand() % HEIGHT;
}

void draw(Snake *snake) {
    clear(); // Clear the screen

    // Draw snake
    for (int i = 0; i < snake->length; ++i) {
        mvprintw(snake->body[i].y, snake->body[i].x, "o");
    }

    // Draw food
    mvprintw(snake->food.y, snake->food.x, "x");

    refresh();
}

void xmove(Snake *snake) {
    // Move the snake's body
    for (int i = snake->length - 1; i > 0; --i) {
        snake->body[i] = snake->body[i - 1];
    }

    // Move the snake's head based on the direction
    switch (snake->direction) {
        case 'u':
            snake->body[0].y--;
            break;
        case 'd':
            snake->body[0].y++;
            break;
        case 'l':
            snake->body[0].x--;
            break;
        case 'r':
            snake->body[0].x++;
            break;
    }
}

void input(Snake *snake) {
    int ch = getch();

    // Change direction based on user input
    switch(ch) {
        case KEY_UP:
            if (snake->direction != 'd')
                snake->direction = 'u';
            break;
        case KEY_DOWN:
            if (snake->direction != 'u')
                snake->direction = 'd';
            break;
        case KEY_LEFT:
            if (snake->direction != 'r')
                snake->direction = 'l';
            break;
        case KEY_RIGHT:
            if (snake->direction != 'l')
                snake->direction = 'r';
            break;
    }
}

int main() {
    initscr();
    noecho();
    curs_set(FALSE);
    keypad(stdscr, TRUE);
    timeout(100); // Set timeout for getch() to 100ms

    Snake snake;
    initialize(&snake);

    while (1) {
        input(&snake);
        xmove(&snake);
        draw(&snake);
        // Check if snake has eaten the food
        if (snake.body[0].x == snake.food.x && snake.body[0].y == snake.food.y) {
            snake.length++;
            snake.food.x = rand() % WIDTH;
            snake.food.y = rand() % HEIGHT;
        }
        // Check if snake has collided with itself
        for (int i = 1; i < snake.length; ++i) {
            if (snake.body[0].x == snake.body[i].x && snake.body[0].y == snake.body[i].y) {
                clear();
                WINDOW *game_over_win = newwin(5, 20, (HEIGHT - 5) / 2, (WIDTH - 20) / 2);
                box(game_over_win, 0, 0);
                mvwprintw(game_over_win, 2, 2, "Game Over!");
                wrefresh(game_over_win);
                wgetch(game_over_win);
                delwin(game_over_win);
                endwin();
                return 0;
            }
        }
        // Check if snake has collided with the walls
        if (snake.body[0].x >= WIDTH || snake.body[0].x < 0 || snake.body[0].y >= HEIGHT || snake.body[0].y < 0) {
            clear();
            WINDOW *game_over_win = newwin(5, 20, (HEIGHT - 5) / 2, (WIDTH - 20) / 2);
            box(game_over_win, 0, 0);
            mvwprintw(game_over_win, 2, 2, "Game Over!");
            wrefresh(game_over_win);
            wgetch(game_over_win);
            delwin(game_over_win);
            endwin();
            return 0;
        }
    }
}
