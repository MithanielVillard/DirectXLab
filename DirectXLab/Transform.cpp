#include "pch.h"
#include "Transform.h"

Transform::Transform(XMFLOAT3 pos, XMFLOAT3 rot, XMFLOAT3 scale)
{
	mPos = pos;
	XMStoreFloat4(&mQuaternionRot ,XMQuaternionRotationRollPitchYaw(rot.x, rot.y, rot.z));
	mScale = scale;

	mBuffer.SetName(L"Transform constant buffer");
}

XMFLOAT3& Transform::GetPosition()
{
	return mPos;
}

void Transform::SetPosition(XMFLOAT3 const& pos)
{
	mPos = pos;
}

void Transform::UpdateTransformMatrix()
{
	XMMATRIX world =
		XMMatrixScaling(mScale.x, mScale.y, mScale.z) *
		XMMatrixRotationQuaternion(XMLoadFloat4(&mQuaternionRot)) *
		XMMatrixTranslation(mPos.x, mPos.y, mPos.z);

	ObjectData data = { XMMatrixTranspose(world) };
	mBuffer.CopyData(data);
}

