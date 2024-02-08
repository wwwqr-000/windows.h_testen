#include <Windows.h>

HWND hwnd;
HBITMAP mainBitMap;
HDC hdcMem; // Declaration of hdcMem

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    if (uMsg == WM_DESTROY) {
        PostQuitMessage(0);
        return 0;
    }
    else if (uMsg == WM_PAINT) {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        if (mainBitMap != NULL) {
            BITMAP bm;
            GetObject(mainBitMap, sizeof(BITMAP), &bm);

            // Calculate the target rectangle for scaling
            RECT targetRect = {0, 0, 200, 200}; // Adjust as needed

            // Use StretchBlt to scale and draw the image
            StretchBlt(hdc, targetRect.left, targetRect.top, targetRect.right, targetRect.bottom,
                       hdcMem, 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY);
        }

        EndPaint(hwnd, &ps);
    }
    else {
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    // Load the image
    mainBitMap = (HBITMAP)LoadImageW(NULL, L"assets/img/sun.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

    // Check if image loading failed
    if (mainBitMap == NULL) {
        MessageBoxW(NULL, L"Image loading failed!", L"Error", MB_ICONERROR);
        return 1;
    }

    // Create a compatible device context and select the bitmap into it
    hdcMem = CreateCompatibleDC(NULL);
    SelectObject(hdcMem, mainBitMap);

    // Register window class
    WNDCLASSW wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = L"MyWindowClass";
    RegisterClassW(&wc);

    // Create the window
    hwnd = CreateWindowExW(0, L"MyWindowClass", L"M", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 500, 500, NULL, NULL, hInstance, NULL);

    // Check if window creation failed
    if (hwnd == NULL) {
        MessageBoxW(NULL, L"Window creation failed!", L"Error", MB_ICONERROR);
        return 1;
    }

    // Show and update the window
    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    // Message loop
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // Cleanup: Delete the compatible device context
    DeleteDC(hdcMem);

    return 0;
}
