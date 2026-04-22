#include "Player.h"

#include "Weapon.h"
#include "Body_Player.h"
#include "GameInstance.h"
#include "Bow.h"
#include "Inventory.h"

CPlayer::CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CContainerObject{ pDevice, pContext }
{
}

CPlayer::CPlayer(const CPlayer& Prototype)
	: CContainerObject{ Prototype }

{
}

HRESULT CPlayer::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPlayer::Initialize(void* pArg)
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

	m_eObjectType = OBJECTTYPE::PLAYER;

	//네비게이션 잠깐 꺼둠

	m_fMaxHp = 200;
	m_fCurrentHp = m_fMaxHp;

	
	
	return S_OK;
}

void CPlayer::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CPlayer::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);

	if (m_fCurrentHp <= 0)
		Set_Dead();

	if (PotioncoolTiem > 0)
	{
		PotioncoolTiem -= fTimeDelta;
		
	}
	if (PotioncoolTiem <= 0)
		m_bPotion = true;

	if (jumpCoolTime > 0)
	{
		jumpCoolTime -= fTimeDelta;

	}

	if (m_pGameInstance->Get_DIKeyDown(DIK_I))
	{
		CInventory* inven = dynamic_cast<CInventory*>(m_pGameInstance->Get_GameObject(TEXT("Inventory"), TEXT("Layer_UI"), ETOI(LEVEL::STATIC)));

		if (inven != nullptr)
		{
			inven->InvenToggle(); // 인벤토리 껐다 켜기
		}
	}
	

	//Intersect_ToMonster();
	

	if (m_bRoll)	m_pTransformCom->Go_Roll(fTimeDelta, 10.f);

	bool bIsActionState = (state == PLAYERSTATE::HEAL || state == PLAYERSTATE::FAILING || state == PLAYERSTATE::BOW || state == PLAYERSTATE::ATTACK);

	if (bIsActionState)
	{
		// 조작 불가 상태

		if (pBody->IsAnimationFinished())
			state = PLAYERSTATE::IDLE;
	}
	else
	{
		// 조작상태
		
		if (m_pGameInstance->Get_DIMouseDown(DIMB::RBUTTON)) // 활 쏘기
		{
			state = PLAYERSTATE::BOW;
		}
		else if (m_pGameInstance->Get_DIMouseDown(DIMB::LBUTTON)) // 칼 공격
		{
			state = PLAYERSTATE::ATTACK;
		}
		
	 	else if (m_pGameInstance->Get_DIKeyDown(DIK_SPACE)) // 구르기
		{
			if (!m_bRoll && jumpCoolTime <= 0)
			{
				jumpCoolTime = 2;
				m_bRoll = true;
				state = PLAYERSTATE::FAILING;
			}
		}
		else if (m_pGameInstance->Get_DIKeyDown(DIK_E)) // 힐
		{
			if (m_bPotion)
			{
				state = PLAYERSTATE::HEAL;
				
				m_bPotion = false;
				PotioncoolTiem = maxcooltiem;

				if (m_fCurrentHp += 100 > m_fMaxHp)
					m_fCurrentHp = m_fMaxHp;
				else
					m_fCurrentHp += 100;

				wcout << "체력회복 " << endl;
				wcout << "현재 체력 : " << m_fCurrentHp << endl;
			}
		}
		else
		{
			state = PLAYERSTATE::IDLE;
		}

		if (m_bRoll) return;

		if (GetKeyState(VK_UP) & 0x8000)
		{
			m_pTransformCom->Go_Straight(fTimeDelta, m_pNavigationCom);
			//m_pTransformCom->Go_Straight(fTimeDelta);
			state = PLAYERSTATE::WALK;
		}
		if (GetKeyState(VK_DOWN) & 0x8000)
		{
			m_pTransformCom->Go_Backward(fTimeDelta);
			state = PLAYERSTATE::WALK;
		}
		if (GetKeyState(VK_LEFT) & 0x8000)
		{
			m_pTransformCom->Turn(XMVectorSet(0.f,1.f,0.f,0.f), fTimeDelta * -1.f);
		}
		if (GetKeyState(VK_RIGHT) & 0x8000)
		{
			m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta );
		}
	
		if (m_pGameInstance->Get_DIKeyDown(DIK_K)) // 구르기
		{
			m_fCurrentHp -= 10;
			wcout << "체력이 깎임 -> 현재 체력 : " << m_fCurrentHp << endl;
		}

		
		
	}


	m_pNavigationCom->Compute_Height(m_pTransformCom);
	
	m_pColliderCom->Update(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()));
	
}

