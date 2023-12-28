#include "Shader.h"

Shader::Shader(const std::wstring& vertexShaderPath, const std::wstring& pixelShaderPath)
{
    mVSByteCode = d3dUtil::CompileShader(vertexShaderPath, nullptr, "VS", "vs_5_0");
    mPSByteCode = d3dUtil::CompileShader(pixelShaderPath, nullptr, "PS", "ps_5_0");

    // Initialize mPSO and mRootSignature here
}

Shader::~Shader()
{
}

Microsoft::WRL::ComPtr<ID3DBlob> Shader::GetVSByteCode() const
{
    return mVSByteCode;
}

Microsoft::WRL::ComPtr<ID3DBlob> Shader::GetPSByteCode() const
{
    return mPSByteCode;
}

void Shader::Draw(ID3D12GraphicsCommandList* cmdList, UINT indexCount)
{
    cmdList->SetPipelineState(mPSO.Get());
    cmdList->SetGraphicsRootSignature(mRootSignature.Get());

    // descriptor heaps?

    cmdList->DrawIndexedInstanced(indexCount, 1, 0, 0, 0);
}