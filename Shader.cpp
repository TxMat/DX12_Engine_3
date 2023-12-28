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

void Shader::BuildPSO()
{
    D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc;
    ZeroMemory(&psoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
    psoDesc.InputLayout = {mInputLayout.data(), static_cast<UINT>(mInputLayout.size())};
    psoDesc.pRootSignature = mRootSignature.Get();
    psoDesc.VS =
    {
        reinterpret_cast<BYTE*>(mvsByteCode->GetBufferPointer()),
        mvsByteCode->GetBufferSize()
    };
    psoDesc.PS =
    {
        reinterpret_cast<BYTE*>(mpsByteCode->GetBufferPointer()),
        mpsByteCode->GetBufferSize()
    };
    psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
    psoDesc.SampleMask = UINT_MAX;
    psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    psoDesc.NumRenderTargets = 1;
    psoDesc.RTVFormats[0] = mBackBufferFormat;
    psoDesc.SampleDesc.Count = m4xMsaaState ? 4 : 1;
    psoDesc.SampleDesc.Quality = m4xMsaaState ? (m4xMsaaQuality - 1) : 0;
    psoDesc.DSVFormat = mDepthStencilFormat;
    ThrowIfFailed(md3dDevice->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&mPSO)));
}