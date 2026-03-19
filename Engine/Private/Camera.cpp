#include "Camera.h"
#include "GameInstance.h"

CCamera::CCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject { pDevice, pContext }
	, m_pGameInstance { CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
}

CCamera::CCamera(const CCamera& Prototype)
	: CGameObject{ Prototype }
	, m_pGameInstance{ Prototype.m_pGameInstance }
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CCamera::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCamera::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	CAMERA_DESC* pDesc = static_cast<CAMERA_DESC*>(pArg);

	m_pTransformCom->Set_State(STATE::POSITION, XMLoadFloat4(&pDesc->vEye));
	m_pTransformCom->LookAt(XMLoadFloat4(&pDesc->vAt));

	_uint		iNumViewports = { 1 };
	D3D11_VIEWPORT	ViewportDesc = {};

	m_pContext->RSGetViewports(&iNumViewports, &ViewportDesc);

	m_fFovy = pDesc->fFovy;
	m_fNear = pDesc->fNear;
	m_fFar = pDesc->fFar;
	m_fAspect = ViewportDesc.Width / ViewportDesc.Height;

	Update_TransformMatrices();

	return S_OK;
}

void CCamera::Priority_Update(_float fTimeDelta)
{
}

void CCamera::Update(_float fTimeDelta)
{
}

void CCamera::Late_Update(_float fTimeDelta)
{
}

HRESULT CCamera::Render()
{
	return S_OK;
}



void CCamera::Update_TransformMatrices()
{
	m_pGameInstance->Set_Transform(D3DTS::VIEW, 
		XMMatrixInverse(nullptr, XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr())));

	m_pGameInstance->Set_Transform(D3DTS::PROJ, 
		XMMatrixPerspectiveFovLH(m_fFovy, m_fAspect, m_fNear, m_fFar));
}

void CCamera::Free()
{
	__super::Free();

	Safe_Release(m_pGameInstance);
}
