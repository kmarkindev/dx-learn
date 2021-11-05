#include <Windows.h>
#include <memory>
#include "BaseApp.h"
#include <stdexcept>
#include <objbase.h>
#include <chrono>

std::unique_ptr<BaseApp> GetApp(const HINSTANCE& hInst, const HWND& hwnd);

LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
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

float GetTime()
{
    using namespace std::chrono;

    /*
        Need to count time from starting of the program
        because float doesn't have such precision to
        show difference between so large divided numbers.
        And I don't want to use double here.
    */

    static long long startMs = duration_cast<milliseconds>(
        system_clock::now().time_since_epoch()
    ).count();

    long long msCurrent = duration_cast<milliseconds>(
        system_clock::now().time_since_epoch()
    ).count();

    long long curMills = msCurrent - startMs;

    return static_cast<float>(curMills / 1000.0);
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR, int nCmdShow) try
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

    float lastTimePoint = GetTime();
    float deltaTime = 0.0;

    while (!shouldExit)
    {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            shouldExit = msg.message == WM_QUIT;

            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            shouldExit = app->Update(deltaTime);

            float timeNow = GetTime();
            deltaTime = timeNow - lastTimePoint;
            lastTimePoint = timeNow;
        }
    }

    app->Shutdown();

    return 0;
}
catch (const std::exception& ex)
{
    MessageBoxA(nullptr, ex.what(), "Exception", 0);
    return 1;
}
catch (...)
{
    MessageBoxA(nullptr, "Unhandled exception of non std::exception type", "Exception", 0);
    return 1;
}