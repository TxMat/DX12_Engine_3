﻿#pragma once
#include "Mesh.h"
#include "Shader.h"
#include "Tranform.h"
#include "Common/d3dApp.h"
#include "Common/UploadBuffer.h"
#include "ObjectConstants.h"

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
    Shader& mShader;

    unique_ptr<UploadBuffer<ObjectConstants>> mObjectCB = nullptr;

    static int objectNumber;
    int mObjectIndex;

    void BuildConstantBuffer(ComPtr<ID3D12Device> md3dDevice);
    void Update(const GameTimer& gt);
    void Draw(ComPtr<ID3D12GraphicsCommandList> mCommandList)
};
