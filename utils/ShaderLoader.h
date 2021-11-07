#pragma once

#include <d3d.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <atlbase.h>
#include <utility>
#include <stdexcept>
#include <string>

class ShaderLoader
{
public:

    explicit ShaderLoader(ID3D11Device* device);

    CComPtr<ID3DBlob> LoadShaderBytecode(LPCWSTR filename, const char* entryPoint, const char* version);
    std::pair<CComPtr<ID3D11VertexShader>, CComPtr<ID3DBlob>> LoadVertexShader(LPCWSTR filename,
        const char* entryPoint);
    std::pair<CComPtr<ID3D11PixelShader>, CComPtr<ID3DBlob>> LoadPixelShader(LPCWSTR filename, const char* entryPoint);

private:

    ID3D11Device* _d3dDevice;

};


