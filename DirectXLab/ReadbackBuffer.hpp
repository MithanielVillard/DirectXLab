#pragma once
#include "RenderContext.h"

template <typename T>
class ReadbackBuffer
{
public:
	ReadbackBuffer(uint size);
	~ReadbackBuffer();

	T* GetData() { return mpData; }
	ID3D12Resource* GetResource() const { return mBuffer; }

private:
	ID3D12Resource* mBuffer;
	T* mpData;
};

template <typename T>
ReadbackBuffer<T>::ReadbackBuffer(uint size)
{
	CD3DX12_HEAP_PROPERTIES prop = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_READBACK);
	CD3DX12_RESOURCE_DESC resource = CD3DX12_RESOURCE_DESC::Buffer(size);

	RenderContext::GetDevice()->CreateCommittedResource(&prop, 
		D3D12_HEAP_FLAG_NONE, 
		&resource, 
		D3D12_RESOURCE_STATE_COPY_DEST, 
		nullptr, 
		IID_PPV_ARGS(&mBuffer));

	mBuffer->Map(0, nullptr, reinterpret_cast<void**>(&mpData));
}

template <typename T>
ReadbackBuffer<T>::~ReadbackBuffer()
{
	mBuffer->Unmap(0, nullptr);
	mBuffer->Release();
}

