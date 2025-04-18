#include "pch.h"
#include "Shader.h"

#include <fstream>

Shader::Shader(std::string_view const path)
{
	std::ifstream vsFile((path.data()), std::ios::binary);
	if (vsFile.is_open())
	{
		vsFile.seekg(0, std::ios::end);
		mSize = vsFile.tellg();
		vsFile.seekg(0, std::ios::beg);
		mData = malloc(mSize);

		if (mData) vsFile.read(static_cast<char*>(mData), mSize);
	}
}

Shader::~Shader()
{
	free(mData);
}
