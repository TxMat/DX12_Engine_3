#pragma once

#include <d3d12.h>
#include <d3dcommon.h>
#include <string>
#include <wrl/client.h>

#include "Common/d3dUtil.h"

class Shader
{
public:
    Shader(const std::wstring& vertexShaderPath, const std::wstring& pixelShaderPath);
    ~Shader();

    Microsoft::WRL::ComPtr<ID3DBlob> GetVSByteCode() const;
    Microsoft::WRL::ComPtr<ID3DBlob> GetPSByteCode() const;
    void Draw(ID3D12GraphicsCommandList* cmdList, UINT indexCount);

private:
    Microsoft::WRL::ComPtr<ID3DBlob> mVSByteCode;
    Microsoft::WRL::ComPtr<ID3DBlob> mPSByteCode;
    Microsoft::WRL::ComPtr<ID3D12PipelineState> mPSO;
    Microsoft::WRL::ComPtr<ID3D12RootSignature> mRootSignature;
};