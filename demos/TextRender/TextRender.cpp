#include "TextRender.h"

std::unique_ptr<BaseApp> GetApp(const HINSTANCE& hInst, const HWND& hwnd)
{
    return std::make_unique<TextRender>(hInst, hwnd);
}

TextRender::TextRender(const HINSTANCE& hInst, const HWND& hwnd)
    : BaseApp(hInst, hwnd)
{
}

void TextRender::Load()
{

}

void TextRender::Unload()
{

}

bool TextRender::Step(float)
{
    return false;
}

void TextRender::Render()
{

}
