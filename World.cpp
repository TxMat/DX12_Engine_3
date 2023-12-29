//***************************************************************************************
// BoxApp.cpp by Frank Luna (C) 2015 All Rights Reserved.
//
// Shows how to draw a box in Direct3D 12.
//
// Controls:
//   Hold the left mouse button down and move the mouse to rotate.
//   Hold the right mouse button down and move the mouse to zoom in and out.
//***************************************************************************************

#include <map>

#include "Common/d3dApp.h"
#include "Common/MathHelper.h"
#include "Common/UploadBuffer.h"
#include "Tranform.h"
#include "Vertex.h"
#include "Mesh.h"
#include "Object.h"
#include "ObjectConstants.h"

using Microsoft::WRL::ComPtr;
using namespace DirectX;
using namespace DirectX::PackedVector;

class BoxApp : public D3DApp
{
public:
    BoxApp(HINSTANCE hInstance);
    BoxApp(const BoxApp& rhs) = delete;
    BoxApp& operator=(const BoxApp& rhs) = delete;
    ~BoxApp() override;

    bool Initialize() override;

private:
    void OnResize() override;
    void Update(const GameTimer& gt) override;
    void Draw(const GameTimer& gt) override;
    void UpdateCamera(const GameTimer& gt);

    void OnMouseDown(WPARAM btnState, int x, int y) override;
    void OnMouseUp(WPARAM btnState, int x, int y) override;
    void OnMouseMove(WPARAM btnState, int x, int y) override;
    void OnKeyLeft() override;
    void OnKeyRight() override;
    void OnKeyUp() override;
    void OnKeyDown() override;

    void BuildDescriptorHeaps();
    void BuildConstantBuffers();
    void BuildSphereMeshes();

    void BuildMeshes();
    void BuildShaders();
    void BuildObjects();

private:
    ComPtr<ID3D12DescriptorHeap> mCbvHeap = nullptr;

    std::unique_ptr<UploadBuffer<ObjectConstants>> mViewCB = nullptr;

    XMFLOAT4X4 mWorld = MathHelper::Identity4x4();
    XMFLOAT4X4 mView = MathHelper::Identity4x4();
    XMFLOAT4X4 mProj = MathHelper::Identity4x4();

    float mTheta = 1.5f * XM_PI;
    float mPhi = XM_PIDIV4;
    float mRadius = 5.0f;

    POINT mLastMousePos;
    Transform mCameraTransform;
    
    vector<Shader*> mShaders;

    vector<Object*> mObjects;
    map<string, Mesh*> mMeshes;
};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance,
                   PSTR cmdLine, int showCmd)
{
    // Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

    try
    {
        BoxApp theApp(hInstance);
        if (!theApp.Initialize())
            return 0;

        return theApp.Run();
    }
    catch (DxException& e)
    {
        MessageBox(nullptr, e.ToString().c_str(), L"HR Failed", MB_OK);
        return 0;
    }
}

BoxApp::BoxApp(HINSTANCE hInstance)
    : D3DApp(hInstance)
{
}

BoxApp::~BoxApp()
{
    if (md3dDevice != nullptr)
        FlushCommandQueue();

    for (auto& object : mObjects)
    {
        delete object;
    }
    for (auto& mesh : mMeshes)
    {
        delete mesh.second;
    }
    for (auto& shader : mShaders)
    {
        delete shader;
    }
}

bool BoxApp::Initialize()
{
    if (!D3DApp::Initialize())
        return false;

    // Reset the command list to prep for initialization commands.
    ThrowIfFailed(mCommandList->Reset(mDirectCmdListAlloc.Get(), nullptr));

    BuildDescriptorHeaps();
    BuildConstantBuffers();

    //BuildMeshes();
    BuildSphereMeshes();
    BuildShaders();
    BuildObjects();

    // Execute the initialization commands.
    ThrowIfFailed(mCommandList->Close());
    ID3D12CommandList* cmdsLists[] = {mCommandList.Get()};
    mCommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

    // Wait until initialization is complete.
    FlushCommandQueue();

    return true;
}

void BoxApp::OnResize()
{
    D3DApp::OnResize();

    // The window resized, so update the aspect ratio and recompute the projection matrix.
    XMMATRIX P = XMMatrixPerspectiveFovLH(0.25f * MathHelper::Pi, AspectRatio(), 1.0f, 1000.0f);
    XMStoreFloat4x4(&mProj, P);
}

