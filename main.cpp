#define UNICODE
#include <iostream>
#include <stdio.h>
#include <Windows.h>

// Globals
CONSOLE_SCREEN_BUFFER_INFO sbInfo;

std::wstring tetrominoes[7];

int pieceSize = 4;
int screenWidth = 80, screenHeight = 30;

int fieldWidth = 12, fieldHeight = 18;
unsigned char *pField = nullptr;

// Globals END

int Rotate(int x, int y, int rotations)
{
    int w = pieceSize;
    switch (rotations)
    {
    case 0:
        return w * y + x;
    case 1:
        return (w * (w - 1)) + y + (w * x);
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

void Draw(wchar_t *screen, int currentPiece, int currentRotation, int currentX, int currentY)
{
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
}

int main()
{
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

    int currentPiece = 0;
    int currentRotation = 0;
    int currentX = fieldWidth / 2;
    int currentY = 0;
    // Game State END

    while (!gameOver)
    {
        // Time

        // Input

        // Logic

        // Draw
        Draw(screen, currentPiece, currentRotation, currentX, currentY);

        // Display
        WriteConsoleOutputCharacter(hConsole, screen, screenWidth * screenHeight, {0, 0}, &dwBytesWritten);
    }

    return 0;
}