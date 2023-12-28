#pragma once

#include "Vertex.h"
#include "Common/d3dApp.h"

using Microsoft::WRL::ComPtr;
using namespace DirectX;
using namespace std;

class Mesh
{
public:
	Mesh();

	void Create(vector<Vertex>& _vertices, vector<uint16_t>& _indices, ComPtr<ID3D12Device> md3dDevice,
	     ComPtr<ID3D12GraphicsCommandList> mCommandList);

private:
	vector<Vertex> vertices = vector<Vertex>();
	vector<uint16_t> indices = vector<uint16_t>();
public:
	unique_ptr<MeshGeometry> mGeometry = nullptr;
};

