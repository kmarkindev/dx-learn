#include "TextRenderer.h"

TextRenderer::TextRenderer(ID3D11Device* device, ID3D11DeviceContext* context, std::weak_ptr<ShaderLoader> shaderLoader)
    : _d3dDevice(device), _d3dContext(context), _shaderLoader(shaderLoader)
{
    if(FT_Init_FreeType(&_ftLibrary))
        throw std::runtime_error("Cannot init FreeType");

    LoadFtFace(&_fontFace, "Assets/Roboto-Medium.ttf");

    CreateStates();
    CreateLetterMeshBuffer();
    CreateShadersAndInputLayout();
}

void TextRenderer::LoadFtFace(FT_Face* face, const char* filename, int faceIndex)
{
    auto err = FT_New_Face(_ftLibrary, filename, faceIndex, face);

    if(err == FT_Err_Unknown_File_Format)
        throw std::runtime_error("Font file format is not supported and cannot be loaded by FreeType");
    else if(err == FT_Err_Cannot_Open_Resource)
        throw std::runtime_error("Cannot load font because it doesn't exist");
    else if(err)
        throw std::runtime_error("Cannot load FreeType Face");

    FT_Set_Pixel_Sizes(*face, 0, 32);
}

void TextRenderer::LoadFtChar(FT_Face face, char symbol)
{
    if(FT_Load_Char(face, symbol, FT_LOAD_RENDER))
        throw std::runtime_error("Cannot load font char");
}

void TextRenderer::CreateTextures(std::string_view text)
{
    for(char chr : text)
    {
        if(chr == ' ' || _textures.find(chr) != _textures.end())
            continue;

        LoadFtChar(_fontFace, chr);
        auto glyph = _fontFace->glyph;

        ID3D11Texture2D* texture = nullptr;
        ID3D11ShaderResourceView* resourceView = nullptr;

        D3D11_TEXTURE2D_DESC textureDesc = {};
        textureDesc.MipLevels = 1;
        textureDesc.Width = glyph->bitmap.width;
        textureDesc.Height = glyph->bitmap.rows;
        textureDesc.Format = DXGI_FORMAT_R8_UNORM;
        textureDesc.Usage = D3D11_USAGE_DEFAULT;
        textureDesc.ArraySize = 1;
        textureDesc.CPUAccessFlags = 0;
        textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        textureDesc.MiscFlags = 0;
        textureDesc.SampleDesc.Count = 1;
        textureDesc.SampleDesc.Quality = 0;

        D3D11_SUBRESOURCE_DATA data = {};
        data.pSysMem = glyph->bitmap.buffer;
        data.SysMemPitch = glyph->bitmap.pitch;
        data.SysMemSlicePitch = 0;

        HRESULT res = _d3dDevice->CreateTexture2D(&textureDesc, &data, &texture);

        if(FAILED(res))
            throw std::runtime_error("Cannot load texture");

        res = _d3dDevice->CreateShaderResourceView(texture, nullptr, &resourceView);

        if(FAILED(res))
            throw std::runtime_error("Cannot load texture resource view");

        _textures.insert_or_assign(chr,
                std::make_pair(CComPtr<ID3D11Texture2D>(texture),
                        CComPtr<ID3D11ShaderResourceView>(resourceView)));
    }
}

void TextRenderer::CreateStates()
{
    D3D11_SAMPLER_DESC samplerDesc = {};
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

    _d3dDevice->CreateSamplerState(&samplerDesc, &_samplerState.p);

    D3D11_BLEND_DESC blendDesc = {};
    blendDesc.AlphaToCoverageEnable = false;
    blendDesc.IndependentBlendEnable = false;
    blendDesc.RenderTarget[0].BlendEnable = true;
    blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
    blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
    blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
    blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
    blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

    _d3dDevice->CreateBlendState(&blendDesc, &_blendState.p);
}

