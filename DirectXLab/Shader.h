#pragma once
class Shader
{
public:
	Shader(std::string_view path);
	Shader(Shader const& other);
	Shader(Shader&& other) noexcept;

	~Shader();

	ulong GetSize() const { return mSize; }
	void* GetData() const { return mData; }

private:
	ulong mSize = 0;
	void* mData = nullptr;
};

