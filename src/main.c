#include <GL/glut.h>
#include <GL/freeglut.h> // Include GLUT's extension library for additional functions
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h> // For rand()
#include <math.h>
#include <time.h>
#include <SOIL/SOIL.h> // Include SOIL for texture loading

#define WIDTH 40
#define HEIGHT 40
#define MAX_MAPS 10 // Maximum number of maps
#define INITIAL_LENGTH 10
#define PANEL_SIZE 4                          // Size of the score panel
#define SCORE_FONT GLUT_BITMAP_TIMES_ROMAN_24 // Font for the score
#define MAX_POINTS 100
#define PI 3.14159265

// Define texture IDs
GLuint wallTextureID;
GLuint busheTextureID;
GLuint snakeTextureID;

time_t lastFruitSpawnTime;
double megaFruitSpawnInterval = 10.0; // Mega Fruit spawns every 10 seconds
bool isMegaFruitSpawned = false;

typedef enum
{
    HOME_SCREEN,
    GAME_SCREEN
} Screen;

Screen currentScreen = HOME_SCREEN; // Start with the home screen

// Global variable to keep track of the selected map
int selectedMapIndex = 0;

typedef struct
{
    int x;
    int y;
} Point;

typedef struct
{
    char name[50];
    Point schema[MAX_POINTS];
    int schemaSize;
} Map;

Map mapList[MAX_MAPS][50]; // List of available maps
int numMaps = 0;

typedef struct
{
    Point body[WIDTH * HEIGHT];
    int length;
    int direction; // 0: up, 1: right, 2: down, 3: left
    int score;     // Add score field
    bool game_over;
} Snake;

Snake snake;

// Define walls
bool walls[WIDTH][HEIGHT] = {false};

// Define walls
bool bushes[WIDTH][HEIGHT] = {false};

// Define food
Point food;
Point megaFood;

// Texture pattern for snake's body
bool snakeTexture[2][2] = {
    {true, false},
    {false, true}};

/*
 * Initializes the game environment including snake's initial length, direction,
 * score, placement of walls, placement of initial food, and random placement of
 * wall obstacles. This function must be called before starting the game.
 */
void initialize()
{
    lastFruitSpawnTime = time(NULL);

    snake.length = INITIAL_LENGTH;
    snake.direction = 1;
    snake.score = 0; // Initialize score

    for (int i = 0; i < INITIAL_LENGTH; ++i)
    {
        snake.body[i].x = (WIDTH / 2) + i;
        snake.body[i].y = HEIGHT / 2;
    }

    snake.game_over = false;

    // Place walls
    for (int i = 0; i < WIDTH; ++i)
    {
        walls[i][0] = true;
        walls[i][HEIGHT - 1] = true;
    }
    for (int j = 0; j < HEIGHT; ++j)
    {
        walls[0][j] = true;
        walls[WIDTH - 1][j] = true;
    }

    // Place inner walls based on the selected map schema
    for (int i = 0; i < mapList[selectedMapIndex]->schemaSize; ++i)
    {
        int x = mapList[selectedMapIndex]->schema[i].x;
        int y = mapList[selectedMapIndex]->schema[i].y;
        bushes[x][y] = true;
    }

    // Place initial food
    generateFood();
}

void drawCircle(GLfloat x, GLfloat y, GLfloat radius)
{
    int i;
    int triangleAmount = 20; // # of triangles used to draw circle

    // GLfloat radius = 0.8f; //radius
    GLfloat twicePi = 2.0f * PI;

    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x, y); // center of circle
    for (i = 0; i <= triangleAmount; i++)
    {
        glVertex2f(
            x + (radius * cos(i * twicePi / triangleAmount)),
            y + (radius * sin(i * twicePi / triangleAmount)));
    }
    glEnd();
}

