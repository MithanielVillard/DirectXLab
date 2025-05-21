#include "pch.h"
#include "Shader.h"

#include <filesystem>
#include <fstream>

Shader::Shader(std::string_view const path)
{
	static std::filesystem::path shaderDir;

	wchar_t moduleFileName[MAX_PATH];
	GetModuleFileNameW(nullptr, moduleFileName, MAX_PATH);
	shaderDir = moduleFileName;
	shaderDir.remove_filename();

	std::ifstream vsFile((shaderDir / path.data()), std::ios::binary);
	if (vsFile.is_open())
	{
		vsFile.seekg(0, std::ios::end);
		mSize = vsFile.tellg();
		vsFile.seekg(0, std::ios::beg);
		mData = malloc(mSize);

		if (mData) vsFile.read(static_cast<char*>(mData), mSize);
	}
}

Shader::Shader(Shader const& other)
{
	mSize = other.mSize;
	mData = malloc(other.mSize);
	memcpy(mData, other.mData, other.mSize);
}

Shader::Shader(Shader&& other) noexcept
{
	mSize = other.mSize;
	mData = other.mData;
	other.mData = nullptr;
}

Shader::~Shader()
{
	free(mData);
}
