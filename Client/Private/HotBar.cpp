#include "HotBar.h"
#include "GameInstance.h"
#include "Player.h"

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

    if (m_iNumTexture == 3)
    {
        m_fOriginalY = pDesc->fY;
        m_fOriginalSizeY = pDesc->fSizeY;
    }
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    if (m_iNumTexture == 3 || m_iNumTexture == 4 || m_iNumTexture == 5 || m_iNumTexture == 6)
    {

        // 이거 씬마다로 해줘야함 씬 추가해야해서
       // m_pPlayer = m_pGameInstance->Get_GameObject(TEXT("Prototype_GameObject_Player0"), TEXT("Load_Layer"), ETOI(LEVEL::DUNGEON));
    }


    return S_OK;
}

void CHotBar::Priority_Update(_float fTimeDelta)
{
}

void CHotBar::Update(_float fTimeDelta)
{
    __super::Update_Transform();

    if (m_iNumTexture == 3 || m_iNumTexture == 4) // 4는 1초뒤로
    {
        m_pPlayer = m_pGameInstance->Get_GameObject(TEXT("Prototype_GameObject_Player0"), TEXT("Load_Layer"), ETOI(LEVEL::DUNGEON));
        CPlayer* player = dynamic_cast<CPlayer*>(m_pPlayer);

        //whiteTiemr += fTimeDelta;
        if (nullptr == m_pPlayer)
            return;

        if (player->Get_TakeHit())
        {
           // m_fSizeY = m_fOriginalSizeY * player->Get_HpRatio();
            player->Set_TakeHit(false);
        }
      
    }
   


}

void CHotBar::Late_Update(_float fTimeDelta)
{
    m_pGameInstance->Add_RenderGroup(RENDERGROUP::UI, this);
}

HRESULT CHotBar::Render()
{
    
    if (m_iNumTexture == 3 || m_iNumTexture == 4)
    {
        m_pPlayer = m_pGameInstance->Get_GameObject(TEXT("Prototype_GameObject_Player0"), TEXT("Load_Layer"), ETOI(LEVEL::DUNGEON));

        if (nullptr == m_pPlayer)
            return E_FAIL;

        CPlayer* player = dynamic_cast<CPlayer*>(m_pPlayer);

        _float fHpRatio = 1 - player->Get_HpRatio();

        if (FAILED(m_pShaderCom->Bind_RawValue("g_HealthRatio", &fHpRatio, sizeof(_float))))
            return E_FAIL;
    }
    else if (m_iNumTexture == 5 || m_iNumTexture == 6)
    {
        m_pPlayer = m_pGameInstance->Get_GameObject(TEXT("Prototype_GameObject_Player0"), TEXT("Load_Layer"), ETOI(LEVEL::DUNGEON));
        if (nullptr == m_pPlayer)
            return E_FAIL;

        CPlayer* player = dynamic_cast<CPlayer*>(m_pPlayer);
        _float maxf = 2.f;
        _float fCoolRatio;

        if (m_iNumTexture == 5)
         fCoolRatio = 1 - (*player->Get_PotionCoolTime() / maxf); 
        else
         fCoolRatio = 1 - (*player->Get_JumpCoolTime() / maxf); 
        
        if (FAILED(m_pShaderCom->Bind_RawValue("coolTime", &fCoolRatio, sizeof(_float))))
            return E_FAIL;
    }
   

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
    _wstring text;
    if (m_iNumTexture == 5 || m_iNumTexture == 6)
    {
        text = TEXT("Prototype_Component_Shader_VtxTex_Blend");
    }
    else if (m_iNumTexture == 3)
    {
        text = TEXT("Prototype_Component_Shader_VtxTex_Hp");
    }
    else
    {
        text = TEXT("Prototype_Component_Shader_VtxTex");
    }

    if (FAILED(__super::Add_Component(m_eSceneType, TEXT("Prototype_Component_Texture_HotBar"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    if (FAILED(__super::Add_Component(ETOI(LEVEL::STATIC), text,
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
