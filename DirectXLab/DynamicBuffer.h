#pragma once
class DynamicBuffer
{
public:
	DynamicBuffer() = default;
	DynamicBuffer(uint size);
	~DynamicBuffer();

	void Init(uint size);

	template <typename T>
	void CopyData(T& data);

	void SetName(std::wstring_view name) const { mBuffer->SetName(name.data()); }

	D3D12_GPU_VIRTUAL_ADDRESS GetGPUAddress() const { return mBuffer->GetGPUVirtualAddress(); }

private:
	ID3D12Resource* mBuffer;
	void* mCPUPointer;
};

template <typename T>
void DynamicBuffer::CopyData(T& data)
{
	memcpy(mCPUPointer, &data, sizeof(data));
}

