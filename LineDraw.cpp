#include <windows.h>
#include <cmath>
#include <algorithm>
using namespace std;

int x_start = 0, y_start = 0;
int x_end = 0, y_end = 0;
bool isDrawing = false;
COLORREF startColor = RGB(255, 0, 0); // Red
COLORREF endColor = RGB(0, 0, 255);   // Blue

void InterpolatedBresenhamLine(HDC hdc, int x1, int y1, int x2, int y2, COLORREF c1, COLORREF c2) {
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);

    int x = x1;
    int y = y1;

    int x_inc = (x2 > x1) ? 1 : -1;
    int y_inc = (y2 > y1) ? 1 : -1;

    int r1 = GetRValue(c1), g1 = GetGValue(c1), b1 = GetBValue(c1);
    int r2 = GetRValue(c2), g2 = GetGValue(c2), b2 = GetBValue(c2);

    int steps = max(dx, dy);
    int r_step = (r2 - r1) / steps;
    int g_step = (g2 - g1) / steps;
    int b_step = (b2 - b1) / steps;

    int r = r1, g = g1, b = b1;

    if (dy <= dx) { // 0 < slop < 1
        int d = dx - 2 * dy ;
        int change1 = 2 * (dx - dy);
        int change2 = -2 * dy ;
        SetPixel(hdc , x , y , RGB(r, g, b));
        for (int i = 0 ; i <= dx ; i++) {
            if (d < 0) {
                d += change1 ;
                y += y_inc ;
            }
            else
                d += change2 ;
            x += x_inc ;
            r += r_step;
            b += b_step;
            g += g_step;
            SetPixel(hdc , x , y , RGB(r , g , b));
        }
    }
    else { // slop > 1
        int d = 2 * dx - dy ;
        int change1 = 2 * (dx - dy);
        int change2 = 2 * dx ;
        SetPixel(hdc , x , y , RGB(r , g , b));
        for (int i = 0 ; i <= dy ; i++) {
            if (d < 0) {
                d += change1 ;
                x += x_inc;
            }
            else
                d += change2 ;
            y += y_inc;
            r += r_step;
            b += b_step;
            g += g_step;
            SetPixel(hdc , x , y , RGB(r , g , b));
        }
    }

}


void InterpolatedColoredLine(HDC hdc, int x1, int y1, int x2, int y2, COLORREF c1, COLORREF c2) {
    int dx = x2 - x1 ;
    int dy = y2 - y1 ;

    int r1 = GetRValue(c1), g1 = GetGValue(c1), b1 = GetBValue(c1);
    int r2 = GetRValue(c2), g2 = GetGValue(c2), b2 = GetBValue(c2);

    if (abs(dy) <= abs(dx)) {
        double slope = (double)dy / (double)dx;

        if (x1 > x2) {
            swap(x1 , x2 );
            swap(y1 , y2 );
            swap(r1 , r2 );
            swap(g1 , g2 );
            swap(b1 , b2 );
        }

        int len = x2 - x1 ;
        for (int x = x1 ; x <= x2 ; x++) {
            int y = round(y1 + (x - x1) * slope);
            double t = (double)(x - x1) / (double)len ;

            int r = (int)(r1 + t * (r2 - r1));
            int g = (int)(g1 + t * (g2 - g1));
            int b = (int)(b1 + t * (b2 - b1));

            SetPixel(hdc , x , y , RGB(r, g ,b));
        }
    }
    else {
        double slope = (double)dx / (double)dy;
        if (y1 > y2) {
            swap(y1 , y2);
            swap(x1 , x2);
            swap(r1 , r2);
            swap(g1 , g2);
            swap(b1 , b2);
        }

        int len = y2 - y1 ;
        for (int y = y1 ; y <= y2 ; y++) {
            double t = (double)(y - y1) / (double)len;
            int x = round(x1 + (y - y1) * slope);

            int r = (int)(r1 + t * (r2 - r1));
            int g = (int)(g1 + t * (g2 - g1));
            int b = (int)(b1 + t * (b2 - b1));

            SetPixel(hdc, x, y, RGB(r, g, b));
        }
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