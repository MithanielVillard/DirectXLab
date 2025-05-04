#pragma once
class Shader;

class D12ComputePipelineObject
{
public:
	D12ComputePipelineObject(Shader const& shader, ID3D12RootSignature& rootsig);
	~D12ComputePipelineObject();

private:
	ID3D12PipelineState* mPso = nullptr;
};