void drawSnake()
{
    for (int i = 0; i < snake.length; ++i)
    {
        glEnable(GL_TEXTURE_2D);                      // Enable texture mapping
        glBindTexture(GL_TEXTURE_2D, snakeTextureID); // Bind the texture

        // Calculate texture coordinates
        float texCoordLeft = 0.0f;
        float texCoordRight = 1.0f;
        float texCoordTop = 1.0f;
        float texCoordBottom = 0.0f;

        float radius = 0.5f; // Radius for the circle

        // Number of triangles to approximate the circle
        int numSegments = 30;

        glBegin(GL_TRIANGLE_FAN);
        glTexCoord2f((texCoordLeft + texCoordRight) / 2, (texCoordTop + texCoordBottom) / 2);
        glVertex2f(snake.body[i].x + 0.5f, snake.body[i].y + 0.5f); // Center of the circle

        for (int j = 0; j <= numSegments; j++)
        {
            float angle = j * 2.0f * PI / numSegments;
            float dx = radius * cosf(angle);
            float dy = radius * sinf(angle);

            glTexCoord2f(texCoordLeft + (texCoordRight - texCoordLeft) / 2 * (1 + cosf(angle)), texCoordBottom + (texCoordTop - texCoordBottom) / 2 * (1 + sinf(angle)));
            glVertex2f(snake.body[i].x + 0.5f + dx, snake.body[i].y + 0.5f + dy);
        }

        glEnd();

        glDisable(GL_TEXTURE_2D); // Disable texture mapping
    }
}

// Function to load texture
void loadTexture()
{
    // Load texture image
    wallTextureID = SOIL_load_OGL_texture(
        "E:\\projects\\snake-game\\resources\\textures\\wall_texture.png",
        SOIL_LOAD_AUTO,
        SOIL_CREATE_NEW_ID,
        SOIL_FLAG_INVERT_Y);

    busheTextureID = SOIL_load_OGL_texture(
        "E:\\projects\\snake-game\\resources\\textures\\bushe.jpg",
        SOIL_LOAD_AUTO,
        SOIL_CREATE_NEW_ID,
        SOIL_FLAG_INVERT_Y);

    snakeTextureID = SOIL_load_OGL_texture(
        "E:\\projects\\snake-game\\resources\\textures\\snake.png",
        SOIL_LOAD_AUTO,
        SOIL_CREATE_NEW_ID,
        SOIL_FLAG_INVERT_Y);

    // Check if texture loaded successfully
    if (wallTextureID == 0)
    {
        printf("Error loading texture: %s\n", SOIL_last_result());
    }

    if (busheTextureID == 0)
    {
        printf("Error loading texture: %s\n", SOIL_last_result());
    }

    if (snakeTextureID == 0)
    {
        printf("Error loading texture: %s\n", SOIL_last_result());
    }
}

void drawWalls()
{
    glEnable(GL_TEXTURE_2D);                     // Enable texture mapping
    glBindTexture(GL_TEXTURE_2D, wallTextureID); // Bind the texture

    // Apply texture coordinates and draw walls
    for (int i = 0; i < WIDTH; ++i)
    {
        for (int j = 0; j < HEIGHT; ++j)
        {
            if (walls[i][j])
            {
                // Calculate texture coordinates
                float texCoordLeft = 0.0f;
                float texCoordRight = 1.0f;
                float texCoordTop = 1.0f;
                float texCoordBottom = 0.0f;

                // Apply texture coordinates
                glBegin(GL_QUADS);
                glTexCoord2f(texCoordLeft, texCoordBottom);
                glVertex2f(i, j);
                glTexCoord2f(texCoordRight, texCoordBottom);
                glVertex2f(i + 1, j);
                glTexCoord2f(texCoordRight, texCoordTop);
                glVertex2f(i + 1, j + 1);
                glTexCoord2f(texCoordLeft, texCoordTop);
                glVertex2f(i, j + 1);
                glEnd();
            }
        }
    }

    glDisable(GL_TEXTURE_2D); // Disable texture mapping
}

