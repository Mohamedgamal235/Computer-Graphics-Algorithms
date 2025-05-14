#include <windows.h>
#include <cmath>
#include <algorithm>
using namespace std;

int x_start = 0, y_start = 0;
int x_end = 0, y_end = 0;
bool isDrawing = false;
COLORREF startColor = RGB(255, 0, 0); // Red
COLORREF endColor = RGB(0, 0, 255);   // Blue

void InterpolatedColoredLine(HDC hdc, int x1, int y1, int x2, int y2, COLORREF c1, COLORREF c2) {
    int dx = abs(x2 - x1), dy = abs(y2 - y1);
    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;
    int err = dx - dy;

    int r1 = GetRValue(c1), g1 = GetGValue(c1), b1 = GetBValue(c1);
    int r2 = GetRValue(c2), g2 = GetGValue(c2), b2 = GetBValue(c2);

    int steps = max(dx, dy);
    float dr = (float)(r2 - r1) / steps;
    float dg = (float)(g2 - g1) / steps;
    float db = (float)(b2 - b1) / steps;

    float r = r1, g = g1, b = b1;

    while (true) {
        SetPixel(hdc, x1, y1, RGB((int)r, (int)g, (int)b));

        if (x1 == x2 && y1 == y2) break;

        int e2 = 2 * err;
        if (e2 > -dy) { err -= dy; x1 += sx; }
        if (e2 < dx) { err += dx; y1 += sy; }

        r += dr;
        g += dg;
        b += db;
    }
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_LBUTTONDOWN:
        x_start = LOWORD(lParam);
        y_start = HIWORD(lParam);
        isDrawing = false;
        break;

    case WM_LBUTTONUP:
        x_end = LOWORD(lParam);
        y_end = HIWORD(lParam);
        isDrawing = true;
        InvalidateRect(hwnd, NULL, TRUE);
        break;

    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        if (isDrawing)
            InterpolatedColoredLine(hdc, x_start, y_start, x_end, y_end, startColor, endColor);
        EndPaint(hwnd, &ps);
        break;
    }

    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    WNDCLASS wc = { 0 };
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = TEXT("ColoredLineWindow");

    if (!RegisterClass(&wc)) {
        MessageBox(NULL, TEXT("Window Registration Failed!"), TEXT("Error"), MB_ICONERROR);
        return 0;
    }

    HWND hwnd = CreateWindow(
        TEXT("ColoredLineWindow"),
        TEXT("Bresenham Colored Line"),
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 800, 600,
        NULL, NULL, hInstance, NULL);

    if (hwnd == NULL) {
        MessageBox(NULL, TEXT("Window Creation Failed!"), TEXT("Error"), MB_ICONERROR);
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return msg.wParam;
}