#include <vector>
#include <thread>
#include <chrono>
#include <Windows.h>

std::vector<std::thread> testVec;
int playerX = 100;
int playerY = 100;
HWND hwnd;
int fps = 144;

void moveTest() {
    const std::chrono::duration<double> fpsDur = std::chrono::duration<double>(1.0 / fps);

    auto initialT = std::chrono::steady_clock::now();


    while (true) {
        auto currentT = std::chrono::steady_clock::now();
        auto deltaTime = std::chrono::duration_cast<std::chrono::duration<double>>(currentT - initialT);

        if (deltaTime >= fpsDur) {
            ++playerX;

            PostMessage(hwnd, WM_USER + 1, 0, 0);

            initialT = currentT;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        case WM_PAINT:
            {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);

            // Fill the entire client area with a background color
            RECT clientRect;
            GetClientRect(hwnd, &clientRect);
            HBRUSH hBackgroundBrush = CreateSolidBrush(RGB(255, 255, 255)); // White
            FillRect(hdc, &clientRect, hBackgroundBrush);
            DeleteObject(hBackgroundBrush);

            // Drawing code here
            RECT rect = { playerX, playerY, playerX + 100, playerY + 100 };

            // Create a red brush
            HBRUSH hBrush = CreateSolidBrush(RGB(255, 0, 0)); // Red
            // Select the red brush into the device context
            HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);

            // Draw a filled rectangle
            Rectangle(hdc, rect.left, rect.top, rect.right, rect.bottom);

            // Restore the original brush
            SelectObject(hdc, hOldBrush);
            // Free the created brush
            DeleteObject(hBrush);

            EndPaint(hwnd, &ps);
            }
            return 0;
        case WM_USER + 1:
            // Update the window when notified
            InvalidateRect(hwnd, NULL, TRUE);
            UpdateWindow(hwnd);
            return 0;
        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    FreeConsole(); // Detach from the console

    WNDCLASSW wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = L"MyWindowClass";

    RegisterClassW(&wc);

    hwnd = CreateWindowExW(
        0,
        L"MyWindowClass",
        L"M",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        500,
        500,
        NULL,
        NULL,
        hInstance,
        NULL
    );

    if (hwnd == NULL) {
        MessageBoxW(NULL, L"Window creation failed!", L"Error", MB_ICONERROR);
        return 1;
    }

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);
    testVec.emplace_back(moveTest);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}
