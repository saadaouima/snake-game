#include <GL/glut.h>
#include <GL/freeglut.h> // Include GLUT's extension library for additional functions
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h> // For rand()
#include <math.h>
#include <SOIL.h> // Include SOIL for texture loading

#define WIDTH 40
#define HEIGHT 40
#define INITIAL_LENGTH 10
#define PANEL_SIZE 4 // Size of the score panel
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

        // Reset color to white (assuming white is the default color for walls)
        glColor3f(1.0f, 1.0f, 1.0f);
    }
}

// Define texture IDs
GLuint wallTextureID;

// Function to load texture
void loadTexture() {
    // Load texture image
    wallTextureID = SOIL_load_OGL_texture(
        "E:\\projects\\snake-game\\resources\\textures\\wall_texture.png",
        SOIL_LOAD_AUTO,
        SOIL_CREATE_NEW_ID,
        SOIL_FLAG_INVERT_Y
    );

    // Check if texture loaded successfully
    if (wallTextureID == 0) {
        printf("Error loading texture: %s\n", SOIL_last_result());
    }
}

void drawWalls() {
    glEnable(GL_TEXTURE_2D); // Enable texture mapping
    glBindTexture(GL_TEXTURE_2D, wallTextureID); // Bind the texture

    // Apply texture coordinates and draw walls
    for (int i = 0; i < WIDTH; ++i) {
        for (int j = 0; j < HEIGHT; ++j) {
            if (walls[i][j]) {
                // Calculate texture coordinates
                float texCoordLeft = 0.0f;
                float texCoordRight = 1.0f;
                float texCoordTop = 1.0f;
                float texCoordBottom = 0.0f;

                // Apply texture coordinates
                glBegin(GL_QUADS);
                glTexCoord2f(texCoordLeft, texCoordBottom); glVertex2f(i, j);
                glTexCoord2f(texCoordRight, texCoordBottom); glVertex2f(i + 1, j);
                glTexCoord2f(texCoordRight, texCoordTop); glVertex2f(i + 1, j + 1);
                glTexCoord2f(texCoordLeft, texCoordTop); glVertex2f(i, j + 1);
                glEnd();
            }
        }
    }

    glDisable(GL_TEXTURE_2D); // Disable texture mapping
}


// Define texture ID for food
GLuint foodTextureID;

void loadFoodTexture() {
    // Load texture image for food
    foodTextureID = SOIL_load_OGL_texture(
         "E:\\projects\\snake-game\\resources\\textures\\apple.png", // Path to your food texture image
        SOIL_LOAD_AUTO,
        SOIL_CREATE_NEW_ID,
        SOIL_FLAG_INVERT_Y
    );

    // Check if texture loaded successfully
    if (foodTextureID == 0) {
        printf("Error loading food texture: %s\n", SOIL_last_result());
    }
}

void drawFood() {
    // Load texture image
    GLuint foodTextureID = SOIL_load_OGL_texture(
        "E:\\projects\\snake-game\\resources\\textures\\apple.png",
        SOIL_LOAD_AUTO,
        SOIL_CREATE_NEW_ID,
        SOIL_FLAG_INVERT_Y
    );

    // Check if texture loaded successfully
    if (foodTextureID == 0) {
        printf("Error loading food texture: %s\n", SOIL_last_result());
        return;
    }

    // Bind texture
    glBindTexture(GL_TEXTURE_2D, foodTextureID);

    // Enable texture mapping
    glEnable(GL_TEXTURE_2D);

    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Define vertices of the food square
    float foodSize = 1.0f; // Size of the food square
    float foodVertices[8] = {
        food.x, food.y,
        food.x + foodSize, food.y,
        food.x + foodSize, food.y + foodSize,
        food.x, food.y + foodSize
    };

    // Define texture coordinates
    float textureCoordinates[8] = {
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f
    };

    // Draw food with texture
    glBegin(GL_QUADS);
    for (int i = 0; i < 4; ++i) {
        glTexCoord2f(textureCoordinates[2*i], textureCoordinates[2*i + 1]);
        glVertex2f(foodVertices[2*i], foodVertices[2*i + 1]);
    }
    glEnd();

    // Disable texture mapping
    glDisable(GL_TEXTURE_2D);
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


void drawScorePanel() {
    // Set the color to black for the panel background
    glColor3f(0.0, 0.0, 0.0);
    glBegin(GL_QUADS);
    glVertex2f(WIDTH - PANEL_SIZE, HEIGHT);
    glVertex2f(WIDTH, HEIGHT);
    glVertex2f(WIDTH, HEIGHT - PANEL_SIZE);
    glVertex2f(WIDTH - PANEL_SIZE, HEIGHT - PANEL_SIZE);
    glEnd();

    // Enable texture mapping
    glEnable(GL_TEXTURE_2D);

    // Bind the food texture
    glBindTexture(GL_TEXTURE_2D, foodTextureID);

    // Calculate the center position of the panel
    float panelCenterX = WIDTH - PANEL_SIZE / 2.0;
    float panelCenterY = HEIGHT - PANEL_SIZE / 2.0;

    // Calculate the half size of the food icon
    float halfIconSize = 0.5;

    // Set texture coordinates to center the icon
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex2f(panelCenterX - halfIconSize, panelCenterY - halfIconSize);
    glTexCoord2f(1.0f, 0.0f); glVertex2f(panelCenterX + halfIconSize, panelCenterY - halfIconSize);
    glTexCoord2f(1.0f, 1.0f); glVertex2f(panelCenterX + halfIconSize, panelCenterY + halfIconSize);
    glTexCoord2f(0.0f, 1.0f); glVertex2f(panelCenterX - halfIconSize, panelCenterY + halfIconSize);
    glEnd();

    // Disable texture mapping
    glDisable(GL_TEXTURE_2D);
}



void drawScore() {
    char buffer[15];
    sprintf(buffer, "Score: %d", snake.score);

    // Adjusted position inside the panel
    float textPosX = WIDTH - PANEL_SIZE + 0.3;
    float textPosY = HEIGHT - PANEL_SIZE + 0.3;

    // Set the color to white for the score text
    glColor3f(1.0, 1.0, 1.0);

    // Position the raster for drawing text
    glRasterPos2f(textPosX, textPosY);

    // Draw each character of the score text
    for (int i = 0; buffer[i] != '\0'; i++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, buffer[i]);
    }
}


void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();
    drawSnake();
    drawWalls(); // Draw walls
    drawFood(); // Draw food
    drawScorePanel(); // Draw panel
    drawScore(); // Draw the score panel
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

    loadTexture();// Load texture
    
    glClearColor(1.0, 1.0, 1.0, 1.0);// Set background color to white
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-1, 1, -1, 1, -1, 1);

    glutReshapeFunc(reshape);
    glutTimerFunc(100, update, 0);
    glutSpecialFunc(keyboard);
    glutMainLoop();
    return 0;
}
