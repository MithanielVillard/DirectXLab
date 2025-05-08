#include "pch.h"
#include "GeometryFactory.h"

#include "Geometry.h"
#include "OBJ_Loader.h"
#include "Vertex.h"

Geometry GeometryFactory::CreateCubeGeo()
{
	Vertex v[24]{
		// FRONT
		{{ -.5f, -.5f, -.5f }, {  0, 0, 0, 1 }, {0.0f, 1.0f}},	// 0
		{{ -.5f,  .5f, -.5f }, {  0, 1, 0, 1 }, {0.0f, 0.0f}},	// 1
		{{  .5f,  .5f, -.5f }, {  1, 1, 0, 1 }, {1.0f, 0.0f}},	// 2
		{{  .5f, -.5f, -.5f }, {  1, 0, 0, 1 }, {1.0f, 1.0f}},	// 3

		// BACK
		{{ -.5f, -.5f,  .5f }, {  0, 0, 1, 1 }, {1.0f, 1.0f}},	// 4
		{{  .5f, -.5f,  .5f }, {  1, 0, 1, 1 }, {0.0f, 1.0f}},	// 5
		{{  .5f,  .5f,  .5f }, {  1, 1, 1, 1 }, {0.0f, 0.0f}},	// 6
		{{ -.5f,  .5f,  .5f }, {  0, 1, 1, 1 }, {1.0f, 0.0f}},	// 7 

		// LEFT
		 {{ -.5f, -.5f,  .5f }, { 0, 0, 1, 1 }, {1.0f, 1.0f}},	// 8
		 {{ -.5f,  .5f,  .5f }, { 0, 1, 1, 1 }, {0.0f, 1.0f}},	// 9
		 {{ -.5f,  .5f, -.5f }, { 0, 1, 0, 1 }, {0.0f, 0.0f}},	// 10
		 {{ -.5f, -.5f, -.5f }, { 0, 0, 0, 1 }, {1.0f, 0.0f}},	// 11

		 // RIGHT
		 {{  .5f, -.5f, -.5f }, {  1, 0, 0, 1 }, {0.0f, 1.0f}},	// 12
		 {{  .5f,  .5f, -.5f }, {  1, 1, 0, 1 }, {0.0f, 0.0f}},	// 13
		 {{  .5f,  .5f,  .5f }, {  1, 1, 1, 1 }, {1.0f, 0.0f}},	// 14
		 {{  .5f, -.5f,  .5f }, {  1, 0, 1, 1 }, {1.0f, 1.0f}},	// 15

		 // TOP
		 {{ -.5f,  .5f, -.5f }, {  0, 1, 0, 1 }, {0.0f, 1.0f}},	// 16
		 {{ -.5f,  .5f,  .5f }, {  0, 1, 1, 1 }, {0.0f, 0.0f}},	// 17
		 {{  .5f,  .5f,  .5f }, {  1, 1, 1, 1 }, {1.0f, 0.0f}},	// 18
		 {{  .5f,  .5f, -.5f }, {  1, 1, 0, 1 }, {1.0f, 1.0f}},	// 19

		 // BOTTOM											
		 {{ -.5f, -.5f,  .5f }, {  0, 0, 1, 1 }, {0.0f, 1.0f}},	// 20
		 {{ -.5f, -.5f, -.5f }, {  0, 0, 0, 1 }, {0.0f, 0.0f}},	// 21
		 {{  .5f, -.5f, -.5f }, {  1, 0, 0, 1 }, {1.0f, 0.0f}},	// 22
		 {{  .5f, -.5f,  .5f }, {  1, 0, 1, 1 }, {1.0f, 1.0f}},	// 23
	};

	uint i[36]{
		// FRONT
		0, 1, 2,
		0, 2, 3,

		// BACK
		4, 5, 6,
		4, 6, 7,

		// LEFT
		8, 9, 10,
		8, 10, 11,

		// RIGHT
		12, 13, 14,
		12, 14, 15,

		// TOP 
		16, 17, 18,
		16, 18, 19,

		// BOTTOM
		20, 21, 22,
		20, 22, 23
	};

	return Geometry(v, _countof(v), i, _countof(i));
}

Geometry GeometryFactory::LoadGeometry(std::string_view const path)
{
	objl::Loader loader;
	bool loaded = loader.LoadFile(path.data());
	if (!loaded) throw std::runtime_error("Failed to load model : " + std::string(path.data()));

	std::vector<Vertex> vertices;
	std::vector<uint> indices;

	for (objl::Mesh mesh : loader.LoadedMeshes)
	{
		for (objl::Vertex v : mesh.Vertices)
		{
			vertices.emplace_back(Vertex({v.Position.X, v.Position.Y, v.Position.Z }, { 1.0f, 0.0f, 0.0f, 1.0f }, { v.TextureCoordinate.X, v.TextureCoordinate.Y }));
		}

		for (uint index : mesh.Indices)
		{
			indices.push_back(index);
		}
	}


	return { vertices.data(), static_cast<uint>(vertices.size()), indices.data(), static_cast<uint>(indices.size()) };

}
