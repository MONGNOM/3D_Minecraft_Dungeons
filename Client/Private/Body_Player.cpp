#include "Body_Player.h"
#include "GameInstance.h"

#include "Player.h"
#include "Bow.h"
#include "Arrow.h"
#include "Weapon.h"

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
	m_pPlayer = pDesc->pPlayer;

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
	m_pModelCom->Ready_Animations("Player_Master_SwordCombo.Anim");				//5 칼
	//m_pModelCom->Ready_Animations("Player_Master_SwordCombo1.Anim");			//5 칼
	//m_pModelCom->Ready_Animations("Player_Master_SwordCombo2.Anim");			//6 칼
	//m_pModelCom->Ready_Animations("Player_Master_SwordCombo3.Anim");			//7 칼
	//m_pModelCom->Ready_Animations("Player_Master_SwordCombo4.Anim");			//8 칼


	m_pModelCom->Set_Animation(0, true);

	return S_OK;
}

void CBody_Player::Priority_Update(_float fTimeDelta)
{
}

void CBody_Player::Update(_float fTimeDelta)
{
	// 이렇게 하면 안될 것 같은데 아무리 봐도 
	Update_CombinedWorldMatrix(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()));

	if (m_PrevPlayerState != *m_pParentPlayerState)
	{
		m_PrevPlayerState = *m_pParentPlayerState;

		m_bIsAnimFinished = false;

		switch (*m_pParentPlayerState)
		{
		case PLAYERSTATE::IDLE:
			m_pModelCom->Set_Animation(0, true);
			break;

		case PLAYERSTATE::WALK:
			m_pModelCom->Set_Animation(1, true);
			break;

		case PLAYERSTATE::HEAL:
			m_pModelCom->Set_Animation(2, false);
			break;

		case PLAYERSTATE::FAILING:
		{
			CCollider* collider = dynamic_cast<CCollider*>(m_pGameInstance->Get_Component(TEXT("Prototype_GameObject_Player0"), m_eSceneType == GAMEPLAY ? TEXT("Layer_Clone") : TEXT("Load_Layer"), ETOI(m_eSceneType), TEXT("Com_Collider")));  // 레이어이름을 저렇게 할까그냥
			collider->SetActive_Collider(false);
			m_pModelCom->Set_Animation(3, false);
			break;
		}
		case PLAYERSTATE::BOW:
		{
			m_bshot = true;
			m_pModelCom->Set_Animation(4, false);
			break;
		}
		case PLAYERSTATE::ATTACK:
		{
			m_pModelCom->Set_Animation(5, false);
			m_WeaponColiider = false;
			break;
		}
		}
	}

	

	_float a = m_pModelCom->Get_CurrentTrackPos();
	_bool aniEnd = m_pModelCom->Play_Animation(fTimeDelta);
	if (PLAYERSTATE::ATTACK == *m_pParentPlayerState && m_pModelCom->Get_CurrentTrackPos() >= _float(3.6f) && m_pModelCom->Get_CurrentTrackPos() <= 4.f ||
		PLAYERSTATE::ATTACK == *m_pParentPlayerState && m_pModelCom->Get_CurrentTrackPos() >= _float(14.6f) && m_pModelCom->Get_CurrentTrackPos() <= 15.f ||
		PLAYERSTATE::ATTACK == *m_pParentPlayerState && m_pModelCom->Get_CurrentTrackPos() >= _float(31.6f) && m_pModelCom->Get_CurrentTrackPos() <= 32.f)
	{
		m_WeaponColiider = true;
	}
	else
		m_WeaponColiider = false;


	if (aniEnd)
	{
		m_bIsAnimFinished = true;

		switch (*m_pParentPlayerState)
		{
		case PLAYERSTATE::BOW:
			{
				m_bshot = false;
				CArrow::ArrowDesc desc{};
				desc.rot = m_pTransformCom->Get_Rotation();
				XMStoreFloat3(&desc.pos, XMLoadFloat4(reinterpret_cast<_float4*>(&m_CombinedWorldMatrix.m[3][0])));
				desc.pos.y += 1.5f;
				desc.look = XMVector3Normalize(XMLoadFloat4((reinterpret_cast<_float4*>(&m_CombinedWorldMatrix.m[2][0]))));
				desc.Scenetype = m_eSceneType;
				desc.NumTexture = 0;

				if (FAILED(m_pGameInstance->Add_GameObject(m_eSceneType, TEXT("Prototype_GameObject_Arrow"), ETOI(m_eSceneType), TEXT("Load_Layer"), &desc)))
				{
					MSG_BOX("화살안만들어졌어");
					return;
				}

				break;
			}
		case PLAYERSTATE::FAILING:
			{
				CCollider* collider = dynamic_cast<CCollider*>(m_pGameInstance->Get_Component(TEXT("Prototype_GameObject_Player0"), m_eSceneType == GAMEPLAY ? TEXT("Layer_Clone") : TEXT("Load_Layer"), ETOI(m_eSceneType), TEXT("Com_Collider")));
				collider->SetActive_Collider(true);
				dynamic_cast<CPlayer*>(m_pPlayer)->Set_Roll(false);
				break;
			}
		}
	
	}

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
	

	if (FAILED(__super::Add_Component(ETOI(m_eSceneType), TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(ETOI(m_eSceneType), TEXT("Prototype_Component_Model_Player"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	CBounding_Sphere::BOUNDING_SPHERE_DESC		SphereDesc{};
	SphereDesc.fRadius = 0.7f;
	SphereDesc.vCenter = _float3(0.f, SphereDesc.fRadius, 0.f);
	SphereDesc.owner = this;

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
