#include "HotBar.h"
#include "GameInstance.h"

CHotBar::CHotBar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUIObject{pDevice, pContext}
{
}

CHotBar::CHotBar(const CHotBar& Prototype)
    :CUIObject(Prototype)
{
}

HRESULT CHotBar::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CHotBar::Initialize(void* pArg)
{
    HOTBAR_DESC* pDesc = static_cast<HOTBAR_DESC*>(pArg);
    m_iNumTexture = pDesc->iNumTexture;

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    return S_OK;
}

void CHotBar::Priority_Update(_float fTimeDelta)
{
}

void CHotBar::Update(_float fTimeDelta)
{
    __super::Update_Transform();
}

void CHotBar::Late_Update(_float fTimeDelta)
{
    m_pGameInstance->Add_RenderGroup(RENDERGROUP::UI, this);
}

HRESULT CHotBar::Render()
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

HRESULT CHotBar::Ready_Components()
{
    if (FAILED(__super::Add_Component(ETOI(LEVEL::DUNGEON), TEXT("Prototype_Component_Texture_HotBar"),
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

CHotBar* CHotBar::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CHotBar* pInstance = new CHotBar(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CHotBar");
        Safe_Release(pInstance);
    }
    return pInstance;
}


CGameObject* CHotBar::Clone(void* pArg)
{
    CHotBar* pInstance = new CHotBar(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CHotbar");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CHotBar::Free()
{
    __super::Free();

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pVIBufferCom);
}
