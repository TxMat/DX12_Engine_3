#pragma once
#include "Common/d3dApp.h"

struct ObjectConstants
{
    XMFLOAT4X4 WorldViewProj = MathHelper::Identity4x4();
};