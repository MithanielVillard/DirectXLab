#pragma once
#include "D12DebugLayer.h"

class RenderContext
{
public:
	static bool Init();

	static IDXGIFactory* GetDXGIFactory() { return Instance().mDxgiFactory; }
	static ID3D12Device* GetDevice() { return Instance().mDevice; }
	static ID3D12Fence* GetFence() { return Instance().mFence; }
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


#ifdef _DEBUG
	D12DebugLayer mDebugLayer;
#endif

};