void BoxApp::Update(const GameTimer& gt)
{
    UpdateCamera(gt);

    for (auto& object : mObjects)
    {
        object->Update(gt);
    }
}
void BoxApp::UpdateCamera(const GameTimer& gt)
{
    mCameraTransform.ApplyChanges();

    // Build the view matrix.
    XMVECTOR pos = XMLoadFloat3(&mCameraTransform.vPos);
    XMVECTOR target = XMLoadFloat3(&mCameraTransform.vPos) + XMLoadFloat3(&mCameraTransform.vDir);
    XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

    XMMATRIX view = XMMatrixLookAtLH(pos, target, up);
    XMStoreFloat4x4(&mView, view);

    XMMATRIX proj = XMLoadFloat4x4(&mProj);
    XMMATRIX viewProj = view * proj;

    // Update the constant buffer with the latest worldViewProj matrix.
    ObjectConstants objConstants;
    XMStoreFloat4x4(&objConstants.WorldViewProj, XMMatrixTranspose(viewProj));
    mViewCB->CopyData(0, objConstants);
}

void BoxApp::Draw(const GameTimer& gt)
{
    // Reuse the memory associated with command recording.
    // We can only reset when the associated command lists have finished execution on the GPU.
    ThrowIfFailed(mDirectCmdListAlloc->Reset());

    // A command list can be reset after it has been added to the command queue via ExecuteCommandList.
    // Reusing the command list reuses memory.
    ThrowIfFailed(mCommandList->Reset(mDirectCmdListAlloc.Get(), nullptr));

    mCommandList->RSSetViewports(1, &mScreenViewport);
    mCommandList->RSSetScissorRects(1, &mScissorRect);

    // Indicate a state transition on the resource usage.
    mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(),
                                                                           D3D12_RESOURCE_STATE_PRESENT,
                                                                           D3D12_RESOURCE_STATE_RENDER_TARGET));

    // Clear the back buffer and depth buffer.
    mCommandList->ClearRenderTargetView(CurrentBackBufferView(), Colors::IndianRed, 0, nullptr);
    mCommandList->ClearDepthStencilView(DepthStencilView(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0,
                                        0, nullptr);

    // Specify the buffers we are going to render to.
    mCommandList->OMSetRenderTargets(1, &CurrentBackBufferView(), true, &DepthStencilView());

    ID3D12DescriptorHeap* descriptorHeaps[] = {mCbvHeap.Get()};
    mCommandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

    for (auto& object : mObjects)
    {
        object->Draw(mCommandList, mViewCB->Resource()->GetGPUVirtualAddress());
    }

    // Indicate a state transition on the resource usage.
    mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(),
                                                                           D3D12_RESOURCE_STATE_RENDER_TARGET,
                                                                           D3D12_RESOURCE_STATE_PRESENT));

    // Done recording commands.
    HRESULT hr = mCommandList->Close();
    if (hr == DXGI_ERROR_DEVICE_REMOVED)
    {
        hr = md3dDevice->GetDeviceRemovedReason();
        OutputDebugStringA("Device Removed Reason: ");
        OutputDebugStringA(std::to_string(hr).c_str());
    }
    ThrowIfFailed(hr);

    // Add the command list to the queue for execution.
    ID3D12CommandList* cmdsLists[] = {mCommandList.Get()};
    mCommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

    // swap the back and front buffers
    ThrowIfFailed(mSwapChain->Present(0, 0));
    mCurrBackBuffer = (mCurrBackBuffer + 1) % SwapChainBufferCount;

    // Wait until frame commands are complete.  This waiting is inefficient and is
    // done for simplicity.  Later we will show how to organize our rendering code
    // so we do not have to wait per frame.
    FlushCommandQueue();
}

void BoxApp::OnMouseDown(WPARAM btnState, int x, int y)
{
    mLastMousePos.x = x;
    mLastMousePos.y = y;

    SetCapture(mhMainWnd);
}

void BoxApp::OnMouseUp(WPARAM btnState, int x, int y)
{
    ReleaseCapture();
}

void BoxApp::OnMouseMove(WPARAM btnState, int x, int y)
{
    if ((btnState & MK_LBUTTON) != 0)
    {
        // Make each pixel correspond to a quarter of a degree.
        float dx = XMConvertToRadians(0.25f * static_cast<float>(x - mLastMousePos.x));
        float dy = XMConvertToRadians(0.25f * static_cast<float>(y - mLastMousePos.y));

        mCameraTransform.Rotate(dx, dy, 0);
    }

    mLastMousePos.x = x;
    mLastMousePos.y = y;
}

void BoxApp::OnKeyLeft()
{
    mCameraTransform.TranslateLocal(-0.1f, 0, 0);
}
void BoxApp::OnKeyRight()
{
    mCameraTransform.TranslateLocal(0.1f, 0, 0);
}
void BoxApp::OnKeyUp()
{
    mCameraTransform.TranslateLocal(0, 0, 0.1f);
}
void BoxApp::OnKeyDown()
{
    mCameraTransform.TranslateLocal(0, 0, -0.1f);
}

