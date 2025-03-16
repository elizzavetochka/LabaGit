#include <windows.h>
#include <glut.h>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <string>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

HDC deviceContext;
HGLRC renderContext;
HWND windowHandle;

// Функции для вычисления параметров прямоугольника
float rectanglePerimeter(float width, float height) {
    return 2 * (width + height);
}

float rectangleArea(float width, float height) {
    return width * height;
}

float rectangleDiagonal(float width, float height) {
    return sqrt(width * width + height * height);
}

void drawRectangle(float width, float height) {
    glBegin(GL_QUADS);
    glVertex2f(-width / 2, -height / 2);
    glVertex2f(width / 2, -height / 2);
    glVertex2f(width / 2, height / 2);
    glVertex2f(-width / 2, height / 2);
    glEnd();
}

void renderText(float x, float y, const std::wstring& text) {
    glRasterPos2f(x, y);
    for (wchar_t c : text) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
    }
}

void renderScene() {
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // цвет фона
    glClear(GL_COLOR_BUFFER_BIT);

    // Параметры прямоугольника
    float width = 0.5f;
    float height = 0.4f;
    float perimeter = rectanglePerimeter(width, height);
    float area = rectangleArea(width, height);
    float diagonal = rectangleDiagonal(width, height);

    // Отображение прямоугольник
    glColor3f(1.0f, 0.0f, 1.0f); // настройка цвета фигуры
    drawRectangle(width, height);

    // результаты вычислений
    glColor3f(0.0f, 0.0f, 0.0f);
    renderText(-0.5f, 0.9f, L"P: 2 * (" + std::to_wstring(width) + L" + " + std::to_wstring(height) + L") = " + std::to_wstring(perimeter));
    renderText(-0.5f, 0.8f, L"S: " + std::to_wstring(width) + L" * " + std::to_wstring(height) + L" = " + std::to_wstring(area));
    renderText(-0.5f, 0.7f, L"D: sqrt(" + std::to_wstring(width) + L"^2 + " + std::to_wstring(height) + L"^2) = " + std::to_wstring(diagonal));

    SwapBuffers(deviceContext);
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_SIZE:
        glViewport(0, 0, LOWORD(lParam), HIWORD(lParam));
        break;
    case WM_KEYDOWN:
        if (wParam == VK_ESCAPE) {
            PostQuitMessage(0);
        }
        break;
    case WM_CLOSE:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}

void initOpenGL(HWND hwnd) {
    PIXELFORMATDESCRIPTOR pfd = { 0 };
    pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER | PFD_DRAW_TO_WINDOW;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 32;

    deviceContext = GetDC(hwnd);
    int pixelFormat = ChoosePixelFormat(deviceContext, &pfd);
    SetPixelFormat(deviceContext, pixelFormat, &pfd);

    renderContext = wglCreateContext(deviceContext);
    wglMakeCurrent(deviceContext, renderContext);


    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0); 
    glMatrixMode(GL_MODELVIEW);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    SetConsoleOutputCP(1251); 
    SetConsoleCP(1251); 

    WNDCLASS wc = { 0 };
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = L"OpenGLWindowClass";

    if (!RegisterClass(&wc)) {
        MessageBox(NULL, L"Window Registration Failed!", L"Error", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    windowHandle = CreateWindowEx(
        0, wc.lpszClassName, L"Geometric Shapes",
        WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 800, 600,
        NULL, NULL, hInstance, NULL);

    if (!windowHandle) {
        MessageBox(NULL, L"Window Creation Failed!", L"Error", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    ShowWindow(windowHandle, nCmdShow);
    UpdateWindow(windowHandle);

    initOpenGL(windowHandle);

    MSG msg = { 0 };
    // Основной цикл обработки сообщений
    while (true) {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) break;
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        renderScene(); // Отрисовка сцены
    }

    // Освобождение ресурсов
    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(renderContext);
    ReleaseDC(windowHandle, deviceContext);

    return msg.wParam;
}