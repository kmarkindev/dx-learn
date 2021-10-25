#include "ShaderLoader.h"

std::pair<CComPtr<ID3D11VertexShader>, CComPtr<ID3DBlob>> ShaderLoader::LoadVertexShader(LPCWSTR filename, const char* entryPoint)
{
    ID3DBlob* bytecode = LoadShaderBytecode(filename, entryPoint, "vs_4_0");
    ID3D11VertexShader* vertexShader = nullptr;

    HRESULT result = _d3dDevice->CreateVertexShader(bytecode->GetBufferPointer(), bytecode->GetBufferSize(), nullptr, &vertexShader);

    if(FAILED(result))
        throw std::runtime_error("Cannot create vertex shader");

    return std::make_pair(CComPtr(vertexShader), CComPtr(bytecode));
}

std::pair<CComPtr<ID3D11PixelShader>, CComPtr<ID3DBlob>> ShaderLoader::LoadPixelShader(LPCWSTR filename, const char* entryPoint)
{
    ID3DBlob* bytecode = LoadShaderBytecode(filename, entryPoint, "ps_4_0");
    ID3D11PixelShader* pixelShader = nullptr;

    HRESULT result = _d3dDevice->CreatePixelShader(bytecode->GetBufferPointer(), bytecode->GetBufferSize(), nullptr, &pixelShader);

    if(FAILED(result))
        throw std::runtime_error("Cannot create pixel shader");

    return std::make_pair(CComPtr(pixelShader), CComPtr(bytecode));
}

CComPtr<ID3DBlob> ShaderLoader::LoadShaderBytecode(LPCWSTR filename, const char* entryPoint, const char* version)
{
    ID3DBlob* bytecode = nullptr;
    CComPtr<ID3DBlob> errorBuffer = nullptr;

    DWORD shaderFlags = D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_DEBUG;

    HRESULT result = D3DCompileFromFile(filename, nullptr, nullptr,
            entryPoint, version, shaderFlags, 0, &bytecode, &errorBuffer.p);

    if(FAILED(result))
    {
        if(errorBuffer != nullptr)
            throw std::runtime_error("Cannot load and compile shader "
                                     + std::string((char*)errorBuffer->GetBufferPointer()));
        else
            throw std::runtime_error("Cannot load and compile shader");
    }

    return CComPtr(bytecode);
}

ShaderLoader::ShaderLoader(ID3D11Device* device)
    : _d3dDevice(device)
{

}
