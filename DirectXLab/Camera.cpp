#include "pch.h"
#include "Camera.h"

Camera::Camera(XMFLOAT3 const& pos, XMFLOAT3 const& up, float const aspectRatio, CameraType type) : mType(type), mPos(pos), mUp(up)
{
	SetAspectRatio(aspectRatio);
}


void Camera::SetAspectRatio(float aspectRatio)
{
	//Calculate projection matrix only where the aspect ratio change
	switch (mType)
	{
	case PERSPECTIVE:
		XMStoreFloat4x4(&mProjMatrix, XMMatrixPerspectiveFovLH(XM_PIDIV4, aspectRatio, 0.1f, 500.0f));
		break;

	case ORTHOGRAPHIC:
		XMStoreFloat4x4(&mProjMatrix, XMMatrixOrthographicLH(9.0f, 7.0f, 0.1f, 500.0f));
		break;

	}
	UpdateVPMatrix();
}

void Camera::UpdateVPMatrix()
{
	XMMATRIX view = XMMatrixLookToLH(XMLoadFloat3(&mPos), XMLoadFloat3(&mForward), XMLoadFloat3(&mUp));

	PassData pd = { XMMatrixTranspose(view * XMLoadFloat4x4(&mProjMatrix)) };
	mBuffer.CopyData(pd);
}
