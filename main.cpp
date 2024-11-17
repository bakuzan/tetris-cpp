#define UNICODE
#include <chrono>
#include <iostream>
#include <stdio.h>
#include <thread>
#include <vector>
#include <Windows.h>

using namespace std::chrono_literals;

// Globals
CONSOLE_SCREEN_BUFFER_INFO sbInfo;

std::wstring tetrominoes[7];

int pieceSize = 4;
int screenWidth = 80, screenHeight = 30;

int fieldWidth = 12, fieldHeight = 18;
unsigned char *pField = nullptr;

// Globals END

void DisableEcho()
{
    HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
    DWORD mode;
    GetConsoleMode(hStdin, &mode);
    mode &= ~ENABLE_ECHO_INPUT; // Disable echo input
    SetConsoleMode(hStdin, mode);
}

void EnableEcho()
{
    HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
    DWORD mode;
    GetConsoleMode(hStdin, &mode);
    mode |= ENABLE_ECHO_INPUT; // Enable echo input
    SetConsoleMode(hStdin, mode);
}

int Rotate(int x, int y, int rotations)
{
    int w = pieceSize;
    switch (rotations % 4)
    {
    case 0:
        return w * y + x;
    case 1:
        return (w * (w - 1)) + y - (w * x);
    case 2:
        return ((w * w) - 1) - (w * y) - x;
    case 3:
        return (w - 1) - y + (w * x);
    default:
        throw "ROTATION NOT HANDLED";
    }
}

bool CanPieceMove(int tetromino, int rotation, int x, int y)
{
    for (int px = 0; px < pieceSize; px++)
    {
        for (int py = 0; py < pieceSize; py++)
        {
            int pieceIndex = Rotate(px, py, rotation);
            int fieldIndex = (y + py) * fieldWidth + (x + px);

            if (x + px >= 0 && x + px < fieldWidth)
            {
                if (y + py >= 0 && y + py < fieldHeight)
                {
                    // Check if spot is occupied
                    if (tetrominoes[tetromino][pieceIndex] == L'X' && pField[fieldIndex] != 0)
                    {
                        return false;
                    }
                }
            }
        }
    }

    return true;
}

