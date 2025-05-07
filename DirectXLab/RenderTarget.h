#pragma once

class D12ComputePipelineObject;
class Transform;
class D12PipelineObject;
class Geometry;
class Camera;

class RenderTarget
{
public:

	RenderTarget(uint width, uint height)
	RenderTarget(RenderTarget const& other);
	RenderTarget(RenderTarget&&) noexcept = delete;

	~RenderTarget();

	void CreateRTVHeap();
	void CreateViewport();

	void Begin(Camera& camera);
	void Draw(Geometry& geo, D12PipelineObject const& pso, Transform const& transform, D12ComputePipelineObject const* computePso);
	void End();

private:
	ID3D12Resource* mBuffer = nullptr;
	ID3D12DescriptorHeap* mHeap = nullptr;

	ID3D12CommandAllocator* mCommandAllocator = nullptr;
	ID3D12GraphicsCommandList* mCommandList = nullptr;

	D3D12_CPU_DESCRIPTOR_HANDLE mHandle;

	D3D12_RECT mScissorRect;
	D3D12_VIEWPORT mViewport;

	uint mWidth;
	uint mHeight;
};

