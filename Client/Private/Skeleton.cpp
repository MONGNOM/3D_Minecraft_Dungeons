#include "Skeleton.h"

//#include "Weapon.h"
#include "Body_Skeleton.h"
#include "GameInstance.h"

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

	if (Desc != nullptr)
	{
		//m_fPos = Desc->pos;
		//m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(Desc->pos.x, Desc->pos.y, Desc->pos.z, 1.f));
		Desc->fSpeedPerSec = 10.f;
		Desc->fDegreePerSec = 180.f;
	}


	/* 백그라운드의 멤버를 채워넣어야한다면 여기서 채운다. */
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_PartObjects()))
		return E_FAIL;

	m_eObjectType = OBJECTTYPE::MONSTER;

	return S_OK;
}

void CSkeleton::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CSkeleton::Update(_float fTimeDelta)
{

	/*if (GetKeyState(VK_DOWN) & 0x8000)
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

	if (GetKeyState(VK_UP) & 0x8000)
	{
		m_pTransformCom->Go_Straight(fTimeDelta);

		if (m_iState & SKELETONSTATE::IDLE)
			m_iState ^= SKELETONSTATE::IDLE;

		m_iState |= SKELETONSTATE::WALK;
	}
	else
	{
		if (m_iState & SKELETONSTATE::WALK)
			m_iState ^= SKELETONSTATE::WALK;

		m_iState |= SKELETONSTATE::IDLE;
	}*/

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

HRESULT CSkeleton::Ready_Components()
{
	CNavigation::NAVIGATION_DESC NavigationDesc;
	NavigationDesc.iCurrentCellIndex = rand() % 100; 
	NavigationDesc.pTransform = m_pTransformCom;

	if (FAILED(__super::Add_Component(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Navigation"),
		TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom), &NavigationDesc)))
		return E_FAIL;

	CBounding_AABB::BOUNDING_AABB_DESC AABBDesc;

	AABBDesc.vExtents = _float3(0.5f, 1.f, 0.5f);
	AABBDesc.vCenter = _float3(0.f, AABBDesc.vExtents.y, 0.f);


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


	if (FAILED(__super::Add_PartObject(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Body_Skeleton"),
		TEXT("Part_Body"), &BodyDesc)))
		return E_FAIL;

	CBody_Skeleton* pBody = dynamic_cast<CBody_Skeleton*>(m_PartObjects[TEXT("Part_Body")]);
	if (nullptr == pBody)
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
