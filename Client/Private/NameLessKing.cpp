#include "NameLessKing.h"

#include "JusinBox.h"
#include "Body_Player.h"
#include "GameInstance.h"
#include "SkeletonVanguard.h"
#include "BossMark.h"

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
	NAMELESSKING_DESC* Desc = static_cast<NAMELESSKING_DESC*>(pArg);
	
	Desc->fSpeedPerSec = 10.f;
	Desc->fDegreePerSec = 180.f;


	m_bShadow = Desc->Shadow;
	m_bTakehit = false;
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
	m_pModelCom->Ready_Animations("NamelessKing_BasicAttack.Anim");
	m_pModelCom->Ready_Animations("NamelessKing_StrongAttack_Composite.Anim");
	m_pModelCom->Ready_Animations("NamelessKing_TeleportIn.Anim");// 소환이랑 tp랑 clone 애니메이션이 똑같음
	m_pModelCom->Ready_Animations("NamelessKing_TeleportOut.Anim");

	//m_pModelCom->Set_Animation(0, false);

	m_fMaxHp = 100;
	m_fCurrentHp = m_fMaxHp;
	
	return S_OK;
}

void CNameLessKing::TakeHit(_uint damage)
{
	m_fCurrentHp -= damage;
	m_bTakehit = true;
}

