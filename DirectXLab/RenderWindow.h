#pragma once

#include <vector>

#include "Camera.h"
#include "D12PipelineObject.h"
#include "ReadbackBuffer.hpp"
#include "StaticBuffer.h"
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
	~RenderWindow() override;

	void BeginDraw(Camera const& camera);
	void Draw(Geometry& geo, D12PipelineObject const& pso, Transform const& transform, D12ComputePipelineObject const* computePso = nullptr);
	void EndDraw();
	void Display();

private:
	Camera const* mpCamera = nullptr;

	//R&D Buffers
	std::vector<int> mComputeData = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32};
	StaticBuffer mComputeInput;
	ID3D12Resource* mComputeOutput;
	ReadbackBuffer<int> mReadBackBuffer = ReadbackBuffer<int>(mComputeData.size() * sizeof(int));
};

