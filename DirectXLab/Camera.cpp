#include "pch.h"
#include "Camera.h"

Camera::Camera(XMFLOAT3 const& pos, XMFLOAT3 const& up, float const aspectRatio, CameraType type) : mType(type), mAspectRatio(aspectRatio), mPos(pos), mUp(up)
{
	UpdateVPMatrix();
}

void Camera::UpdateVPMatrix()
{
	XMMATRIX view = XMMatrixLookToLH(XMLoadFloat3(&mPos), XMLoadFloat3(&mForward), XMLoadFloat3(&mUp));
	XMMATRIX proj = XMMatrixPerspectiveFovLH(XM_PIDIV4, mAspectRatio, 0.1f, 500.0f);

	PassData pd = { XMMatrixTranspose(view * proj) };
	mBuffer.CopyData(pd);
}
