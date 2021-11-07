#pragma once

#include <d3d.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include <atlbase.h>
#include "ShaderLoader.h"
#include <memory>

class ImageRenderer
{
public:

    explicit ImageRenderer(HWND hwnd, ID3D11Device* device, ID3D11DeviceContext* context, std::weak_ptr<ShaderLoader> shaderLoader);

    void RenderImage(DirectX::XMFLOAT4 posSize, ID3D11ShaderResourceView* texture2d);

private:

    struct Vertex
    {
        DirectX::XMFLOAT3 pos;
        DirectX::XMFLOAT2 texture;
    };

    std::weak_ptr<ShaderLoader> _shaderLoader;
    HWND _hwnd;
    ID3D11Device* _d3dDevice;
    ID3D11DeviceContext* _d3dContext;
    CComPtr<ID3D11Buffer> _meshBuffer;
    CComPtr<ID3D11VertexShader> _vShader;
    CComPtr<ID3D11PixelShader> _pShader;
    CComPtr<ID3D11InputLayout> _inputLayout;
    CComPtr<ID3D11Buffer> _transformMatrixCBuffer;
    CComPtr<ID3D11BlendState> _blendState;
    CComPtr<ID3D11SamplerState> _samplerState;

    void CreateBuffers();
    void CreateStates();
    void CreateShadersAndInputLayout();

};


