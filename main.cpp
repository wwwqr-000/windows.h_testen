#include <Windows.h>

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        case WM_PAINT:
            {
                PAINTSTRUCT ps;
                HDC hdc = BeginPaint(hwnd, &ps);

                // Drawing code here
                RECT rect = { 50, 50, 200, 200 };

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

    HWND hwnd = CreateWindowExW(
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

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}
