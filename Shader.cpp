#include "Shader.h"

Shader::Shader()
{
}

Shader::Shader(const std::wstring& ShaderPath)
{
    mVSByteCode = d3dUtil::CompileShader(ShaderPath, nullptr, "VS", "vs_5_0");
    mPSByteCode = d3dUtil::CompileShader(ShaderPath, nullptr, "PS", "ps_5_0");

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

void Shader::Init(ComPtr<ID3D12Device> md3dDevice,
    DXGI_FORMAT backBufferFormat, bool m4xMsaaState, int m4xMsaaQuality, DXGI_FORMAT depthStencilFormat)
{
    BuildRootSignature(md3dDevice);
    BuildShadersAndInputLayout();
    BuildPSO(md3dDevice, backBufferFormat, m4xMsaaState, m4xMsaaQuality, depthStencilFormat);
}

void Shader::BuildRootSignature(ComPtr<ID3D12Device> md3dDevice)
{
    // Shader programs typically require resources as input (constant buffers,
    // textures, samplers).  The root signature defines the resources the shader
    // programs expect.  If we think of the shader programs as a function, and
    // the input resources as function parameters, then the root signature can be
    // thought of as defining the function signature.  

    // Root parameter can be a table, root descriptor or root constants.
    CD3DX12_ROOT_PARAMETER slotRootParameter[2];

    slotRootParameter[0].InitAsConstantBufferView(0);
    slotRootParameter[1].InitAsConstantBufferView(1);

    // A root signature is an array of root parameters.
    CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(2, slotRootParameter, 0, nullptr,
        D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

    // create a root signature with a single slot which points to a descriptor range consisting of a single constant buffer
    ComPtr<ID3DBlob> serializedRootSig = nullptr;
    ComPtr<ID3DBlob> errorBlob = nullptr;
    HRESULT hr = D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1,
        serializedRootSig.GetAddressOf(), errorBlob.GetAddressOf());

    if (errorBlob != nullptr)
    {
        OutputDebugStringA(static_cast<char*>(errorBlob->GetBufferPointer()));
    }
    ThrowIfFailed(hr);

    ThrowIfFailed(md3dDevice->CreateRootSignature(
        0,
        serializedRootSig->GetBufferPointer(),
        serializedRootSig->GetBufferSize(),
        IID_PPV_ARGS(&mRootSignature)));
}

void Shader::BuildShadersAndInputLayout()
{
    HRESULT hr = S_OK;

    //mVSByteCode = d3dUtil::CompileShader(L"Shaders\\color.hlsl", nullptr, "VS", "vs_5_0");
    //mPSByteCode = d3dUtil::CompileShader(L"Shaders\\color.hlsl", nullptr, "PS", "ps_5_0");

    mInputLayout =
    {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
        {"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
    };
}

void Shader::BuildPSO(ComPtr<ID3D12Device> md3dDevice, 
    DXGI_FORMAT backBufferFormat, bool m4xMsaaState, int m4xMsaaQuality, DXGI_FORMAT depthStencilFormat)
{
    D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc;
    ZeroMemory(&psoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
    psoDesc.InputLayout = {mInputLayout.data(), static_cast<UINT>(mInputLayout.size())};
    psoDesc.pRootSignature = mRootSignature.Get();
    psoDesc.VS =
    {
        reinterpret_cast<BYTE*>(mVSByteCode->GetBufferPointer()),
        mVSByteCode->GetBufferSize()
    };
    psoDesc.PS =
    {
        reinterpret_cast<BYTE*>(mPSByteCode->GetBufferPointer()),
        mPSByteCode->GetBufferSize()
    };
    psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
    psoDesc.SampleMask = UINT_MAX;
    psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    psoDesc.NumRenderTargets = 1;
    psoDesc.RTVFormats[0] = backBufferFormat;
    psoDesc.SampleDesc.Count = m4xMsaaState ? 4 : 1;
    psoDesc.SampleDesc.Quality = m4xMsaaState ? (m4xMsaaQuality - 1) : 0;
    psoDesc.DSVFormat = depthStencilFormat;
    ThrowIfFailed(md3dDevice->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&mPSO)));
}