int main()
{
    DisableEcho();

    // Console size check
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &sbInfo);
    screenWidth = sbInfo.dwSize.X;
    screenHeight = sbInfo.dwSize.Y;

    // Assets
    tetrominoes[0].append(L"..X.");
    tetrominoes[0].append(L"..X.");
    tetrominoes[0].append(L"..X.");
    tetrominoes[0].append(L"..X.");

    tetrominoes[1].append(L"....");
    tetrominoes[1].append(L"..X.");
    tetrominoes[1].append(L"..X.");
    tetrominoes[1].append(L".XX.");

    tetrominoes[2].append(L"....");
    tetrominoes[2].append(L".X..");
    tetrominoes[2].append(L".X..");
    tetrominoes[2].append(L".XX.");

    tetrominoes[3].append(L"....");
    tetrominoes[3].append(L".XX.");
    tetrominoes[3].append(L".XX.");
    tetrominoes[3].append(L"....");

    tetrominoes[4].append(L".X..");
    tetrominoes[4].append(L".XX.");
    tetrominoes[4].append(L"..X.");
    tetrominoes[4].append(L"....");

    tetrominoes[5].append(L"..X.");
    tetrominoes[5].append(L".XX.");
    tetrominoes[5].append(L"..X.");
    tetrominoes[5].append(L"....");

    tetrominoes[6].append(L"..X.");
    tetrominoes[6].append(L".XX.");
    tetrominoes[6].append(L".X..");
    tetrominoes[6].append(L"....");

    // Create playing field with walls
    pField = new unsigned char[fieldWidth * fieldHeight];
    for (int x = 0; x < fieldWidth; x++)
    {
        for (int y = 0; y < fieldHeight; y++)
        {
            pField[(fieldWidth * y) + x] = (x == 0 || x == fieldWidth - 1 || y == fieldHeight - 1)
                                               ? 9
                                               : 0;
        }
    }

    // Setup for writing to console via buffer
    wchar_t *screen = new wchar_t[screenWidth * screenHeight];
    for (int i = 0; i < screenWidth * screenHeight; i++)
    {
        screen[i] = L' ';
    }

    HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
    SetConsoleActiveScreenBuffer(hConsole);
    DWORD dwBytesWritten = 0;

    // Game State
    bool gameOver = false;

    int nextPiece = rand() % 7;    // Random piece next!
    int currentPiece = rand() % 7; // Random starting piece
    int currentRotation = 0;
    int currentX = fieldWidth / 2;
    int currentY = 0;

    bool keys[4];
    bool rotateHold = false;

    int speed = 20;
    int speedCounter = 0;
    bool forceDown = false;
    int pieceCounter = 0;
    int score = 0;
    int lineCount = 0;

    std::vector<int> lines;

    // Game State END

    while (!gameOver)
    {
        // Time
        std::this_thread::sleep_for(50ms);
        speedCounter++;
        forceDown = (speedCounter == speed);

        /*
         * INPUT START
         */
        for (int k = 0; k < pieceSize; k++)
        {
            keys[k] = (0x8000 & GetAsyncKeyState((unsigned char)("ASDZ"[k]))) != 0;
        }

        // LEFT
        if (keys[0] && CanPieceMove(currentPiece, currentRotation, currentX - 1, currentY))
        {
            currentX -= 1;
        }
        // DOWN
        if (keys[1] && CanPieceMove(currentPiece, currentRotation, currentX, currentY + 1))
        {
            currentY += 1;
        }
        // RIGHT
        if (keys[2] && CanPieceMove(currentPiece, currentRotation, currentX + 1, currentY))
        {
            currentX += 1;
        }
        // ROTATE
        if (keys[3])
        {
            if (!rotateHold && CanPieceMove(currentPiece, currentRotation + 1, currentX, currentY))
            {
                currentRotation += 1;
                rotateHold = true;
            }
        }
        else
        {
            rotateHold = false;
        }
        /*
         * INPUT END
         */

        /*
         * LOGIC START
         */
        if (forceDown)
        {
            if (CanPieceMove(currentPiece, currentRotation, currentX, currentY + 1))
            {
                currentY++;
            }
            else
            {
                // Lock current piece
                for (int px = 0; px < pieceSize; px++)
                {
                    for (int py = 0; py < pieceSize; py++)
                    {
                        if (tetrominoes[currentPiece][Rotate(px, py, currentRotation)] == L'X')
                        {
                            pField[(fieldWidth * (currentY + py)) + (currentX + px)] = currentPiece + 1;
                        }
                    }
                }

                // Piece locked, get harder?
                pieceCounter++;
                if (pieceCounter % 10 == 0)
                {
                    if (speed >= 10)
                    {
                        speed--;
                    }
                }

                // Complete line?
                for (int py = 0; py < pieceSize; py++)
                {
                    if (currentY + py < fieldHeight - 1)
                    {
                        bool line = true;
                        for (int px = 1; px < fieldWidth - 1; px++)
                        {
                            line &= (pField[(fieldWidth * (currentY + py)) + px]) != 0;
                        }

                        if (line)
                        {
                            // Display completed line, will be shortly removed
                            for (int px = 1; px < fieldWidth - 1; px++)
                            {
                                pField[(fieldWidth * (currentY + py)) + px] = 8;
                            }

                            lines.push_back(currentY + py);
                        }
                    }
                }

                // Update score
                score += 25;
                if (!lines.empty())
                {
                    score += (1 << lines.size()) * 100;
                }

                // Set next piece
                currentX = fieldWidth / 2;
                currentY = 0;
                currentRotation = 0;
                currentPiece = nextPiece; // Move next piece to current
                nextPiece = rand() % 7;   // Random next piece

                // Is game over?
                gameOver = !CanPieceMove(currentPiece, currentRotation, currentX, currentY);
            }

            speedCounter = 0;
        }
        /*
         * LOGIC END
         */

        /*
         * DRAW START
         */
        int drawOffset = 2;
        for (int x = 0; x < fieldWidth; x++)
        {
            for (int y = 0; y < fieldHeight; y++)
            {
                screen[(screenWidth * (y + drawOffset)) + (x + drawOffset)] = L" CBOYGPR=#"[pField[(fieldWidth * y) + x]];
            }
        }

        // Draw Piece
        for (int px = 0; px < pieceSize; px++)
        {
            for (int py = 0; py < pieceSize; py++)
            {
                if (tetrominoes[currentPiece][Rotate(px, py, currentRotation)] == L'X')
                {
                    screen[(screenWidth * (currentY + py + drawOffset)) + (currentX + px + drawOffset)] = L"CBOYGPR"[currentPiece];
                }
            }
        }

        // Draw score
        swprintf_s(&screen[2 * screenWidth + fieldWidth + 6], 16, L"SCORE: %8d", score);
        swprintf_s(&screen[3 * screenWidth + fieldWidth + 6], 16, L"LINES: %8d", lineCount);
        swprintf_s(&screen[5 * screenWidth + fieldWidth + 6], 16, L"Next Piece:");

        // Draw Next Piece Preview
        for (int px = 0; px < pieceSize; px++)
        {
            for (int py = 0; py < pieceSize; py++)
            {
                bool solid = tetrominoes[nextPiece][Rotate(px, py, 0)] == L'X';
                screen[((7 + py) * screenWidth) + fieldWidth + 9 + px] = solid
                                                                             ? L"CBOYGPR"[nextPiece]
                                                                             : L" "[0];
            }
        }

        // Wait! It has completed lines?
        if (!lines.empty())
        {
            // Let complete line be shown for a while
            WriteConsoleOutputCharacter(hConsole, screen, screenWidth * screenHeight, {0, 0}, &dwBytesWritten);
            std::this_thread::sleep_for(400ms);

            // Remove completed lines
            for (auto &l : lines)
            {
                for (int px = 1; px < fieldWidth - 1; px++)
                {
                    for (int py = l; py > 0; py--)
                    {
                        pField[(fieldWidth * py) + px] = pField[(fieldWidth * (py - 1)) + px];
                    }

                    pField[px] = 0;
                }
            }

            lineCount += lines.size();
            lines.clear();
        }
        /*
         * DRAW END
         */

        // Display
        WriteConsoleOutputCharacter(hConsole, screen, screenWidth * screenHeight, {0, 0}, &dwBytesWritten);
    }

    // Game over, tidy up
    CloseHandle(hConsole);
    std::cout << "Game Over!" << '\n';
    std::cout << "Score: " << score << std::endl;

    // TODO
    // Create a highscore list in txt file!
    // Add people with highscores to the list!
    // Display the highscores!

    std::cout << '\n'
              << "Press any key to exit..." << std::endl;
    getchar();

    EnableEcho();

    return 0;
}