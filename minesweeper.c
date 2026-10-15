#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Reveal a cell and recursively reveal neighbouring empty cells
void reveal(char **board, int **mines, int **neighbourMines,
            int size, int row, int col, int *safeCells) {

    // Stop if the cell is outside the board
    if (row < 0 || row >= size || col < 0 || col >= size)
        return;

    // Stop if the cell has already been revealed or flagged
    if (board[row][col] != '.')
        return;

    // Do not reveal a cell containing a mine
    if (mines[row][col] == 1)
        return;

    // Reveal the number of neighbouring mines
    if (neighbourMines[row][col] != 0) {
        board[row][col] = '0' + neighbourMines[row][col];
        (*safeCells)++;
        return;
    }

    // Reveal an empty cell
    board[row][col] = ' ';
    (*safeCells)++;

    // Examine all eight neighbouring cells
    for (int rowOffset = -1; rowOffset <= 1; rowOffset++) {
        for (int colOffset = -1; colOffset <= 1; colOffset++) {

            // Skip the current cell
            if (rowOffset == 0 && colOffset == 0)
                continue;

            // Recursively reveal the neighbouring cell
            reveal(board, mines, neighbourMines, size,
                   row + rowOffset, col + colOffset, safeCells);
        }
    }
}

// Print the board with row and column headers
void printBoard(char **board, int size) {
    printf("  ");

    // Print column headers
    for (int col = 0; col < size; col++)
        printf("%d ", col);

    printf("\n");

    // Print each row
    for (int row = 0; row < size; row++) {
        printf("%d ", row);

        for (int col = 0; col < size; col++)
            printf("%c ", board[row][col]);

        printf("\n");
    }
}

