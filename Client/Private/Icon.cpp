#include "Icon.h"
#include "GameInstance.h"
#include "Player.h"

CIcon::CIcon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUIObject{pDevice, pContext}
{
}

CIcon::CIcon(const CIcon& Prototype)
    :CUIObject(Prototype)
{
}

HRESULT CIcon::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CIcon::Initialize(void* pArg)
{
    ICON_DESC* pDesc = static_cast<ICON_DESC*>(pArg);
    m_iNumTexture = pDesc->NumTexture;
    m_pParentActive = pDesc->pParentActive;

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_Name = TEXT("Icon_" + pDesc->name);

    return S_OK;
}

void CIcon::Priority_Update(_float fTimeDelta)
{
}

void CIcon::Update(_float fTimeDelta)
{
    
    __super::Update_Transform();

}

void CIcon::Late_Update(_float fTimeDelta)
{
    
    if(*m_pParentActive)
    m_pGameInstance->Add_RenderGroup(RENDERGROUP::INVEN, this);
}

HRESULT CIcon::Render()
{
    if (*m_pParentActive)
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
    }

    return S_OK;
}

HRESULT CIcon::Ready_Components()
{

    if (FAILED(__super::Add_Component(STATIC, TEXT("Prototype_Component_Texture_InventroyIcon"),
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

CIcon* CIcon::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CIcon* pInstance = new CIcon(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CIcon");
        Safe_Release(pInstance);
    }
    return pInstance;
}


CGameObject* CIcon::Clone(void* pArg)
{
    CIcon* pInstance = new CIcon(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CHotbar");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CIcon::Free()
{
    __super::Free();

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pVIBufferCom);
}
