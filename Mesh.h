#pragma once

#include "Vertex.h"
#include "Common/d3dApp.h"

using Microsoft::WRL::ComPtr;
using namespace DirectX;
using namespace std;

class Mesh
{
public:
	Mesh(Vertex[] _vertices, uint16_t[] _indices, ComPtr<ID3D12Device> md3dDevice, ComPtr<ID3D12GraphicsCommandList> mCommandList);

private:
	Vertex[] vertices;
	uint16_t[] indices;

	unique_ptr<MeshGeometry> mGeometry = nullptr;
};

