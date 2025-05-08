#include "pch.h"
#include "RenderContext.h"

#include "StaticBuffer.h"
#include "Window.h"

bool RenderContext::Init()
{
	RenderContext& context = Instance();

	HRESULT res = CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG,IID_PPV_ARGS(&context.mDxgiFactory));
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

	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

	res = context.mDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&context.mCommandQueue));
	if (FAILED(res))
	{
		PRINT_COM_ERROR("Failed to create command queue", res);
		return false;
	}


	res = context.mDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&context.mCommandAllocator));
	if (FAILED(res))
	{
		PRINT_COM_ERROR("Failed to create RenderContext command allocator", res);
		return false;
	}

	res = context.mDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, context.mCommandAllocator, nullptr, IID_PPV_ARGS(&context.mCommandList));
	if (FAILED(res))
	{
		PRINT_COM_ERROR("Failed to create RenderContext command list", res);
		return false;
	}

	res = context.mDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&context.mFence));
	if (FAILED(res))
	{
		PRINT_COM_ERROR("Failed to create fence", res);
		return false;
	}

	context.mCommandList->Close();

	context.sRtvDescriptorSize = context.mDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	context.sDsvDescriptorSize = context.mDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	context.sCbvSrvUavDescriptorSize = context.mDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	return true;
}

void RenderContext::FlushCommandQueue()
{
	RenderContext& context = Instance();

	context.mCurrentFenceValue++;
	context.mCommandQueue->Signal(context.mFence, context.mCurrentFenceValue);

	if (context.mFence->GetCompletedValue() < context.mCurrentFenceValue)
	{
		HANDLE eventHandle = CreateEventEx(nullptr, L"", false, EVENT_ALL_ACCESS);

		context.mFence->SetEventOnCompletion(context.mCurrentFenceValue, eventHandle);

		WaitForSingleObject(eventHandle, INFINITE);
		CloseHandle(eventHandle);
	}
}

void RenderContext::EndFrame()
{
	RenderContext& context = Instance();
	context.mCommandQueue->ExecuteCommandLists(static_cast<UINT>(context.mPendingLists.size()), context.mPendingLists.data());
	FlushCommandQueue();
	context.mPendingLists.clear();
}

void RenderContext::UploadBuffer(StaticBuffer& buffer)
{
	RenderContext& context = Instance();

	context.mCommandAllocator->Reset();
	context.mCommandList->Reset(context.mCommandAllocator, nullptr);

	CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(buffer.mDefaultBuffer, D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST);
	context.mCommandList->ResourceBarrier(1, &barrier);

	UpdateSubresources<1>(context.mCommandList, buffer.mDefaultBuffer, buffer.mUploadBuffer, 0, 0, 1, &buffer.mSubResourceData);

	barrier = CD3DX12_RESOURCE_BARRIER::Transition(buffer.mDefaultBuffer, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_GENERIC_READ);
	context.mCommandList->ResourceBarrier(1, &barrier);

	context.mCommandList->Close();
	ID3D12CommandList* lists[] = { context.mCommandList };
	context.mCommandQueue->ExecuteCommandLists(1, lists);

	FlushCommandQueue();
	buffer.mUploadBuffer->Release();
	buffer.mUploadBuffer = nullptr;
}

uint RenderContext::GetMSAAQualityLevel(uint count)
{
	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS desc = {};
	desc.Format = Window::sBackBufferFormat;
	desc.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
	desc.NumQualityLevels = 0;
	desc.SampleCount = count;
	Instance().mDevice->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, &desc, sizeof(desc));

	return desc.NumQualityLevels - 1;
}

RenderContext::~RenderContext()
{
	mDxgiFactory->Release();
	mDevice->Release();
	mFence->Release();
	mCommandQueue->Release();
	mCommandAllocator->Release();
	mCommandList->Release();
}
