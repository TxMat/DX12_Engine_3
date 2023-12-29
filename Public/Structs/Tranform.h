#pragma once
#include <DirectXMath.h>

using namespace DirectX;

struct Transform
{
    XMFLOAT3 vSca;
    XMFLOAT4X4 mSca;

    XMFLOAT3 vDir;
    XMFLOAT3 vRight;
    XMFLOAT3 vUp;

    XMFLOAT4 qRot;
    XMFLOAT4X4 mRot;

    XMFLOAT3 vPos;
    XMFLOAT4X4 mPos;

    XMFLOAT4X4 matrix;

    bool anyChange = false;

    Transform()
    {
        Identity();
    }

    void Identity()
    {
        XMStoreFloat3(&vSca, XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f));
        XMStoreFloat4x4(&mSca, XMMatrixIdentity());

        XMStoreFloat3(&vDir, XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f));
        XMStoreFloat3(&vRight, XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f));
        XMStoreFloat3(&vUp, XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));

        XMStoreFloat4(&qRot, XMQuaternionIdentity());
        XMStoreFloat4x4(&mRot, XMMatrixIdentity());

        XMStoreFloat3(&vPos, XMVectorZero());
        XMStoreFloat4x4(&mPos, XMMatrixIdentity());

        XMStoreFloat4x4(&matrix, XMMatrixIdentity());

        anyChange = true;
    }

    void Transform::TranslateLocal(float x, float y, float z)
    {
        XMVECTOR displacement = XMLoadFloat3(&vRight) * x +
                                XMLoadFloat3(&vUp) * y +
                                XMLoadFloat3(&vDir) * z;
        XMStoreFloat3(&vPos, XMLoadFloat3(&vPos) + displacement);
        XMStoreFloat4x4(&mPos, XMMatrixTranslationFromVector(XMLoadFloat3(&vPos)));

        anyChange = true;
    }

    void Transform::TranslateWorld(float x, float y, float z)
    {
        XMStoreFloat3(&vPos, XMLoadFloat3(&vPos) + XMVectorSet(x, y, z, 0.0f));
        XMStoreFloat4x4(&mPos, XMMatrixTranslationFromVector(XMLoadFloat3(&vPos)));

        anyChange = true;
    }

    void Rotate(float yaw, float pitch, float roll)
    {
        XMVECTOR quatRot = XMQuaternionIdentity();
        quatRot = XMQuaternionMultiply(quatRot, XMQuaternionRotationAxis(XMLoadFloat3(&vDir), roll));
        quatRot = XMQuaternionMultiply(quatRot, XMQuaternionRotationAxis(XMLoadFloat3(&vRight), pitch));
        quatRot = XMQuaternionMultiply(quatRot, XMQuaternionRotationAxis(XMLoadFloat3(&vUp), yaw));

        XMStoreFloat4(&qRot, XMQuaternionMultiply(XMLoadFloat4(&qRot), quatRot));

        XMStoreFloat4x4(&mRot, XMMatrixRotationQuaternion(XMLoadFloat4(&qRot)));

        vRight.x = mRot._11;
        vRight.y = mRot._12;
        vRight.z = mRot._13;
        vUp.x = mRot._21;
        vUp.y = mRot._22;
        vUp.z = mRot._23;
        vDir.x = mRot._31;
        vDir.y = mRot._32;
        vDir.z = mRot._33;

        anyChange = true;
    }

    void ApplyChanges()
    {
        if (anyChange)
        {
            XMStoreFloat4x4(&matrix, XMLoadFloat4x4(&mSca) * XMLoadFloat4x4(&mRot) * XMLoadFloat4x4(&mPos));
            anyChange = false;
        }
    }
};
