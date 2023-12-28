#include "Mesh.h"

Mesh::Mesh(vector<Vertex>& _vertices, vector<uint16_t>& _indices, ComPtr<ID3D12Device> md3dDevice, ComPtr<ID3D12GraphicsCommandList> mCommandList)
{
	vertices.assign(_vertices.begin(), _vertices.end());
	indices.assign(_indices.begin(), _indices.end());
}
