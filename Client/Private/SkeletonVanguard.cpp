#include "SkeletonVanguard.h"

#include "Weapon.h"
#include "Body_Player.h"
#include "GameInstance.h"

CSkeletonVanguard::CSkeletonVanguard(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CSkeletonVanguard::CSkeletonVanguard(const CSkeletonVanguard& Prototype)
	: CGameObject{ Prototype }

{
}

HRESULT CSkeletonVanguard::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CSkeletonVanguard::Initialize(void* pArg)
{
	
	CGameObject::GAMEOBJECT_DESC* desc = reinterpret_cast<GAMEOBJECT_DESC*>(pArg);

	desc->fSpeedPerSec = 5.f;
	desc->fDegreePerSec = 180.f;


	/* 백그라운드의 멤버를 채워넣어야한다면 여기서 채운다. */
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;
	

	if (desc != nullptr)
	{
		m_fPos = desc->pos;
		
		m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(m_fPos.x, m_fPos.y, m_fPos.z, 1.f));
	}

	m_eObjectType = OBJECTTYPE::MONSTER;

	m_pModelCom->Ready_Animations("Skeleton_Vanguard_Idle.Anim");
	m_pModelCom->Ready_Animations("Skeleton_Vanguard_Walk.Anim");
	m_pModelCom->Ready_Animations("Skeleton_Vanguard_Attack.Anim");

	m_pModelCom->Set_Animation(0, true);

	damage = 20;

	return S_OK;
}

void CSkeletonVanguard::Priority_Update(_float fTimeDelta)
{
}

void CSkeletonVanguard::Update(_float fTimeDelta)
{

	if (Intersect_ToPlayer())
	{
		m_pModelCom->Set_Animation(2, true);

		if (m_pModelCom->Get_CurrentTrackPos() >= 9.6f && m_pModelCom->Get_CurrentTrackPos() <= 10.f)
		{
			Intersect_ToPlayerAttack();
		}
		else
		{
			m_pColliderCom[ETOI(COLLIDER::AABB)]->SetActive_Collider(false);
		}
	}
	else if (Intersect_ToPlayerSphere())
	{
		m_pModelCom->Set_Animation(1, true);
		m_pTransformCom->Go_Straight(fTimeDelta);
	}
	else
	{
		m_pModelCom->Set_Animation(0, true);
	}

	if (true == m_pModelCom->Play_Animation(fTimeDelta))
		int a = 10;

	for (auto& iter : m_pColliderCom)
	iter->Update(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()));
}

void CSkeletonVanguard::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(RENDERGROUP::NONBLEND, this);
}

HRESULT CSkeletonVanguard::Render()
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


#ifdef _DEBUG
	for (auto& iter : m_pColliderCom)
		iter->Render();
	//m_pNavigationCom->Render();
#endif // _DEBUG


	return S_OK;
}

