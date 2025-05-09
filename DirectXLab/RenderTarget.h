#pragma once
#include <vector>

#include "StaticBuffer.h"

class D12ComputePipelineObject;
class Transform;
class D12PipelineObject;
class Geometry;
class Camera;

class RenderTarget
{
public:

	RenderTarget(uint width, uint height);
	RenderTarget(RenderTarget const& other);
	RenderTarget(RenderTarget&&) noexcept = delete;

	~RenderTarget();

	void Begin(const Camera& camera);
	void Draw(Geometry& geo, D12PipelineObject const& pso, Transform const& transform, ParticleEmitter& emitter, D12ComputePipelineObject
	          const* computePso);
	void End();

	void Resize(uint width, uint height);

protected:
	void CreateRTVHeap();
	void CreateDSVHeap();

	void RetrieveRTBuffer();
	void RetrieveDSBuffer();

	void CreateViewport();

protected:
	ID3D12Resource* mRenderTargetBuffer = nullptr;
	ID3D12Resource* mDepthStencilBuffer = nullptr;
	ID3D12DescriptorHeap* mRTVHeap = nullptr;
	ID3D12DescriptorHeap* mDSVHeap = nullptr;

	ID3D12CommandAllocator* mCommandAllocator = nullptr;
	ID3D12GraphicsCommandList* mCommandList = nullptr;

	D3D12_RECT mScissorRect;
	D3D12_VIEWPORT mViewport;

	uint mWidth;
	uint mHeight;

	Camera const* mpCamera;

	friend class RenderWindow;
};

