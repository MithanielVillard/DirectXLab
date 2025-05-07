#include "pch.h"
#include "RenderWindow.h"

#include "Camera.h"
#include "D12ComputePipelineObject.h"
#include "D12PipelineObject.h"
#include "Geometry.h"
#include "RenderContext.h"
#include "Transform.h"

RenderWindow::RenderWindow(std::wstring_view const title, int const width, int const height) : Window(title, width, height)
{
	mComputeInput.Init(mComputeData.data(), mComputeData.size() * sizeof(int));

	CD3DX12_HEAP_PROPERTIES prop = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	CD3DX12_RESOURCE_DESC resource = CD3DX12_RESOURCE_DESC::Buffer(mComputeData.size() * sizeof(int), D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);

	HRESULT res = RenderContext::GetDevice()->CreateCommittedResource(&prop, D3D12_HEAP_FLAG_NONE, &resource, D3D12_RESOURCE_STATE_COMMON, nullptr, IID_PPV_ARGS(&mComputeOutput));
	if (FAILED(res))
	{
		PRINT_COM_ERROR("Failed to create compute output buffer", res);
		return;
	}
}

RenderWindow::~RenderWindow()
{
	mComputeOutput->Release();
}

void RenderWindow::BeginDraw(const Camera& camera)
{
	mpCamera = &camera;

	mCommandAllocator->Reset();
	mCommandList->Reset(mCommandAllocator, nullptr);

	CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(GetCurrentBackBuffer(),D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

	mCommandList->ResourceBarrier(1, &barrier);

	mCommandList->RSSetViewports(1, &mViewPort);
	mCommandList->RSSetScissorRects(1, &mScissorRect);

	D3D12_CPU_DESCRIPTOR_HANDLE depthStencilView = mDSHeap->GetCPUDescriptorHandleForHeapStart();

	mCommandList->OMSetRenderTargets(1, &mRtvHandles[mCurrentBackBuffer], true, &depthStencilView);

	mCommandList->ClearRenderTargetView(mRtvHandles[mCurrentBackBuffer], sClearColor, 0, nullptr);
	mCommandList->ClearDepthStencilView(depthStencilView, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

}

void RenderWindow::Draw(Geometry& geo, D12PipelineObject const& pso, Transform const& transform, D12ComputePipelineObject const* computePso)
{
	//compute shader
	if (computePso != nullptr)
	{
		mCommandList->SetComputeRootSignature(pso.mRootSignature);
		mCommandList->SetPipelineState(computePso->mPipelineState);

		mCommandList->SetComputeRootShaderResourceView(2, mComputeInput.GetGPUAddress());
		mCommandList->SetComputeRootUnorderedAccessView(3, mComputeOutput->GetGPUVirtualAddress());

		mCommandList->Dispatch(1, 1, 1);

		//CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(mComputeOutput, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_SOURCE);
		//mCommandList->ResourceBarrier(1, &barrier);
		//
		//mCommandList->CopyResource(mReadBackBuffer.GetResource(), mComputeOutput);
		//
		//barrier = CD3DX12_RESOURCE_BARRIER::Transition(mComputeOutput, D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
		//mCommandList->ResourceBarrier(1, &barrier);

		//PRINT_DEBUG(mReadBackBuffer.GetData()[1]);
	}

	mCommandList->SetGraphicsRootSignature(pso.mRootSignature);
	mCommandList->SetPipelineState(pso.mPipelineState);

	mCommandList->SetGraphicsRootConstantBufferView(0, transform.mBuffer.GetGPUAddress());
	mCommandList->SetGraphicsRootConstantBufferView(1, mpCamera->mBuffer.GetGPUAddress());
	mCommandList->SetGraphicsRootUnorderedAccessView(3, mComputeOutput->GetGPUVirtualAddress());

	D3D12_VERTEX_BUFFER_VIEW vertexBuffer = geo.GetVertexBufferView();
	D3D12_INDEX_BUFFER_VIEW indexBuffer = geo.GetIndexBufferView();

	mCommandList->IASetVertexBuffers(0, 1, &vertexBuffer);
	mCommandList->IASetIndexBuffer(&indexBuffer);
	mCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	mCommandList->DrawIndexedInstanced(geo.GetIndicesCount(), 1, 0, 0, 0);
}

void RenderWindow::EndDraw()
{
	CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(GetCurrentBackBuffer(),D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
	mCommandList->ResourceBarrier(1, &barrier);

	mCommandList->Close();
	RenderContext::AddPendingList(mCommandList);
}

void RenderWindow::Display()
{
	mSwapChain->Present(0, 0);
	mCurrentBackBuffer = (mCurrentBackBuffer + 1) % sSwapChainBufferCount;
}

