#pragma once

#include "BaseApp.h"
#include <memory>
#include <cmath>

struct Vertex
{
    DirectX::XMFLOAT3 pos;
    DirectX::XMFLOAT2 uv;
};

class FirstRender : public BaseApp
{
public:
    FirstRender(const HINSTANCE& hInst, const HWND& hwnd);

protected:

    void Render() override;

    void Load() override;

    void Unload() override;

    bool Step(float deltaTime) override;

private:

    CComPtr<ID3D11Buffer> _triangleData;
    CComPtr<ID3D11VertexShader> _vertShader;
    CComPtr<ID3D11PixelShader> _pixelShader;
    CComPtr<ID3D11InputLayout> _inputLayout;
    CComPtr<ID3DBlob> _vertShaderBytecode;
    CComPtr<ID3DBlob> _pixelShaderBytecode;
    CComPtr<ID3D11Texture2D> _texture;
    CComPtr<ID3D11ShaderResourceView> _shaderResView;
    CComPtr<ID3D11SamplerState> _samplerState;
    CComPtr<ID3D11Buffer> _rotMatrixBuffer;

    float _timer = 0;

    void CreateBuffer();

    void LoadShaders();

    void CreateInputLayout();

    void LoadVertexShader();

    void LoadPixelShader();

    void LoadTexture();
};