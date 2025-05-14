#include <windows.h>
#include <math.h>

int x_center = 0, y_center = 0;
int radius = 0; 
bool isCircle = false; 

void Draw8Points(HDC hdc, int xc, int yc, int x, int y, COLORREF color) {
    SetPixel(hdc, xc + x, yc + y, color);
    SetPixel(hdc, xc - x, yc + y, color);
    SetPixel(hdc, xc + x, yc - y, color);
    SetPixel(hdc, xc - x, yc - y, color);
    SetPixel(hdc, xc + y, yc + x, color);
    SetPixel(hdc, xc - y, yc + x, color);
    SetPixel(hdc, xc + y, yc - x, color);
    SetPixel(hdc, xc - y, yc - x, color);
}

void CircleFasterBresenham(HDC hdc , int xc , int yc , int R , COLORREF color) {
    int x = 0, y = R;
    int d = 1 - R;

    int c1 = 3, c2 = 5 - 2 * R;
    Draw8Points(hdc, xc, yc, x, y, color);
    while (x < y) {
        if (d < 0) {
            d += c1;
            c2 += 2;
        }
        else {
            d += c2;
            c2 += 4;
            y--;
        }
        c1 += 2;
        x++;
        Draw8Points(hdc, xc, yc, x, y, color);
    }
}

// Window procedure
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_LBUTTONDOWN: 
        x_center = LOWORD(lParam);
        y_center = HIWORD(lParam);
        isCircle = false; 
        break;

    case WM_LBUTTONUP: {
        int x = LOWORD(lParam);
        int y = HIWORD(lParam);

        int dx = x - x_center; 
        int dy = y - y_center; 
        radius = static_cast<int>(sqrt(dx * dx + dy * dy));

        isCircle = true; 
        InvalidateRect(hwnd, NULL, TRUE);
        break;
    }

    case WM_PAINT: {
        PAINTSTRUCT ps; 
        HDC hdc = BeginPaint(hwnd, &ps);
        if (isCircle)
            CircleFasterBresenham(hdc, x_center, y_center, radius, RGB(0, 0, 0));
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
    // Step 1: Define a window class
    WNDCLASS wc = { 0 };
    wc.lpfnWndProc = WndProc;       // Window procedure
    wc.hInstance = hInstance;       // Application instance handle
    wc.lpszClassName = TEXT("MyWindowClass"); // Class name (use TEXT() for compatibility)

    // Step 2: Register the window class
    if (!RegisterClass(&wc)) {
        MessageBox(NULL, TEXT("Window Registration Failed!"), TEXT("Error"), MB_ICONERROR);
        return 0;
    }

    // Step 3: Create the window
    HWND hwnd = CreateWindow(
        TEXT("MyWindowClass"),       // Class name
        TEXT("Draw Circle"),    // Window title
        WS_OVERLAPPEDWINDOW,        // Window style (corrected typo)
        CW_USEDEFAULT,              // Default x position
        CW_USEDEFAULT,              // Default y position
        800,                        // Width
        600,                        // Height
        NULL,                       // Parent window
        NULL,                       // Menu
        hInstance,                  // Application instance
        NULL                        // Additional data
    );

    if (hwnd == NULL) {
        MessageBox(NULL, TEXT("Window Creation Failed!"), TEXT("Error"), MB_ICONERROR);
        return 0;
    }

    // Step 4: Show and update the window
    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    // Step 5: Message loop
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return msg.wParam;
}