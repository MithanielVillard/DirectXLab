#include "pch.h"
#include "RenderWindow.h"

#include "Camera.h"
#include "RenderContext.h"
#include "RenderTarget.h"

RenderWindow::RenderWindow(std::wstring_view const title, int const width, int const height) : Window(title, width, height) {}

void RenderWindow::Begin3D(Camera const& camera)
{
	mCommandAllocator->Reset();
	mCommandList->Reset(mCommandAllocator, nullptr);

	CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(GetCurrentBackBuffer(),D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RESOLVE_DEST);
	mCommandList->ResourceBarrier(1, &barrier);

	mTarget.Begin3D(camera);

}

void RenderWindow::Draw(Geometry& geo, D12PipelineObject const& pso, Transform const& transform)
{
	mTarget.Draw(geo, pso, transform);
}

void RenderWindow::EndDraw()
{
	//use CopyResource() to copy resource without MSAA
	//mCommandList->CopyResource(mSwapChainBuffers[mCurrentBackBuffer], rt.mRenderTargetBuffer);
	mTarget.End();

	mCommandList->ResolveSubresource(mSwapChainBuffers[mCurrentBackBuffer], 0, mTarget.mRenderTargetBuffer, 0, sBackBufferFormat);

	CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(GetCurrentBackBuffer(), D3D12_RESOURCE_STATE_RESOLVE_DEST, D3D12_RESOURCE_STATE_PRESENT);
	mCommandList->ResourceBarrier(1, &barrier);

	mCommandList->Close();
	RenderContext::AddPendingList(mCommandList);
	RenderContext::ExecuteLists();

	FlushCmdQueue();

	mSwapChain->Present(0, 0);
	mCurrentBackBuffer = (mCurrentBackBuffer + 1) % sSwapChainBufferCount;
}

void RenderWindow::OnWindowResize()
{
	Window::OnWindowResize();
	mTarget.Resize(mWidth, mHeight);
}