int main(void) {
    int size;

    // Ask the player to choose the board size
    printf("Enter board size (3-20): ");

    if (scanf("%d", &size) != 1) {
        printf("Invalid input. Enter a number.\n");
        return 1;
    }

    // Validate the board size
    if (size < 3 || size > 20) {
        printf("Board size must be between 3 and 20.\n");
        return 1;
    }

    int totalCells = size * size;
    int totalMines = totalCells / 3;
    int totalSafeCells = totalCells - totalMines;

    // Allocate memory for the visible board's row pointers
    char **board = malloc(size * sizeof(char *));

    if (board == NULL) {
        printf("Memory allocation failed.\n");
        return 1;
    }

    // Allocate memory for each row of the visible board
    for (int row = 0; row < size; row++) {
        board[row] = malloc(size * sizeof(char));

        if (board[row] == NULL) {
            printf("Memory allocation failed.\n");
            return 1;
        }
    }

    // Initialize every visible cell as hidden
    for (int row = 0; row < size; row++) {
        for (int col = 0; col < size; col++)
            board[row][col] = '.';
    }

    // Allocate memory for the mine grid
    int **mines = malloc(size * sizeof(int *));

    if (mines == NULL) {
        printf("Memory allocation failed.\n");
        return 1;
    }

    // Allocate and initialize each row of the mine grid
    for (int row = 0; row < size; row++) {
        mines[row] = calloc(size, sizeof(int));

        if (mines[row] == NULL) {
            printf("Memory allocation failed.\n");
            return 1;
        }
    }

    // Seed the random number generator
    srand(time(NULL));

    // Place mines in unique random positions
    int minesPlaced = 0;

    while (minesPlaced < totalMines) {
        int mineRow = rand() % size;
        int mineCol = rand() % size;

        if (mines[mineRow][mineCol] == 0) {
            mines[mineRow][mineCol] = 1;
            minesPlaced++;
        }
    }

    // Allocate memory for neighbouring mine counts
    int **neighbourMines = malloc(size * sizeof(int *));

    if (neighbourMines == NULL) {
        printf("Memory allocation failed.\n");
        return 1;
    }

    // Initialize neighbouring mine counts to zero
    for (int row = 0; row < size; row++) {
        neighbourMines[row] = calloc(size, sizeof(int));

        if (neighbourMines[row] == NULL) {
            printf("Memory allocation failed.\n");
            return 1;
        }
    }

    // Calculate neighbouring mine counts for every cell
    for (int row = 0; row < size; row++) {
        for (int col = 0; col < size; col++) {

            // Examine all eight neighbouring positions
            for (int rowOffset = -1; rowOffset <= 1; rowOffset++) {
                for (int colOffset = -1; colOffset <= 1; colOffset++) {

                    if (rowOffset == 0 && colOffset == 0)
                        continue;

                    int neighbourRow = row + rowOffset;
                    int neighbourCol = col + colOffset;

                    // Skip positions outside the board
                    if (neighbourRow < 0 || neighbourRow >= size ||
                        neighbourCol < 0 || neighbourCol >= size)
                        continue;

                    // Count the neighbouring mine
                    if (mines[neighbourRow][neighbourCol] == 1)
                        neighbourMines[row][col]++;
                }
            }
        }
    }

    // Initialize game state
    int row, col;
    int action;
    int gameRunning = 0;
    int safeCells = 0;

    // Continue until the player loses or wins
    while (gameRunning == 0 && safeCells != totalSafeCells) {

        // Display the action menu
        printf("\n1. Reveal cell\n");
        printf("2. Flag/Unflag cell\n");
        printf("Choose action: ");

        if (scanf("%d", &action) != 1) {
            printf("Invalid input. Enter a number.\n");
            return 1;
        }

        // Validate the selected action
        if (action != 1 && action != 2) {
            printf("Invalid action. Choose 1 or 2.\n");
            continue;
        }

        // Ask the player to select a cell
        printf("Enter row (0-%d): ", size - 1);

        if (scanf("%d", &row) != 1) {
            printf("Invalid input. Enter a number.\n");
            return 1;
        }

        printf("Enter column (0-%d): ", size - 1);

        if (scanf("%d", &col) != 1) {
            printf("Invalid input. Enter a number.\n");
            return 1;
        }

        // Validate coordinates before accessing the board
        if (row < 0 || row >= size || col < 0 || col >= size) {
            printf("Invalid cell coordinates.\n");
            continue;
        }

        // Handle flagging and unflagging
        if (action == 2) {
            if (board[row][col] == '.') {
                board[row][col] = 'F';
                printf("Cell flagged.\n");
            } else if (board[row][col] == 'F') {
                board[row][col] = '.';
                printf("Flag removed.\n");
            } else {
                printf("Cannot flag a revealed cell.\n");
            }

            // Display the updated board immediately
            printBoard(board, size);
            continue;
        }

        // Prevent revealing an already revealed or flagged cell
        if (board[row][col] != '.') {
            printf("Cell already revealed or flagged. Choose another one.\n");
            continue;
        }

        // Check whether the selected cell contains a mine
        if (mines[row][col] == 1) {
            board[row][col] = '*';
            gameRunning = 1;

            printf("BOOM! You hit a mine.\n");

            // Reveal all mines after the game ends
            for (int r = 0; r < size; r++) {
                for (int c = 0; c < size; c++) {
                    if (mines[r][c] == 1)
                        board[r][c] = '*';
                }
            }
        } else if (neighbourMines[row][col] != 0) {

            // Reveal the number of neighbouring mines
            board[row][col] = '0' + neighbourMines[row][col];
            safeCells++;

        } else {
            // Reveal the empty area and its neighbours
            reveal(board, mines, neighbourMines, size,
                   row, col, &safeCells);
        }

        // Display the updated board
        printBoard(board, size);
    }

    // Display the final game result
    if (gameRunning == 1)
        printf("YOU LOST!\n");

    if (safeCells == totalSafeCells)
        printf("YOU WON!\n");

    // Free memory allocated for the visible board
    for (int row = 0; row < size; row++)
        free(board[row]);

    free(board);

    // Free memory allocated for the mine grid
    for (int row = 0; row < size; row++)
        free(mines[row]);

    free(mines);

    // Free memory allocated for neighbouring mine counts
    for (int row = 0; row < size; row++)
        free(neighbourMines[row]);

    free(neighbourMines);

    return 0;
}