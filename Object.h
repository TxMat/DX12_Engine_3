#pragma once
#include "Mesh.h"
#include "Tranform.h"
#include "Common/d3dApp.h"
#include "Common/UploadBuffer.h"

class Object
{
public:
    explicit Object(Mesh& m_mesh)
        : mMesh(m_mesh)
    {
        mTransform = Transform();

        mObjectIndex = objectNumber;
        objectNumber++;
    }

    Transform mTransform;
    Mesh& mMesh;

    unique_ptr<UploadBuffer<ObjectConstants>> mObjectCB = nullptr;

    static int objectNumber;
    int mObjectIndex;

    void Update(const GameTimer& gt);
    
    void Draw(ComPtr<ID3D12GraphicsCommandList> mCommandList);
};
