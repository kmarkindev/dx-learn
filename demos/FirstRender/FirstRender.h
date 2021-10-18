#pragma once

#include "BaseApp.h"
#include <memory>

class FirstRender : public BaseApp
{
public:
    FirstRender(const HINSTANCE& hInst, const HWND& hwnd);

protected:

    void Render() override;

    void Load() override;

    void Unload() override;

    bool Step(float dt) override;

private:

    CComPtr<ID3D11Buffer> _triangleData;
    CComPtr<ID3D11VertexShader> _vertShader;
    CComPtr<ID3D11PixelShader> _pixelShader;
    CComPtr<ID3D11InputLayout> _inputLayout;
    CComPtr<ID3DBlob> _vertShaderBytecode;
    CComPtr<ID3DBlob> _pixelShaderBytecode;

    void CreateBuffer();

    void LoadShaders();

    void CreateInputLayout();

    void LoadVertexShader();

    void LoadPixelShader();
};