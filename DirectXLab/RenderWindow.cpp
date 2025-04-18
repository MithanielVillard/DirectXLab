#include "pch.h"
#include "RenderWindow.h"

#include "RenderContext.h"

void RenderWindow::BeginFrame()
{
	OpenCommandList();

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

void RenderWindow::EndFrame()
{
	
	CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(GetCurrentBackBuffer(),D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
	mCommandList->ResourceBarrier(1, &barrier);

	ExecuteCommandList();
	mSwapChain->Present(0, 0);
	mCurrentBackBuffer = (mCurrentBackBuffer + 1) % sSwapChainBufferCount;
}

