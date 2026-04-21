#include "Weapon.h"
#include "GameInstance.h"

#include "Player.h"

CWeapon::CWeapon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPartObject{ pDevice, pContext }, m_iSwordDamage{ 0 }
{
}

CWeapon::CWeapon(const CWeapon& Prototype)
	: CPartObject{ Prototype }, m_iSwordDamage{ Prototype.m_iSwordDamage}
{
}

HRESULT CWeapon::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CWeapon::Initialize(void* pArg)
{
	auto	pDesc = static_cast<WEAPON_DESC*>(pArg);

	m_pSocketMatrix = pDesc->pSocketMatrix;

	m_pShot = pDesc->shot;

	m_pOnColiider = pDesc->onColiider;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_Name = TEXT("Weapon");
	/*m_pTransformCom->SetUp_Scale(0.1f, 0.1f, 0.1f);
	m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), 90.f);
	m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(0.8f, 0.f, 0.f, 1.f));

	/*m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(
		m_pGameInstance->Random(0.f, 10.f),
		2.f,
		m_pGameInstance->Random(0.f, 10.f),
		1.f
	));*/
	m_pColliderCom->Set_isColl(false);

	
	
	if (m_eSceneType == GAMEPLAY)
		object = &m_pGameInstance->Get_LayerObjects(m_eSceneType, TEXT("Layer_Clone"));
	else
		object = &m_pGameInstance->Get_LayerObjects(m_eSceneType, TEXT("Load_Layer"));

	// 2. 포인터가 유효한지 먼저 검사하고, 유효하다면 비어있는지(empty) 확인합니다.
	if (object == nullptr || object->empty())
	{
		// 레이어 자체가 없거나, 레이어는 있는데 안에 든 오브젝트가 0개일 때
		object = nullptr;

		// 여기에 중단점(F9 키)을 걸어서 이 안으로 잘 들어오는지 확인해 보세요!
		int a = 10;
	}
	
	return S_OK;
}

void CWeapon::Priority_Update(_float fTimeDelta)
{
}

void CWeapon::Update(_float fTimeDelta)
{
	
	_matrix		SocketMatrix = XMLoadFloat4x4(m_pSocketMatrix);

	for (size_t i = 0; i < 3; i++)
		SocketMatrix.r[i] = XMVector3Normalize(SocketMatrix.r[i]);

	Intersect_ToMonster();

	if (*m_pOnColiider)
	{
		m_pColliderCom->SetActive_Collider(true);
	}
	else
		m_pColliderCom->SetActive_Collider(false);

	

	Update_CombinedWorldMatrix(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()) * SocketMatrix);

	m_pColliderCom->Update(XMLoadFloat4x4(&m_CombinedWorldMatrix));

}

void CWeapon::Late_Update(_float fTimeDelta)
{
	if (!*m_pShot)
	m_pGameInstance->Add_RenderGroup(RENDERGROUP::NONBLEND, this);
}

HRESULT CWeapon::Render()
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

void CWeapon::Intersect_ToMonster()
{
	if (nullptr == object ||  0 >= object->size() )
		return;
	

	// 콜라이더 색깔이 이상하ㅔㄱ 바뀜 이거 체크 해야할듯
	m_iSwordDamage = m_pGameInstance->Random(100, 400);

	for (auto& iter : *object)
	{
		CCollider* collider = dynamic_cast<CCollider*>(iter->Get_Component(TEXT("Com_Collider")));
		
		if (collider == nullptr || collider->Get_Owner() == this) continue;

		if (m_pColliderCom->Intersect(collider) && collider->Get_Owner()->Get_ObjectType() == OBJECTTYPE::MONSTER)
		{
			
			collider->Get_Owner()->TakeHit(m_iSwordDamage);

			m_pColliderCom->Set_isColl(true);
			collider->Set_isColl(true);
			wcout << collider->Get_Owner()->Get_ObjectName() << m_iSwordDamage <<" 에게 피해를 입혔다" << endl;
			//DecreaseHp(m_iSwordDamage);
		
			// m_pColliderCom->SetActive_Collider(false);
			// 애니메이션 때릴떄 콜라이더 떄리는거 3번만 켜야하는데 이걸 어떻게 3번만 켜주냐 그것도 타이밍 맞춰서 시간으로 노가다 해야하나?
			// 특정 프레임에만 콜라이더 키게 할 수 있나
			// 
		}
	}

}

HRESULT CWeapon::Ready_Components()
{

	if (FAILED(__super::Add_Component(ETOI(m_eSceneType), TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(ETOI(m_eSceneType), TEXT("Prototype_Component_Model_Sword"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;


	CBounding_OBB::BOUNDING_OBB_DESC Desc{};
	Desc.vCenter = _float3(0.f, Desc.vExtents.y, 1.5f);
	Desc.vExtents = _float3(0.5f,1.5f,1.5f);
	Desc.vRadians = _float3(0.f, XMConvertToRadians(0.f), 0.f);
	Desc.owner = this;

	if (FAILED(__super::Add_Component(ETOI(LEVEL::STATIC), TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &Desc)))
		return E_FAIL;

	m_pColliderCom->SetActive_Collider(false);

	return S_OK;
}

HRESULT CWeapon::Bind_ShaderResources()
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

CWeapon* CWeapon::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CWeapon* pInstance = new CWeapon(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CBody_Player");
		Safe_Release(pInstance);
	}
	return pInstance;
}


CGameObject* CWeapon::Clone(void* pArg)
{
	CWeapon* pInstance = new CWeapon(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CBody_Player");
		Safe_Release(pInstance);
	}
	return pInstance;
}


void CWeapon::Free()
{
	__super::Free();
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pModelCom);

}
