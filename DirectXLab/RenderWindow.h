#pragma once

#include "Camera.h"
#include "D12PipelineObject.h"
#include "RenderTarget.h"
#include "Window.h"

class Transform;
class Shader;
class Geometry;

struct DrawInfo
{
	Geometry* geo;
	D12PipelineObject const* pso;
	Transform const* transform;
};

class RenderWindow : public Window
{
public:
	RenderWindow(std::wstring_view title, int width, int height);
	RenderWindow(RenderWindow const& other) = delete;
	RenderWindow(RenderWindow&& other) noexcept = delete;

	auto operator=(RenderWindow const& other) = delete;
	auto operator=(RenderWindow&& other) = delete;

	~RenderWindow() override = default;

	void BeginDraw(Camera const& camera);
	void Draw(Geometry& geo, D12PipelineObject const& pso, Transform const& transform);
	void EndDraw();

private:
	RenderTarget mTarget = RenderTarget(mWidth, mHeight);

private:
	void OnWindowResize() override;
};

