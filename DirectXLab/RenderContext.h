#pragma once
#include "D12DebugLayer.h"

class StaticBuffer;

class RenderContext
{
public:
	static bool Init();
	static void FlushCommandQueue();
	static void UploadBuffer(StaticBuffer& buffer);

	static IDXGIFactory* GetDXGIFactory() { return Instance().mDxgiFactory; }
	static ID3D12Device* GetDevice() { return Instance().mDevice; }
	static ID3D12Fence* GetFence() { return Instance().mFence; }
	static ID3D12CommandQueue* GetCommandQueue() { return Instance().mCommandQueue; }
	static ulong& GetCurrentFenceValue() { return Instance().mCurrentFenceValue; }

	inline static uint sRtvDescriptorSize;
	inline static uint sDsvDescriptorSize;
	inline static uint sCbvSrvUavDescriptorSize;
private:
	RenderContext() = default;
	~RenderContext();

	static RenderContext& Instance()
	{
		static RenderContext rc;
		return rc;
	}

private:

	IDXGIFactory* mDxgiFactory;
	ID3D12Device* mDevice;

	ID3D12Fence* mFence;
	ulong mCurrentFenceValue = 0;

	ID3D12CommandQueue* mCommandQueue;
	ID3D12CommandAllocator* mCommandAllocator;
	ID3D12GraphicsCommandList* mCommandList;

#ifdef _DEBUG
	D12DebugLayer mDebugLayer;
#endif

};

