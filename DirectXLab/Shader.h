#pragma once
class Shader
{
public:
	Shader(std::string_view path);
	~Shader();

	ulong GetSize() const { return mSize; }
	void* GetData() const { return mData; }

private:
	ulong mSize;
	void* mData;
};

