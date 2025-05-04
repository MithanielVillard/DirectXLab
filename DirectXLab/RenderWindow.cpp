#include "pch.h"
#include "RenderWindow.h"

#include <thread>

#include "Camera.h"
#include "D12PipelineObject.h"
#include "Geometry.h"
#include "RenderContext.h"
#include "Transform.h"

RenderWindow::RenderWindow(std::wstring_view const title, int const width, int const height) : Window(title, width, height){}

void RenderWindow::BeginDraw(const Camera& camera)
{
	mpCamera = &camera;

	mCommandAllocator->Reset();
	mCommandList->Reset(mCommandAllocator, nullptr);

	CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(GetCurrentBackBuffer(),D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
	mCommandList->ResourceBarrier(1, &barrier);

	mCommandList->RSSetViewports(1, &mViewPort);
	mCommandList->RSSetScissorRects(1, &mScissorRect);

	CD3DX12_CPU_DESCRIPTOR_HANDLE currentBackBufferView(mSCBuffersHeap->GetCPUDescriptorHandleForHeapStart(), mCurrentBackBuffer, RenderContext::sRtvDescriptorSize);
	D3D12_CPU_DESCRIPTOR_HANDLE depthStencilView = mDSHeap->GetCPUDescriptorHandleForHeapStart();

	mCommandList->OMSetRenderTargets(1, &currentBackBufferView, true, &depthStencilView);

	mCommandList->ClearRenderTargetView(currentBackBufferView, DirectX::Colors::LightSteelBlue, 0, nullptr);
	mCommandList->ClearDepthStencilView(depthStencilView, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

}

void RenderWindow::Draw(Geometry& geo, const D12PipelineObject& pso, const Transform& transform)
{
	mCommandList->SetGraphicsRootSignature(pso.mRootSignature);
	mCommandList->SetPipelineState(pso.mPipelineState);

	mCommandList->SetGraphicsRootConstantBufferView(0, transform.mBuffer.GetGPUAddress());
	mCommandList->SetGraphicsRootConstantBufferView(1, mpCamera->mBuffer.GetGPUAddress());

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

