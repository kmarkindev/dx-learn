#include "FirstRender.h"

std::unique_ptr<BaseApp> GetApp(const HINSTANCE& hInst, const HWND& hwnd)
{
    return std::make_unique<FirstRender>(hInst, hwnd);
}

FirstRender::FirstRender(const HINSTANCE& hInst, const HWND& hwnd)
    : BaseApp(hInst, hwnd)
{

}

void FirstRender::Render()
{
    _d3dContext->OMSetRenderTargets(1, &_backBufferRenderView.p, 0);

    float clearColor[4] = {0.3f, 0.2f, 0.2f, 1.0f};
    _d3dContext->ClearRenderTargetView(_backBufferRenderView, clearColor);

    UINT stride = sizeof(DirectX::XMFLOAT3);
    UINT offset = 0;

    _d3dContext->IASetInputLayout(_inputLayout.p);
    _d3dContext->IASetVertexBuffers(0, 1, &_triangleData.p, &stride, &offset);
    _d3dContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    _d3dContext->VSSetShader(_vertShader.p, 0, 0);
    _d3dContext->PSSetShader(_pixelShader.p, 0 ,0);
    _d3dContext->Draw(3, 0);

    _dxgiSwapChain->Present(0, 0);
}

void FirstRender::Load()
{
    CreateBuffer();

    LoadShaders();

    CreateInputLayout();
}

void FirstRender::CreateBuffer()
{
    using namespace DirectX;

    XMFLOAT3 verts[3] = {
        XMFLOAT3(0.5f, 0.5f, 0.5f),
        XMFLOAT3(0.5f, -0.5f, 0.5f),
        XMFLOAT3(-0.5f, -0.5f, 0.5f)
    };

    D3D11_BUFFER_DESC bufDesc = {};
    bufDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bufDesc.Usage = D3D11_USAGE_DEFAULT;
    bufDesc.ByteWidth = sizeof(verts);

    D3D11_SUBRESOURCE_DATA subresData = {};
    subresData.pSysMem = verts;

    HRESULT result = _d3dDevice->CreateBuffer(&bufDesc, &subresData, &_triangleData.p);

    if(FAILED(result))
        throw std::runtime_error("Cannot load vertices into buffer");
}

void FirstRender::Unload()
{

}

bool FirstRender::Step(float dt)
{
    return false;
}

void FirstRender::LoadShaders()
{
    LoadVertexShader();

    LoadPixelShader();
}

void FirstRender::LoadVertexShader()
{
    CComPtr<ID3DBlob> errorBuffer = nullptr;

    DWORD shaderFlags = D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_DEBUG;

    HRESULT result = D3DCompileFromFile(L"Assets/sampleShader.fx", nullptr, nullptr,
"VS_Main", "vs_4_0", shaderFlags, 0, &_vertShaderBytecode.p, &errorBuffer.p);

    if(FAILED(result))
    {
        if(errorBuffer != nullptr)
            throw std::runtime_error("Cannot load and compile vertex shader from 'sampleShader.fx': "
                + std::string((char*)errorBuffer->GetBufferPointer()));
        else
            throw std::runtime_error("Cannot load and compile vertex shader from sampleShader.fx");
    }

    result = _d3dDevice->CreateVertexShader(_vertShaderBytecode->GetBufferPointer(),
    _vertShaderBytecode->GetBufferSize(), 0, &_vertShader.p);

    if(FAILED(result))
        throw std::runtime_error("Cannot create vertex shader from sampleShader.fx");
}

void FirstRender::CreateInputLayout()
{
    D3D11_INPUT_ELEMENT_DESC vertexLayout[] =
    {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
         0, D3D11_INPUT_PER_VERTEX_DATA, 0}
    };

    HRESULT result = _d3dDevice->CreateInputLayout(vertexLayout, ARRAYSIZE(vertexLayout),
        _vertShaderBytecode->GetBufferPointer(), _vertShaderBytecode->GetBufferSize(),
        &_inputLayout.p);

    if(FAILED(result))
        throw std::runtime_error("Cannot create Input Layout");
}

void FirstRender::LoadPixelShader()
{
    CComPtr<ID3DBlob> errorBuffer = nullptr;

    DWORD shaderFlags = D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_DEBUG;

    HRESULT result = D3DCompileFromFile(L"Assets/sampleShader.fx", nullptr, nullptr,
                                        "PS_Main", "ps_4_0", shaderFlags, 0, &_pixelShaderBytecode.p, &errorBuffer.p);

    if(FAILED(result))
    {
        if(errorBuffer != nullptr)
            throw std::runtime_error("Cannot load and compile pixel shader from 'sampleShader.fx': "
                                     + std::string((char*)errorBuffer->GetBufferPointer()));
        else
            throw std::runtime_error("Cannot load and compile pixel shader from sampleShader.fx");
    }

    result = _d3dDevice->CreatePixelShader(_pixelShaderBytecode->GetBufferPointer(),
                                            _pixelShaderBytecode->GetBufferSize(), 0, &_pixelShader.p);

    if(FAILED(result))
        throw std::runtime_error("Cannot create pixel shader from sampleShader.fx");
}

