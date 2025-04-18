#include "pch.h"
#include "D12PipelineObject.h"

#include "RenderContext.h"
#include "Shader.h"
#include "Window.h"

D12PipelineObject::D12PipelineObject(Shader const& VS, Shader const& PS, Shader const& rootsig)
{
	HRESULT res = RenderContext::GetDevice()->CreateRootSignature(0, rootsig.GetData(), rootsig.GetSize(), IID_PPV_ARGS(&mRootSignature));
	if (FAILED(res))
	{
		PRINT_COM_ERROR("Failed to create root signature", res);
		return;
	}

	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
	psoDesc.pRootSignature = mRootSignature;
	psoDesc.InputLayout = { inputLayout, _countof(inputLayout) };
	psoDesc.VS = { VS.GetData(), VS.GetSize() };
	psoDesc.PS = { PS.GetData(), PS.GetSize() };
	psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	psoDesc.SampleMask = UINT_MAX;
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.NumRenderTargets = 1;
	psoDesc.SampleDesc.Count = 1;
	psoDesc.SampleDesc.Quality = 0;
	psoDesc.RTVFormats[0] = Window::sBackBufferFormat;
	psoDesc.DSVFormat = Window::sDepthStencilFormat;

	res = RenderContext::GetDevice()->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&mPipelineState));
	if (FAILED(res))
	{
		PRINT_COM_ERROR("Failed to create pipeline state object", res);
		return;
	}
}

D12PipelineObject::~D12PipelineObject()
{
	mRootSignature->Release();
	mPipelineState->Release();
}