void BoxApp::BuildDescriptorHeaps()
{
    D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDesc;
    cbvHeapDesc.NumDescriptors = 1;
    cbvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    cbvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    cbvHeapDesc.NodeMask = 0;
    ThrowIfFailed(md3dDevice->CreateDescriptorHeap(&cbvHeapDesc,
        IID_PPV_ARGS(&mCbvHeap)));
}

void BoxApp::BuildConstantBuffers()
{
    mViewCB = std::make_unique<UploadBuffer<ObjectConstants>>(md3dDevice.Get(), 1, true);
}

void BoxApp::BuildSphereMeshes()
{
    // build a sphere
    float radius = 1.0f;
    int sliceCount = 20;
    int stackCount = 20;

    vector<Vertex> vertices;
    vector<std::uint16_t> indices;

    for (int i = 0; i <= stackCount; i++)
    {
        float phi = XM_PIDIV2 - i * XM_PI / stackCount;
        float y = radius * sinf(phi);
        float r = radius * cosf(phi);

        for (int j = 0; j <= sliceCount; j++)
        {
            float theta = j * 2.0f * XM_PI / sliceCount;
            float x = r * sinf(theta);
            float z = r * cosf(theta);

            float red = (float)i / stackCount;
            float green = (float)j / sliceCount;
            float blue = 1.0f - red;

            Vertex v;
            v.Pos = XMFLOAT3(x, y, z);
            v.Color = XMFLOAT4(red, green, blue, 1.0f);

            vertices.push_back(v);
        }
    }

    // indices
    for (int i = 0; i < stackCount; ++i)
    {
        for (int j = 0; j < sliceCount; ++j)
        {
            indices.push_back(i * (sliceCount + 1) + j);
            indices.push_back((i + 1) * (sliceCount + 1) + j);
            indices.push_back((i + 1) * (sliceCount + 1) + j + 1);

            indices.push_back(i * (sliceCount + 1) + j);
            indices.push_back((i + 1) * (sliceCount + 1) + j + 1);
            indices.push_back(i * (sliceCount + 1) + j + 1);
        }
    }

    mMeshes["sphere"] = new Mesh();
    mMeshes["sphere"]->Create(vertices, indices, md3dDevice, mCommandList);
    
    
}

void BoxApp::BuildMeshes()
{
    vector<Vertex> vertices =
    {
        // Base vertices
        Vertex({XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT4(Colors::Red)}),
        Vertex({XMFLOAT3(+1.0f, -1.0f, -1.0f), XMFLOAT4(Colors::Green)}),
        Vertex({XMFLOAT3(+1.0f, -1.0f, +1.0f), XMFLOAT4(Colors::Blue)}),
        Vertex({XMFLOAT3(-1.0f, -1.0f, +1.0f), XMFLOAT4(Colors::Red)}),

        // Top vertices
        Vertex({XMFLOAT3(0.0f, +1.0f, 0.0f), XMFLOAT4(Colors::Yellow)}),
    };

    vector<std::uint16_t> indices =
    {
        // Base indices
        0, 1, 2,
        0, 2, 3,

        // Top indices
        4, 1, 0,
        4, 2, 1,
        4, 3, 2,
        4, 0, 3,
    };

    mMeshes["holy_prism"] = new Mesh();
    mMeshes["holy_prism"]->Create(vertices, indices, md3dDevice, mCommandList);
}
void BoxApp::BuildShaders()
{
    mShaders.push_back(new Shader(L"Shaders\\color.hlsl"));
    mShaders.back()->Init(md3dDevice, mBackBufferFormat, m4xMsaaState, m4xMsaaQuality, mDepthStencilFormat);
}
void BoxApp::BuildObjects()
{
    // mObjects.push_back(new Object(mMesh, mShader, XMFLOAT3(-2.5f, +2.0f, 4.0f), md3dDevice));
    // mObjects.push_back(new Object(mMesh, mShader, XMFLOAT3(0.5f, +2.0f, 4.0f), md3dDevice));
    // mObjects.push_back(new Object(mMesh, mShader, XMFLOAT3(0.5f, +2.0f, 1.0f), md3dDevice));
    // mObjects.push_back(new Object(mMesh, mShader, XMFLOAT3(-2.5f, +2.0f, 1.0f), md3dDevice));
    
    float x = -20;
    float z = -20;
    for (int i = 1; i <= 1000; i++)
    {
        mObjects.push_back(new Object(mMeshes["sphere"], mShaders[0], XMFLOAT3(x, i % 2 == 0 ? 1.0f : 0.0f, z), XMFLOAT3(0, i % 2 == 0 ? 3.1416f : 0, 0), md3dDevice));
        if (i % 40 == 0)
        {
            x = -20;
            z += 2;
        }
        else
        {
            x += 2;
        }
    }
}
