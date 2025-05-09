#pragma once
class StaticBuffer
{
public:
	StaticBuffer() = default;
	~StaticBuffer();

	void Init(void const* initData, ulong initDataSize);
	void SetName(std::wstring_view name) const { mDefaultBuffer->SetName(name.data()); }

	D3D12_GPU_VIRTUAL_ADDRESS GetGPUAddress() const { return mDefaultBuffer->GetGPUVirtualAddress(); }
private:
	ID3D12Resource* mDefaultBuffer;
	ID3D12Resource* mUploadBuffer;

	D3D12_SUBRESOURCE_DATA mSubResourceData {};

	friend class RenderContext;
	friend class ParticleEmitter;
};