HRESULT CSkeletonVanguard::Ready_Components()
{
	CBounding_AABB::BOUNDING_AABB_DESC AABBDesc;

	AABBDesc.vExtents = _float3(1.f, 1.f, 1.f);
	AABBDesc.vCenter = _float3(0.f, AABBDesc.vExtents.y, 0.f);
	AABBDesc.owner = this;

	if (FAILED(__super::Add_Component(ETOI(LEVEL::STATIC), TEXT("Prototype_Component_Collider_AABB"),
		TEXT("Com_Collider_AABB"), reinterpret_cast<CComponent**>(&m_pColliderCom[ETOI(COLLIDER::AABB)]), &AABBDesc)))
		return E_FAIL;

	AABBDesc.vExtents = _float3(1.f, 1.f, 1.f);
	AABBDesc.vCenter = _float3(0.f, AABBDesc.vExtents.y, 0.f);
	AABBDesc.owner = this;

	if (FAILED(__super::Add_Component(ETOI(LEVEL::STATIC), TEXT("Prototype_Component_Collider_AABB"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom[1]), &AABBDesc)))
		return E_FAIL;

	CBounding_Sphere::BOUNDING_SPHERE_DESC SphereDesc{};
	SphereDesc.vCenter = _float3(0.f, SphereDesc.fRadius, 0.f);
	SphereDesc.fRadius = 10.0f;
	SphereDesc.owner = this;

	if (FAILED(__super::Add_Component(ETOI(LEVEL::STATIC), TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Collider_Sphere"), reinterpret_cast<CComponent**>(&m_pColliderCom[ETOI(COLLIDER::SPHERE)]), &SphereDesc)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(ETOI(m_eSceneType), TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(ETOI(m_eSceneType), TEXT("Prototype_Component_Model_SkeletonVanguard"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;




	//CNavigation::NAVIGATION_DESC		NavigationDesc{};
	//NavigationDesc.iCurrentCellIndex = 1;
	//NavigationDesc.pTransform = m_pTransformCom;
	//
	//if (FAILED(__super::Add_Component(ETOI(m_eSceneType), TEXT("Prototype_Component_Navigation"),
	//	TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom), &NavigationDesc)))
	//	return E_FAIL;

	return S_OK;
}

HRESULT CSkeletonVanguard::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
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


_bool CSkeletonVanguard::Intersect_ToPlayer()
{
	CCollider* collider = dynamic_cast<CCollider*>(m_pGameInstance->Get_Component(TEXT("Prototype_GameObject_Player0"), TEXT("Layer_Clone"), ETOI(m_eSceneType), TEXT("Com_Collider")));

	if (collider == nullptr) return false;

	if (m_pColliderCom[1]->Intersect(collider))
	{
		m_pColliderCom[1]->Set_isColl(true);
		return true;
	}
	else
	{
		m_pColliderCom[1]->Set_isColl(false);
		return false;
	}
}

_bool CSkeletonVanguard::Intersect_ToPlayerAttack()
{
	m_pColliderCom[ETOI(COLLIDER::AABB)]->SetActive_Collider(true);

	CCollider* collider = dynamic_cast<CCollider*>(m_pGameInstance->Get_Component(TEXT("Prototype_GameObject_Player0"), TEXT("Layer_Clone"), ETOI(m_eSceneType), TEXT("Com_Collider")));

	if (collider == nullptr) return false;


	if (m_pColliderCom[ETOI(COLLIDER::AABB)]->Intersect(collider))
	{

		m_pColliderCom[ETOI(COLLIDER::AABB)]->Set_isColl(true);
		collider->Get_Owner()->TakeHit(damage);
		wcout << collider->Get_Owner()->Get_ObjectName() << "에게 피해를 입혔다" << endl;
		return true;
	}
	else
	{
		m_pColliderCom[ETOI(COLLIDER::AABB)]->Set_isColl(false);
		return false;
	}
}

_bool CSkeletonVanguard::Intersect_ToPlayerSphere()
{
	CCollider* collider = dynamic_cast<CCollider*>(m_pGameInstance->Get_Component(TEXT("Prototype_GameObject_Player0"), TEXT("Layer_Clone"), ETOI(m_eSceneType), TEXT("Com_Collider")));

	if (collider == nullptr) return false;

	if (m_pColliderCom[ETOI(COLLIDER::SPHERE)]->Intersect(collider))
	{

		m_pTransformCom->LookAt(dynamic_cast<CTransform*>(collider->Get_Owner()->Get_Component(TEXT("Com_Transform")))->Get_State(STATE::POSITION));

		m_pColliderCom[ETOI(COLLIDER::SPHERE)]->Set_isColl(true);
		return true;
	}
	else
	{
		m_pColliderCom[ETOI(COLLIDER::SPHERE)]->Set_isColl(false);
		return false;
	}
}



CSkeletonVanguard* CSkeletonVanguard::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSkeletonVanguard* pInstance = new CSkeletonVanguard(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CSkeletonVanguard");
		Safe_Release(pInstance);
	}
	return pInstance;
}


CGameObject* CSkeletonVanguard::Clone(void* pArg)
{
	CSkeletonVanguard* pInstance = new CSkeletonVanguard(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CSkeletonVanguard");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CSkeletonVanguard::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	for (auto& iter : m_pColliderCom)
		Safe_Release(iter);
	//Safe_Release(m_pNavigationCom);
}
