#pragma once

#include <d3d12.h>
#include <d3dcommon.h>
#include <string>
#include <wrl/client.h>

#include "Common/d3dUtil.h"
#include "Common/d3dApp.h"

using Microsoft::WRL::ComPtr;

class Shader
{
public:
    Shader();
    Shader(const std::wstring& ShaderPath);
    ~Shader();

    ComPtr<ID3DBlob> GetVSByteCode() const;
    ComPtr<ID3DBlob> GetPSByteCode() const;
    void Draw(ID3D12GraphicsCommandList* cmdList, UINT indexCount);
    void Init(ComPtr<ID3D12Device> md3dDevice,
        DXGI_FORMAT backBufferFormat, bool m4xMsaaState, int m4xMsaaQuality, DXGI_FORMAT depthStencilFormat);
    void BuildRootSignature(ComPtr<ID3D12Device> md3dDevice);
    void BuildShadersAndInputLayout();
    void BuildPSO(ComPtr<ID3D12Device> md3dDevice,
        DXGI_FORMAT backBufferFormat, bool m4xMsaaState, int m4xMsaaQuality, DXGI_FORMAT depthStencilFormat);

    ComPtr<ID3D12PipelineState> mPSO;
    ComPtr<ID3D12RootSignature> mRootSignature;

private:
    ComPtr<ID3DBlob> mVSByteCode;
    ComPtr<ID3DBlob> mPSByteCode;

    std::vector<D3D12_INPUT_ELEMENT_DESC> mInputLayout;
};