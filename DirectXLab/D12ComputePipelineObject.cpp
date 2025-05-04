#include "pch.h"
#include "D12ComputePipelineObject.h"

#include "RenderContext.h"
#include "Shader.h"

D12ComputePipelineObject::D12ComputePipelineObject(Shader const& shader, ID3D12RootSignature& rootsig)
{
	D3D12_COMPUTE_PIPELINE_STATE_DESC psoDesc = {};
	psoDesc.pRootSignature = &rootsig;
	psoDesc.CS = { shader.GetData(), shader.GetSize() };
	psoDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

	HRESULT res = RenderContext::GetDevice()->CreateComputePipelineState(&psoDesc, IID_PPV_ARGS(&mPso));
	if (FAILED(res))
	{
		PRINT_COM_ERROR("Error while creating compute PSO", res);
		return;
	}
}

D12ComputePipelineObject::~D12ComputePipelineObject()
{
	mPso->Release();
}
