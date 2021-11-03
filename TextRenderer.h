#pragma once

#include <d3d.h>
#include <d3d11.h>
#include <atlbase.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include "ShaderLoader.h"
#include <DirectXMath.h>
#include <Windows.h>
#include <memory>
#include <map>

class TextRenderer
{
public:

    explicit TextRenderer(HWND hwnd, ID3D11Device* device, ID3D11DeviceContext* context, std::weak_ptr<ShaderLoader> shaderLoader);

    void RenderString(std::string_view text, DirectX::XMFLOAT2 position);

private:

    struct Vertex
    {
        DirectX::XMFLOAT3 pos;
        DirectX::XMFLOAT2 uv;
    };

    struct Letter
    {
        CComPtr<ID3D11Texture2D> texture;
        CComPtr<ID3D11ShaderResourceView> resource;
        DirectX::XMUINT2 size;
        DirectX::XMUINT2 bearing;
        unsigned int advance;
    };

    HWND _hwnd;

    ID3D11Device* _d3dDevice;
    ID3D11DeviceContext* _d3dContext;
    CComPtr<ID3D11VertexShader> _vertexShader;
    CComPtr<ID3D11PixelShader> _pixelShader;
    CComPtr<ID3D11Buffer> _planeBuffer;
    CComPtr<ID3D11SamplerState> _samplerState;
    CComPtr<ID3D11BlendState> _blendState;
    CComPtr<ID3D11InputLayout> _inputLayout;
    CComPtr<ID3D11Buffer> _transformMatrixBuffer;

    FT_Library _ftLibrary;
    FT_Face _fontFace;

    std::weak_ptr<ShaderLoader> _shaderLoader;
    std::map<char, Letter> _letters;

    void LoadFtFace(FT_Face* face, const char* filename, int faceIndex = 0);
    void LoadFtChar(FT_Face face, char symbol);

    void CreateShadersAndInputLayout();
    void CreateLetterMeshBuffer();
    void CreateInputLayout(ID3DBlob* bytecode);
    void CreateTextures(std::string_view text);
    void CreateStates();
};


