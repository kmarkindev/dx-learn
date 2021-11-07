#include "ImageRenderer.h"

ImageRenderer::ImageRenderer(HWND hwnd, ID3D11Device* device,
    ID3D11DeviceContext* context,
    std::weak_ptr<ShaderLoader> shaderLoader)
    : _d3dDevice(device), _d3dContext(context), _shaderLoader(shaderLoader), _hwnd(hwnd)
{
    CreateBuffers();
    CreateShadersAndInputLayout();
}

void ImageRenderer::RenderImage(DirectX::XMFLOAT4 posSize, ID3D11ShaderResourceView* texture2d)
{
    RECT rect = {};
    GetClientRect(_hwnd, &rect);
    if (rect.right == 0 || rect.bottom == 0)
    {
        rect.right = 1;
        rect.bottom = 1;
    }

    DirectX::XMMATRIX transform =
        DirectX::XMMatrixScaling(posSize.z, posSize.w, 1.0f)
        * DirectX::XMMatrixTranslation(posSize.x, posSize.y, 0.0f)
        * DirectX::XMMatrixOrthographicOffCenterLH(0, static_cast<float>(rect.right),
            static_cast<float>(rect.bottom), 0, 0.0f, 1.0f);

    DirectX::XMFLOAT4X4 mat = {};
    DirectX::XMStoreFloat4x4(&mat, transform);

    UINT stride = sizeof(ImageRenderer::Vertex);
    UINT offset = 0;

    _d3dContext->IASetInputLayout(_inputLayout.p);
    _d3dContext->IASetVertexBuffers(0, 1, &_meshBuffer.p, &stride, &offset);
    _d3dContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
    _d3dContext->OMSetBlendState(_blendState.p, blendFactor, 0xffffffff);

    _d3dContext->VSSetShader(_vShader.p, 0, 0);
    _d3dContext->PSSetShader(_pShader.p, 0, 0);

    _d3dContext->PSSetShaderResources(0,
        1,
        &texture2d);
    _d3dContext->PSSetSamplers(0, 1, &_samplerState.p);

    _d3dContext->UpdateSubresource(_transformMatrixCBuffer.p, 0, 0, &mat, 0, 0);
    _d3dContext->VSSetConstantBuffers(0, 1, &_transformMatrixCBuffer.p);

    _d3dContext->Draw(6, 0);
}

void ImageRenderer::CreateBuffers()
{
    using namespace DirectX;

    ImageRenderer::Vertex verts[] = {
        //first triangle
        { XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT2(0.0f, 1.0f) },
        { XMFLOAT3(1.0f, 0.0f, 1.0f), XMFLOAT2(1.0f, 1.0f) },
        { XMFLOAT3(0.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 0.0f) },
        //second triangle
        { XMFLOAT3(1.0f, 0.0f, 1.0f), XMFLOAT2(1.0f, 1.0f) },
        { XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT2(1.0f, 0.0f) },
        { XMFLOAT3(0.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 0.0f) }
    };

    D3D11_BUFFER_DESC bufDesc = {};
    bufDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bufDesc.Usage = D3D11_USAGE_DEFAULT;
    bufDesc.ByteWidth = sizeof(verts);

    D3D11_SUBRESOURCE_DATA subresData = {};
    subresData.pSysMem = verts;

    HRESULT result = _d3dDevice->CreateBuffer(&bufDesc, &subresData, &_meshBuffer.p);

    if (FAILED(result))
        throw std::runtime_error("Cannot load vertices into buffer");

    bufDesc = {};
    bufDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bufDesc.Usage = D3D11_USAGE_DEFAULT;
    bufDesc.ByteWidth = sizeof(XMFLOAT4X4);

    result = _d3dDevice->CreateBuffer(&bufDesc, nullptr, &_transformMatrixCBuffer.p);

    if (FAILED(result))
        throw std::runtime_error("Cannot create buffer for transformation matrix");
}

void ImageRenderer::CreateShadersAndInputLayout()
{
    auto shaderLoader = _shaderLoader.lock();

    auto pixelShaderData = shaderLoader->LoadPixelShader(L"Assets/imageRendererShader.fx", "PS_Main");
    auto vertexShaderData = shaderLoader->LoadVertexShader(L"Assets/imageRendererShader.fx", "VS_Main");

    _pShader = pixelShaderData.first;
    _vShader = vertexShaderData.first;

    D3D11_INPUT_ELEMENT_DESC vertexLayout[] =
        {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
              0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "TEXCOORDS", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
              sizeof(DirectX::XMFLOAT3), D3D11_INPUT_PER_VERTEX_DATA, 0 }
        };

    HRESULT result = _d3dDevice->CreateInputLayout(vertexLayout, ARRAYSIZE(vertexLayout),
        vertexShaderData.second->GetBufferPointer(), vertexShaderData.second->GetBufferSize(),
        &_inputLayout.p);

    if (FAILED(result))
        throw std::runtime_error("Cannot create Input Layout");
}

void ImageRenderer::CreateStates()
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
