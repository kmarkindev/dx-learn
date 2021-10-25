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

bool TextRender::Step(float dt)
{
    return false;
}

void TextRender::Render()
{
    _d3dContext->OMSetRenderTargets(1, &_backBufferRenderView.p, 0);

    float clearColor[4] = {0.3f, 0.3f, 0.5f, 1.0f};
    _d3dContext->ClearRenderTargetView(_backBufferRenderView.p, clearColor);

    _textRenderer->RenderString("A b", {0, 0});

    _dxgiSwapChain->Present(0, 0);
}


