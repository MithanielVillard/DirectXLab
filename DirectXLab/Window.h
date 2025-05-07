#pragma once
class Window
{
public:
	Window(std::wstring_view title, int width, int height);
	virtual ~Window();

	void Update() const;

	bool IsOpen() const { return mIsOpen; }
	int GetWidth() const { return mWidth; }
	int GetHeight() const { return mHeight; }
	float GetAspectRatio() const { return static_cast<float>(mWidth) / static_cast<float>(mHeight); }

	static constexpr DXGI_FORMAT sBackBufferFormat{ DXGI_FORMAT_R8G8B8A8_UNORM };
	static constexpr DXGI_FORMAT sDepthStencilFormat{ DXGI_FORMAT_D24_UNORM_S8_UINT };

	inline static const DirectX::XMVECTORF32 sClearColor{ DirectX::Colors::LightBlue };

protected:
	ID3D12GraphicsCommandList* mCommandList;
	ID3D12CommandAllocator* mCommandAllocator;

	static constexpr int sSwapChainBufferCount { 2 };
	int mCurrentBackBuffer = 0;
	IDXGISwapChain* mSwapChain;

	D3D12_CPU_DESCRIPTOR_HANDLE mRtvHandles[sSwapChainBufferCount];

	ID3D12Resource* mSwapChainBuffers[sSwapChainBufferCount];
	ID3D12Resource* mDepthStencilBuffer;

	ID3D12DescriptorHeap* mSCBuffersHeap;
	ID3D12DescriptorHeap* mDSHeap;

	D3D12_VIEWPORT mViewPort;
	D3D12_RECT mScissorRect;

	ATOM mWindowClass;
	HINSTANCE mHInstance;
	HWND mWindowHandle;

	bool mIsOpen;
	int mWidth;
	int mHeight;

protected:
	ID3D12Resource* GetCurrentBackBuffer() const { return mSwapChainBuffers[mCurrentBackBuffer]; }

private:
	static LRESULT CALLBACK MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK SubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);


	LRESULT HandleEvent(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	void ResizeViewport();
	void ResizeWindow(int newWidth, int newHeight);

	bool CreateWindowClass(std::wstring_view title, int width, int height);
	bool CreateCmdObjects();
	bool CreateSwapChain();

	bool CreateSwapChainBuffersHeap();
	bool CreateDepthStencilHeap();

	bool RetrieveSwapChainBuffers();
	bool RetrieveDepthStencilBuffer();

	bool ReleaseSwapChainBuffers();
	bool ReleaseDepthStencilBuffer();
};

