#pragma once
#include "StaticBuffer.h"

struct Particle
{
	DirectX::XMMATRIX transform;
 };

class ParticleEmitter
{
public:
	ParticleEmitter(uint maxParticle);
	~ParticleEmitter();

private:
	ID3D12DescriptorHeap* mDescriptorHeap;
	ID3D12Resource* mResource;

	uint maxParticle;

	friend class RenderTarget;
};

