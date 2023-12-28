#include "Mesh.h"

class Mesh
{
	Mesh(vector<Vertex> _vertices, vector<uint16_t> _indices, ComPtr<ID3D12Device> md3dDevice, ComPtr<ID3D12GraphicsCommandList> mCommandList)
	{
		Mesh::vertices = _vertices;
		Mesh::indices = _indices;
	}
};