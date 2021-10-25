#pragma once

#include "BaseApp.h"

class TextRender : public BaseApp
{
public:

    TextRender(const HINSTANCE& hInst, const HWND& hwnd);

protected:

    void Load() override;

    void Unload() override;

    bool Step(float dt) override;

    void Render() override;
};