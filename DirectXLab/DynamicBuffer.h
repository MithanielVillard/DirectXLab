#pragma once
class DynamicBuffer
{
public:
	DynamicBuffer(uint size);
	~DynamicBuffer();

	template <typename T>
	void CopyData(T& data);
	void SetName(std::wstring_view name) { mBuffer->SetName(name.data()); }

private:
	ID3D12Resource* mBuffer;
	void* mCPUPointer;
};

template <typename T>
void DynamicBuffer::CopyData(T& data)
{
	memcpy(mCPUPointer, &data, sizeof(data));
}

