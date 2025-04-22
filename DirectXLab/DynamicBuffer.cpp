#include "pch.h"
#include "DynamicBuffer.h"

#include "RenderContext.h"

DynamicBuffer::DynamicBuffer(uint size)
{
	CD3DX12_HEAP_PROPERTIES heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	CD3DX12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(CalcConstantBufferByteSize(size));

	RenderContext::GetDevice()->CreateCommittedResource(&heapProp, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, 
		nullptr, IID_PPV_ARGS(&mBuffer));

	mBuffer->Map(0, nullptr, &mCPUPointer);
}

DynamicBuffer::~DynamicBuffer()
{
	mBuffer->Unmap(0, nullptr);
	mBuffer->Release();
}
