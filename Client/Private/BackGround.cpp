#include "BackGround.h"
#include "GameInstance.h"

CBackGround::CBackGround(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIObject { pDevice, pContext }
{
}

CBackGround::CBackGround(const CBackGround& Prototype)
	: CUIObject ( Prototype )

{
}

HRESULT CBackGround::Initialize_Prototype()
{
	


 	return S_OK;
}

HRESULT CBackGround::Initialize(void* pArg)
{
	BACKGROUND_DESC			Desc{};

	Desc.fDegreePerSec = 90.f;
	Desc.fSpeedPerSec = 1.f;
	Desc.fX = g_iWinSizeX * 0.5f;
	Desc.fY = g_iWinSizeY * 0.5f;
	Desc.fSizeX = g_iWinSizeX;
	Desc.fSizeY = g_iWinSizeY;

		
	/* 백그라운드의 멤버를 채워넣어야한다면 여기서 채운다. */
	if (FAILED(__super::Initialize(&Desc)))
		return E_FAIL;
	
	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CBackGround::Priority_Update(_float fTimeDelta)
{
	int a = 10;
}

void CBackGround::Update(_float fTimeDelta)
{
	 
	//m_fX += 10.f * fTimeDelta;

	__super::Update_Transform();
}

void CBackGround::Late_Update(_float fTimeDelta)
{
 	m_pGameInstance->Add_RenderGroup(RENDERGROUP::UI, this);
}

HRESULT CBackGround::Render()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(__super::Bind_ShaderResource(m_pShaderCom, "g_ViewMatrix", D3DTS::VIEW)))
		return E_FAIL;

	if (FAILED(__super::Bind_ShaderResource(m_pShaderCom, "g_ProjMatrix", D3DTS::PROJ)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResourceView(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(0)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Resources()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;


	return S_OK;
}

// 로딩백그라운드 하나 만들까 
HRESULT CBackGround::Ready_Components()
{
	
	if (FAILED(__super::Add_Component(ETOI(LEVEL::LOGO), TEXT("Prototype_Component_Texture_Title"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(ETOI(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxTex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(ETOI(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CBackGround::Switch_Components()
{
	return E_NOTIMPL;
}

CBackGround* CBackGround::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBackGround* pInstance = new CBackGround(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CBackGround");
		Safe_Release(pInstance);
	}
	return pInstance;
}


CGameObject* CBackGround::Clone(void* pArg)
{
	CBackGround* pInstance = new CBackGround(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CBackGround");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBackGround::Free()
{
	__super::Free();

	
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}
