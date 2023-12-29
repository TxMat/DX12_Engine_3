#include "Object.h"

void Object::Update(const GameTimer& gt)
{
    mTransform.Rotate(gt.DeltaTime(), 0, 0);
    mTransform.TranslateLocal(0, 0, gt.DeltaTime());
    mTransform.ApplyChanges();

    ObjectConstants objConstants;
    XMStoreFloat4x4(&objConstants.WorldViewProj, XMMatrixTranspose(XMLoadFloat4x4(&mTransform.matrix)));

    mObjectCB->CopyData(0, objConstants);
}

Object::Object(Mesh* mesh, Shader* shader, XMFLOAT3 startPos, XMFLOAT3 startRot, ComPtr<ID3D12Device> md3dDevice) : mMesh(mesh), mShader(shader)
{
    mTransform = Transform();
    mTransform.TranslateWorld(startPos.x, startPos.y, startPos.z);
    mTransform.Rotate(startRot.x, startRot.y, startRot.z);
    mTransform.ApplyChanges();

    BuildConstantBuffer(md3dDevice);
}

void Object::BuildConstantBuffer(ComPtr<ID3D12Device> md3dDevice)
{
    mObjectCB = std::make_unique<UploadBuffer<ObjectConstants>>(md3dDevice.Get(), 1, true);
}

void Object::Draw(ComPtr<ID3D12GraphicsCommandList> mCommandList, D3D12_GPU_VIRTUAL_ADDRESS add)
{
    mCommandList->IASetVertexBuffers(0, 1, &mMesh->mGeometry->VertexBufferView());
    mCommandList->IASetIndexBuffer(&mMesh->mGeometry->IndexBufferView());

    mCommandList->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    mCommandList->SetGraphicsRootSignature(mShader->mRootSignature.Get());
    mCommandList->SetGraphicsRootConstantBufferView(0, mObjectCB->Resource()->GetGPUVirtualAddress());
    mCommandList->SetGraphicsRootConstantBufferView(1, add);

    mCommandList->SetPipelineState(mShader->mPSO.Get());

    // End
    mCommandList->DrawIndexedInstanced(mMesh->mGeometry->DrawArgs["Geometry"].IndexCount, 1, 0, 0, 0);
}
