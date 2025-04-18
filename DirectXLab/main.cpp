#include "pch.h"

#include "D12PipelineObject.h"
#include "Geometry.h"
#include "GeometryFactory.h"
#include "RenderContext.h"
#include "RenderWindow.h"
#include "Shader.h"

int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow)
{
	RenderContext::Init();
	RenderWindow window(L"Hello engine", 900, 500);

	Shader vs("VertexShader.cso");
	Shader ps("PixelShader.cso");
	Shader rootSig("RootSignature.cso");
	D12PipelineObject pso(vs, ps, rootSig);

	Geometry cubeGeo = GeometryFactory::CreateCubeGeo();

	while (window.IsOpen())
	{
		window.Update();

		window.BeginFrame();
		window.EndFrame();
	}
}
