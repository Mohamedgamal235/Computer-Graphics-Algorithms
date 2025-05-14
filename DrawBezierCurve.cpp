#include <windows.h>
#include <cmath>
#include <algorithm>


struct Point {
    double x, y;
};


Point points[4]; // p0 , p1 , p2 , p3
int pointsCount = 0;
COLORREF startColor = RGB(255, 0, 0); // -> Red
COLORREF endColor = RGB(0, 0, 255); // -> Blue

BYTE LinearInterpolation(BYTE a , BYTE b , double t) {

}

COLORREF InterpolationColor(COLORREF c1 , COLORREF c2 , double t) {

}

Point BezierEquation(Point p0, Point p1, Point p2, Point p3, double t) {

}


void DrawBezierCurve(HDC hdc, Point p0, Point p1, Point p2, Point p3, COLORREF c1, COLORREF c2, int numOfPoints = 300) {

}



LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_LBUTTONDOWN:
        if (pointsCount < 4) {
            points[pointsCount].x = LOWORD(lParam);
            points[pointsCount].y = HIWORD(lParam);
            pointsCount++;
            InvalidateRect(hwnd, NULL, TRUE);
        }
        break;

    case WM_KEYDOWN:
        if (wParam == VK_SPACE) {
            startColor = RGB(255, 0, 0);
            endColor = RGB(0, 0, 255);
            InvalidateRect(hwnd, NULL, TRUE);
        }
        break;

    case WM_RBUTTONDOWN:
        pointsCount = 0;
        InvalidateRect(hwnd, NULL, TRUE);
        break;

    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        // Draw control points
        for (int i = 0; i < pointsCount; ++i) {
            HBRUSH hBrush = CreateSolidBrush(i == 0 ? startColor : (i == 3 ? endColor : RGB(0, 0, 0)));
            HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, hBrush);
            Ellipse(hdc, (int)points[i].x - 5, (int)points[i].y - 5, (int)points[i].x + 5, (int)points[i].y + 5);
            SelectObject(hdc, oldBrush);
            DeleteObject(hBrush);
        }

        // Draw curve
        if (pointsCount == 4)
            DrawBezierCurve(hdc, points[0], points[1], points[2], points[3], startColor, endColor);

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

// --- WinMain Entry Point ---
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    WNDCLASS wc = { 0 };
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = TEXT("Draw Bezier Curve");
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

    if (!RegisterClass(&wc)) {
        MessageBox(NULL, TEXT("Window Registration Failed!"), TEXT("Error"), MB_ICONERROR);
        return 0;
    }

    HWND hwnd = CreateWindow(
        TEXT("Draw Bezier Curve"),
        TEXT("Draw Bezier Curve with Mouse"),
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 800, 600,
        NULL, NULL, hInstance, NULL);

    if (!hwnd) {
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

    return (int)msg.wParam;
}
