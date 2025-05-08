#include "pch.h"
#include "Window.h"

#include "RenderContext.h"

Window::Window(std::wstring_view title, int width, int height) : mHInstance(GetModuleHandle(nullptr)),
                                                                 mWindowHandle(nullptr), mWidth(width), mHeight(height)
{
	CreateWindowClass(title, width, height);
	CreateCmdObjects();
	CreateSwapChain();

	CreateSwapChainBuffersHeap();
	CreateDepthStencilHeap();

	RetrieveSwapChainBuffers();
	RetrieveDepthStencilBuffer();

	ShowWindow(mWindowHandle, SW_SHOW);
	UpdateWindow(mWindowHandle);

	mIsOpen = true;
}

Window::~Window()
{
	mCommandAllocator->Release();
	mCommandList->Release();
	mSwapChain->Release();

	for (ID3D12Resource* buffer : mSwapChainBuffers) buffer->Release();
	mDepthStencilBuffer->Release();

	mSCBuffersHeap->Release();
	mDSHeap->Release();

	DestroyWindow(mWindowHandle);
	UnregisterClassW(reinterpret_cast<LPCWSTR>(mWindowClass), mHInstance);
}

LRESULT Window::MainWndProc(const HWND hwnd, const UINT msg, const WPARAM wParam, const LPARAM lParam)
{
	return DefWindowProc(hwnd, msg, wParam, lParam);
}

LRESULT Window::SubclassProc(const HWND hWnd, const UINT uMsg, const WPARAM wParam, const LPARAM lParam, UINT_PTR uIdSubclass,
	DWORD_PTR dwRefData)
{
	return reinterpret_cast<Window*>(dwRefData)->HandleEvent(hWnd, uMsg, wParam, lParam);
}

LRESULT Window::HandleEvent(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		case WM_SIZE:
			mWidth = LOWORD(lParam);
			mHeight = HIWORD(lParam);
			ResizeWindow(mWidth, mHeight);

			return 0;
		case WM_DESTROY:
			PostQuitMessage(0);
			mIsOpen = false;

			return 0;
		default: break;
	}

	return DefWindowProc(hwnd, msg, wParam, lParam);
}

void Window::ResizeViewport()
{
	mViewPort.TopLeftX = mViewPort.TopLeftY = 0;
	mViewPort.Width = static_cast<float>(mWidth);
	mViewPort.Height = static_cast<float>(mHeight);
	mViewPort.MinDepth = 0.f;
	mViewPort.MaxDepth = 1.f;

	mScissorRect.left = mScissorRect.top = 0;
	mScissorRect.right = mWidth;
	mScissorRect.bottom = mHeight;
}

void Window::ResizeWindow(int const newWidth, int const newHeight)
{
	RenderContext::FlushCommandQueue();

	ReleaseDepthStencilBuffer();
	ReleaseSwapChainBuffers();

	mSwapChain->ResizeBuffers(sSwapChainBufferCount, mWidth, mHeight, sBackBufferFormat, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH);
	ResizeViewport();

	RetrieveDepthStencilBuffer();
	RetrieveSwapChainBuffers();
	mCurrentBackBuffer = 0;
}

bool Window::CreateWindowClass(std::wstring_view title, int width, int height)
{

	WNDCLASS wc{};
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = mHInstance;
	wc.lpfnWndProc = MainWndProc;
	wc.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wc.hbrBackground = nullptr;
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = title.data();

	mWindowClass = RegisterClass(&wc);
	if (!mWindowClass)
	{
		PRINT_DEBUG("RegisterClass Failed !")
			return false;
	}

	mWindowHandle = CreateWindow(title.data(), title.data(), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, width,
		height, 0, 0, mHInstance, 0);
	if (!mWindowHandle)
	{
		PRINT_DEBUG("Failed to create window !")
			return false;
	}

	SetWindowSubclass(mWindowHandle, SubclassProc, 1, reinterpret_cast<DWORD_PTR>(this));

	return true;
}

bool Window::CreateCmdObjects()
{
	ID3D12Device* device = RenderContext::GetDevice();

	HRESULT res = device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&mCommandAllocator));
	if (FAILED(res))
	{
		PRINT_COM_ERROR("Failed to create command allocator", res);
		return false;
	}

	res = device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, mCommandAllocator, nullptr, IID_PPV_ARGS(&mCommandList));
	if (FAILED(res))
	{
		PRINT_COM_ERROR("Failed to create command list", res);
		return false;
	}

	mCommandList->Close();

	return true;
}

