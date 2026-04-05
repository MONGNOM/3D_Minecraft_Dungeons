#include "DefalutBlock.h"
#include "GameInstance.h"

CDefalutBlock::CDefalutBlock(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CDefalutBlock::CDefalutBlock(const CDefalutBlock& Prototype)
	: CGameObject{ Prototype }

{
}

HRESULT CDefalutBlock::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CDefalutBlock::Initialize(void* pArg)
{
	m_eObjectType = OBJECTTYPE::ENVIRONMENT;

	BLOCK_DESC* Desc = static_cast<BLOCK_DESC*>(pArg);
	/* 백그라운드의 멤버를 채워넣어야한다면 여기서 채운다. */
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (Desc != nullptr)
	{
		m_fPos = Desc->pos;
		m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(Desc->pos.x, Desc->pos.y + 0.5f,  Desc->pos.z, 1.f));
	}
	// 설치 될 때 큐브의 크기 만큼 위로 올려줘야할텐데

	return S_OK;
}

void CDefalutBlock::Priority_Update(_float fTimeDelta)
{

}

void CDefalutBlock::Update(_float fTimeDelta)
{
	
}

void CDefalutBlock::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(RENDERGROUP::NONBLEND, this);
}

HRESULT CDefalutBlock::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(0)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Resources()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;


	return S_OK;
}

HRESULT CDefalutBlock::Ready_Components()
{
	if (FAILED(__super::Add_Component(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Texture_Sky"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Shader_VtxDefaultCube"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_VIBuffer_Cube"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CDefalutBlock::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_TransformMatrix(D3DTS::VIEW, m_pShaderCom, "g_ViewMatrix")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_TransformMatrix(D3DTS::PROJ, m_pShaderCom, "g_ProjMatrix")))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResourceView(m_pShaderCom, "g_Texture", m_iNumTexture)))
		return E_FAIL;

	return S_OK;
}

CDefalutBlock* CDefalutBlock::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CDefalutBlock* pInstance = new CDefalutBlock(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CDefalutBlock");
		Safe_Release(pInstance);
	}
	return pInstance;
}


CGameObject* CDefalutBlock::Clone(void* pArg)
{
	CDefalutBlock* pInstance = new CDefalutBlock(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CDefalutBlock");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CDefalutBlock::Free()
{
	__super::Free();


	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}
