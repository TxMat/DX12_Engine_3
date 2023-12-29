#include "RotatingObject.h"

RotatingObject::RotatingObject(Mesh* mesh, Shader* shader, const XMFLOAT3& startPos, const XMFLOAT3& startRot,
    const ComPtr<ID3D12Device>& md3dDevice): Object(mesh, shader, startPos, md3dDevice)
{
    mTransform.Rotate(startRot.x, startRot.y, startRot.z);
    mTransform.ApplyChanges();
}

void RotatingObject::Update(const GameTimer& gt)
{
    Object::Update(gt);
    mTransform.Rotate(gt.DeltaTime(), 0, 0);
    mTransform.TranslateLocal(0, 0, gt.DeltaTime());
    mTransform.ApplyChanges();

    ObjectConstants objConstants;
    XMStoreFloat4x4(&objConstants.WorldViewProj, XMMatrixTranspose(XMLoadFloat4x4(&mTransform.matrix)));

    mObjectCB->CopyData(0, objConstants);
}