void CNameLessKing::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CNameLessKing::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
	
	if (m_bShadow == 0)
	{
		CCollider* collider = dynamic_cast<CCollider*>(m_pGameInstance->Get_Component(TEXT("Prototype_GameObject_Player0"), m_eSceneType == GAMEPLAY ? TEXT("Layer_Clone") : TEXT("Load_Layer"), ETOI(m_eSceneType), TEXT("Com_Collider")));
		if (collider == nullptr) return;
		m_pTransformCom->LookAt(dynamic_cast<CTransform*>(collider->Get_Owner()->Get_Component(TEXT("Com_Transform")))->Get_State(STATE::POSITION));

		m_pModelCom->Set_Animation(2, false);
		if (m_pModelCom->Get_CurrentTrackPos() >= 14.f && m_pModelCom->Get_CurrentTrackPos() <= 14.4f)
		{
			// 큐브 생성
			CJusinBox::JUSINBOX_DESC JusinBoxDesc{};
			JusinBoxDesc.Scenetype = m_eSceneType;
			JusinBoxDesc.NumTexture = 16;
			JusinBoxDesc.state = JUSINSTATE::MISSILE;
			XMStoreFloat3(&JusinBoxDesc.pos, m_pTransformCom->Get_State(STATE::POSITION));
			JusinBoxDesc.pos.y += 1;
			JusinBoxDesc.Scenetype = m_eSceneType;
			JusinBoxDesc.look = XMVector3Normalize(m_pTransformCom->Get_State(STATE::LOOK));
			// 이거 안되는데?

			if (FAILED(m_pGameInstance->Add_GameObject(ETOI(m_eSceneType), TEXT("Prototype_GameObject_JusinBox"),
				ETOI(m_eSceneType), TEXT("Load_Layer"), &JusinBoxDesc)))
				return;
		}

		if (true == m_pModelCom->Play_Animation(fTimeDelta))
			Set_Dead();

		return;
	}

	// ===위 분신 === 아래 본체===

	if (m_fCurrentHp <= 0)
		Set_Dead();

	if (m_bAwake)
	{
		attacking = true;

		m_pModelCom->Set_Animation(0, false);
		if (true == m_pModelCom->Play_Animation(fTimeDelta))
		{
			m_bAwake = false;
			attacking = false;
			// 던전씬에 옮길 보스 ui
			CBossMark::BOSSMARK_DESC Desc{};


			//// 체력바 프레임
			Desc.fSizeX = 950;
			Desc.fSizeY = 30;
			Desc.fX = g_iWinSizeX * 0.5f + 10;
			Desc.fY = 140;
			Desc.iNumTexture = 1;
			Desc.Scenetype = CGameObject::SCENETYPE::BOSS;
			Desc.connet = &m_pHpBar;

			if (FAILED(m_pGameInstance->Add_GameObject(ETOI(LEVEL::BOSS), TEXT("Prototype_GameObject_BossMark"),
				ETOI(LEVEL::BOSS), TEXT("Layer_UI"), &Desc)))
				return;

			// 체력바
			Desc.fSizeX = 950;
			Desc.fSizeY = 30;
			Desc.fX = g_iWinSizeX * 0.5f + 10;
			Desc.fY = 140;
			Desc.iNumTexture = 0;
			Desc.Scenetype = CGameObject::SCENETYPE::BOSS;
			Desc.owner = this;
			Desc.connet = &m_pHpBar1;
			if (FAILED(m_pGameInstance->Add_GameObject(ETOI(LEVEL::BOSS), TEXT("Prototype_GameObject_BossMark"),
				ETOI(LEVEL::BOSS), TEXT("Layer_UI"), &Desc)))
				return;


			////마커 1
			Desc.fSizeX = 29;
			Desc.fSizeY = 29;
			Desc.fX = g_iWinSizeX * 0.5f - 150;
			Desc.fY = 140;
			Desc.iNumTexture = 3;
			Desc.connet = &m_pHpBar2;
			if (FAILED(m_pGameInstance->Add_GameObject(ETOI(LEVEL::BOSS), TEXT("Prototype_GameObject_BossMark"),
				ETOI(LEVEL::BOSS), TEXT("Layer_UI"), &Desc)))
				return;

			////마커 2
			Desc.fSizeX = 29;
			Desc.fSizeY = 29;
			Desc.fX = g_iWinSizeX * 0.5f + 150;
			Desc.fY = 140;
			Desc.iNumTexture = 3;
			Desc.connet = &m_pHpBar3;
			if (FAILED(m_pGameInstance->Add_GameObject(ETOI(LEVEL::BOSS), TEXT("Prototype_GameObject_BossMark"),
				ETOI(LEVEL::BOSS), TEXT("Layer_UI"), &Desc)))
				return;

			// 이름 옆에 마커
			Desc.fSizeX = 49;
			Desc.fSizeY = 39;
			Desc.fX = g_iWinSizeX * 0.5f - 150;
			Desc.fY = 80;
			Desc.iNumTexture = 2;
			Desc.Scenetype = CGameObject::SCENETYPE::BOSS;
			Desc.connet = &m_pHpBar4;
			if (FAILED(m_pGameInstance->Add_GameObject(ETOI(LEVEL::BOSS), TEXT("Prototype_GameObject_BossMark"),
				ETOI(LEVEL::BOSS), TEXT("Layer_UI"), &Desc)))
				return;
		}
	}
	else if (Intersect_ToPlayerSphere())
	{
		if (!attacking)
		{
			m_fStateTime += fTimeDelta;

			if (m_fStateTime > 0.5f && !attacking)
			{
				attacking = true;
				state = (BOSSSTATE)(m_pGameInstance->Random(1, ETOI(BOSS_END)));

				switch (state)
				{
				case BOSS_ATTACK:
				{
					m_pModelCom->Set_Animation(2, false);
					break;
				}
				case BOSS_TP:
				{
					m_pModelCom->Set_Animation(3, false);
					break;
				}
				case BOSS_SPAWN:
				{
					m_pModelCom->Set_Animation(1, false);
					break;
				}
				case BOSS_CLONE:
				{
					m_pModelCom->Set_Animation(1, false);

					break;
				}
				}
			}
		}
		else
		{
			switch (state)
			{
			case BOSS_ATTACK:
			{
				if (m_pModelCom->Get_CurrentTrackPos() >= 23.6f && m_pModelCom->Get_CurrentTrackPos() <= 23.8f)
				{
					// 큐브 생성
					CJusinBox::JUSINBOX_DESC JusinBoxDesc{};
					JusinBoxDesc.Scenetype = m_eSceneType;
					JusinBoxDesc.NumTexture = 16;
					JusinBoxDesc.state = JUSINSTATE::MISSILE;
					XMStoreFloat3(&JusinBoxDesc.pos, m_pTransformCom->Get_State(STATE::POSITION));
					JusinBoxDesc.pos.y += 1;
					JusinBoxDesc.Scenetype = m_eSceneType;
					JusinBoxDesc.look = XMVector3Normalize(m_pTransformCom->Get_State(STATE::LOOK));

					if (FAILED(m_pGameInstance->Add_GameObject(ETOI(m_eSceneType), TEXT("Prototype_GameObject_JusinBox"),
						ETOI(m_eSceneType), TEXT("Load_Layer"), &JusinBoxDesc)))
						return;
				}
				break;
			}
			case BOSS_TP:
			{
				if (m_pModelCom->Get_CurrentTrackPos() >= 9.6f && m_pModelCom->Get_CurrentTrackPos() <= 10.f)
				{
					_float3 svPos;
					XMStoreFloat3(&svPos, m_pTransformCom->Get_State(STATE::POSITION));
					svPos.x += m_pGameInstance->Random(-3, 3);
					svPos.y;
					svPos.z += m_pGameInstance->Random(-3, 3);

					m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(svPos.x, svPos.y, svPos.z, 1.f));
				}
				break;
			}
			case BOSS_SPAWN:
			{
				if (m_pModelCom->Get_CurrentTrackPos() >= 163.6f && m_pModelCom->Get_CurrentTrackPos() <= 164.f)
				{
					// 해골병사 생성
					CSkeletonVanguard::VANGUARD_Desc desc{};
					_float3 svPos;
					XMStoreFloat3(&svPos, m_pTransformCom->Get_State(STATE::POSITION));
					_vector vLook = XMVector3Normalize(m_pTransformCom->Get_State(STATE::LOOK));
					desc.Scenetype = m_eSceneType;
					desc.NumTexture = 0;
					_float fDistance = 5.f;

					desc.look = vLook;
					desc.pos = svPos;
					for (size_t i = 0; i < 3; i++)
					{
						_vector vSpawnPos = XMLoadFloat3(&svPos) + (vLook * fDistance);

						XMStoreFloat3(&desc.pos, vSpawnPos);

						desc.pos.x += (i * 3);

						if (FAILED(m_pGameInstance->Add_GameObject(ETOI(m_eSceneType), TEXT("Prototype_GameObject_SkeletonVanguard"),
							ETOI(m_eSceneType), TEXT("Load_Layer"), &desc)))
							return;
					}
				}
				break;
			}
			case BOSS_CLONE:
			{
				if (m_pModelCom->Get_CurrentTrackPos() >= 163.6f && m_pModelCom->Get_CurrentTrackPos() <= 164.f)
				{
					NAMELESSKING_DESC desc{};
					_float3 svPos;
					XMStoreFloat3(&svPos, m_pTransformCom->Get_State(STATE::POSITION));
					
					desc.Shadow = 0;
					desc.Scenetype = m_eSceneType;
					int randNum = m_pGameInstance->Random(3, 7);

					for (size_t i = 0; i < randNum; i++)
					{
						desc.pos.x = svPos.x + m_pGameInstance->Random(-3, 3);
						desc.pos.y = svPos.y;
						desc.pos.z = svPos.z + m_pGameInstance->Random(-3, 3);

						if (FAILED(m_pGameInstance->Add_GameObject(ETOI(m_eSceneType), TEXT("Prototype_GameObject_NameLessKing"),
							ETOI(m_eSceneType), TEXT("Load_Layer"), &desc)))
							return;
					}
				}
				break;
			}
			}

			
		}

		if (true == m_pModelCom->Play_Animation(fTimeDelta))
		{
			if (attacking)
			{
				attacking = false;
				m_fStateTime = 0;
			}
		}
		
	}
	

	for (auto& iter : m_pColliderCom)
		iter->Update(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()));

	if (m_fCurrentHp <= 0)
	{
		Set_Dead();

		// 이거 하드코딩이라서 수정해야함 

		if (m_pHpBar != nullptr)
			dynamic_cast<CBossMark*>(m_pHpBar)->Set_OwnerDead();
		if (m_pHpBar1 != nullptr)
			dynamic_cast<CBossMark*>(m_pHpBar1)->Set_OwnerDead();
		 if (m_pHpBar2 != nullptr)
			dynamic_cast<CBossMark*>(m_pHpBar2)->Set_OwnerDead();
		if (m_pHpBar3 != nullptr)
			dynamic_cast<CBossMark*>(m_pHpBar3)->Set_OwnerDead();
		if (m_pHpBar4 != nullptr)
			dynamic_cast<CBossMark*>(m_pHpBar4)->Set_OwnerDead();
	}

}

