#include "pch.h"
#include "Window.h"

#include "RenderContext.h"

Window::Window(std::wstring_view title, int width, int height) : mHInstance(GetModuleHandle(nullptr)),
                                                                 mWindowHandle(nullptr), mWidth(width), mHeight(height)
{
	Create(title, width, height);
}

Window::~Window()
{
	FlushCmdQueue();

	mCommandQueue->Release();
	mCommandAllocator->Release();
	mCommandList->Release();
	mSwapChain->Release();
	mFence->Release();

	for (ID3D12Resource* buffer : mSwapChainBuffers) buffer->Release();

	mSCBuffersHeap->Release();

	DestroyWindow(mWindowHandle);
	UnregisterClassW(reinterpret_cast<LPCWSTR>(mWindowClass), mHInstance);
}

void Window::Create(std::wstring_view title, int width, int height)
{
	CreateWindowClass(title, width, height);
	CreateCmdObjects();
	CreateSwapChain();

	CreateSwapChainBuffersHeap();

	RetrieveSwapChainBuffers();

	ShowWindow(mWindowHandle, SW_SHOW);
	UpdateWindow(mWindowHandle);

	mIsOpen = true;
}

void Window::ExecuteLists()
{
	mCommandQueue->ExecuteCommandLists(static_cast<UINT>(mPendingLists.size()), mPendingLists.data());
	FlushCmdQueue();
	mPendingLists.clear();
}

void Window::FlushCmdQueue()
{
	mCommandQueue->Signal(mFence, ++mFenceValue);

	if (mFence->GetCompletedValue() < mFenceValue)
	{
		HANDLE eventHandle = CreateEventEx(nullptr, L"", false, EVENT_ALL_ACCESS);

		mFence->SetEventOnCompletion(mFenceValue, eventHandle);

		WaitForSingleObject(eventHandle, INFINITE);
		CloseHandle(eventHandle);
	}
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
	FlushCmdQueue();

	ReleaseSwapChainBuffers();

	mSwapChain->ResizeBuffers(sSwapChainBufferCount, mWidth, mHeight, sBackBufferFormat, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH);
	ResizeViewport();

	RetrieveSwapChainBuffers();
	mCurrentBackBuffer = 0;

	if (mResizeCallBack) mResizeCallBack(this);
	OnWindowResize();
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

	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	queueDesc.NodeMask = 0;
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

	HRESULT res = device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&mCommandQueue));
	if (FAILED(res))
	{
		PRINT_COM_ERROR("Failed to create command queue", res);
		return false;
	}

	res = device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&mCommandAllocator));
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

	res = device->CreateFence(mFenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&mFence));
	if (FAILED(res))
	{
		PRINT_COM_ERROR("Failed to create window fence", res);
		return false;
	}

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

	HRESULT res = RenderContext::GetDXGIFactory()->CreateSwapChain(mCommandQueue, &sd, &mSwapChain);
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

bool Window::ReleaseSwapChainBuffers()
{
	for (int i = 0; i < sSwapChainBufferCount; i++) 
	{
		mSwapChainBuffers[i]->Release();
		mSwapChainBuffers[i] = nullptr;
	}
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
