#include <GL/glut.h>
#include <iostream>
#include <vector>
#include <ctime>
#include <string> 

// --- Ukuran Blok & Tinggi Grid ---
#define BLOCK_SIZE 40   // Ukuran blok 
#define GRID_HEIGHT 16  // Tinggi grid 
#define GRID_WIDTH 20   // Lebar grid 

// --- Tentukan ukuran Window & Area Game ---
#define WINDOW_WIDTH 1152
#define WINDOW_HEIGHT 648
#define GAME_AREA_WIDTH (GRID_WIDTH * BLOCK_SIZE)

// Struktur untuk menggambarkan tetromino
struct Tetromino {
    std::vector<std::vector<int>> shape;
    int x, y;
    int color;
};

// Global variables
int grid[GRID_WIDTH][GRID_HEIGHT] = {0};
Tetromino currentTetromino;
int score = 0;
bool gameOver = false;

// Bentuk Tetromino (I, O, T, S, Z, J, L)
std::vector<std::vector<std::vector<int>>> tetrominoShapes = {
    {{1, 1, 1, 1}}, // I
    {{1, 1}, {1, 1}}, // O
    {{0, 1, 0}, {1, 1, 1}}, // T
    {{0, 1, 1}, {1, 1, 0}}, // S
    {{1, 1, 0}, {0, 1, 1}}, // Z
    {{1, 0, 0}, {1, 1, 1}}, // J
    {{0, 0, 1}, {1, 1, 1}}  // L
};

// Warna untuk setiap tetromino
float colors[7][3] = {
    {0.0, 1.0, 1.0}, // Cyan (I)
    {1.0, 1.0, 0.0}, // Yellow (O)
    {0.5, 0.0, 0.5}, // Purple (T)
    {0.0, 1.0, 0.0}, // Green (S)
    {1.0, 0.0, 0.0}, // Red (Z)
    {0.0, 0.0, 1.0}, // Blue (J)
    {1.0, 0.5, 0.0}  // Orange (L)
};

void drawBlock(int x, int y, int colorIndex) {
    float r = colors[colorIndex][0];
    float g = colors[colorIndex][1];
    float b = colors[colorIndex][2];

    glColor3f(r, g, b);
    glBegin(GL_QUADS);
    glVertex2f(x * BLOCK_SIZE, y * BLOCK_SIZE);
    glVertex2f((x + 1) * BLOCK_SIZE, y * BLOCK_SIZE);
    glVertex2f((x + 1) * BLOCK_SIZE, (y + 1) * BLOCK_SIZE);
    glVertex2f(x * BLOCK_SIZE, (y + 1) * BLOCK_SIZE);
    glEnd();

    glColor3f(0.0, 0.0, 0.0);
    glLineWidth(2);
    glBegin(GL_LINE_LOOP);
    glVertex2f(x * BLOCK_SIZE, y * BLOCK_SIZE);
    glVertex2f((x + 1) * BLOCK_SIZE, y * BLOCK_SIZE);
    glVertex2f((x + 1) * BLOCK_SIZE, (y + 1) * BLOCK_SIZE);
    glVertex2f(x * BLOCK_SIZE, (y + 1) * BLOCK_SIZE);
    glEnd();
}

void spawnNewTetromino() {
    int shapeIndex = rand() % tetrominoShapes.size();
    currentTetromino.shape = tetrominoShapes[shapeIndex];
    currentTetromino.color = shapeIndex;
    currentTetromino.x = GRID_WIDTH / 2 - currentTetromino.shape[0].size() / 2;
    currentTetromino.y = 0;
}

bool checkCollision(int newX, int newY, const std::vector<std::vector<int>>& shape) {
    for (int y = 0; y < shape.size(); ++y) {
        for (int x = 0; x < shape[y].size(); ++x) {
            if (shape[y][x] == 1) {
                int gridX = newX + x;
                int gridY = newY + y;

                if (gridX < 0 || gridX >= GRID_WIDTH || gridY >= GRID_HEIGHT) {
                    return true;
                }
                if (gridY >= 0 && grid[gridX][gridY] != 0) {
                    return true;
                }
            }
        }
    }
    return false;
}

void placeTetromino() {
    for (int y = 0; y < currentTetromino.shape.size(); ++y) {
        for (int x = 0; x < currentTetromino.shape[y].size(); ++x) {
            if (currentTetromino.shape[y][x] == 1) {
                grid[currentTetromino.x + x][currentTetromino.y + y] = currentTetromino.color + 1;
            }
        }
    }
}

