#pragma once
#include "Mesh.h"
#include "Shader.h"
#include "Tranform.h"
#include "Common/d3dApp.h"
#include "Common/UploadBuffer.h"
#include "ObjectConstants.h"

class Object
{
public:
    Object(Mesh* mesh, Shader& shader, XMFLOAT3 startPos, XMFLOAT3 startRot, ComPtr<ID3D12Device> md3dDevice);

    Transform mTransform;
    Mesh* mMesh;
    Shader& mShader;

    unique_ptr<UploadBuffer<ObjectConstants>> mObjectCB = nullptr;

    void BuildConstantBuffer(ComPtr<ID3D12Device> md3dDevice);
    void Update(const GameTimer& gt);
    void Draw(ComPtr<ID3D12GraphicsCommandList> mCommandList, D3D12_GPU_VIRTUAL_ADDRESS add);
};
