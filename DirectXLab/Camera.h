#pragma once
#include "DynamicBuffer.h"

using namespace DirectX;

struct PassData
{
	XMMATRIX viewProj;
};

enum CameraType : char
{
	PERSPECTIVE, ORTHOGRAPHIC
};

class Camera
{
public:
	Camera(XMFLOAT3 const& pos, XMFLOAT3 const& up, float aspectRatio, CameraType type = PERSPECTIVE);
	~Camera() = default;

	void SetAspectRatio(float aspectRatio) { mAspectRatio = aspectRatio; UpdateVPMatrix(); }

private:
	void UpdateVPMatrix();

private:
	DynamicBuffer mBuffer = DynamicBuffer(sizeof(PassData));

	CameraType mType;

	float mAspectRatio;
	XMFLOAT3 mPos;
	XMFLOAT3 mForward = { 0.0f, 0.0f, 1.0f };
	XMFLOAT3 mUp;
	XMFLOAT4 mRotation;

	friend class RenderWindow;
	friend class RenderTarget;
};