void drawBushes()
{
    glEnable(GL_TEXTURE_2D);                      // Enable texture mapping
    glBindTexture(GL_TEXTURE_2D, busheTextureID); // Bind the texture

    // Apply texture coordinates and draw walls
    for (int i = 0; i < WIDTH; ++i)
    {
        for (int j = 0; j < HEIGHT; ++j)
        {
            if (bushes[i][j])
            {
                // Calculate texture coordinates
                float texCoordLeft = 0.0f;
                float texCoordRight = 1.0f;
                float texCoordTop = 1.0f;
                float texCoordBottom = 0.0f;

                // Apply texture coordinates
                glBegin(GL_QUADS);
                glTexCoord2f(texCoordLeft, texCoordBottom);
                glVertex2f(i, j);
                glTexCoord2f(texCoordRight, texCoordBottom);
                glVertex2f(i + 1, j);
                glTexCoord2f(texCoordRight, texCoordTop);
                glVertex2f(i + 1, j + 1);
                glTexCoord2f(texCoordLeft, texCoordTop);
                glVertex2f(i, j + 1);
                glEnd();
            }
        }
    }

    glDisable(GL_TEXTURE_2D); // Disable texture mapping
}

// Define texture ID for food
GLuint foodTextureID;

void loadFoodTexture()
{
    // Load texture image for food
    foodTextureID = SOIL_load_OGL_texture(
        "E:\\projects\\snake-game\\resources\\textures\\apple.png", // Path to your food texture image
        SOIL_LOAD_AUTO,
        SOIL_CREATE_NEW_ID,
        SOIL_FLAG_INVERT_Y);

    // Check if texture loaded successfully
    if (foodTextureID == 0)
    {
        printf("Error loading food texture: %s\n", SOIL_last_result());
    }
}

void drawFood()
{
    // Load texture image
    GLuint foodTextureID = SOIL_load_OGL_texture(
        "E:\\projects\\snake-game\\resources\\textures\\apple.png",
        SOIL_LOAD_AUTO,
        SOIL_CREATE_NEW_ID,
        SOIL_FLAG_INVERT_Y);

    // Check if texture loaded successfully
    if (foodTextureID == 0)
    {
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
        food.x, food.y + foodSize};

    // Define texture coordinates
    float textureCoordinates[8] = {
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f};

    // Draw food with texture
    glBegin(GL_QUADS);
    for (int i = 0; i < 4; ++i)
    {
        glTexCoord2f(textureCoordinates[2 * i], textureCoordinates[2 * i + 1]);
        glVertex2f(foodVertices[2 * i], foodVertices[2 * i + 1]);
    }
    glEnd();

    // Disable texture mapping
    glDisable(GL_TEXTURE_2D);
}

