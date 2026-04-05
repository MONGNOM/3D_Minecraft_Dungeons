#include "Body_Skeleton.h"
#include "GameInstance.h"

#include "Skeleton.h"

CBody_Skeleton::CBody_Skeleton(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPartObject{ pDevice, pContext }
{
}

CBody_Skeleton::CBody_Skeleton(const CBody_Skeleton& Prototype)
	: CPartObject{ Prototype }
{
}

const _float4x4* CBody_Skeleton::Get_SocketBoneMatrixPtr(const _char* pBoneName)
{
	return m_pModelCom->Get_BoneMatrixPtr(pBoneName);
}

HRESULT CBody_Skeleton::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBody_Skeleton::Initialize(void* pArg)
{
	auto	pDesc = static_cast<CBODY_SKELETONDESC*>(pArg);

	m_pParentState = pDesc->pParentState;


	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	
	m_pModelCom->Ready_Animations("Skeleton_Idle.Anim");
	m_pModelCom->Ready_Animations("Skeleton_BowAction.Anim");

	m_pModelCom->Set_Animation(0, true);
	


	return S_OK;
}

void CBody_Skeleton::Priority_Update(_float fTimeDelta)
{
}

void CBody_Skeleton::Update(_float fTimeDelta)
{

	if (*m_pParentState & CSkeleton::SKELETONSTATE::IDLE)
		m_pModelCom->Set_Animation(0, true);

	if (*m_pParentState & CSkeleton::SKELETONSTATE::ATTACK)
		m_pModelCom->Set_Animation(1, true);

	if (true == m_pModelCom->Play_Animation(fTimeDelta))
		int a = 10;
	
	if (Intersect_ToPlayer())
	{
		//damage¹ÞÀ½
	}
	else
	{
		// ? 
	}

	Update_CombinedWorldMatrix(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()));
	m_pColliderCom->Update(XMLoadFloat4x4(&m_CombinedWorldMatrix));
}

void CBody_Skeleton::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(RENDERGROUP::NONBLEND, this);
}

HRESULT CBody_Skeleton::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;


	size_t iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE, 0);
		m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i);

		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;

		m_pModelCom->Render(i);
	}


	m_pColliderCom->Render();
	return S_OK;
}

HRESULT CBody_Skeleton::Ready_Components()
{

	if (FAILED(__super::Add_Component(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Model_Skeleton"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	CBounding_AABB::BOUNDING_AABB_DESC AABBDesc;

	AABBDesc.vExtents = _float3(0.5f, 1.f, 0.5f);
	AABBDesc.vCenter = _float3(0.f, AABBDesc.vExtents.y, 0.f);

	if (FAILED(__super::Add_Component(ETOI(LEVEL::STATIC), TEXT("Prototype_Component_Collider_AABB"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &AABBDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CBody_Skeleton::Bind_ShaderResources()
{
	/*if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;*/
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_TransformMatrix(D3DTS::VIEW, m_pShaderCom, "g_ViewMatrix")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_TransformMatrix(D3DTS::PROJ, m_pShaderCom, "g_ProjMatrix")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_CamPosition(m_pShaderCom, "g_vCamPosition")))
		return E_FAIL;

	const LIGHT_DESC* pLightDesc = m_pGameInstance->Get_LightDesc(0);
	if (nullptr == pLightDesc)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDir", &pLightDesc->vDirection, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDiffuse", &pLightDesc->vDiffuse, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightAmbient", &pLightDesc->vAmbient, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightSpecular", &pLightDesc->vSpecular, sizeof(_float4))))
		return E_FAIL;

	return S_OK;
}

_bool CBody_Skeleton::Intersect_ToPlayer()
{

	CCollider* collider = dynamic_cast<CCollider*>(m_pGameInstance->Get_Component(TEXT("Prototype_GameObject_Player0"), TEXT("Layer_Clone"), ETOI(LEVEL::GAMEPLAY), TEXT("Com_Collider")));

	return collider ? m_pColliderCom->Intersect(collider) : false;
}

CBody_Skeleton* CBody_Skeleton::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBody_Skeleton* pInstance = new CBody_Skeleton(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CBody_Skeleton");
		Safe_Release(pInstance);
	}
	return pInstance;
}


CGameObject* CBody_Skeleton::Clone(void* pArg)
{
	CBody_Skeleton* pInstance = new CBody_Skeleton(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CBody_Skeleton");
		Safe_Release(pInstance);
	}
	return pInstance;
}


void CBody_Skeleton::Free()
{
	__super::Free();
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pModelCom);

	
}
