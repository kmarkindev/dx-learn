#include "BaseApp.h"

BaseApp::BaseApp(const HINSTANCE& hInstance, const HWND& hwnd)
    : _hInstance(hInstance), _hwnd(hwnd)
{

}

bool BaseApp::Update(float deltaTime)
{
    if(Step(deltaTime))
        return true;

    Render();

    return false;
}

void BaseApp::Init()
{
    LoadD3d();
    Load();
}

void BaseApp::Shutdown()
{
    Unload();
}

void BaseApp::LoadD3d()
{
    RECT rect;
    GetWindowRect(_hwnd, &rect);
    const int windowSize[2] = {rect.right - rect.left, rect.bottom - rect.top};

    UINT createDeviceFlags = D3D11_CREATE_DEVICE_DEBUG;

    //create swap chain description
    DXGI_SWAP_CHAIN_DESC scDesc = {};
    scDesc.BufferCount = 2;
    scDesc.OutputWindow = _hwnd;
    scDesc.Windowed = true;
    scDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    scDesc.BufferDesc.Width = windowSize[0];
    scDesc.BufferDesc.Height = windowSize[1];
    scDesc.BufferDesc.RefreshRate.Numerator = 60;
    scDesc.BufferDesc.RefreshRate.Numerator = 1;
    scDesc.BufferDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
    scDesc.SampleDesc.Count = 1;
    scDesc.SampleDesc.Quality = 0;
    scDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
    scDesc.Flags = 0;

    D3D_FEATURE_LEVEL fLevels[] = {
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0
    };

    HRESULT error = D3D11CreateDeviceAndSwapChain(
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        createDeviceFlags,
        fLevels,
        ARRAYSIZE(fLevels),
        D3D11_SDK_VERSION,
        &scDesc,
        &_dxgiSwapChain.p,
        &_d3dDevice.p,
        &_selectedFeatureLevel,
        &_d3dContext.p
    );

    if (FAILED(error))
    {
        throw std::runtime_error("Cannot create D3D device, context or swap chain");
    }

    //Get ref to back buffer as a 2d texture resource
    ID3D11Texture2D* backBufferTexture = nullptr;
    _dxgiSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBufferTexture);

    _d3dDevice->CreateRenderTargetView(backBufferTexture, nullptr, &_backBufferRenderView);

    backBufferTexture->Release();

    D3D11_VIEWPORT viewportDesc = {};
    viewportDesc.Width = static_cast<float>(windowSize[0]);
    viewportDesc.Height = static_cast<float>(windowSize[1]);
    viewportDesc.MaxDepth = 1.0f;
    viewportDesc.MinDepth = 0.0f;

    _d3dContext->RSSetViewports(1, &viewportDesc);
}

std::pair<CComPtr<ID3D11PixelShader>, CComPtr<ID3DBlob>> BaseApp::LoadPixelShader(LPWSTR filename, LPSTR entryPoint)
{
    CComPtr<ID3DBlob> bytecode = LoadShaderBytecode(filename, entryPoint);
    ID3D11PixelShader* res = nullptr;

    HRESULT result = _d3dDevice->CreatePixelShader(bytecode->GetBufferPointer(),
        bytecode->GetBufferSize(), 0, &res);

    if(FAILED(result))
        throw std::runtime_error("Cannot create pixel shader");

    return std::pair(CComPtr(res), bytecode);
}

std::pair<CComPtr<ID3D11VertexShader>, CComPtr<ID3DBlob>> BaseApp::LoadVertexShader(LPWSTR filename, LPSTR entryPoint)
{
    CComPtr<ID3DBlob> bytecode = LoadShaderBytecode(filename, entryPoint);
    ID3D11VertexShader* res = nullptr;

    HRESULT result = _d3dDevice->CreateVertexShader(bytecode->GetBufferPointer(),
                                            bytecode->GetBufferSize(), 0, &res);

    if(FAILED(result))
        throw std::runtime_error("Cannot create vertex shader");

    return std::make_pair(CComPtr(res), bytecode);
}

CComPtr<ID3DBlob> BaseApp::LoadShaderBytecode(LPWSTR filename, LPSTR entryPoint)
{
    CComPtr<ID3DBlob> errorBuffer = nullptr;
    ID3DBlob* bytecode = nullptr;

    DWORD flags = D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_DEBUG;

    HRESULT result = D3DCompileFromFile(filename, nullptr, nullptr,
                                        entryPoint, "ps_4_0", flags, 0, &bytecode, &errorBuffer.p);

    if(FAILED(result))
    {
        if(errorBuffer != nullptr)
            throw std::runtime_error("Cannot load and compile pixel shader from 'sampleShader.fx': "
                                     + std::string((char*)errorBuffer->GetBufferPointer()));
        else
            throw std::runtime_error("Cannot load and compile pixel shader from sampleShader.fx");
    }

    return CComPtr(bytecode);
}
