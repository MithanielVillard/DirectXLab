#pragma once
#include "StaticBuffer.h"

struct Vertex;

class Geometry
{
public:
	Geometry(const Vertex* vertex, uint vertexCount, const uint* indices, uint indicesCount);
	~Geometry() = default;

	D3D12_VERTEX_BUFFER_VIEW& GetVertexBufferView() { return mVertexBufferView; }
	D3D12_INDEX_BUFFER_VIEW& GetIndexBufferView() { return mIndexBufferView; }
	uint GetIndicesCount() const { return mIndicesCount; }

	static constexpr DXGI_FORMAT sIndexFormat{ DXGI_FORMAT_R32_UINT };

private:
	StaticBuffer mVertexBuffer;
	StaticBuffer mIndexBuffer;

	D3D12_VERTEX_BUFFER_VIEW mVertexBufferView;
	D3D12_INDEX_BUFFER_VIEW mIndexBufferView;

	uint mIndicesCount;
};

