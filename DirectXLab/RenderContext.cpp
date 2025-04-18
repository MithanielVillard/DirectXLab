#include "pch.h"
#include "RenderContext.h"

bool RenderContext::Init()
{
	RenderContext& context = Instance();

	HRESULT res = CreateDXGIFactory(IID_PPV_ARGS(&context.mDxgiFactory));
	if (FAILED(res))
	{
		PRINT_COM_ERROR("Error while creating factory", res);
		return false;
	}

	res = D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&context.mDevice));
	if (FAILED(res))
	{
		PRINT_COM_ERROR("Error while creating device", res);
		return false;
	}

	context.mDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&context.mFence));

	context.sRtvDescriptorSize = context.mDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	context.sDsvDescriptorSize = context.mDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	context.sCbvSrvUavDescriptorSize = context.mDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	return true;
}

RenderContext::~RenderContext()
{
	mDxgiFactory->Release();
	mDevice->Release();
	mFence->Release();
}
