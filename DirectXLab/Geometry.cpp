#include "pch.h"
#include "Geometry.h"

#include "Vertex.h"

Geometry::Geometry(const Vertex* vertex, uint vertexCount, const uint* indices, uint indicesCount) : mIndicesCount(indicesCount)
{
	const uint vbByteSize = vertexCount * sizeof(Vertex);
	const uint ibByteSize = indicesCount * sizeof(uint);

	mVertexBuffer.Init(vertex, vbByteSize);
	mVertexBuffer.SetName(L"Vertex Buffer");

	mIndexBuffer.Init(indices, ibByteSize);
	mIndexBuffer.SetName(L"Index Buffer");

	mVertexBufferView.BufferLocation = mVertexBuffer.GetGPUAddress();
	mVertexBufferView.StrideInBytes = sizeof(Vertex);
	mVertexBufferView.SizeInBytes = vbByteSize;

	mIndexBufferView.BufferLocation = mIndexBuffer.GetGPUAddress();
	mIndexBufferView.Format = sIndexFormat;
	mIndexBufferView.SizeInBytes = ibByteSize;
}
