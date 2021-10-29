#include "BlankApp.h"

std::unique_ptr<BaseApp> GetApp(const HINSTANCE& hInst, const HWND& hwnd)
{
    return std::make_unique<BlankApp>(hInst, hwnd);
}

BlankApp::BlankApp(const HINSTANCE& hInst, const HWND& hwnd)
    : BaseApp(hInst, hwnd)
{
}

void BlankApp::Load()
{

}

void BlankApp::Unload()
{

}

bool BlankApp::Step(float)
{
    return false;
}

void BlankApp::Render()
{

}
