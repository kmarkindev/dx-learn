#include <Windows.h>
#include <memory>
#include "BaseApp.h"
#include <stdexcept>
#include <objbase.h>

std::unique_ptr<BaseApp> GetApp(const HINSTANCE& hInst, const HWND& hwnd);

LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch(uMsg) {
        case WM_PAINT:
        {
            PAINTSTRUCT ps = {};
            BeginPaint(hwnd, &ps);
            EndPaint(hwnd, &ps);

            break;
        }
        case WM_DESTROY:
        {
            PostQuitMessage(0);
            break;
        }
        default:
        {
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
        }
    }

    return 0;
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR pCmdLine, int nCmdShow) try
{
    CoInitialize(nullptr);

    LPCWSTR WINDOW_CLASS = L"Main Window";

    WNDCLASSW windowClass = {};
    windowClass.lpfnWndProc = WindowProcedure;
    windowClass.hInstance = hInstance;
    windowClass.lpszClassName = WINDOW_CLASS;

    RegisterClassW(&windowClass);

    // Create registered window
    HWND hwnd = CreateWindowExW(
        0,
        WINDOW_CLASS,
        APP_NAME,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        800,
        600,
        nullptr,
        nullptr,
        hInstance,
        nullptr
    );

    if (hwnd == nullptr)
    {
        throw std::runtime_error("Cannot create window");
    }

    // Finally, show window
    ShowWindow(hwnd, nCmdShow);

    auto app = GetApp(hInstance, hwnd);

    MSG msg = {};
    bool shouldExit = false;

    app->Init();

    while(!shouldExit)
    {
        if(PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            shouldExit = msg.message == WM_QUIT;

            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            shouldExit = app->Update();
        }
    }

    app->Shutdown();

    return 0;
}
catch(const std::exception& ex)
{
    MessageBoxA(nullptr, ex.what(), "Exception", 0);
    return 1;
}
catch(...)
{
    MessageBoxA(nullptr, "Unhandled exception of non std::exception type", "Exception", 0);
    return 1;
}