void drawMegaFood()
{
    // Load texture image
    GLuint foodTextureID = SOIL_load_OGL_texture(
        "E:\\projects\\snake-game\\resources\\textures\\apple.png",
        SOIL_LOAD_AUTO,
        SOIL_CREATE_NEW_ID,
        SOIL_FLAG_INVERT_Y);

    // Check if texture loaded successfully
    if (foodTextureID == 0)
    {
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
    float foodSize = 1.5f; // Size of the food square
    float foodVertices[8] = {
        megaFood.x, megaFood.y,
        megaFood.x + foodSize, megaFood.y,
        megaFood.x + foodSize, megaFood.y + foodSize,
        megaFood.x, megaFood.y + foodSize};

    // Define texture coordinates
    float textureCoordinates[8] = {
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f};

    // Draw food with texture
    glBegin(GL_QUADS);
    for (int i = 0; i < 4; ++i)
    {
        glTexCoord2f(textureCoordinates[2 * i], textureCoordinates[2 * i + 1]);
        glVertex2f(foodVertices[2 * i], foodVertices[2 * i + 1]);
    }
    glEnd();

    // Disable texture mapping
    glDisable(GL_TEXTURE_2D);
}

// Generate food at a random position
void generateFood()
{
    do
    {
        food.x = rand() % (WIDTH - 2) + 1; // Exclude the walls
        food.y = rand() % (HEIGHT - 2) + 1;
    } while (walls[food.x][food.y] && bushes[food.x][food.y]); // Keep generating until not on a wall

    // Ensure food is not on snake
    for (int i = 0; i < snake.length; ++i)
    {
        if (snake.body[i].x == food.x && snake.body[i].y == food.y)
        {
            generateFood(); // Recursively generate food again
            return;
        }
    }
}

void generateMegaFood()
{
    do
    {
        megaFood.x = rand() % (WIDTH - 2) + 1; // Exclude the walls
        megaFood.y = rand() % (HEIGHT - 2) + 1;
    } while (walls[megaFood.x][megaFood.y] && bushes[megaFood.x][megaFood.y]); // Keep generating until not on a wall

    // Ensure food is not on snake
    for (int i = 0; i < snake.length; ++i)
    {
        if (snake.body[i].x == megaFood.x && snake.body[i].y == megaFood.y)
        {
            generateMegaFood(); // Recursively generate food again
            return;
        }
    }
}

void drawScorePanel()
{
    // Define panel colors
    GLfloat panelColor1[3] = {0.2f, 0.2f, 0.2f}; // Dark gray
    GLfloat panelColor2[3] = {0.4f, 0.4f, 0.4f}; // Light gray

    // Set the color to black for the panel background
    glColor3fv(panelColor1);

    // Draw the background rectangle
    glBegin(GL_QUADS);
    glVertex2f(WIDTH - PANEL_SIZE, HEIGHT);
    glVertex2f(WIDTH, HEIGHT);
    glVertex2f(WIDTH, HEIGHT - PANEL_SIZE);
    glVertex2f(WIDTH - PANEL_SIZE, HEIGHT - PANEL_SIZE);
    glEnd();

    // Draw a border around the panel
    glColor3f(1.0f, 1.0f, 1.0f); // White border color
    glLineWidth(2.0f);           // Set border line width
    glBegin(GL_LINE_LOOP);
    glVertex2f(WIDTH - PANEL_SIZE, HEIGHT);
    glVertex2f(WIDTH, HEIGHT);
    glVertex2f(WIDTH, HEIGHT - PANEL_SIZE);
    glVertex2f(WIDTH - PANEL_SIZE, HEIGHT - PANEL_SIZE);
    glEnd();

    // Draw a gradient background
    glBegin(GL_QUADS);
    glColor3fv(panelColor1);
    glVertex2f(WIDTH - PANEL_SIZE, HEIGHT);
    glVertex2f(WIDTH, HEIGHT);
    glColor3fv(panelColor2);
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
    glTexCoord2f(0.0f, 0.0f);
    glVertex2f(panelCenterX - halfIconSize, panelCenterY - halfIconSize);
    glTexCoord2f(1.0f, 0.0f);
    glVertex2f(panelCenterX + halfIconSize, panelCenterY - halfIconSize);
    glTexCoord2f(1.0f, 1.0f);
    glVertex2f(panelCenterX + halfIconSize, panelCenterY + halfIconSize);
    glTexCoord2f(0.0f, 1.0f);
    glVertex2f(panelCenterX - halfIconSize, panelCenterY + halfIconSize);
    glEnd();

    // Disable texture mapping
    glDisable(GL_TEXTURE_2D);
}

void drawScore()
{
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
    for (int i = 0; buffer[i] != '\0'; i++)
    {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, buffer[i]);
    }
}

