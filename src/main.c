#include <GL/glut.h>
#include <stdbool.h>
#include <stdlib.h> // For rand()
#include <math.h>

#define WIDTH 40
#define HEIGHT 40
#define INITIAL_LENGTH 10
#define PANEL_SIZE 6 // Size of the score panel
#define SCORE_FONT GLUT_BITMAP_TIMES_ROMAN_24 // Font for the score

#define PI 3.14159265

typedef struct {
    int x;
    int y;
} Point;

typedef struct {
    Point body[WIDTH * HEIGHT];
    int length;
    int direction; // 0: up, 1: right, 2: down, 3: left
    int score; // Add score field
    bool game_over;
} Snake;

Snake snake;

// Define walls
bool walls[WIDTH][HEIGHT] = {false};

// Define food
Point food;

// Texture pattern for snake's body
bool snakeTexture[2][2] = {
    {true, false},
    {false, true}
};

/*
 * Initializes the game environment including snake's initial length, direction,
 * score, placement of walls, placement of initial food, and random placement of
 * wall obstacles. This function must be called before starting the game.
 */
void initialize() {
    snake.length = INITIAL_LENGTH;
    snake.direction = 1;
    snake.score = 0; // Initialize score

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

    // Place random wall obstacles
    for (int i = 0; i < WIDTH * HEIGHT / 1000; ++i) { // Adjust density as needed
        int obstacleX = rand() % (WIDTH - 2) + 1; // Exclude the borders
        int obstacleY = rand() % (HEIGHT - 2) + 1;
        walls[obstacleX][obstacleY] = true;
    }
}

void drawCircle(GLfloat x, GLfloat y, GLfloat radius) {
    int i;
    int triangleAmount = 20; //# of triangles used to draw circle
    
    //GLfloat radius = 0.8f; //radius
    GLfloat twicePi = 2.0f * PI;
    
    glBegin(GL_TRIANGLE_FAN);
        glVertex2f(x, y); // center of circle
        for(i = 0; i <= triangleAmount; i++) {
            glVertex2f(
                x + (radius * cos(i *  twicePi / triangleAmount)), 
                y + (radius * sin(i * twicePi / triangleAmount))
            );
        }
    glEnd();
}


void drawSnake() {
    for (int i = 0; i < snake.length; ++i) {
        int snakeTextureIndex = (i % 2 == 0) ? 0 : 1; // Alternate the texture pattern

        // Apply texture
        bool isGreen = snakeTexture[snakeTextureIndex][0];
        glColor3f(isGreen ? 0.0f : 1.0f, 1.0f, 0.0f); // Green or black color
        
        // Draw circular snake body
        drawCircle(snake.body[i].x + 0.5, snake.body[i].y + 0.5, 0.5);
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
    float radius = 0.5f; // Set the radius of the circle
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

void drawScoreText() {
    glColor3f(1.0f, 1.0f, 1.0f); // White color for text

    // Draw score text
    char scoreStr[50];
    sprintf(scoreStr, "Score: %d", snake.score);
    int textPosX = (WIDTH - glutBitmapLength(GLUT_BITMAP_TIMES_ROMAN_24, (unsigned char*)scoreStr)) / 2;
    int textPosY = HEIGHT / 2;

    glRasterPos2i(textPosX, textPosY);
    for (int i = 0; scoreStr[i] != '\0'; i++) {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, scoreStr[i]);
    }
}



void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();
    drawSnake();
    drawWalls(); // Draw walls
    drawFood(); // Draw food
    drawScoreText(); // Draw score panel
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
            snake.score++; // Increment score
            generateFood(); // Generate new food
            printf("Score: %d\n", snake.score); // Debug output
        }

        //check forcollision with tale 
        if (snake.body[0].x == snake.body[snake.length -1].x && snake.body[0].y == snake.body[snake.length -1].y) {
            snake.game_over = true;
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
    /*
 * Initialize the GLUT library, preparing the environment for creating graphical
 * user interfaces and handling input events. This function must be called before
 * any other GLUT functions in your program.
 */
    glutInit(&argc, argv);
   
    /*
 * Set the display mode for the GLUT window, specifying the initial display mode 
 * options such as color mode and buffering. This function must be called before 
 * creating the GLUT window.
 */
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
