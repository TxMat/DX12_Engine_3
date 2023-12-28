#pragma once
#include "Mesh.h"
#include "Tranform.h"

class Object
{
public:
    explicit Object(Mesh& m_mesh)
        : mMesh(m_mesh)
    {
        mTransform = Transform();
    }

    Transform mTransform;
    Mesh& mMesh;

    void Update(const GameTimer& gt);
    
    void Draw(ComPtr<ID3D12GraphicsCommandList> mCommandList);
};
