#pragma once
#include "Camera.h"
#include "D12PipelineObject.h"
#include "Window.h"


class Transform;
class Shader;
class Geometry;

class RenderWindow : public Window
{
public:
	RenderWindow(std::wstring_view const title, int const width, int const height) : Window(title, width, height) {}
	~RenderWindow() override = default;

	void BeginFrame(Camera const& camera);
	void Draw(Geometry& geo, const D12PipelineObject& pso, const Transform& transform);
	void EndFrame();

private:
	Camera const* mpCamera = nullptr;
};

