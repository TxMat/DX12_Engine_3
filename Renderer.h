#include <d3d12.h>
#include <wrl/client.h>

#include "Object.h"

// Renderer.h
class Renderer
{
public:
    Renderer(ComPtr<ID3D12Device> device, ComPtr<ID3D12GraphicsCommandList> commandList);

    void DrawObject(const Object& object);

private:
    ComPtr<ID3D12Device> mDevice;
    ComPtr<ID3D12GraphicsCommandList> mCommandList;

    // Other necessary DirectX resources...
};