bool Window::CreateSwapChain()
{
	DXGI_SWAP_CHAIN_DESC sd;
	sd.BufferDesc.Width = mWidth;
	sd.BufferDesc.Height = mHeight;
	sd.BufferDesc.RefreshRate.Numerator = 0;
	sd.BufferDesc.RefreshRate.Denominator = 0;
	sd.BufferDesc.Format = sBackBufferFormat;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount = sSwapChainBufferCount;
	sd.OutputWindow = mWindowHandle;
	sd.Windowed = true;
	sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	HRESULT res = RenderContext::GetDXGIFactory()->CreateSwapChain(RenderContext::GetCommandQueue(), &sd, &mSwapChain);
	if (FAILED(res))
	{
		PRINT_COM_ERROR("Failed to create swap chain", res);
		return false;
	}

	return true;
}

bool Window::CreateSwapChainBuffersHeap()
{
	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc = {};
	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	descHeapDesc.NumDescriptors = sSwapChainBufferCount;
	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	descHeapDesc.NodeMask = 0;

	HRESULT res = RenderContext::GetDevice()->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&mSCBuffersHeap));
	if (FAILED(res))
	{
		PRINT_COM_ERROR("Failed to create swap chain buffers heap", res);
		return false;
	}

	CD3DX12_CPU_DESCRIPTOR_HANDLE firstHandle(mSCBuffersHeap->GetCPUDescriptorHandleForHeapStart());
	for (int i = 0; i < sSwapChainBufferCount; i++)
	{
		mRtvHandles[i] = firstHandle;
		firstHandle.Offset(1, RenderContext::sRtvDescriptorSize);
	}

	return true;
}

bool Window::CreateDepthStencilHeap()
{
	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc;
	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	descHeapDesc.NumDescriptors = 1;
	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	descHeapDesc.NodeMask = 0;
	HRESULT res = RenderContext::GetDevice()->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&mDSHeap));
	if (FAILED(res))
	{
		PRINT_COM_ERROR("Failed to create DSV Heap", res);
		return false;
	}

	return true;
}

bool Window::RetrieveSwapChainBuffers()
{
	for (int i = 0; i < sSwapChainBufferCount; i++)
	{
		mSwapChain->GetBuffer(i, IID_PPV_ARGS(&mSwapChainBuffers[i]));

		D3D12_RENDER_TARGET_VIEW_DESC rtvDesc;
		rtvDesc.Format = sBackBufferFormat;
		rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
		rtvDesc.Texture2D.MipSlice = 0;
		rtvDesc.Texture2D.PlaneSlice = 0;
		RenderContext::GetDevice()->CreateRenderTargetView(mSwapChainBuffers[i], &rtvDesc, mRtvHandles[i]);
	}

	return true;
}

bool Window::RetrieveDepthStencilBuffer()
{
	CD3DX12_RESOURCE_DESC depthStencilDesc(D3D12_RESOURCE_DIMENSION_TEXTURE2D, 
		0, 
		mWidth, 
		mHeight, 
		1, 
		1, 
		sDepthStencilFormat, 
		1, 
		0,
		D3D12_TEXTURE_LAYOUT_UNKNOWN, 
		D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);

	D3D12_CLEAR_VALUE optClear = {};
	optClear.Format = sDepthStencilFormat;
	optClear.DepthStencil.Depth = 1.0f;
	optClear.DepthStencil.Stencil = 0;

	CD3DX12_HEAP_PROPERTIES dsvHeapProp(D3D12_HEAP_TYPE_DEFAULT);
	RenderContext::GetDevice()->CreateCommittedResource(&dsvHeapProp, D3D12_HEAP_FLAG_NONE, &depthStencilDesc, D3D12_RESOURCE_STATE_DEPTH_WRITE, &optClear, IID_PPV_ARGS(&mDepthStencilBuffer));

	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.Format = sDepthStencilFormat;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
	RenderContext::GetDevice()->CreateDepthStencilView(mDepthStencilBuffer, &dsvDesc, mDSHeap->GetCPUDescriptorHandleForHeapStart());

	return true;
}

bool Window::ReleaseSwapChainBuffers()
{
	for (int i = 0; i < sSwapChainBufferCount; i++) 
	{
		mSwapChainBuffers[i]->Release();
		mSwapChainBuffers[i] = nullptr;
	}
	return true;
}

bool Window::ReleaseDepthStencilBuffer()
{
	mDepthStencilBuffer->Release();
	mDepthStencilBuffer = nullptr;
	return true;
}

void Window::Update() const
{
	MSG msg;
	while (PeekMessageW(&msg, mWindowHandle, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessageW(&msg);
	}
}
