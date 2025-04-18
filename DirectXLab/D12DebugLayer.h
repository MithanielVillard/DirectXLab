#pragma once
class D12DebugLayer
{
public:
	D12DebugLayer();
	~D12DebugLayer();

private:
	ID3D12Debug* mDebugLayer;
	IDXGIDebug1* mDxgiDebugLayer;
};

