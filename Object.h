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
    Object(Mesh& mesh, Shader& shader, ComPtr<ID3D12Device> md3dDevice);

    Transform mTransform;
    Mesh& mMesh;
    Shader& mShader;

    unique_ptr<UploadBuffer<ObjectConstants>> mObjectCB = nullptr;

    void BuildConstantBuffer(ComPtr<ID3D12Device> md3dDevice);
    void Update(const GameTimer& gt);
    void Draw(ComPtr<ID3D12GraphicsCommandList> mCommandList);
};
