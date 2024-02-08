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

        RECT clientRect;
        GetClientRect(hwnd, &clientRect);

        HBRUSH bgColor = CreateSolidBrush(RGB(210, 210, 210));
        FillRect(hdc, &clientRect, bgColor);
        DeleteObject(bgColor);

        if (mainBitMap != NULL) {
            BITMAP bm;
            GetObject(mainBitMap, sizeof(BITMAP), &bm);
            //BitBlt(hdc, 100, 100, 10, 10, hdcMem, 0, 0, SRCCOPY);
            /**
            * 1: Positie in window
            * 2: ImgSize in pixels
            * 3: offset binnen image
            */
            StretchBlt(hdc, 10/*x positie in window*/, 10/*y positie in window*/, 120/*Width*/, 64, hdcMem, 18, 1, (bm.bmWidth - 2), (bm.bmHeight - 2), SRCCOPY);

        }

        EndPaint(hwnd, &ps);
    }
    else {
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {

    mainBitMap = (HBITMAP)LoadImageW(NULL, L"assets/img/Blocks.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

    if (mainBitMap == NULL) {
        MessageBoxW(NULL, L"Image loading failed!", L"Error", MB_ICONERROR);
        return 1;
    }

    hdcMem = CreateCompatibleDC(NULL);
    SelectObject(hdcMem, mainBitMap);

    WNDCLASSW wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = L"MyWindowClass";
    RegisterClassW(&wc);

    hwnd = CreateWindowExW(0, L"MyWindowClass", L"M", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 500, 500, NULL, NULL, hInstance, NULL);

    if (hwnd == NULL) {
        MessageBoxW(NULL, L"Window creation failed!", L"Error", MB_ICONERROR);
        return 1;
    }

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);


    MSG msg;
    while (true) {
        InvalidateRect(hwnd, NULL, FALSE);
        UpdateWindow(hwnd);

        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        Sleep(16);
    }

    DeleteDC(hdcMem);

    return 0;
}
