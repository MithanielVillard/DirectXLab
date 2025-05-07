#include "pch.h"
#include "RenderTarget.h"

#include "RenderContext.h"
#include "Window.h"

RenderTarget::RenderTarget(uint const width, uint const height) : mWidth(width), mHeight(height)
{
	HRESULT res = RenderContext::GetDevice()->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&mCommandAllocator));
	if (FAILED(res))
	{
		PRINT_COM_ERROR("Failed to create command allocator in render target", res);
		return;
	}

	res = RenderContext::GetDevice()->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, mCommandAllocator, nullptr, IID_PPV_ARGS(&mCommandList));
	if (FAILED(res))
	{
		PRINT_COM_ERROR("Failed to create command list in render target", res);
		return;
	}

	CD3DX12_HEAP_PROPERTIES heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	CD3DX12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		Window::sBackBufferFormat, 
		width, 
		height,
		1,
		0,
		1,
		0,
		D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET
	);

	D3D12_CLEAR_VALUE optClear = {};
	optClear.Format = Window::sBackBufferFormat;
	memcpy(optClear.Color, Window::sClearColor, sizeof(float) * 4);

	res = RenderContext::GetDevice()->CreateCommittedResource(
		&heapProp, 
		D3D12_HEAP_FLAG_NONE, 
		&resourceDesc, 
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE,
		&optClear,
		IID_PPV_ARGS(&mBuffer)
	);

	if (FAILED(res))
	{
		PRINT_COM_ERROR("Failed to create render target", res);
		return;
	}

	CreateRTVHeap();
	CreateViewport();
}

void RenderTarget::CreateRTVHeap()
{
	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc = {};
	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	descHeapDesc.NumDescriptors = 1;
	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	descHeapDesc.NodeMask = 0;

	HRESULT res = RenderContext::GetDevice()->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&mHeap));
	if (FAILED(res))
	{
		PRINT_COM_ERROR("Failed to create render target heap", res);
		return;
	}

	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
	rtvDesc.Format = Window::sBackBufferFormat;
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

	RenderContext::GetDevice()->CreateRenderTargetView(mBuffer, &rtvDesc, mHeap->GetCPUDescriptorHandleForHeapStart());
}

void RenderTarget::CreateViewport()
{
	mViewport.TopLeftX = mViewport.TopLeftY = 0;
	mViewport.Width = static_cast<float>(mWidth);
	mViewport.Height = static_cast<float>(mHeight);
	mViewport.MinDepth = 0.f;
	mViewport.MaxDepth = 1.f;

	mScissorRect.left = mScissorRect.top = 0;
	mScissorRect.right = static_cast<LONG>(mWidth);
	mScissorRect.bottom = static_cast<LONG>(mHeight);
}

void RenderTarget::Begin(Camera& camera)
{
	mCommandAllocator->Reset();
	mCommandList->Reset(mCommandAllocator, nullptr);

	CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(mBuffer, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET);

	mCommandList->ResourceBarrier(1, &barrier);

	mCommandList->RSSetViewports(1, &mViewport);
	mCommandList->RSSetScissorRects(1, &mScissorRect);

	//mCommandList->OMSetRenderTargets(1, &mHeap->GetCPUDescriptorHandleForHeapStart(), false, )
}

RenderTarget::RenderTarget(RenderTarget const& other)
{
	mBuffer = other.mBuffer;
	mBuffer->AddRef();
}

RenderTarget::~RenderTarget()
{
	mBuffer->Release();
}

