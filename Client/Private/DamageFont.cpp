#include "DamageFont.h"
#include "GameInstance.h"

CDamageFont::CDamageFont(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUIObject{pDevice, pContext}
{
}

CDamageFont::CDamageFont(const CDamageFont& Prototype)
    :CUIObject(Prototype)
{
}

HRESULT CDamageFont::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CDamageFont::Initialize(void* pArg)
{
    DAMAGEFONT_DESC* pDesc = static_cast<DAMAGEFONT_DESC*>(pArg);
    m_fSizeX = pDesc->fSizeX;
    m_fSizeY = pDesc->fSizeY;
    m_fPos = pDesc->pos;
    m_iDamage = pDesc->damage;

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;
   
    return S_OK;
}

void CDamageFont::Priority_Update(_float fTimeDelta)
{
}

void CDamageFont::Update(_float fTimeDelta)
{
    m_fDeleteTime += fTimeDelta;
    if (m_fDeleteTime >= 1.f)
    {
        m_fDeleteTime = 0;
        Set_Dead();
    }
    __super::Update_Transform();

    m_fPos.y += fTimeDelta * 2.0f;

    m_pTransformCom->Set_State(STATE::POSITION, XMLoadFloat3(&m_fPos));

}

void CDamageFont::Late_Update(_float fTimeDelta)
{
    m_pGameInstance->Add_RenderGroup(RENDERGROUP::UI, this);
}

HRESULT CDamageFont::Render()
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
    // 1. 현재 내 오브젝트의 3D 위치를 가져옴
    _vector vWorldPos = m_pTransformCom->Get_State(STATE::POSITION);

    // 2. 그 위치를 매 프레임 카메라 기준 2D 화면 좌표로 투영 (Project)
    _vector vScreenPos = XMVector3Project(vWorldPos, 0, 0, g_iWinSizeX, g_iWinSizeY, 0.0f, 1.0f, XMLoadFloat4x4(m_pGameInstance->Get_Transform(D3DTS::PROJ)),
        XMLoadFloat4x4(m_pGameInstance->Get_Transform(D3DTS::VIEW)),
        XMMatrixIdentity());

    // 3. 투영된 2D 좌표를 추출
    _float3 vResult;
    XMStoreFloat3(&vResult, vScreenPos);

    // 4. 비로소 이 화면 좌표로 Draw_Font를 호출!
    m_pGameInstance->Draw_Font(TEXT("Font_Damage"), to_wstring(m_iDamage).data(), _float2(vResult.x, vResult.y));
    return S_OK;
}

HRESULT CDamageFont::Ready_Components()
{
    if (FAILED(__super::Add_Component(m_eSceneType, TEXT("Prototype_Component_Texture_Damage"),
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

CDamageFont* CDamageFont::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CDamageFont* pInstance = new CDamageFont(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CDamageFont");
        Safe_Release(pInstance);
    }
    return pInstance;
}


CGameObject* CDamageFont::Clone(void* pArg)
{
    CDamageFont* pInstance = new CDamageFont(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CDamageFont");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CDamageFont::Free()
{
    __super::Free();

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pTextureCom);
}
