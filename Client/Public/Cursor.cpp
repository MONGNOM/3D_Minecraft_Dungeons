#include "Cursor.h"
#include "GameInstance.h"

CCursor::CCursor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUIObject{pDevice, pContext}
{
}

CCursor::CCursor(const CCursor& Prototype)
    : CUIObject(Prototype)
{
}

HRESULT CCursor::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CCursor::Initialize(void* pArg)
{
	CUIObject::tagUIObjectDesc			Desc{};

	
	Desc.fSizeX = 64;
	Desc.fSizeY = 64;

	ShowCursor(FALSE);


	/* 백그라운드의 멤버를 채워넣어야한다면 여기서 채운다. */
	if (FAILED(__super::Initialize(&Desc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CCursor::Priority_Update(_float fTimeDelta)
{
}

void CCursor::Update(_float fTimeDelta)
{
	__super::Update_Transform();

	::POINT ptMouse;

	GetCursorPos(&ptMouse);

	ScreenToClient(g_hWnd, &ptMouse);
	
	_float fX = (ptMouse.x - (g_iWinSizeX * 0.5f)) + (64 * 0.5f) - 32 * 0.5f;
	_float fY = (-ptMouse.y + (g_iWinSizeY * 0.5f)) - (64 * 0.5f) + 32 * 0.5f;

	m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(fX, fY, 0.f, 1.f));

}

void CCursor::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(RENDERGROUP::CURSOR, this);
}

HRESULT CCursor::Render()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(__super::Bind_ShaderResource(m_pShaderCom, "g_ViewMatrix", D3DTS::VIEW)))
		return E_FAIL;

	if (FAILED(__super::Bind_ShaderResource(m_pShaderCom, "g_ProjMatrix", D3DTS::PROJ)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResourceView(m_pShaderCom, "g_Texture", m_iNumTexture)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(0)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Resources()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;


	return S_OK;
}

HRESULT CCursor::Ready_Components()
{
	if (FAILED(__super::Add_Component(ETOI(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Cursor"),
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

CCursor* CCursor::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCursor* pInstance = new CCursor(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CBackGround");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CCursor::Clone(void* pArg)
{
	CCursor* pInstance = new CCursor(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CBackGround");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCursor::Free()
{
	__super::Free();


	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}
