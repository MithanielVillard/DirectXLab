#pragma once
#include "D12DebugLayer.h"

enum MSAA_COUNT : char
{
	NONE = 1,
	MSAA_COUNT_X2 = 2,
	MSAA_COUNT_X4 = 4,
	MSAA_COUNT_X8 = 8
};

class StaticBuffer;

class RenderContext
{
public:
	static bool Init();
	static void FlushCommandQueue();
	static void UploadBuffer(StaticBuffer& buffer);

	static void SetMSAA(MSAA_COUNT const count) { Instance().mMSAACount = count; }

	static IDXGIFactory* GetDXGIFactory() { return Instance().mDxgiFactory; }
	static ID3D12Device* GetDevice() { return Instance().mDevice; }
	static ID3D12Fence* GetFence() { return Instance().mFence; }
	static ID3D12CommandQueue* GetCommandQueue() { return Instance().mCommandQueue; }
	static ulong& GetCurrentFenceValue() { return Instance().mCurrentFenceValue; }

	inline static uint sRtvDescriptorSize;
	inline static uint sDsvDescriptorSize;
	inline static uint sCbvSrvUavDescriptorSize;
	static MSAA_COUNT GetMSAACount() { return Instance().mMSAACount; }

private:
	RenderContext() = default;
	~RenderContext();

	static RenderContext& Instance()
	{
		static RenderContext rc;
		return rc;
	}

	static uint GetMSAAQualityLevel();

private:

	IDXGIFactory* mDxgiFactory = nullptr;
	ID3D12Device* mDevice = nullptr;

	ID3D12Fence* mFence = nullptr;
	ulong mCurrentFenceValue = 0;

	ID3D12CommandQueue* mCommandQueue = nullptr;
	ID3D12CommandAllocator* mCommandAllocator = nullptr;
	ID3D12GraphicsCommandList* mCommandList = nullptr;

	MSAA_COUNT mMSAACount = NONE;

#ifdef _DEBUG
	D12DebugLayer mDebugLayer;
#endif

	friend class RenderTarget;
	friend class D12PipelineObject;

};

