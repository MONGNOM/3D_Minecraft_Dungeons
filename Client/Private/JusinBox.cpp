#include "JusinBox.h"
#include "GameInstance.h"
#include "ImGui_Manager.h"

CJusinBox::CJusinBox(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPartObject{ pDevice, pContext }
{
}

CJusinBox::CJusinBox(const CJusinBox& Prototype)
	: CPartObject{ Prototype }

{
}

HRESULT CJusinBox::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CJusinBox::Initialize(void* pArg)
{
	auto	pDesc = static_cast<JUSINBOX_DESC*>(pArg);

	m_pSocketMatrix = pDesc->pSocketMatrix;
	/* 백그라운드의 멤버를 채워넣어야한다면 여기서 채운다. */
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_iBoXDamage = 10;

	m_Name = TEXT("JusinBoX");

	return S_OK;
}

void CJusinBox::Priority_Update(_float fTimeDelta)
{

}

_bool CJusinBox::Intersect_ToPlayer()
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

void CJusinBox::Update(_float fTimeDelta)
{
	_matrix		SocketMatrix = XMLoadFloat4x4(m_pSocketMatrix);

	for (size_t i = 0; i < 3; i++)
		SocketMatrix.r[i] = XMVector3Normalize(SocketMatrix.r[i]);
	
	// 일단 지팡이 위에 하나 날릴거 하나 더 만들어
	
	if (Intersect_ToPlayer())
	{
		//attack재생
		//if (*m_pParentState & CSkeleton::SKELETONSTATE::ATTACK)
	}
	else
	{
		//if (*m_pParentState & CSkeleton::SKELETONSTATE::IDLE)
		//idle
	}


	Update_CombinedWorldMatrix(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()) * SocketMatrix);

	m_pColliderCom->Update(XMLoadFloat4x4(&m_CombinedWorldMatrix));
}

void CJusinBox::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(RENDERGROUP::NONBLEND, this);
}

HRESULT CJusinBox::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(0)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Resources()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	m_pColliderCom->Render();

	return S_OK;
}

HRESULT CJusinBox::Ready_Components()
{
	if (FAILED(__super::Add_Component(ETOI(m_eSceneType), TEXT("Prototype_Component_Texture_Block"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(ETOI(m_eSceneType), TEXT("Prototype_Component_Shader_VtxDefaultCube"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(ETOI(m_eSceneType), TEXT("Prototype_Component_VIBuffer_Cube"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	CBounding_AABB::BOUNDING_AABB_DESC Desc{};
	Desc.vCenter = _float3(0.f, Desc.vExtents.y, 0.f);
	Desc.vExtents = _float3(0.5f, 0.5f, 0.5f);
	Desc.owner = this;

	if (FAILED(__super::Add_Component(ETOI(LEVEL::STATIC), TEXT("Prototype_Component_Collider_AABB"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &Desc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CJusinBox::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_TransformMatrix(D3DTS::VIEW, m_pShaderCom, "g_ViewMatrix")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_TransformMatrix(D3DTS::PROJ, m_pShaderCom, "g_ProjMatrix")))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResourceView(m_pShaderCom, "g_Texture", 17)))
		return E_FAIL;

	return S_OK;
}

CJusinBox* CJusinBox::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CJusinBox* pInstance = new CJusinBox(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CJusinBox");
		Safe_Release(pInstance);
	}
	return pInstance;
}



CGameObject* CJusinBox::Clone(void* pArg)
{
	CJusinBox* pInstance = new CJusinBox(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CJusinBox");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CJusinBox::Free()
{
	__super::Free();


	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pVIBufferCom);
}
