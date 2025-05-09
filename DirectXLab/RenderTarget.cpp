#include "pch.h"
#include "RenderTarget.h"

#include "Camera.h"
#include "D12ComputePipelineObject.h"
#include "D12PipelineObject.h"
#include "Geometry.h"
#include "RenderContext.h"
#include "Transform.h"
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

	mCommandList->Close();

	CreateRTVHeap();
	CreateDSVHeap();

	CreateViewport();

	RetrieveRTBuffer();
	RetrieveDSBuffer();
}

void RenderTarget::CreateDSVHeap()
{
	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc = {};
	descHeapDesc.NodeMask = 0;
	descHeapDesc.NumDescriptors = 1;
	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;

	HRESULT res = RenderContext::GetDevice()->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&mDSVHeap));
	if (FAILED(res))
	{
		PRINT_COM_ERROR("Failed to create depth stencil heap", res);
	}
}

void RenderTarget::RetrieveDSBuffer()
{

	CD3DX12_HEAP_PROPERTIES dsvHeapProp(D3D12_HEAP_TYPE_DEFAULT);
	CD3DX12_RESOURCE_DESC depthStencilDesc(D3D12_RESOURCE_DIMENSION_TEXTURE2D,
		0,
		mWidth,
		mHeight,
		1,
		1,
		Window::sDepthStencilFormat,
		RenderContext::GetMSAACount(),
		RenderContext::GetMSAAQualityLevel(),
		D3D12_TEXTURE_LAYOUT_UNKNOWN,
		D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);

	D3D12_CLEAR_VALUE optClear = {};
	optClear.Format = Window::sDepthStencilFormat;
	optClear.DepthStencil.Depth = 1.0f;
	optClear.DepthStencil.Stencil = 0;

	RenderContext::GetDevice()->CreateCommittedResource(&dsvHeapProp, D3D12_HEAP_FLAG_NONE, &depthStencilDesc, D3D12_RESOURCE_STATE_DEPTH_WRITE, &optClear, IID_PPV_ARGS(&mDepthStencilBuffer));

	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.Format = Window::sDepthStencilFormat;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2DMS;
	dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
	RenderContext::GetDevice()->CreateDepthStencilView(mDepthStencilBuffer, &dsvDesc, mDSVHeap->GetCPUDescriptorHandleForHeapStart());
}


void RenderTarget::CreateRTVHeap()
{
	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc = {};
	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	descHeapDesc.NumDescriptors = 1;
	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	descHeapDesc.NodeMask = 0;

	HRESULT res = RenderContext::GetDevice()->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&mRTVHeap));
	if (FAILED(res))
	{
		PRINT_COM_ERROR("Failed to create render target heap", res);
	}
}

void RenderTarget::RetrieveRTBuffer()
{
	CD3DX12_HEAP_PROPERTIES heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	CD3DX12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		Window::sBackBufferFormat,
		mWidth,
		mHeight,
		1,
		1,
		RenderContext::GetMSAACount(),
		RenderContext::GetMSAAQualityLevel(),
		D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET
	);

	D3D12_CLEAR_VALUE optClear = {};
	optClear.Format = Window::sBackBufferFormat;
	memcpy(optClear.Color, Window::sClearColor, sizeof(float) * 4);

	HRESULT res = RenderContext::GetDevice()->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		D3D12_RESOURCE_STATE_RESOLVE_SOURCE,
		&optClear,
		IID_PPV_ARGS(&mRenderTargetBuffer)
	);
	mRenderTargetBuffer->SetName(L"Render Target Texture");

	if (FAILED(res))
	{
		PRINT_COM_ERROR("Failed to create render target", res);
	}

	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
	rtvDesc.Format = Window::sBackBufferFormat;
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DMS;

	RenderContext::GetDevice()->CreateRenderTargetView(mRenderTargetBuffer, &rtvDesc, mRTVHeap->GetCPUDescriptorHandleForHeapStart());
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

void RenderTarget::Resize(uint width, uint height)
{
	mRenderTargetBuffer->Release();
	mDepthStencilBuffer->Release();

	mWidth = width;
	mHeight = height;

	CreateViewport();

	RetrieveRTBuffer();
	RetrieveDSBuffer();
}

void RenderTarget::Begin(Camera const& camera)
{
	mpCamera = &camera;

	mCommandAllocator->Reset();
	mCommandList->Reset(mCommandAllocator, nullptr);

	CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(mRenderTargetBuffer, D3D12_RESOURCE_STATE_RESOLVE_SOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET);

	mCommandList->ResourceBarrier(1, &barrier);

	mCommandList->RSSetViewports(1, &mViewport);
	mCommandList->RSSetScissorRects(1, &mScissorRect);

	D3D12_CPU_DESCRIPTOR_HANDLE rtv = mRTVHeap->GetCPUDescriptorHandleForHeapStart();
	D3D12_CPU_DESCRIPTOR_HANDLE dsv = mDSVHeap->GetCPUDescriptorHandleForHeapStart();

	mCommandList->OMSetRenderTargets(1, &rtv, false, &dsv);

	mCommandList->ClearRenderTargetView(rtv, Window::sClearColor, 0, nullptr);
	mCommandList->ClearDepthStencilView(dsv, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);
}

void RenderTarget::Draw(Geometry& geo, D12PipelineObject const& pso, Transform const& transform, D3D12_GPU_VIRTUAL_ADDRESS addr,
	D12ComputePipelineObject const* computePso)
{

	//== Compute shader ==
	mCommandList->SetPipelineState(computePso->mPipelineState);
	mCommandList->SetComputeRootSignature(pso.mRootSignature);
	mCommandList->SetComputeRootUnorderedAccessView(3, addr);

	mCommandList->SetComputeRoot32BitConstant(4, reinterpret_cast<uint&>(RenderContext::sDeltaTime), 0);
	mCommandList->Dispatch(1, 1, 1);
	// =====

	mCommandList->SetPipelineState(pso.mPipelineState);
	mCommandList->SetGraphicsRootSignature(pso.mRootSignature);

	mCommandList->SetGraphicsRootConstantBufferView(0, transform.mBuffer.GetGPUAddress());
	mCommandList->SetGraphicsRootConstantBufferView(1, mpCamera->mBuffer.GetGPUAddress());
	mCommandList->SetGraphicsRootShaderResourceView(2, addr);

	D3D12_VERTEX_BUFFER_VIEW vertexBuffer = geo.GetVertexBufferView();
	D3D12_INDEX_BUFFER_VIEW indexBuffer = geo.GetIndexBufferView();

	mCommandList->IASetVertexBuffers(0, 1, &vertexBuffer);
	mCommandList->IASetIndexBuffer(&indexBuffer);
	mCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	mCommandList->DrawIndexedInstanced(geo.GetIndicesCount(), 20, 0, 0, 0);
}

void RenderTarget::End()
{
	CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(mRenderTargetBuffer, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_RESOLVE_SOURCE);
	mCommandList->ResourceBarrier(1, &barrier);

	mCommandList->Close();
	RenderContext::AddPendingList(mCommandList);
}

RenderTarget::RenderTarget(RenderTarget const& other)
{
	mRenderTargetBuffer = other.mRenderTargetBuffer;
	mRenderTargetBuffer->AddRef();
}

RenderTarget::~RenderTarget()
{
	mCommandAllocator->Release();
	mCommandList->Release();
	mDepthStencilBuffer->Release();
	mRenderTargetBuffer->Release();
	mRTVHeap->Release();
	mDSVHeap->Release();
}
