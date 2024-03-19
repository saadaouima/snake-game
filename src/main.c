#include <GL/glut.h>
#include <stdbool.h>
#include <stdlib.h> // For rand()
#include <math.h>

#define WIDTH 40
#define HEIGHT 40
#define INITIAL_LENGTH 2

typedef struct {
    int x;
    int y;
} Point;

typedef struct {
    Point body[WIDTH * HEIGHT];
    int length;
    int direction; // 0: up, 1: right, 2: down, 3: left
    bool game_over;
} Snake;

Snake snake;

// Define walls
bool walls[WIDTH][HEIGHT] = {false};

// Define food
Point food;

void initialize() {
    snake.length = INITIAL_LENGTH;
    snake.direction = 1;

    for (int i = 0; i < INITIAL_LENGTH; ++i) {
        snake.body[i].x = (WIDTH / 2) + i;
        snake.body[i].y = HEIGHT / 2;
    }

    snake.game_over = false;

    // Place walls
    for (int i = 0; i < WIDTH; ++i) {
        walls[i][0] = true;
        walls[i][HEIGHT - 1] = true;
    }
    for (int j = 0; j < HEIGHT; ++j) {
        walls[0][j] = true;
        walls[WIDTH - 1][j] = true;
    }

    // Place initial food
    generateFood();
}

void drawSnake() {
    glColor3f(0.0f, 1.0f, 0.0f); // Green color
    for (int i = 0; i < snake.length; ++i) {
        glRectf(snake.body[i].x, snake.body[i].y, snake.body[i].x + 1, snake.body[i].y + 1);
    }
}

// Draw walls
void drawWalls() {
    glColor3f(1.0f, 0.0f, 0.0f); // Red color for walls
    for (int i = 0; i < WIDTH; ++i) {
        for (int j = 0; j < HEIGHT; ++j) {
            if (walls[i][j]) {
                glRectf(i, j, i + 1, j + 1);
            }
        }
    }
}

// Draw food
void drawFood() {
    float radius = 0.25f; // Set the radius of the circle
    int numSegments = 50; // Set the number of segments for the circle

    glColor3f(1.0f, 1.0f, 0.0f); // Set color to yellow

    // Calculate the position of the food inside the same pixel as the snake's head
    float foodPosX = food.x + 0.5f; // Center of the pixel
    float foodPosY = food.y + 0.5f; // Center of the pixel

    // Draw the circular food item
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(foodPosX, foodPosY); // Center of the circle
    for (int i = 0; i <= numSegments; i++) {
        float theta = 2.0f * 3.1415926f * ((float)i) / ((float)numSegments); // Get the current angle
        float x = radius * cosf(theta); // Calculate the x component
        float y = radius * sinf(theta); // Calculate the y component
        glVertex2f(foodPosX + x, foodPosY + y); // Output vertex
    }
    glEnd();
}



// Generate food at a random position
void generateFood() {
    do {
        food.x = rand() % (WIDTH - 2) + 1; // Exclude the walls
        food.y = rand() % (HEIGHT - 2) + 1;
    } while (walls[food.x][food.y]); // Keep generating until not on a wall

    // Ensure food is not on snake
    for (int i = 0; i < snake.length; ++i) {
        if (snake.body[i].x == food.x && snake.body[i].y == food.y) {
            generateFood(); // Recursively generate food again
            return;
        }
    }
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();
    drawSnake();
    drawWalls(); // Draw walls
    drawFood(); // Draw food
    glutSwapBuffers();
}

void reshape(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, WIDTH, 0, HEIGHT);
    glMatrixMode(GL_MODELVIEW);
}

void update(int value) {
    if (!snake.game_over) {
        // Move the snake
        for (int i = snake.length - 1; i > 0; --i) {
            snake.body[i] = snake.body[i - 1];
        }

        if (snake.direction == 0) snake.body[0].y++;
        else if (snake.direction == 1) snake.body[0].x++;
        else if (snake.direction == 2) snake.body[0].y--;
        else if (snake.direction == 3) snake.body[0].x--;

        // Check for collision with walls
        if (walls[snake.body[0].x][snake.body[0].y]) {
            snake.game_over = true;
        }

        // Check for collision with food
        if (snake.body[0].x == food.x && snake.body[0].y == food.y) {
            snake.length++;
            generateFood(); // Generate new food
        }

        // Redraw
        glutPostRedisplay();
        glutTimerFunc(100, update, 0);
    }
}

void keyboard(int key, int x, int y) {
    switch (key) {
        case GLUT_KEY_UP:
            if (snake.direction != 2) snake.direction = 0;
            break;
        case GLUT_KEY_DOWN:
            if (snake.direction != 0) snake.direction = 2;
            break;
        case GLUT_KEY_LEFT:
            if (snake.direction != 1) snake.direction = 3;
            break;
        case GLUT_KEY_RIGHT:
            if (snake.direction != 3) snake.direction = 1;
            break;
    }
}

int main(int argc, char** argv) {
    initialize();
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(WIDTH * 20, HEIGHT * 20);
    glutCreateWindow("Snake Game");
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutTimerFunc(100, update, 0);
    glutSpecialFunc(keyboard);
    glutMainLoop();
    return 0;
}
