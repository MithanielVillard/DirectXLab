#pragma once
class D12DebugLayer
{
public:
	D12DebugLayer();
	D12DebugLayer(D12DebugLayer const& other) = delete;
	D12DebugLayer(D12DebugLayer&& other) noexcept = delete;

	auto operator=(D12DebugLayer const& other) = delete;
	auto operator=(D12DebugLayer&& other) = delete;

	~D12DebugLayer();

private:
	ID3D12Debug* mDebugLayer = nullptr;
	IDXGIDebug1* mDxgiDebugLayer = nullptr;
};

