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
    return (BYTE)(a + (b - a) * t);
}

COLORREF InterpolationColor(COLORREF c1 , COLORREF c2 , double t) {
    BYTE r = LinearInterpolation(GetRValue(c1), GetRValue(c2), t);
    BYTE g = LinearInterpolation(GetGValue(c1), GetGValue(c2), t);
    BYTE b = LinearInterpolation(GetBValue(c1), GetBValue(c2), t);
    return RGB(r, g, b);
}

Point BezierEquation(Point p0, Point p1, Point p2, Point p3, double t) {
    // (1-t)^3p0 + 3(1-t)^2*tp1 + 3(1-t)*t^2*p2 + t^3 * p3

    double u = 1 - t;
    double u3 = u * u * u;
    double u2 = u * u;
    double t2 = t * t;
    double t3 = t * t * t;

    Point res;

    res.x = (u3 * p0.x) + (3 * u2 * t * p1.x) + (3 * u * t2 * p2.x) + (t3 * p3.x);
    res.y = (u3 * p0.y) + (3 * u2 * t * p1.y) + (3 * u * t2 * p2.y) + (t3 * p3.y);

    return res;
}


void DrawBezierCurve(HDC hdc, Point p0, Point p1, Point p2, Point p3, COLORREF c1, COLORREF c2, int numOfPoints = 300) {
    if (numOfPoints < 2)
        return;

    Point prev = BezierEquation(p0, p1, p2, p3, 0); // start point at t = 0
    for (int i = 1; i <= numOfPoints; i++) {
        double t = (double)i / numOfPoints;
        Point curr = BezierEquation(p0, p1, p2, p3, t);
        COLORREF col = LinearInterpolation(c1, c2, t);

        HPEN pen = CreatePen(PS_SOLID, 1, col);
        HGDIOBJ oldPen = SelectObject(hdc, pen);

        MoveToEx(hdc, (int)prev.x, (int)prev.y, NULL);
        LineTo(hdc, (int)curr.x, (int)curr.y);

        SelectObject(hdc, oldPen);
        DeleteObject(pen);

        prev = curr;
    }
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
