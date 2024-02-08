#include <Windows.h>
#include <windowsx.h>

HWND hwnd;
HBITMAP mainBitMap;
HDC hdcMem; // Declaration of hdcMem
const int worldHeight = 10;
const int worldWidth = 10;
int world[worldHeight][worldWidth];
int dirtTest[worldHeight][worldWidth];


void genWorld() {
    for (int y = 0; y < worldHeight; y++) {
        for (int x = 0; x < worldWidth; x++) {
            world[y][x] = 1;//Stone
        }
    }
}
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
            for (int y = 0, incr1 = 0; y < worldHeight; y++, incr1 += 31) {
                for (int x = 0, incr2 = 0; x < worldWidth; x++, incr2 += 31) {
                    switch (world[y][x]) {
                        case 1://Stone
                            StretchBlt(hdc, incr2/*x positie in window*/, incr1/*y positie in window*/, (128 / 2)/*Width*/, (64 / 2)/*Height*/, hdcMem, 19/*x positie bitmap offset*/, 1, (bm.bmWidth - 2), (bm.bmHeight - 2), SRCCOPY);
                        break;
                        case 2://Dirt
                            StretchBlt(hdc, incr2/*x positie in window*/, incr1/*y positie in window*/, (128 / 2)/*Width*/, (64 / 2)/*Height*/, hdcMem, 2/*x positie bitmap offset*/, 1, (bm.bmWidth - 2), (bm.bmHeight - 2), SRCCOPY);
                            dirtTest[0][0] = incr2;
                            dirtTest[0][1] = incr1;
                            dirtTest[0][2] = 128 / 2;
                            dirtTest[0][3] = 64 / 2;
                        break;
                    }
                }
            }
        }

        EndPaint(hwnd, &ps);
    }
    else if (uMsg == WM_LBUTTONDOWN) {
        int cursor[2];
        cursor[1] = GET_Y_LPARAM(lParam);
        cursor[0] = GET_X_LPARAM(lParam);
        if (cursor[0] >= dirtTest[0][0] && cursor[0] <= dirtTest[0][0] + dirtTest[0][3] && cursor[1] >= dirtTest[0][1] && cursor[1] <= dirtTest[0][1] + dirtTest[0][3]) {
            MessageBox(hwnd, "Dirt detected!", "click", MB_OK | MB_ICONINFORMATION);
        }
    }
    else {
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {

    genWorld();
    world[2][2] = 2;

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
        //InvalidateRect(hwnd, NULL, FALSE);
        UpdateWindow(hwnd);

        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        Sleep(20);
    }

    DeleteDC(hdcMem);

    return 0;
}
