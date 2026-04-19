#include "BossMark.h"
#include "GameInstance.h"
#include "NameLessKing.h"

CBossMark::CBossMark(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUIObject{pDevice, pContext}
{
}

CBossMark::CBossMark(const CBossMark& Prototype)
    :CUIObject(Prototype)
{
}

HRESULT CBossMark::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CBossMark::Initialize(void* pArg)
{
    BOSSMARK_DESC* pDesc = static_cast<BOSSMARK_DESC*>(pArg);
    m_fSizeX = pDesc->fSizeX;
    m_fSizeY = pDesc->fSizeY;
    m_fOriginalSizeX = pDesc->fSizeX;
    m_fX = pDesc->fX;
    m_fY = pDesc->fY;
    m_fPos = pDesc->pos;
    m_iNumTexture = pDesc->iNumTexture;
    m_eSceneType = pDesc->Scenetype;
    m_fOriginalX = m_fX;
   
    if (m_iNumTexture == 0)
    {
        m_pOwner = pDesc->owner;
    }

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    return S_OK;
    // m_iNumTexture 0 이면 체력임
}

void CBossMark::Priority_Update(_float fTimeDelta)
{
}

void CBossMark::Update(_float fTimeDelta)
{
    __super::Update_Transform();

    if (m_iNumTexture == 0)
    {
        CNameLessKing* pNameLessKing = dynamic_cast<CNameLessKing*>(m_pOwner);

       

        if (*pNameLessKing->Get_TakeHit())
        {
            m_fSizeX = m_fOriginalSizeX * m_pOwner->Get_HpRatio();
            pNameLessKing->Set_TakeHit(false);
        }

        //m_pTransformCom->SetUp_Scale(m_fSizeX, m_fSizeY, 1.f);

        _float fLostWidth = m_fOriginalSizeX - m_fSizeX;
        _float fOffsetX = fLostWidth * 0.5f;
        m_fX = m_fOriginalX - fOffsetX;
        //m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(m_fX, m_fY, 0.f, 1.f));

     
    }

}

void CBossMark::Late_Update(_float fTimeDelta)
{
    m_pGameInstance->Add_RenderGroup(RENDERGROUP::UI, this);
}

HRESULT CBossMark::Render()
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

    if (m_iNumTexture == 3)
    {
        m_pGameInstance->Draw_Font(TEXT("Font_BossUI1"), TEXT("이름 없는 자"), _float2(g_iWinSizeX * 0.5f - 100, 50.f));

        m_pGameInstance->Draw_Font(TEXT("Font_BossUI1"), TEXT("이름 없는 자를 격파하세요"), _float2(g_iWinSizeX - 570, 50.f));
        m_pGameInstance->Draw_Font(TEXT("Font_Damage"), TEXT("이름 없는 왕국"), _float2(g_iWinSizeX - 258, 110.f), XMVectorSet(0.918f, 0.690f, 0.235f, 1.0f));
    }

    return S_OK;
}

HRESULT CBossMark::Ready_Components()
{
    if (FAILED(__super::Add_Component(m_eSceneType, TEXT("Prototype_Component_Texture_BossMark"),
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

CBossMark* CBossMark::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CBossMark* pInstance = new CBossMark(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CBossMark");
        Safe_Release(pInstance);
    }
    return pInstance;
}


CGameObject* CBossMark::Clone(void* pArg)
{
    CBossMark* pInstance = new CBossMark(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CBossMark");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CBossMark::Free()
{
    __super::Free();

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pVIBufferCom);
}
