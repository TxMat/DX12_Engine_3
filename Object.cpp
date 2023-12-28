#include "Object.h"

void Object::Update(const GameTimer& gt)
{
    
}

void Object::Draw(ComPtr<ID3D12GraphicsCommandList> mCommandList)
{
    mMesh.Draw(mCommandList);
}
