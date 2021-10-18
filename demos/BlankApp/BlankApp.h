#pragma once

#include "BaseApp.h"
#include <memory>

class BlankApp : public BaseApp
{
public:

    BlankApp(const HINSTANCE& hInst, const HWND& hwnd);

protected:
    void Load() override;

    void Unload() override;

    bool Step(float dt) override;

    void Render() override;

};


