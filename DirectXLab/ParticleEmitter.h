#pragma once
#include "DynamicBuffer.h"

struct Particle
{
	DirectX::XMMATRIX transform;
 };

struct SimulationParameter
{
	DirectX::XMFLOAT3 emitterPos;
};

class ParticleEmitter
{
public:
	ParticleEmitter(uint maxParticle);
	~ParticleEmitter();

private:
	ID3D12DescriptorHeap* mDescriptorHeap;
	ID3D12Resource* mResource;
	DynamicBuffer mParametersBuffer;

	uint maxParticle;

	friend class RenderTarget;
};

