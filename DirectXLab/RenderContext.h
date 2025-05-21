#pragma once
#include <vector>
#include "D12DebugLayer.h"

class StaticBuffer;

class RenderContext
{
public:
	static bool Init();
	static void FlushCommandQueue();
	static void ExecuteLists();
	static void AddPendingList(ID3D12CommandList* pList) { Instance().mPendingLists.push_back(pList); }
	static void UploadBuffer(StaticBuffer& buffer);

	static void SetMSAA(const uint count) { Instance().mMSAACount = count; }

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

	static uint GetMSAAQualityLevel();
	static uint GetMSAACount() { return Instance().mMSAACount; }

private:

	IDXGIFactory* mDxgiFactory = nullptr;
	ID3D12Device* mDevice = nullptr;

	ID3D12Fence* mFence = nullptr;
	ulong mCurrentFenceValue = 0;

	ID3D12CommandQueue* mCommandQueue = nullptr;
	ID3D12CommandAllocator* mCommandAllocator = nullptr;
	ID3D12GraphicsCommandList* mCommandList = nullptr;

	std::vector<ID3D12CommandList*> mPendingLists;

	uint mMSAACount = 1;

#ifdef _DEBUG
	D12DebugLayer mDebugLayer;
#endif

	friend class RenderTarget;
	friend class D12PipelineObject;

};