void drawGameOver()
{
    // Set background color
    // glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Black background

    // Set text color
    glColor3f(1.0f, 0.0f, 0.0f); // Red text

    // Set position for text
    float textPosX = WIDTH / 2 - 7;
    float textPosY = HEIGHT / 2;

    // Draw text
    glRasterPos2f(textPosX, textPosY);
    const char *text = "Game Over - Press Enter to Restart";
    glutBitmapString(GLUT_BITMAP_HELVETICA_18, (unsigned char *)text);
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();

    switch (currentScreen)
    {
    case HOME_SCREEN:
        drawHomeScreen();
        break;
    case GAME_SCREEN:
        drawSnake();
        drawWalls();  // Draw walls
        drawBushes(); // Draw walls
        drawFood();   // Draw food
        if (isMegaFruitSpawned) drawMegaFood(); // Draw food
        drawScorePanel();   // Draw panel
        drawScore();        // Draw the score panel
        if (snake.game_over)
            drawGameOver();
        break;
    }

    glutSwapBuffers();
}

void reshape(int w, int h)
{
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, WIDTH, 0, HEIGHT);
    glMatrixMode(GL_MODELVIEW);
}

void update(int value)
{
    if (!snake.game_over)
    {
        if (currentScreen != HOME_SCREEN)
        {
            if (difftime(time(NULL), lastFruitSpawnTime) >= megaFruitSpawnInterval)
            {
                generateMegaFood();
                lastFruitSpawnTime = time(NULL);
                isMegaFruitSpawned = true;
            }

            // Move the snake
            for (int i = snake.length - 1; i > 0; --i)
            {
                snake.body[i] = snake.body[i - 1];
            }

            if (snake.direction == 0)
                snake.body[0].y++;
            else if (snake.direction == 1)
                snake.body[0].x++;
            else if (snake.direction == 2)
                snake.body[0].y--;
            else if (snake.direction == 3)
                snake.body[0].x--;

            // Check for collision with walls
            if (walls[snake.body[0].x][snake.body[0].y])
            {
                snake.game_over = true;
            }

            // Check for collision with obstacles
            if (bushes[snake.body[0].x][snake.body[0].y])
            {
                snake.game_over = true;
            }

            // Check for collision with food
            if (snake.body[0].x == food.x && snake.body[0].y == food.y)
            {
                snake.length++;
                snake.score++;                      // Increment score
                generateFood();                     // Generate new food
                printf("Score: %d\n", snake.score); // Debug output
            }

            // Check for collision with mega food
            if (snake.body[0].x == megaFood.x && snake.body[0].y == megaFood.y)
            {
                snake.length+= 10;
                snake.score += 10;  // Increase score by 10
                isMegaFruitSpawned = false;
                printf("Score: %d\n", snake.score); // Debug output
            }

            // check forcollision with tale
            if (snake.body[0].x == snake.body[snake.length - 1].x && snake.body[0].y == snake.body[snake.length - 1].y)
            {
                snake.game_over = true;
            }
        }

        // Redraw
        glutPostRedisplay();
        glutTimerFunc(100, update, 0);
    }
}

void drawHomeScreen()
{
    glColor3f(0.0f, 1.0f, 0.0f); // Green text

    // Draw "Start Game" text
    float startTextPosX = WIDTH / 2 - 4;
    float startTextPosY = HEIGHT / 2;

    glRasterPos2f(startTextPosX, startTextPosY);
    const char *startText = "Press ENTER";
    glutBitmapString(GLUT_BITMAP_HELVETICA_18, (unsigned char *)startText);

    // Draw map selection
    glColor3f(0.0f, 0.0f, 0.0f); // Cyan text

    float mapTextPosX = WIDTH / 2 - 4;
    float mapTextPosY = HEIGHT / 2 - 1;

    glRasterPos2f(mapTextPosX, mapTextPosY);
    const char *mapText = "Select Map:";
    glutBitmapString(GLUT_BITMAP_HELVETICA_12, (unsigned char *)mapText);

    for (size_t i = 0; i < numMaps; i++)
    {
        float mapPosX = WIDTH / 2 - 4;
        float mapPosY = HEIGHT / 2 - 2 - i;

        glRasterPos2f(mapPosX, mapPosY);
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, '1' + i);
        glutBitmapString(GLUT_BITMAP_HELVETICA_12, (unsigned char *)mapList[i]->name);
    }

    // Highlight the selected map
    glColor3f(1.0f, 0.0f, 0.0f); // Red text for selected map
    float selectedMapPosX = WIDTH / 2 - 5;
    float selectedMapPosY = HEIGHT / 2 - 2 - selectedMapIndex;

    glRasterPos2f(selectedMapPosX, selectedMapPosY);
    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, '>');

    glutPostRedisplay(); // Redraw the screen to reflect the changes
    // glutTimerFunc(100, update, 0);
}

