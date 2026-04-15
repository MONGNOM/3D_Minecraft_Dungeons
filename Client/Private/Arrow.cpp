#include "Arrow.h"
#include "GameInstance.h"

#include "Player.h"

CArrow::CArrow(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }, m_iArrowDamage{ 0 }
{
}

CArrow::CArrow(const CArrow& Prototype)
	: CGameObject{ Prototype }, m_iArrowDamage{ Prototype.m_iArrowDamage}
{
}

HRESULT CArrow::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CArrow::Initialize(void* pArg)
{
	CArrow::ArrowDesc* desc = reinterpret_cast<ArrowDesc*>(pArg);

	desc->fSpeedPerSec = 40.f;
	
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_iArrowDamage = m_pGameInstance->Random(10, 30);


	if (desc != nullptr)
	{
		m_fRot = desc->rot;
		m_fPos = desc->pos;
		m_eSceneType = desc->Scenetype;
		m_etype = desc->type;
	}
	m_Name = TEXT("Arrow");


	m_pTransformCom->Set_Rotation(m_fRot);
	m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(m_fPos.x, m_fPos.y, m_fPos.z, 1.f));
	m_pTransformCom->Set_State(STATE::LOOK, desc->look);
	object = m_eSceneType == GAMEPLAY ? &m_pGameInstance->Get_LayerObjects(m_eSceneType, TEXT("Layer_Clone")) : &m_pGameInstance->Get_LayerObjects(m_eSceneType, TEXT("Load_Layer"));

	return S_OK;
}

void CArrow::Priority_Update(_float fTimeDelta)
{
}

void CArrow::Update(_float fTimeDelta)
{
	m_fDeleteTime += fTimeDelta;

	Intersect_ToMonster();

	m_pTransformCom->Go_Straight(fTimeDelta);
	

	m_pColliderCom->Update(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()));

}

void CArrow::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(RENDERGROUP::NONBLEND, this);
}

HRESULT CArrow::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;


	size_t iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE, 0);

		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;

		m_pModelCom->Render(i);
	}

	m_pColliderCom->Render();

	return S_OK;
}

void CArrow::Intersect_ToMonster()
{
	m_pColliderCom->Set_isColl(false);

	CCollider* colliderplayer = dynamic_cast<CCollider*>(m_pGameInstance->Get_Component(TEXT("Prototype_GameObject_Player0"), TEXT("Layer_Clone"), ETOI(m_eSceneType), TEXT("Com_Collider")));

	if (colliderplayer == nullptr) return;

	if (m_pColliderCom->Intersect(colliderplayer) && m_etype == OBJECTTYPE::MONSTER)
	{
		colliderplayer->Get_Owner()->TakeHit(m_iArrowDamage);
		m_pColliderCom->Set_isColl(true);
		colliderplayer->Set_isColl(true);
		wcout << colliderplayer->Get_Owner()->Get_ObjectName() << "에게 피해를 입혔다" << endl;
		//DecreaseHp(m_iSwordDamage);
		Set_Dead();
		return;
	}
	else
	{
		if (m_fDeleteTime >= 1.5f)
		{
			// 삭제?
			Set_Dead();
			m_fDeleteTime = 0;
			return;
		}
	}

	if (nullptr == object)
		return;

	for (auto& iter : *object)
	{
		CCollider* collider = dynamic_cast<CCollider*>(iter->Get_Component(TEXT("Com_Collider")));

		if (collider == nullptr || collider->Get_Owner() == this) continue;

		if (m_pColliderCom->Intersect(collider) && collider->Get_Owner()->Get_ObjectType() == OBJECTTYPE::MONSTER && m_etype == OBJECTTYPE::PLAYER)
		{
			collider->Get_Owner()->TakeHit(m_iArrowDamage);
			m_pColliderCom->Set_isColl(true);
			collider->Set_isColl(true);
			wcout << collider->Get_Owner()->Get_ObjectName() << "에게 피해를 입혔다" << endl;
			Set_Dead();
		}
		else
		{
			if (m_fDeleteTime >= 1.5f)
			{
				// 삭제?
				Set_Dead();
				m_fDeleteTime = 0;
			}
		}
	}

}

HRESULT CArrow::Ready_Components()
{

	if (FAILED(__super::Add_Component(ETOI(m_eSceneType), TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(ETOI(m_eSceneType), TEXT("Prototype_Component_Model_Arrow"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;


	CBounding_AABB::BOUNDING_AABB_DESC Desc{};
	Desc.vCenter = _float3(0.f, Desc.vExtents.y, 0.f);
	Desc.vExtents = _float3(0.5f,0.5f,0.5f);
	Desc.owner = this;

	if (FAILED(__super::Add_Component(ETOI(LEVEL::STATIC), TEXT("Prototype_Component_Collider_AABB"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &Desc)))
		return E_FAIL;


	return S_OK;
}

HRESULT CArrow::Bind_ShaderResources()
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

CArrow* CArrow::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CArrow* pInstance = new CArrow(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CArrow");
		Safe_Release(pInstance);
	}
	return pInstance;
}


CGameObject* CArrow::Clone(void* pArg)
{
	CArrow* pInstance = new CArrow(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CArrow");
		Safe_Release(pInstance);
	}
	return pInstance;
}


void CArrow::Free()
{
	__super::Free();
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pModelCom);

}
