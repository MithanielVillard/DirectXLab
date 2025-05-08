#pragma once
class Geometry;

class GeometryFactory
{
public:
	static Geometry CreateCubeGeo();
	static Geometry LoadGeometry(std::string_view path);
};

