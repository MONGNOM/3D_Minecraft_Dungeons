#include "Skeleton.h"

#include "HpBar.h"
#include "Body_Skeleton.h"
#include "GameInstance.h"
#include "Bow.h"
#include "DamageFont.h"

CSkeleton::CSkeleton(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CContainerObject{ pDevice, pContext }
{
}

CSkeleton::CSkeleton(const CSkeleton& Prototype)
	: CContainerObject{ Prototype }

{
}

HRESULT CSkeleton::Initialize_Prototype()
{
	return S_OK;
}



HRESULT CSkeleton::Initialize(void* pArg)
{
	CContainerObject::CONTAINEROBJECT_DESC* Desc = static_cast<CONTAINEROBJECT_DESC*>(pArg);

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
		m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(Desc->pos.x, Desc->pos.y, Desc->pos.z, 1.f));
		Desc->fSpeedPerSec = 10.f;
		Desc->fDegreePerSec = 180.f;
	}

	CHpBar::HPBAR_DESC desc{};
	desc.fSizeY = 11;
	desc.fSizeX = 98;
	desc.Scenetype = m_eSceneType;
	desc.iNumTexture = 0;
	desc.owner = this;
	
	if (FAILED(m_pGameInstance->Add_GameObject(m_eSceneType, TEXT("Prototype_GameObject_HpBar"), m_eObjectType, m_eSceneType == GAMEPLAY ? TEXT("Layer_Clone") : TEXT("Load_Layer"), &desc)))
	{
		MSG_BOX("스켈레톤 Hp바 안 만들어짐");
		return E_FAIL;
	}

	m_eObjectType = OBJECTTYPE::MONSTER;

	m_fMaxHp = 60;
	m_fCurrentHp = m_fMaxHp;
	m_bTakehit = false;
	return S_OK;
}

void CSkeleton::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CSkeleton::Update(_float fTimeDelta)
{
	if (m_fCurrentHp <= 0)
	{
		m_bOwner = false;
		dynamic_cast<CHpBar*>(m_pHpBar)->isDead();

		Set_Dead();
	}
	Intersect_ToPlayer();
	
	if (m_iState & SKELETONSTATE::ATTACK)
		m_iState ^= SKELETONSTATE::ATTACK;

	m_iState |= SKELETONSTATE::IDLE;

	m_pColliderCom->Update(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()));

	__super::Update(fTimeDelta);
}

void CSkeleton::Late_Update(_float fTimeDelta)
{

	__super::Late_Update(fTimeDelta);

	m_pGameInstance->Add_RenderGroup(RENDERGROUP::NONBLEND, this);
}

HRESULT CSkeleton::Render()
{
#ifdef _DEBUG
	m_pColliderCom->Render();
#endif 

	return S_OK;
}

void CSkeleton::Intersect_ToPlayer()
{

	CTransform* pPlayerTransform = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(TEXT("Prototype_GameObject_Player0"), m_eSceneType == GAMEPLAY ? TEXT("Layer_Clone") : TEXT("Load_Layer"), ETOI(m_eSceneType), TEXT("Com_Transform")));

	if (pPlayerTransform == nullptr) return;

	m_pTransformCom->LookAt(pPlayerTransform->Get_State(STATE::POSITION));

	//CCollider* collider = dynamic_cast<CCollider*>(m_pGameInstance->Get_Component(TEXT("Prototype_GameObject_Player0"), TEXT("Layer_Clone"), ETOI(m_eSceneType), TEXT("Com_Collider")));

	//if (collider == nullptr) return;

	//if (m_pColliderCom->Intersect(collider))
	//{
	//	collider->Get_Owner()->TakeHit(10);
	//	m_pColliderCom->Set_isColl(true);
	//	wcout << collider->Get_Owner()->Get_ObjectName() << "에게 피해를 입혔다" << endl;
	//}
	//else
	//{
	//	m_pColliderCom->Set_isColl(false);
	//}


	/*const list<CGameObject*>& object = m_pGameInstance->Get_LayerObjects(m_eSceneType, TEXT("Layer_Clone"));
	m_pColliderCom->Set_isColl(false);

	for (auto& iter : object)
	{
		CCollider* collider = dynamic_cast<CCollider*>(iter->Get_Component(TEXT("Com_Collider")));
		
		if (collider == nullptr || collider->Get_Owner() == this) continue;
		
		if (m_pColliderCom->Intersect(collider) && collider->Get_Owner()->Get_ObjectType() == OBJECTTYPE::PLAYER)
		{
			collider->Get_Owner()->TakeHit(10);
			m_pColliderCom->Set_isColl(true);
			wcout << collider->Get_Owner()->Get_ObjectName() << "에게 피해를 입혔다" << endl;
		}
	}*/
}

