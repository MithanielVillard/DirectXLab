#pragma once
class D12PipelineObject;
class Shader;

class D12ComputePipelineObject
{
public:
	D12ComputePipelineObject(Shader const& shader, D12PipelineObject& rootsig);
	D12ComputePipelineObject(D12ComputePipelineObject const& other);
	D12ComputePipelineObject(D12ComputePipelineObject&& other) = delete;

	D12ComputePipelineObject& operator= (D12ComputePipelineObject const& other);
	D12ComputePipelineObject& operator= (D12ComputePipelineObject&& other) = delete;

	~D12ComputePipelineObject();

private:
	ID3D12PipelineState* mPipelineState = nullptr;

	friend class RenderWindow;
	friend class RenderTarget;
};

