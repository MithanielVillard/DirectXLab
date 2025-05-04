#pragma once
#include <vector>

#include "Camera.h"
#include "D12PipelineObject.h"
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
	RenderWindow(std::wstring_view const title, int const width, int const height);
	~RenderWindow() override = default;

	void BeginDraw(Camera const& camera);
	void Draw(Geometry& geo, const D12PipelineObject& pso, const Transform& transform);
	void EndDraw();
	void Display();

private:
	Camera const* mpCamera = nullptr;
};

