#include "NameLessKing.h"

#include "Weapon.h"
#include "Body_Player.h"
#include "GameInstance.h"

CNameLessKing::CNameLessKing(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CContainerObject{ pDevice, pContext }
{
}

CNameLessKing::CNameLessKing(const CNameLessKing& Prototype)
	: CContainerObject{ Prototype }

{
}

HRESULT CNameLessKing::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CNameLessKing::Initialize(void* pArg)
{
	CContainerObject::CONTAINEROBJECT_DESC* Desc = static_cast<CONTAINEROBJECT_DESC*>(pArg);
	
	Desc->fSpeedPerSec = 10.f;
	Desc->fDegreePerSec = 180.f;


	/* 백그라운드의 멤버를 채워넣어야한다면 여기서 채운다. */
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_PartObjects()))
		return E_FAIL;

	if (Desc != nullptr)
	{
		m_fPos = Desc->pos;
		
		m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(m_fPos.x, m_fPos.y, m_fPos.z, 1.f));
	}

	m_Name = TEXT("NameLessKing");

	m_eObjectType = OBJECTTYPE::MONSTER;

	m_pModelCom->Ready_Animations("NamelessKing_Awake.Anim");
	m_pModelCom->Ready_Animations("NamelessKing_Walk.Anim");
	m_pModelCom->Ready_Animations("NamelessKing_BasicAttack.Anim");
	m_pModelCom->Ready_Animations("NamelessKing_StrongAttack_Composite.Anim");
	m_pModelCom->Ready_Animations("NamelessKing_TeleportIn.Anim");
	m_pModelCom->Ready_Animations("NamelessKing_TeleportOut.Anim");

	m_pModelCom->Set_Animation(0, true);

	//네비게이션 잠깐 꺼둠

	
	return S_OK;
}

void CNameLessKing::Priority_Update(_float fTimeDelta)
{
}

void CNameLessKing::Update(_float fTimeDelta)
{

	if (Intersect_ToPlayer())
	{
		//attack재생
		//if (*m_pParentState & CSkeleton::SKELETONSTATE::ATTACK)

		m_pModelCom->Set_Animation(1, true);
	}
	else
	{
		//if (*m_pParentState & CSkeleton::SKELETONSTATE::IDLE)
		m_pModelCom->Set_Animation(0, true);

		//idle
	}

	if (true == m_pModelCom->Play_Animation(fTimeDelta))
		int a = 10;

	m_pColliderCom->Update(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()));
	
}

void CNameLessKing::Late_Update(_float fTimeDelta)
{

	m_pGameInstance->Add_RenderGroup(RENDERGROUP::NONBLEND, this);
}

HRESULT CNameLessKing::Render()
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
	m_pColliderCom->Render();
	//m_pNavigationCom->Render();
#endif // _DEBUG


	return S_OK;
}

HRESULT CNameLessKing::Ready_Components()
{
	CBounding_AABB::BOUNDING_AABB_DESC AABBDesc;

	AABBDesc.vExtents = _float3(0.5f, 1.f, 0.5f);
	AABBDesc.vCenter = _float3(0.f, AABBDesc.vExtents.y, 0.f);
	AABBDesc.owner = this;

	if (FAILED(__super::Add_Component(ETOI(LEVEL::STATIC), TEXT("Prototype_Component_Collider_AABB"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &AABBDesc)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(ETOI(m_eSceneType), TEXT("Prototype_Component_Model_NameLessKing"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(ETOI(m_eSceneType), TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
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

HRESULT CNameLessKing::Ready_PartObjects()
{
	/*CBody_Player::BODY_PLAYER_DESC		BodyDesc{};
	BodyDesc.pParentState = &state;
	BodyDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
	BodyDesc.Scenetype = m_eSceneType;

	if (FAILED(__super::Add_PartObject(ETOI(m_eSceneType), TEXT("Prototype_GameObject_Body_Player"),
		TEXT("Part_Body"), &BodyDesc)))
		return E_FAIL;
	pBody = dynamic_cast<CBody_Player*>(m_PartObjects[TEXT("Part_Body")]);
	if (nullptr == pBody)
		return E_FAIL;

	

	CWeapon::WEAPON_DESC				WeaponDesc{};
	WeaponDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
	WeaponDesc.pSocketMatrix = pBody->Get_SocketBoneMatrixPtr("J_R_Weapon_Socket");
	WeaponDesc.Scenetype = m_eSceneType;

	if (FAILED(__super::Add_PartObject(ETOI(m_eSceneType), TEXT("Prototype_GameObject_Weapon"),
		TEXT("Part_Weapon"), &WeaponDesc)))
		return E_FAIL;*/

	return S_OK;
}

HRESULT CNameLessKing::Bind_ShaderResources()
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

_bool CNameLessKing::Intersect_ToPlayer()
{
	CCollider* collider = dynamic_cast<CCollider*>(m_pGameInstance->Get_Component(TEXT("Prototype_GameObject_Player0"), TEXT("Layer_Clone"), ETOI(m_eSceneType), TEXT("Com_Collider")));

	if (collider == nullptr) return false;

	if (m_pColliderCom->Intersect(collider))
	{
		m_pColliderCom->Set_isColl(true);
		return true;
	}
	else
	{
		m_pColliderCom->Set_isColl(false);
		return false;
	}
}



CNameLessKing* CNameLessKing::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CNameLessKing* pInstance = new CNameLessKing(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CNameLessKing");
		Safe_Release(pInstance);
	}
	return pInstance;
}


CGameObject* CNameLessKing::Clone(void* pArg)
{
	CNameLessKing* pInstance = new CNameLessKing(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CNameLessKing");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CNameLessKing::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
}
