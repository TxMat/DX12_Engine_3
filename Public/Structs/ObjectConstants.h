#pragma once
#include <DirectXMath.h>

#include "MathHelper.h"

struct ObjectConstants
{
    DirectX::XMFLOAT4X4 WorldViewProj = MathHelper::Identity4x4();
};