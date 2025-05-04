#pragma once
#include "DynamicBuffer.h"

using namespace DirectX;

struct ObjectData
{
	XMMATRIX world;
};

class Transform
{
public:
	Transform() = default;
	Transform(XMFLOAT3 pos, XMFLOAT3 rot, XMFLOAT3 scale);
	~Transform() = default;

	XMFLOAT3& GetPosition();

	void SetPosition(XMFLOAT3 const& pos);

	void UpdateTransformMatrix();

	XMFLOAT3 mPos;
	XMFLOAT4 mQuaternionRot;
	XMFLOAT3 mScale;
private:

	DynamicBuffer mBuffer = DynamicBuffer(sizeof(ObjectData));

	friend class RenderWindow;
};

