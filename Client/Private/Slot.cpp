#include "Slot.h"
#include "GameInstance.h"
#include "Player.h"
#include "Icon.h"
#include "ItemObject.h"

CSlot::CSlot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUIObject{pDevice, pContext}
{
}

CSlot::CSlot(const CSlot& Prototype)
    :CUIObject(Prototype)
{
}

HRESULT CSlot::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CSlot::Initialize(void* pArg)
{
    SLOT_DESC* pDesc = static_cast<SLOT_DESC*>(pArg);
    m_iNumTexture = pDesc->NumTexture;
    m_pParentActive = pDesc->pParentActive;
    //m_bHover = pDesc->pHover;
    //m_bClick = pDesc->pClick;
    m_bAddItem = pDesc->pAddItem;

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

   // m_Name = TEXT("Slot");


    CIcon::ICON_DESC DefaultSlot_Desc{}; // 슬롯 이미지
    DefaultSlot_Desc.fSizeX = pDesc->fSizeX;
    DefaultSlot_Desc.fSizeY = pDesc->fSizeY;
    DefaultSlot_Desc.fX = pDesc->fX;
    DefaultSlot_Desc.fY = pDesc->fY;
    DefaultSlot_Desc.NumTexture = m_iNumTexture;
    DefaultSlot_Desc.name = TEXT("DeafultSlotTEXT");
    DefaultSlot_Desc.pParentActive = m_pParentActive;

    if (FAILED(m_pGameInstance->Add_GameObject(ETOI(LEVEL::STATIC), TEXT("Prototype_GameObject_InventoryIcon"),
        ETOI(LEVEL::STATIC), TEXT("Load_Layer"), &DefaultSlot_Desc)))
        return E_FAIL;


    CIcon::ICON_DESC AddItem_Desc{}; // 슬롯 이미지
    AddItem_Desc.fSizeX = pDesc->fSizeX;
    AddItem_Desc.fSizeY = pDesc->fSizeY;
    AddItem_Desc.fX = pDesc->fX;
    AddItem_Desc.fY = pDesc->fY;
    AddItem_Desc.NumTexture = 18;
    AddItem_Desc.name = TEXT("DeafultAddItem");
    AddItem_Desc.pParentActive = m_pParentActive;
    AddItem_Desc.pAddItem = m_bAddItem;

    if (FAILED(m_pGameInstance->Add_GameObject(ETOI(LEVEL::STATIC), TEXT("Prototype_GameObject_InventoryIcon"),
        ETOI(LEVEL::STATIC), TEXT("Load_Layer"), &AddItem_Desc)))
        return E_FAIL;


    CIcon::ICON_DESC Hover_Desc{}; // 슬롯 이미지
    Hover_Desc.fSizeX = pDesc->fSizeX;
    Hover_Desc.fSizeY = pDesc->fSizeY;
    Hover_Desc.fX = pDesc->fX;
    Hover_Desc.fY = pDesc->fY;
    Hover_Desc.NumTexture = 5;
    Hover_Desc.name = TEXT("DeafultSlotHover");
    Hover_Desc.pParentActive = m_pParentActive;
    Hover_Desc.pHover = &m_bHover;

    if (FAILED(m_pGameInstance->Add_GameObject(ETOI(LEVEL::STATIC), TEXT("Prototype_GameObject_InventoryIcon"),
        ETOI(LEVEL::STATIC), TEXT("Load_Layer"), &Hover_Desc)))
        return E_FAIL;


    CIcon::ICON_DESC Select_Desc{}; // 슬롯 이미지
    Select_Desc.fSizeX = pDesc->fSizeX;
    Select_Desc.fSizeY = pDesc->fSizeY;
    Select_Desc.fX = pDesc->fX;
    Select_Desc.fY = pDesc->fY;
    Select_Desc.NumTexture = 4;
    Select_Desc.name = TEXT("DeafultSlotOnSelect");
    Select_Desc.pParentActive = m_pParentActive;
    Select_Desc.pClick = &m_bClick;

    if (FAILED(m_pGameInstance->Add_GameObject(ETOI(LEVEL::STATIC), TEXT("Prototype_GameObject_InventoryIcon"),
        ETOI(LEVEL::STATIC), TEXT("Load_Layer"), &Select_Desc)))
        return E_FAIL;


    CIcon::ICON_DESC DefaultItem_Desc{}; // 슬롯 이미지
    DefaultItem_Desc.fSizeX = 150;
    DefaultItem_Desc.fSizeY = 150;
    DefaultItem_Desc.fX = m_fX;
    DefaultItem_Desc.fY = m_fY;
    DefaultItem_Desc.NumTexture = 0; //  해당 텍스처번호 넣어줘야함
    DefaultItem_Desc.name = TEXT("DeafultItemImage");
    DefaultItem_Desc.pParentActive = m_pParentActive;
    DefaultItem_Desc.pDefault = &m_bItmeImage;
    DefaultItem_Desc.pNumTexture = &m_uItemNumTexture;

    if (FAILED(m_pGameInstance->Add_GameObject(ETOI(LEVEL::STATIC), TEXT("Prototype_GameObject_InventoryIcon"),
        ETOI(LEVEL::STATIC), TEXT("Load_Layer"), &DefaultItem_Desc)))
        return E_FAIL;



    return S_OK;
}

void CSlot::Priority_Update(_float fTimeDelta)
{
}

void CSlot::Update(_float fTimeDelta)
{
    if (!*m_pParentActive)
        return;

    __super::Update_Transform();

}

void CSlot::Late_Update(_float fTimeDelta)
{
    if (!*m_pParentActive)
        return;

    m_pGameInstance->Add_RenderGroup(RENDERGROUP::INVEN, this);
}

HRESULT CSlot::Render()
{
    if (*m_pParentActive)
    {
        // 클릭된 아이템 Data 슬롯에 클릭된 곳 대괄호 연산자로 데이터 접근 가능 
        
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

void CSlot::Set_ItemDate(wstring itemName, wstring itemDesc, _float itemDamage, _uint iNumTexture)
{
    // 아이템 데이터를 넘겨줘야함
    m_sItemName = itemName;
    m_sItemDesc = itemDesc;
    m_fItemDamage = itemDamage;
    m_uItemNumTexture = iNumTexture;

    if (m_bAddItem != nullptr)
    *m_bAddItem = true;

    m_bItmeImage = true;
    


}

HRESULT CSlot::Ready_Components()
{

    if (FAILED(__super::Add_Component(STATIC, TEXT("Prototype_Component_Texture_Inventroy"),
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

CSlot* CSlot::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CSlot* pInstance = new CSlot(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CSlot");
        Safe_Release(pInstance);
    }
    return pInstance;
}


CGameObject* CSlot::Clone(void* pArg)
{
    CSlot* pInstance = new CSlot(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CHotbar");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CSlot::Free()
{
    __super::Free();

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pVIBufferCom);
}
