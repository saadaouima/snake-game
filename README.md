# Snake Game

Welcome to the Snake Game project! This is a simple implementation of the classic Snake game using OpenGL/GLUT.

## Table of Contents

- [Introduction](#introduction)
- [Installation](#installation)
- [Usage](#usage)
- [Game Description](#game-description)
- [Code Structure](#code-structure)
- [Compilation and Configuration](#compilation-and-configuration)
- [Troubleshooting](#troubleshooting)
- [Contributing](#contributing)
- [License](#license)

## Introduction

This project aims to recreate the classic Snake game in a simple and intuitive manner. It utilizes OpenGL/GLUT for graphics rendering and provides a straightforward gameplay experience.

## Installation

To install and run the game, follow these steps:

1. Clone the repository to your local machine:

    ```bash
    git clone https://github.com/your-username/snake-game.git
    ```

2. Navigate to the project directory:

    ```bash
    cd snake-game
    ```

3. Compile the code using your preferred C compiler. Make sure OpenGL/GLUT is installed and properly configured on your system.

    ```bash
    gcc main.c -o snake -lGL -lGLU -lglut
    ```

4. Run the compiled executable:

    ```bash
    ./snake
    ```

## Usage

Once the game is running, use the arrow keys to control the movement of the snake. Try to eat as much food as possible without colliding with the walls or yourself. Have fun!

## Game Description

The snake starts with a certain length and moves around the game board. Eating food increases the length of the snake, while colliding with walls or the snake's own body ends the game.

## Code Structure

The codebase is organized into several components:

- `main.c`: Contains the main game logic and OpenGL/GLUT setup.
- `README.md`: The documentation file you are currently reading.
- `LICENSE`: The license file specifying the project's usage terms.

## Compilation and Configuration

Ensure that you have OpenGL/GLUT installed on your system. You may need to adjust compiler and linker flags in the `gcc` command to properly link the OpenGL/GLUT libraries.

## Troubleshooting

If you encounter any issues during installation or gameplay, please refer to the [Troubleshooting](#troubleshooting) section of the wiki for assistance.

## Contributing

Contributions to this project are welcome! If you find any bugs or have ideas for improvements, feel free to submit a pull request or open an issue on GitHub.

## License

This project is licensed under the [MIT License](LICENSE).
