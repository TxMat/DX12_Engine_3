#include "Object.h"

void Object::Update(const GameTimer& gt)
{
}

Object::Object(): mMesh(), mShader()
{
    mTransform = Transform();

    mObjectIndex = objectNumber;
    objectNumber++;
}

void Object::Init(Mesh& mesh, Shader& shader, ComPtr<ID3D12Device> md3dDevice)
{
    mMesh = mesh;
    mShader = shader;

    BuildConstantBuffer(md3dDevice);
}

void Object::BuildConstantBuffer(ComPtr<ID3D12Device> md3dDevice)
{
    mObjectCB = std::make_unique<UploadBuffer<ObjectConstants>>(md3dDevice.Get(), 1, true);
}

void Object::Draw(ComPtr<ID3D12GraphicsCommandList> mCommandList)
{
    mCommandList->IASetVertexBuffers(0, 1, &mMesh.mGeometry->VertexBufferView());
    mCommandList->IASetIndexBuffer(&mMesh.mGeometry->IndexBufferView());

    mCommandList->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    mCommandList->SetGraphicsRootSignature(mShader.mRootSignature.Get());
    mCommandList->SetGraphicsRootConstantBufferView(0, mObjectCB->Resource()->GetGPUVirtualAddress());

    mCommandList->SetPipelineState(mShader.mPSO.Get());

    // End
    mCommandList->DrawIndexedInstanced(mMesh.mGeometry->DrawArgs["Geometry"].IndexCount, 1, 0, 0, 0);
}
