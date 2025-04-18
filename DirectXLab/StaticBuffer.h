#pragma once
class StaticBuffer
{
public:
	StaticBuffer(void const* initData, ulong initDataSize);
	~StaticBuffer();

private:
	ID3D12Resource* mDefaultBuffer;
	ID3D12Resource* mUploadBuffer;

	D3D12_SUBRESOURCE_DATA mSubResourceData {};
	friend class RenderContext;
};

