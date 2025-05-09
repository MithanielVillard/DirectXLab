#include "pch.h"

#include "Camera.h"
#include "Chrono.h"
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
	RenderContext::SetMSAA(4);

	RenderWindow window(L"Window 1", 900, 700);
	RenderTarget target(window.GetWidth(), window.GetHeight());

	Shader vs("VertexShader.cso");
	Shader ps("PixelShader.cso");
	Shader compute("ComputeShader.cso");
	Shader rootSig("RootSignature.cso");

	D12PipelineObject pso(vs, ps, rootSig);
	D12ComputePipelineObject computePso(compute, pso);

	Camera cam({ 0.0f, 1.0f, -10.0f }, { 0.0f, 1.0f, 0.0f }, window.GetAspectRatio());

	//Geometry cubeGeo = GeometryFactory::LoadGeometry(R"(C:\Users\mitha\Documents\github\DirectXLab\Resources\chest_gold.obj)");
	Geometry cubeGeo = GeometryFactory::CreateCubeGeo();

	Transform cubeTransform({ 0.0f, 0.0f, 0.0f }, { 0.0f, 45.0f, 0.0f }, { 1.0f, 1.0f, 1.0f });
	cubeTransform.UpdateTransformMatrix();
	Transform cubeTransform2({ 5.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f });
	cubeTransform2.UpdateTransformMatrix();

	window.OnWindowResize([&](Window* w)->void
	{
			target.Resize(w->GetWidth(), w->GetHeight());
			cam.SetAspectRatio(w->GetAspectRatio());
	});


	// =========
	StaticBuffer transformsBuffer;
	XMMATRIX transforms[32] = {
		//cubeTransform.mMatrix,
		//cubeTransform2.mMatrix
	};
	transformsBuffer.Init(transforms, sizeof(XMMATRIX) * _countof(transforms));
	transformsBuffer.SetName(L"Transform buffer");
	// ========


	Chrono chrono;
	float time = 0.0f;
	while (window.IsOpen())
	{
		time += chrono.Reset();
		RenderContext::sDeltaTime = time;

		window.Update();

		//Render Target
		target.Begin(cam);
		target.Draw(cubeGeo, pso, cubeTransform, transformsBuffer.GetGPUAddress(), &computePso);
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
