#pragma once
#include "Object.h"

class RotatingObject : public Object
{
public:
    RotatingObject(Mesh* mesh, Shader* shader, const XMFLOAT3& startPos, const XMFLOAT3& startRot,
        const ComPtr<ID3D12Device>& md3dDevice);

    void Update(const GameTimer& gt) override;
};
