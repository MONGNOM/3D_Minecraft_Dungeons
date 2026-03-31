#include "Player.h"

#include "Weapon.h"
#include "Body_Player.h"
#include "GameInstance.h"

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

	return S_OK;
}

void CPlayer::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CPlayer::Update(_float fTimeDelta)
{

	if (GetKeyState(VK_DOWN) & 0x8000)
	{
		m_pTransformCom->Go_Backward(fTimeDelta);
	}

	if (GetKeyState(VK_LEFT) & 0x8000)
	{
		m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * -1.f);
	}

	if (GetKeyState(VK_RIGHT) & 0x8000)
	{

		m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta);
	}
	if (m_pGameInstance->Get_DIMouseDown(DIMB::RBUTTON)) // 활 쏘기
	{
		state = PLAYERSTATE::BOW;
	}
	else if (m_pGameInstance->Get_DIMouseDown(DIMB::LBUTTON)) // 칼 공격
	{
		state = PLAYERSTATE::ATTACK;
	}
	else if (GetKeyState(VK_UP) & 0x8000)
	{
		m_pTransformCom->Go_Straight(fTimeDelta, m_pNavigationCom);

		state = PLAYERSTATE::WALK;
	}
	else if (m_pGameInstance->Get_DIKeyDown(DIK_SPACE)) // 구르는 애니메이션
	{
		state = PLAYERSTATE::FAILING;
	}
	else if (m_pGameInstance->Get_DIKeyDown(DIK_R)) // 포션 마시는 애니메이션
	{
		state = PLAYERSTATE::HEAL;
	}
	else
	{
	//	끝나면 해줘야하늗네 애니메이션이 진행이 끝나면 이거 켜ㅑ줘야하는데 이거 근데 바디에서 해주잖아 애니메이션 판단이;
	// 	state = PLAYERSTATE::IDLE;
	}

	m_pNavigationCom->Compute_Height(m_pTransformCom);
	
	m_pColliderCom->Update(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()));
	
	__super::Update(fTimeDelta);
}

void CPlayer::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);

	m_pGameInstance->Add_RenderGroup(RENDERGROUP::NONBLEND, this);
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


	if (FAILED(__super::Add_Component(ETOI(LEVEL::STATIC), TEXT("Prototype_Component_Collider_AABB"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &AABBDesc)))
		return E_FAIL;

	CNavigation::NAVIGATION_DESC		NavigationDesc{};
	NavigationDesc.iCurrentCellIndex = 1;
	NavigationDesc.pTransform = m_pTransformCom;

	if (FAILED(__super::Add_Component(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Navigation"),
		TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom), &NavigationDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer::Ready_PartObjects()
{
	CBody_Player::BODY_PLAYER_DESC		BodyDesc{};
	BodyDesc.pParentState = &state;
	BodyDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();

	if (FAILED(__super::Add_PartObject(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Body_Player"),
		TEXT("Part_Body"), &BodyDesc)))
		return E_FAIL;

	CBody_Player* pBody = dynamic_cast<CBody_Player*>(m_PartObjects[TEXT("Part_Body")]);
	if (nullptr == pBody)
		return E_FAIL;

	CWeapon::WEAPON_DESC				WeaponDesc{};
	WeaponDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
	WeaponDesc.pSocketMatrix = pBody->Get_SocketBoneMatrixPtr("J_R_Weapon_Socket");

	if (FAILED(__super::Add_PartObject(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Weapon"),
		TEXT("Part_Weapon"), &WeaponDesc)))
		return E_FAIL;

	return S_OK;
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
