#include "pch.h"
#include "StaticBuffer.h"

#include "RenderContext.h"

void StaticBuffer::Init(void const* initData, ulong initDataSize)
{
    CD3DX12_HEAP_PROPERTIES heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
    CD3DX12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(initDataSize);

    RenderContext::GetDevice()->CreateCommittedResource(
        &heapProps,
        D3D12_HEAP_FLAG_NONE,
        &resourceDesc,
        D3D12_RESOURCE_STATE_COMMON,
        nullptr,
        IID_PPV_ARGS(&mDefaultBuffer));

    heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
    resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(initDataSize);
    RenderContext::GetDevice()->CreateCommittedResource(
        &heapProps,
        D3D12_HEAP_FLAG_NONE,
        &resourceDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&mUploadBuffer));

    mSubResourceData.pData = initData;
    mSubResourceData.RowPitch = initDataSize;
    mSubResourceData.SlicePitch = mSubResourceData.RowPitch;

    RenderContext::UploadBuffer(*this);
}

StaticBuffer::~StaticBuffer()
{
	mDefaultBuffer->Release();
    if (mUploadBuffer) mUploadBuffer->Release();
}
