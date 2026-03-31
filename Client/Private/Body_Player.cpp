#include "Body_Player.h"
#include "GameInstance.h"

#include "Player.h"

CBody_Player::CBody_Player(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPartObject{ pDevice, pContext }, m_pParentPlayerState {nullptr}
{
	
}

CBody_Player::CBody_Player(const CBody_Player& Prototype)
	: CPartObject{ Prototype }, m_pParentPlayerState{ nullptr }
{

}

const _float4x4* CBody_Player::Get_SocketBoneMatrixPtr(const _char* pBoneName)
{
	return m_pModelCom->Get_BoneMatrixPtr(pBoneName);
}

HRESULT CBody_Player::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBody_Player::Initialize(void* pArg)
{
	auto	pDesc = static_cast<BODY_PLAYER_DESC*>(pArg);

	m_pParentPlayerState = pDesc->pParentState;


	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	/*m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(
		m_pGameInstance->Random(0.f, 10.f),
		2.f,
		m_pGameInstance->Random(0.f, 10.f),
		1.f
	));*/

	m_pModelCom->Ready_Animations("Player_Master_Idle.Anim");					//0 기본동작
	m_pModelCom->Ready_Animations("Player_Master_Run.Anim");					//1 달리기 
	m_pModelCom->Ready_Animations("Player_Master_Drink.Anim");					//2 마시기
	m_pModelCom->Ready_Animations("Player_Master_DodgeRoll.Anim");				//3 구르기
	m_pModelCom->Ready_Animations("Player_Master_BowAction.Anim");				//4 활
	m_pModelCom->Ready_Animations("Player_Master_SwordCombo.001.Anim");			//5 칼


	m_pModelCom->Set_Animation(0, true);

	return S_OK;
}

void CBody_Player::Priority_Update(_float fTimeDelta)
{
}

void CBody_Player::Update(_float fTimeDelta)
{
	switch (*m_pParentPlayerState)
	{
	case PLAYERSTATE::IDLE:
		m_pModelCom->Set_Animation(0, true);
		if (true == m_pModelCom->Play_Animation(fTimeDelta))
			PLAYERSTATE::IDLE;
		break;

	case PLAYERSTATE::WALK:
		m_pModelCom->Set_Animation(1, true);
		if (true == m_pModelCom->Play_Animation(fTimeDelta))
			m_pModelCom->Set_Animation(0, true);
		break;

	case PLAYERSTATE::HEAL:
		m_pModelCom->Set_Animation(2, false);
		if (true == m_pModelCom->Play_Animation(fTimeDelta))
			m_pModelCom->Set_Animation(0, true);

		break;

	case PLAYERSTATE::FAILING:
		m_pModelCom->Set_Animation(3, true);
		if (true == m_pModelCom->Play_Animation(fTimeDelta))
			PLAYERSTATE::IDLE;
		break;

	case PLAYERSTATE::BOW:
		m_pModelCom->Set_Animation(4, true);
		if (true == m_pModelCom->Play_Animation(fTimeDelta))
			PLAYERSTATE::IDLE;
		break;

	case PLAYERSTATE::ATTACK:
		m_pModelCom->Set_Animation(5, true);
		if (true == m_pModelCom->Play_Animation(fTimeDelta))
			PLAYERSTATE::IDLE;
		break;
	}
	

	Update_CombinedWorldMatrix(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()));

	m_pColliderCom->Update(XMLoadFloat4x4(&m_CombinedWorldMatrix));
}

void CBody_Player::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(RENDERGROUP::NONBLEND, this);
}

HRESULT CBody_Player::Render()
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

HRESULT CBody_Player::Ready_Components()
{
	

	if (FAILED(__super::Add_Component(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Model_Player"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	CBounding_Sphere::BOUNDING_SPHERE_DESC		SphereDesc{};
	SphereDesc.fRadius = 0.7f;
	SphereDesc.vCenter = _float3(0.f, SphereDesc.fRadius, 0.f);

	if (FAILED(__super::Add_Component(ETOI(LEVEL::STATIC), TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &SphereDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CBody_Player::Bind_ShaderResources()
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

CBody_Player* CBody_Player::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBody_Player* pInstance = new CBody_Player(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CBody_Player");
		Safe_Release(pInstance);
	}
	return pInstance;
}


CGameObject* CBody_Player::Clone(void* pArg)
{
	CBody_Player* pInstance = new CBody_Player(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CBody_Player");
		Safe_Release(pInstance);
	}
	return pInstance;
}


void CBody_Player::Free()
{
	__super::Free();
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pModelCom);

}