void CSkeleton::TakeHit(_uint damage)
{
	 m_fCurrentHp -= damage; 
	 m_bTakehit = true;
	 /*if (FAILED(m_pGameInstance->Add_Font(TEXT("Font_Default"), TEXT("../Bin/Resources/Fonts/158ex.SpriteFont"))))
		 return;*/

	 _vector vSkeletonPos = m_pTransformCom->Get_State(STATE::POSITION);

	 // 2. 머리 위 높이만 살짝 더해줍니다. (여전히 3D 월드 좌표입니다)
	 vSkeletonPos = XMVectorSetY(vSkeletonPos, XMVectorGetY(vSkeletonPos) + 3.f);
	 vSkeletonPos = XMVectorSetZ(vSkeletonPos, XMVectorGetZ(vSkeletonPos) + 0.3f);
	 CDamageFont::DAMAGEFONT_DESC desc{};
	 desc.fSizeY = 400;
	 desc.fSizeX = 400;
	 desc.Scenetype = m_eSceneType;
	 XMStoreFloat3(&desc.pos, vSkeletonPos);
	 desc.damage = damage;
	 //m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(fOrthoX, fOrthoY, BarPos.z, 1.f));

	 if (FAILED(m_pGameInstance->Add_GameObject(m_eSceneType, TEXT("Prototype_GameObject_DamageFont"), m_eObjectType, m_eSceneType == GAMEPLAY ? TEXT("Layer_Clone") : TEXT("Load_Layer"), &desc)))
	 {
		 MSG_BOX("스켈레톤 폰트 안 만들어짐");
		 return;
	 }
	 
}

HRESULT CSkeleton::Ready_Components()
{
	/*CNavigation::NAVIGATION_DESC NavigationDesc;
	NavigationDesc.iCurrentCellIndex = rand() % 100; 
	NavigationDesc.pTransform = m_pTransformCom;

	if (FAILED(__super::Add_Component(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Navigation"),
		TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom), &NavigationDesc)))
		return E_FAIL;*/

	
	CBounding_AABB::BOUNDING_AABB_DESC AABBDesc;

	AABBDesc.vExtents = _float3(0.5f, 1.f, 0.5f);
	AABBDesc.vCenter = _float3(0.f, AABBDesc.vExtents.y, 0.f);
	AABBDesc.owner = this;

	if (FAILED(__super::Add_Component(ETOI(LEVEL::STATIC), TEXT("Prototype_Component_Collider_AABB"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &AABBDesc)))
		return E_FAIL;


	return S_OK;
}

HRESULT CSkeleton::Ready_PartObjects()
{
	CBody_Skeleton::CBODY_SKELETONDESC		BodyDesc{};
	BodyDesc.pParentState = &m_iState;
	BodyDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
	BodyDesc.Scenetype = m_eSceneType;

	if (FAILED(__super::Add_PartObject(ETOI(m_eSceneType), TEXT("Prototype_GameObject_Body_Skeleton"),
		TEXT("Part_Body"), &BodyDesc)))
		return E_FAIL;

	pBody = dynamic_cast<CBody_Skeleton*>(m_PartObjects[TEXT("Part_Body")]);
	if (nullptr == pBody)
		return E_FAIL;


	CBow::BOW_DESC				BowDesc{};
	BowDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
	BowDesc.pSocketMatrix = pBody->Get_SocketBoneMatrixPtr("J_L_WeaponSocket");
	BowDesc.Scenetype = m_eSceneType;
	BowDesc.shot = Get_FakeBool();

	if (FAILED(__super::Add_PartObject(ETOI(m_eSceneType), TEXT("Prototype_GameObject_Bow"),
		TEXT("Part_Bow"), &BowDesc)))
		return E_FAIL;

	// 활 달아 줍시다 플레이어도 달아야함

	/*CWeapon::WEAPON_DESC				WeaponDesc{};
	WeaponDesc.pParentState = &m_iState;
	WeaponDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
	WeaponDesc.pSocketMatrix = pBody->Get_SocketBoneMatrixPtr("SWORD");

	if (FAILED(__super::Add_PartObject(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Weapon"),
		TEXT("Part_Weapon"), &WeaponDesc)))
		return E_FAIL;*/

	return S_OK;
}


CSkeleton* CSkeleton::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSkeleton* pInstance = new CSkeleton(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CSkeleton");
		Safe_Release(pInstance);
	}
	return pInstance;
}


CGameObject* CSkeleton::Clone(void* pArg)
{
	CSkeleton* pInstance = new CSkeleton(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CSkeleton");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CSkeleton::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pNavigationCom);

}
