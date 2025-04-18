#pragma once
class Shader;

class D12PipelineObject
{
public:
	D12PipelineObject(Shader const& VS, Shader const& PS, Shader const& rootsig);
	~D12PipelineObject();

private:
	ID3D12PipelineState* mPipelineState;
	ID3D12RootSignature* mRootSignature;

	D3D12_INPUT_ELEMENT_DESC inputLayout[3]
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "UV", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 28, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};

	friend class RenderWindow;
};

