#include "Transform.h"
#include "Shader.h"

#include "Navigation.h"

CTransform::CTransform(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CComponent { pDevice, pContext }
{
}

CTransform::CTransform(const CTransform& Prototype)
    : CComponent{ Prototype }
{
}

HRESULT CTransform::Initialize_Prototype()
{
	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());

    return S_OK;
}

HRESULT CTransform::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return S_OK;

	TRANSFORM_DESC* pDesc = static_cast<TRANSFORM_DESC*>(pArg);

	m_fSpeedPerSec = pDesc->fSpeedPerSec;
	m_fRadianPerSec = XMConvertToRadians(pDesc->fDegreePerSec);

    return S_OK;
}

void CTransform::Set_Rotation(_float3 rotation)
{
	m_fRotation = rotation;
	
	_float3 scale = Get_Scaled();

	_vector vRight = XMVectorSet(1.f, 0.f, 0.f, 0.f) * scale.x;
	_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f) * scale.y;
	_vector vLook = XMVectorSet(0.f, 0.f, 1.f, 0.f) * scale.z;

	_matrix rotationMatrix = XMMatrixRotationRollPitchYaw(XMConvertToRadians(m_fRotation.x), XMConvertToRadians(m_fRotation.y), XMConvertToRadians(m_fRotation.z));


	Set_State(STATE::RIGHT, XMVector3TransformNormal(vRight, rotationMatrix));
	Set_State(STATE::UP, XMVector3TransformNormal(vUp, rotationMatrix));
	Set_State(STATE::LOOK, XMVector3TransformNormal(vLook, rotationMatrix));

}

void CTransform::SetUp_Scale(_float fScaleX, _float fScaleY, _float fScaleZ)
{
	Set_State(STATE::RIGHT, XMVector3Normalize(Get_State(STATE::RIGHT)) * fScaleX);
	Set_State(STATE::UP, XMVector3Normalize(Get_State(STATE::UP))		* fScaleY);
	Set_State(STATE::LOOK, XMVector3Normalize(Get_State(STATE::LOOK)) * fScaleZ);
}

void CTransform::Scaling(_float fScaleX, _float fScaleY, _float fScaleZ)
{
	Set_State(STATE::RIGHT, Get_State(STATE::RIGHT) * fScaleX);
	Set_State(STATE::UP, Get_State(STATE::UP) * fScaleY);
	Set_State(STATE::LOOK, Get_State(STATE::LOOK) * fScaleZ);
}

void CTransform::Go_Straight(_float fTimeDelta, class CNavigation* pNavigation)
{
	_vector			vPosition = Get_State(STATE::POSITION);
	_vector			vLook = Get_State(STATE::LOOK);

	vPosition += XMVector3Normalize(vLook) * m_fSpeedPerSec * fTimeDelta;


	if (nullptr == pNavigation ||
		true == pNavigation->isMove(vPosition))
		Set_State(STATE::POSITION, vPosition);
}

void CTransform::Go_Roll(_float fTimeDelta, _float fSpeed)
{
	_vector			vPosition = Get_State(STATE::POSITION);
	_vector			vLook = Get_State(STATE::LOOK);

	vPosition += XMVector3Normalize(vLook) * fSpeed * (fTimeDelta * 1.5f);

	Set_State(STATE::POSITION, vPosition);
}



void CTransform::Go_Backward(_float fTimeDelta)
{
	_vector			vPosition = Get_State(STATE::POSITION);
	_vector			vLook = Get_State(STATE::LOOK);

	vPosition -= XMVector3Normalize(vLook) * m_fSpeedPerSec * fTimeDelta;

	Set_State(STATE::POSITION, vPosition);
}

void CTransform::Go_Right(_float fTimeDelta)
{
	_vector			vPosition = Get_State(STATE::POSITION);
	_vector			vRight = Get_State(STATE::RIGHT);

	vPosition += XMVector3Normalize(vRight) * m_fSpeedPerSec * fTimeDelta;

	Set_State(STATE::POSITION, vPosition);
}

void CTransform::Go_Left(_float fTimeDelta)
{
	_vector			vPosition = Get_State(STATE::POSITION);
	_vector			vRight = Get_State(STATE::RIGHT);

	vPosition -= XMVector3Normalize(vRight) * m_fSpeedPerSec * fTimeDelta;

	Set_State(STATE::POSITION, vPosition);
}

void CTransform::Go_Down(_float fTimeDelta)
{
	_vector vPosition = Get_State(STATE::POSITION);
	_vector vUp = Get_State(STATE::UP);

	vPosition -= XMVector3Normalize(vUp)* m_fSpeedPerSec * fTimeDelta;

	Set_State(STATE::POSITION, vPosition);
}

void CTransform::Go_Up(_float fTimeDelta)
{
	_vector vPosition = Get_State(STATE::POSITION);
	_vector vUp = Get_State(STATE::UP);

	vPosition += XMVector3Normalize(vUp) * m_fSpeedPerSec * fTimeDelta;

	Set_State(STATE::POSITION, vPosition);
}

void CTransform::Rotation(_fvector vAxis, _float fDegree)
{
	_float3		vScaled = Get_Scaled();

	_vector		vRight = XMVectorSet(1.f, 0.f, 0.f, 0.f) * vScaled.x;
	_vector		vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f) * vScaled.y;
	_vector		vLook = XMVectorSet(0.f, 0.f, 1.f, 0.f) * vScaled.z;

	_matrix		RotationMatrix = XMMatrixRotationAxis(vAxis, XMConvertToRadians(fDegree));

	//XMVector3TransformCoord();	
	// XMVector4Transform();

	Set_State(STATE::RIGHT, XMVector3TransformNormal(vRight, RotationMatrix));
	Set_State(STATE::UP, XMVector3TransformNormal(vUp, RotationMatrix));
	Set_State(STATE::LOOK, XMVector3TransformNormal(vLook, RotationMatrix));
}

void CTransform::Turn(_fvector vAxis, _float fTimeDelta)
{
	_vector		vRight = Get_State(STATE::RIGHT);
	_vector		vUp = Get_State(STATE::UP);
	_vector		vLook = Get_State(STATE::LOOK);

	_matrix		RotationMatrix = XMMatrixRotationAxis(vAxis, m_fRadianPerSec * fTimeDelta);

	Set_State(STATE::RIGHT, XMVector3TransformNormal(vRight, RotationMatrix));
	Set_State(STATE::UP, XMVector3TransformNormal(vUp, RotationMatrix));
	Set_State(STATE::LOOK, XMVector3TransformNormal(vLook, RotationMatrix));
}

void CTransform::LookAt(_fvector vAt)
{
	_vector		vPosition = Get_State(STATE::POSITION);
	_float3		vScaled = Get_Scaled();

	_vector		vLook = vAt - vPosition;
	_vector		vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook);
	_vector		vUp = XMVector3Cross(vLook, vRight);

	Set_State(STATE::RIGHT, XMVector3Normalize(vRight) * vScaled.x);
	Set_State(STATE::UP, XMVector3Normalize(vUp) * vScaled.y);
	Set_State(STATE::LOOK, XMVector3Normalize(vLook) * vScaled.z);
}

HRESULT CTransform::Bind_ShaderResource(CShader* pShader, const _char* pConstantName)
{
	return pShader->Bind_Matrix(pConstantName, &m_WorldMatrix);
}

CTransform* CTransform::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTransform* pInstance = new CTransform(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CTransform");
		Safe_Release(pInstance);
	}
	return pInstance;
}


CComponent* CTransform::Clone(void* pArg)
{
	return nullptr;
}


void CTransform::Free()
{
    __super::Free();


}
