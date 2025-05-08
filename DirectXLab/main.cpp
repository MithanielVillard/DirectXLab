#include "pch.h"

#include "Camera.h"
#include "D12ComputePipelineObject.h"
#include "D12PipelineObject.h"
#include "Geometry.h"
#include "GeometryFactory.h"
#include "RenderContext.h"
#include "RenderTarget.h"
#include "RenderWindow.h"
#include "Shader.h"
#include "Transform.h"

int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow)
{
	RenderContext::Init();
	RenderWindow window(L"Window 1", 900, 700);

	Shader vs("VertexShader.cso");
	Shader ps("PixelShader.cso");
	Shader compute("ComputeShader.cso");
	Shader rootSig("RootSignature.cso");

	D12PipelineObject pso(vs, ps, rootSig);
	D12ComputePipelineObject computePso(compute, pso);

	Camera cam({ 0.0f, 3.5f, -10.0f }, { 0.0f, 1.0f, 0.0f }, window.GetAspectRatio());

	Geometry cubeGeo = GeometryFactory::LoadGeometry(R"(C:\Users\mitha\Documents\github\DirectXLab\Resources\barrel_large_decorated.obj)");
	Transform cubeTransform({ 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 2.0f, 2.0f, 2.0f });
	cubeTransform.UpdateTransformMatrix();

	RenderTarget target(window.GetWidth(), window.GetHeight());

	double t = 0.0;

	while (window.IsOpen())
	{
		t += 0.00016f;

		window.Update();

		cam.SetAspectRatio(window.GetAspectRatio());

		//Render Target
		target.Begin(cam);
		target.Draw(cubeGeo, pso, cubeTransform, &computePso);
		target.End();

		window.BeginDraw(cam);
		//window.Draw(cubeGeo, pso, cubeTransform, &computePso);
		window.Draw(target);
		window.EndDraw();

		RenderContext::EndFrame();
		window.Display();

		cubeTransform.Rotate(0.0f,  0.008f, 0.0f);
		cubeTransform.mPos = { 0, 0.0f, 0.0f };
		cubeTransform.UpdateTransformMatrix();

	}
}
