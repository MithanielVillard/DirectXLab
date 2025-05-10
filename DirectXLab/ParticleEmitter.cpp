#include "pch.h"
#include "ParticleEmitter.h"

#include "RenderContext.h"
#include "Window.h"

ParticleEmitter::ParticleEmitter(uint maxParticle) : maxParticle(maxParticle)
{
	CD3DX12_HEAP_PROPERTIES heapProp(D3D12_HEAP_TYPE_DEFAULT);
	CD3DX12_RESOURCE_DESC resDesc = CD3DX12_RESOURCE_DESC::Buffer(sizeof(Particle) * maxParticle, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);
	RenderContext::GetDevice()->CreateCommittedResource(&heapProp, D3D12_HEAP_FLAG_NONE, &resDesc, D3D12_RESOURCE_STATE_COMMON, nullptr, IID_PPV_ARGS(&mResource)); 

	mParametersBuffer.Init(sizeof(SimulationParameter));

	D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	heapDesc.NodeMask = 0;
	heapDesc.NumDescriptors = 3;
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;

	HRESULT res = RenderContext::GetDevice()->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&mDescriptorHeap));
	if (FAILED(res))
	{
		PRINT_COM_ERROR("Failed to create descriptor heap for particle emitter", res);
		return;
	}

	CD3DX12_CPU_DESCRIPTOR_HANDLE firstHandle(mDescriptorHeap->GetCPUDescriptorHandleForHeapStart());

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
	srvDesc.Buffer.NumElements = maxParticle;
	srvDesc.Buffer.StructureByteStride = sizeof(Particle);

	RenderContext::GetDevice()->CreateShaderResourceView(mResource, &srvDesc, firstHandle);
	firstHandle.Offset(1, RenderContext::sCbvSrvUavDescriptorSize);

	D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
	uavDesc.Format = DXGI_FORMAT_UNKNOWN;
	uavDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
	uavDesc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_NONE;
	uavDesc.Buffer.NumElements = maxParticle;
	uavDesc.Buffer.StructureByteStride = sizeof(Particle);

	RenderContext::GetDevice()->CreateUnorderedAccessView(mResource, nullptr, &uavDesc, firstHandle);
	firstHandle.Offset(1, RenderContext::sCbvSrvUavDescriptorSize);

	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
	cbvDesc.BufferLocation = mParametersBuffer.GetGPUAddress();
	cbvDesc.SizeInBytes = CalcConstantBufferByteSize(sizeof(SimulationParameter));

	RenderContext::GetDevice()->CreateConstantBufferView(&cbvDesc, firstHandle);
}

ParticleEmitter::~ParticleEmitter()
{
	mResource->Release();
	mDescriptorHeap->Release();
}