void TextRenderer::CreateInputLayout(ID3DBlob* bytecode)
{
    D3D11_INPUT_ELEMENT_DESC vertexLayout[] =
            {
                    {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
                            0, D3D11_INPUT_PER_VERTEX_DATA, 0},
                    {"TEXCOORDS", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
                            sizeof(DirectX::XMFLOAT3), D3D11_INPUT_PER_VERTEX_DATA, 0}
            };

    HRESULT result = _d3dDevice->CreateInputLayout(vertexLayout, ARRAYSIZE(vertexLayout),
            bytecode->GetBufferPointer(), bytecode->GetBufferSize(),
            &_inputLayout.p);

    if(FAILED(result))
        throw std::runtime_error("Cannot create Input Layout");
}

void TextRenderer::CreateLetterMeshBuffer()
{
    using namespace DirectX;

    TextRenderer::Vertex verts[] = {
            //first triangle
            { XMFLOAT3(  1.0f,  1.0f, 1.0f ), XMFLOAT2( 1.0f, 1.0f ) },
            { XMFLOAT3(  1.0f, -1.0f, 1.0f ), XMFLOAT2( 1.0f, 0.0f ) },
            { XMFLOAT3( -1.0f, -1.0f, 1.0f ), XMFLOAT2( 0.0f, 0.0f ) },
            //second triangle
            { XMFLOAT3( -1.0f, -1.0f, 1.0f ), XMFLOAT2( 0.0f, 0.0f ) },
            { XMFLOAT3( -1.0f,  1.0f, 1.0f ), XMFLOAT2( 0.0f, 1.0f ) },
            { XMFLOAT3(  1.0f,  1.0f, 1.0f ), XMFLOAT2( 1.0f, 1.0f ) }
    };

    D3D11_BUFFER_DESC bufDesc = {};
    bufDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bufDesc.Usage = D3D11_USAGE_DEFAULT;
    bufDesc.ByteWidth = sizeof(verts);

    D3D11_SUBRESOURCE_DATA subresData = {};
    subresData.pSysMem = verts;

    HRESULT result = _d3dDevice->CreateBuffer(&bufDesc, &subresData, &_planeBuffer.p);

    if(FAILED(result))
        throw std::runtime_error("Cannot load vertices into buffer");
}

void TextRenderer::CreateShadersAndInputLayout()
{
    std::shared_ptr<ShaderLoader> ptr = _shaderLoader.lock();

    if(!ptr)
        throw std::runtime_error("Cannot access ShaderLoader");

    auto vertexShader = ptr->LoadVertexShader(L"Assets/textShader.fx", "VS_Main");
    auto pixelShader = ptr->LoadPixelShader(L"Assets/textShader.fx", "PS_Main");

    CreateInputLayout(vertexShader.second.p);

    _vertexShader = vertexShader.first;
    _pixelShader = pixelShader.first;
}

void TextRenderer::RenderString(std::string_view text, DirectX::XMFLOAT2 position)
{
    CreateTextures(text);

    for(char chr : text)
    {
        auto textureIter = _textures.find(chr);

        if(textureIter == _textures.end())
        {
            //skip
            continue;
        }

        float blendFactor[4] = {0.0f, 0.0f, 0.0f, 0.0f};
        _d3dContext->OMSetBlendState(_blendState.p, blendFactor, 0xffffffff);

        UINT offset = 0;
        UINT stride = sizeof(TextRenderer::Vertex);

        _d3dContext->IASetInputLayout(_inputLayout.p);
        _d3dContext->IASetVertexBuffers(0, 1, &_planeBuffer.p, &stride, &offset);
        _d3dContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        _d3dContext->VSSetShader(_vertexShader.p, 0, 0);
        _d3dContext->PSSetShader(_pixelShader.p, 0 ,0);

        _d3dContext->PSSetShaderResources(0, 1, &textureIter->second.second.p);
        _d3dContext->PSSetSamplers(0, 1, &_samplerState.p);

        _d3dContext->Draw(6, 0);
    }
}