void keyboard(unsigned char key, int x, int y)
{
    if (currentScreen == HOME_SCREEN && key == 13)
    { // 13 is the ASCII code for Enter key
        currentScreen = GAME_SCREEN;
        printf("ENTER pressed : %d", selectedMapIndex);
        initialize(); // Start the game
    }
    else
    {
        switch (key)
        {
        case 27: // ASCII code for ESC key
            exit(0);
            break;
        }
    }
}

void specialKeys(int key, int x, int y)
{
    if (currentScreen != HOME_SCREEN)
    {
        switch (key)
        {
        case GLUT_KEY_UP:
            if (snake.direction != 2)
                snake.direction = 0;
            break;
        case GLUT_KEY_DOWN:
            if (snake.direction != 0)
                snake.direction = 2;
            break;
        case GLUT_KEY_LEFT:
            if (snake.direction != 1)
                snake.direction = 3;
            break;
        case GLUT_KEY_RIGHT:
            if (snake.direction != 3)
                snake.direction = 1;
            break;
        }
    }
    else if (currentScreen == HOME_SCREEN)
    {
        switch (key)
        {
        case GLUT_KEY_UP:
            if (selectedMapIndex > 0)
            {
                selectedMapIndex--;
            }
            break;
        case GLUT_KEY_DOWN:
            if (selectedMapIndex < numMaps - 1)
            {
                selectedMapIndex++;
            }
            break;
        }
        glutPostRedisplay(); // Redraw the screen to reflect the changes
        // glutTimerFunc(100, update, 0);
    }
}

void loadMapsFromFile(const char *filename)
{
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        printf("Error opening file.\n");
        exit(1);
    }

    char line[1024];
    fgets(line, sizeof(line), file); // Read the header line and discard it

    while (fgets(line, sizeof(line), file))
    {
        char *token;
        token = strtok(line, ",");
        // Remove quotes from the map name
        strncpy(mapList[numMaps]->name, token + 1, strlen(token) - 2);
        mapList[numMaps]->name[strlen(token) - 2] = '\0'; // Ensure null-termination

        token = strtok(NULL, ",");
        printf("token : %s", token);
        char *coords = strtok(token, ";"); // Get the first coordinate
        int i = 0;
        while (coords != NULL && i < MAX_POINTS)
        {
            sscanf(coords, "%d|%d", &mapList[numMaps]->schema[i].x, &mapList[numMaps]->schema[i].y);
            i++;
            coords = strtok(NULL, ";"); // Get the next coordinate
            printf("coor : %d", coords);
        }
        mapList[numMaps]->schemaSize = i; // Store the size of the schema

        numMaps++;
    }

    fclose(file);
}

int main(int argc, char **argv)
{
    loadMapsFromFile("E:\\projects\\snake-game\\resources\\maps.txt"); // Load maps from file

    //initialize();
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(WIDTH * 20, HEIGHT * 20);
    glutCreateWindow("Snake Game");
    glutDisplayFunc(display);

    loadTexture(); // Load texture

    glClearColor(1.0, 1.0, 1.0, 1.0); // Set background color to white
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-1, 1, -1, 1, -1, 1);

    glutReshapeFunc(reshape);
    glutTimerFunc(100, update, 0);
    glutSpecialFunc(specialKeys); // use arrows
    glutKeyboardFunc(keyboard);
    glutMainLoop();
    return 0;
}