void clearLines() {
    for (int y = GRID_HEIGHT - 1; y >= 0; --y) {
        bool lineIsFull = true;
        for (int x = 0; x < GRID_WIDTH; ++x) {
            if (grid[x][y] == 0) {
                lineIsFull = false;
                break;
            }
        }

        if (lineIsFull) {
            score += 100;
            for (int k = y; k > 0; --k) {
                for (int x = 0; x < GRID_WIDTH; ++x) {
                    grid[x][k] = grid[x][k - 1];
                }
            }
            y++;
        }
    }
}

void rotateTetromino() {
    std::vector<std::vector<int>> newShape(currentTetromino.shape[0].size(), std::vector<int>(currentTetromino.shape.size()));
    for (int y = 0; y < currentTetromino.shape.size(); ++y) {
        for (int x = 0; x < currentTetromino.shape[y].size(); ++x) {
            newShape[x][currentTetromino.shape.size() - 1 - y] = currentTetromino.shape[y][x];
        }
    }
    if (!checkCollision(currentTetromino.x, currentTetromino.y, newShape)) {
        currentTetromino.shape = newShape;
    }
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();

    // Gambar background untuk area skor
    glColor3f(0.2, 0.2, 0.25);
    glBegin(GL_QUADS);
        glVertex2i(GAME_AREA_WIDTH, 0);
        glVertex2i(WINDOW_WIDTH, 0);
        glVertex2i(WINDOW_WIDTH, WINDOW_HEIGHT);
        glVertex2i(GAME_AREA_WIDTH, WINDOW_HEIGHT);
    glEnd();
    
    // Gambar grid yang terisi
    for (int x = 0; x < GRID_WIDTH; ++x) {
        for (int y = 0; y < GRID_HEIGHT; ++y) {
            if (grid[x][y] != 0) {
                drawBlock(x, y, grid[x][y] - 1);
            }
        }
    }

    // Gambar tetromino yang jatuh
    if (!gameOver) {
        for (int y = 0; y < currentTetromino.shape.size(); ++y) {
            for (int x = 0; x < currentTetromino.shape[y].size(); ++x) {
                if (currentTetromino.shape[y][x] == 1) {
                    drawBlock(currentTetromino.x + x, currentTetromino.y + y, currentTetromino.color);
                }
            }
        }
    }

    // Memunculkan score pada area skor
    glColor3f(1.0, 1.0, 1.0); // White text color
    glRasterPos2i(GAME_AREA_WIDTH + 50, 50); 
    std::string scoreText = "Score: " + std::to_string(score);
    for(char c : scoreText) {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, c);
    }
    
    // Memunculkan Teks "Game Over" pada layar
    if (gameOver) {
        glColor3f(1.0, 0.0, 0.0);
        glRasterPos2i(GAME_AREA_WIDTH / 2 - 70, WINDOW_HEIGHT / 2);
        std::string text = "GAME OVER";
        for (char c : text) {
            glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, c);
        }
    }

    glutSwapBuffers();
}

void update(int value) {
    if (gameOver) return;

    if (!checkCollision(currentTetromino.x, currentTetromino.y + 1, currentTetromino.shape)) {
        currentTetromino.y++;
    } else {
        placeTetromino();
        clearLines();
        spawnNewTetromino();
        if (checkCollision(currentTetromino.x, currentTetromino.y, currentTetromino.shape)) {
            gameOver = true;
            std::cout << "Game Over! Final Score: " << score << std::endl;
        }
    }
    glutPostRedisplay();
    glutTimerFunc(500, update, 0);
}

void handleKeypress(int key, int, int) {
    if (gameOver) return;
    switch (key) {
        case GLUT_KEY_LEFT:
            if (!checkCollision(currentTetromino.x - 1, currentTetromino.y, currentTetromino.shape)) currentTetromino.x--;
            break;
        case GLUT_KEY_RIGHT:
            if (!checkCollision(currentTetromino.x + 1, currentTetromino.y, currentTetromino.shape)) currentTetromino.x++;
            break;
        case GLUT_KEY_DOWN:
            if (!checkCollision(currentTetromino.x, currentTetromino.y + 1, currentTetromino.shape)) currentTetromino.y++;
            break;
        case GLUT_KEY_UP:
            rotateTetromino();
            break;
    }
    glutPostRedisplay();
}

void init() {
    glClearColor(0.1, 0.1, 0.2, 1.0); // Game area background color
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    glMatrixMode(GL_MODELVIEW);
    srand(time(0));
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Simple Tetris - C++ OpenGL");

    init();
    spawnNewTetromino();

    glutDisplayFunc(display);
    glutSpecialFunc(handleKeypress);
    glutTimerFunc(0, update, 0);

    glutMainLoop();
    return 0;
}
