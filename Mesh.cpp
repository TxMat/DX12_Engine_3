#include "Mesh.h"

Mesh::Mesh(vector<Vertex>& _vertices, vector<uint16_t>& _indices, ComPtr<ID3D12Device> md3dDevice, ComPtr<ID3D12GraphicsCommandList> mCommandList)
{
	vertices.assign(_vertices.begin(), _vertices.end());
	indices.assign(_indices.begin(), _indices.end());

    constexpr UINT vbByteSize = static_cast<UINT>(vertices.size()) * sizeof(Vertex);
    constexpr UINT ibByteSize = static_cast<UINT>(indices.size()) * sizeof(std::uint16_t);

    mGeometry = std::make_unique<MeshGeometry>();
    mGeometry->Name = "boxGeo";

    ThrowIfFailed(D3DCreateBlob(vbByteSize, &mGeometry->VertexBufferCPU));
    CopyMemory(mGeometry->VertexBufferCPU->GetBufferPointer(), vertices.data(), vbByteSize);

    ThrowIfFailed(D3DCreateBlob(ibByteSize, &mGeometry->IndexBufferCPU));
    CopyMemory(mGeometry->IndexBufferCPU->GetBufferPointer(), indices.data(), ibByteSize);

    mGeometry->VertexBufferGPU = d3dUtil::CreateDefaultBuffer(md3dDevice.Get(),
        mCommandList.Get(), vertices.data(), vbByteSize,
        mGeometry->VertexBufferUploader);

    mGeometry->IndexBufferGPU = d3dUtil::CreateDefaultBuffer(md3dDevice.Get(),
        mCommandList.Get(), indices.data(), ibByteSize,
        mGeometry->IndexBufferUploader);

    mGeometry->VertexByteStride = sizeof(Vertex);
    mGeometry->VertexBufferByteSize = vbByteSize;
    mGeometry->IndexFormat = DXGI_FORMAT_R16_UINT;
    mGeometry->IndexBufferByteSize = ibByteSize;

    SubmeshGeometry submesh;
    submesh.IndexCount = static_cast<UINT>(indices.size());
    submesh.StartIndexLocation = 0;
    submesh.BaseVertexLocation = 0;

    mGeometry->DrawArgs["box"] = submesh;
}
