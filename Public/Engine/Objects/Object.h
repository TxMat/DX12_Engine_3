#pragma once
#include "Engine/Graphic/Mesh.h"
#include "Engine/Graphic/Shader.h"
#include "Structs/Tranform.h"
#include "d3dApp.h"
#include "UploadBuffer.h"
#include "Structs/ObjectConstants.h"

class Object
{
public:
    virtual ~Object() = default;
    Object(Mesh* mesh, Shader* shader, XMFLOAT3 startPos, ComPtr<ID3D12Device> md3dDevice);

    Transform mTransform;
    Mesh* mMesh;
    Shader* mShader;

    unique_ptr<UploadBuffer<ObjectConstants>> mObjectCB = nullptr;

    void BuildConstantBuffer(ComPtr<ID3D12Device> md3dDevice);
    virtual void Update(const GameTimer& gt);
    virtual void Draw(ComPtr<ID3D12GraphicsCommandList> mCommandList, D3D12_GPU_VIRTUAL_ADDRESS add);
};