void CNameLessKing::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
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
	if (m_bShadow != 0) 
	{
		for (auto& iter : m_pColliderCom)
			iter->Render();
	}
	//m_pNavigationCom->Render();
#endif // _DEBUG

		

	return S_OK;
}

HRESULT CNameLessKing::Ready_Components()
{
	if (m_bShadow != 0)
	{
		CBounding_AABB::BOUNDING_AABB_DESC AABBDesc;

		AABBDesc.vExtents = _float3(1.f, 1.f, 1.f);
		AABBDesc.vCenter = _float3(0.f, AABBDesc.vExtents.y, 0.f);
		AABBDesc.owner = this;

		if (FAILED(__super::Add_Component(ETOI(LEVEL::STATIC), TEXT("Prototype_Component_Collider_AABB"),
			TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom[0]), &AABBDesc)))
			return E_FAIL;

		CBounding_Sphere::BOUNDING_SPHERE_DESC SphereDesc{};
		SphereDesc.vCenter = _float3(0.f, SphereDesc.fRadius, 0.f);
		SphereDesc.fRadius = 20.0f;
		SphereDesc.owner = this;

		if (FAILED(__super::Add_Component(ETOI(LEVEL::STATIC), TEXT("Prototype_Component_Collider_Sphere"),
			TEXT("Com_Collider_Sphere"), reinterpret_cast<CComponent**>(&m_pColliderCom[1]), &SphereDesc)))
			return E_FAIL;
	}

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
		return E_FAIL;*/

	CJusinBox::JUSINBOX_DESC JusinBoxDesc{};
	JusinBoxDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
	JusinBoxDesc.pSocketMatrix = m_pModelCom->Get_BoneMatrixPtr("J_R_WeaponSocket");
	JusinBoxDesc.Scenetype = m_eSceneType;
	JusinBoxDesc.NumTexture = 16;
	JusinBoxDesc.state = JUSINSTATE::STAFF;

	if (FAILED(__super::Add_PartObject(ETOI(m_eSceneType), TEXT("Prototype_GameObject_JusinBox"),
		TEXT("Part_Box"), &JusinBoxDesc)))
		return E_FAIL;

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




_bool CNameLessKing::Intersect_ToPlayerSphere()
{
	CCollider* collider = dynamic_cast<CCollider*>(m_pGameInstance->Get_Component(TEXT("Prototype_GameObject_Player0"), m_eSceneType == GAMEPLAY ? TEXT("Layer_Clone") : TEXT("Load_Layer"), ETOI(m_eSceneType), TEXT("Com_Collider")));

	if (collider == nullptr) return false;

	if (m_pColliderCom[1]->Intersect(collider))
	{
		m_pTransformCom->LookAt(dynamic_cast<CTransform*>(collider->Get_Owner()->Get_Component(TEXT("Com_Transform")))->Get_State(STATE::POSITION));

		m_pColliderCom[1]->Set_isColl(true);

		return true;
	}
	else
	{
		m_pColliderCom[1]->Set_isColl(false);
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

	for (auto& iter : m_pColliderCom)
	Safe_Release(iter);

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
}