void CPlayer::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);

	m_pGameInstance->Add_RenderGroup(RENDERGROUP::NONBLEND, this);
}

void CPlayer::TakeHit(_uint damage)
{
	m_fCurrentHp -= damage;
	m_bTakehit = true;
}


HRESULT CPlayer::Render()
{
#ifdef _DEBUG
	m_pColliderCom->Render();
	m_pNavigationCom->Render();
#endif // _DEBUG


	return S_OK;
}

HRESULT CPlayer::Ready_Components()
{
	CBounding_AABB::BOUNDING_AABB_DESC AABBDesc;

	AABBDesc.vExtents = _float3(0.5f, 1.f, 0.5f);
	AABBDesc.vCenter = _float3(0.f, AABBDesc.vExtents.y, 0.f);
	AABBDesc.owner = this;

	if (FAILED(__super::Add_Component(ETOI(LEVEL::STATIC), TEXT("Prototype_Component_Collider_AABB"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &AABBDesc)))
		return E_FAIL;

	CNavigation::NAVIGATION_DESC		NavigationDesc{};
	NavigationDesc.iCurrentCellIndex = 1;
	NavigationDesc.pTransform = m_pTransformCom;
	
	if (FAILED(__super::Add_Component(ETOI(m_eSceneType), TEXT("Prototype_Component_Navigation"),
		TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom), &NavigationDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer::Ready_PartObjects()
{
	CBody_Player::BODY_PLAYER_DESC		BodyDesc{};
	BodyDesc.pParentState = &state;
	BodyDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
	BodyDesc.Scenetype = m_eSceneType;
	BodyDesc.pPlayer = this;
	//BodyDesc.pWeapon = 


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
	WeaponDesc.shot = pBody->IsShot();
	WeaponDesc.onColiider = pBody->IsAttack();

	if (FAILED(__super::Add_PartObject(ETOI(m_eSceneType), TEXT("Prototype_GameObject_Weapon"),
		TEXT("Part_Weapon"), &WeaponDesc)))
		return E_FAIL;

	CBow::BOW_DESC				BowDesc{};
	BowDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
	BowDesc.pSocketMatrix = pBody->Get_SocketBoneMatrixPtr("J_L_Weapon_Socket");
	BowDesc.Scenetype = m_eSceneType;
	BowDesc.shot = pBody->IsShot();

	if (FAILED(__super::Add_PartObject(ETOI(m_eSceneType), TEXT("Prototype_GameObject_Bow"),
		TEXT("Part_Bow"), &BowDesc)))
		return E_FAIL;

	return S_OK;
}

void CPlayer::Intersect_ToMonster()
{
	//if (nullptr == object)
	//	return;

	//// 콜라이더 색깔이 이상하ㅔㄱ 바뀜 이거 체크 해야할듯

	//for (auto& iter : *object)
	//{
	//	CCollider* collider = dynamic_cast<CCollider*>(iter->Get_Component(TEXT("Com_Collider")));

	//	if (collider == nullptr || collider->Get_Owner() == this) continue;

	//	if (m_pColliderCom->Intersect(collider) && collider->Get_Owner()->Get_ObjectType() == OBJECTTYPE::MONSTER)
	//	{
	//		collider->Get_Owner()->TakeHit();
	//		wcout << collider->Get_Owner()->Get_ObjectName() << "에게 피해를 입혔다" << endl;
	//	}
	//}
}


CPlayer* CPlayer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPlayer* pInstance = new CPlayer(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CPlayer");
		Safe_Release(pInstance);
	}
	return pInstance;
}


CGameObject* CPlayer::Clone(void* pArg)
{
	CPlayer* pInstance = new CPlayer(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CPlayer");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPlayer::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pNavigationCom);
}
