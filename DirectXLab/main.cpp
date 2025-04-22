#include "pch.h"

#include "Camera.h"
#include "D12PipelineObject.h"
#include "Geometry.h"
#include "GeometryFactory.h"
#include "RenderContext.h"
#include "RenderWindow.h"
#include "Shader.h"
#include "Transform.h"

int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow)
{
	RenderContext::Init();
	RenderWindow window(L"Window 1", 900, 500);
	RenderWindow window2(L"Window 2", 400, 700);

	Shader vs("VertexShader.cso");
	Shader ps("PixelShader.cso");
	Shader rootSig("RootSignature.cso");
	D12PipelineObject pso(vs, ps, rootSig);

	Camera cam({ 0.0f, 1.0f, -10.0f }, { 0.0f, 1.0f, 0.0f }, window.GetAspectRatio());
	Camera cam2({ 0.0f, 1.0f, -10.0f }, { 0.0f, 1.0f, 0.0f }, window.GetAspectRatio());

	Geometry cubeGeo = GeometryFactory::CreateCubeGeo();
	Transform cubeTransform({ 0.0f, 0.0f, 0.0f }, { 0.0f, 45.0f, 0.0f }, { 1.0f, 1.0f, 1.0f });
	cubeTransform.UpdateTransformMatrix();

	while (window.IsOpen())
	{
		window.Update();
		window2.Update();

		cam.SetAspectRatio(window.GetAspectRatio());
		cam2.SetAspectRatio(window2.GetAspectRatio());

		window.BeginFrame(cam);
		window.Draw(cubeGeo, pso, cubeTransform);
		window.EndFrame();

		window2.BeginFrame(cam2);
		window2.Draw(cubeGeo, pso, cubeTransform);
		window2.EndFrame();

		RenderContext::FlushCommandQueue();
	}
}
