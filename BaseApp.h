#pragma once

#include <Windows.h>
#include <d3d.h>
#include <d3d11.h>
#include <stdexcept>
#include "atlbase.h"
#include <DirectXMath.h>
#include <WICTextureLoader.h>
#include "Utils/TextRendererLegacy.h"
#include "Utils/ImageRenderer.h"
#include "Utils/ShaderLoader.h"
#include <memory>

class BaseApp
{
public:

    BaseApp(const HINSTANCE& hInstance, const HWND& hwnd);
    virtual ~BaseApp() = default;
    BaseApp(const BaseApp&) = delete;
    BaseApp(BaseApp&&) = delete;
    BaseApp& operator=(BaseApp) = delete;
    BaseApp& operator=(BaseApp&) = delete;

    void Init();
    void Shutdown();
    bool Update(float deltaTime);

protected:

    virtual void Load() = 0;
    virtual void Unload() = 0;
    virtual bool Step(float deltaTime) = 0;
    virtual void Render() = 0;

    std::shared_ptr<TextRendererLegacy> _textRenderer;
    std::shared_ptr<ShaderLoader> _shaderLoader;
    std::shared_ptr<ImageRenderer> _imageRenderer;

    HINSTANCE _hInstance;
    HWND _hwnd;

    CComPtr<ID3D11Device> _d3dDevice;
    CComPtr<ID3D11DeviceContext> _d3dContext;
    CComPtr<IDXGISwapChain> _dxgiSwapChain;
    D3D_FEATURE_LEVEL _selectedFeatureLevel;
    CComPtr<ID3D11RenderTargetView> _backBufferRenderView;

private:

    void LoadD3d();
    void LoadDependencies();

};


