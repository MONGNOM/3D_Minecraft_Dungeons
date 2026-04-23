#include "Inventory.h"
#include "GameInstance.h"
#include "Player.h"
#include "Icon.h"
#include "Slot.h"

CInventory::CInventory(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUIObject{pDevice, pContext}
{
}

CInventory::CInventory(const CInventory& Prototype)
    :CUIObject(Prototype)
{
}

HRESULT CInventory::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CInventory::Initialize(void* pArg)
{
    INVENTORY_DESC* pDesc = static_cast<INVENTORY_DESC*>(pArg);
    m_iNumTexture = pDesc->NumTexture;


    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_bIsOpen = true;
    m_Name = TEXT("Inventory");


    CIcon::ICON_DESC iconDesc{}; // X버튼
    iconDesc.fSizeX = 86;
    iconDesc.fSizeY = 86;
    iconDesc.fX = g_iWinSizeX - iconDesc.fSizeX - 20;
    iconDesc.fY = 100;
    iconDesc.NumTexture = 9;
    iconDesc.name = TEXT("XButton");
    iconDesc.pParentActive = &m_bIsOpen;

    if (FAILED(m_pGameInstance->Add_GameObject(ETOI(LEVEL::STATIC), TEXT("Prototype_GameObject_InventoryIcon"),
        ETOI(LEVEL::STATIC), TEXT("Load_Layer"), &iconDesc)))
        return E_FAIL;


    CIcon::ICON_DESC hoverBariconDesc{}; // 바 이미지
    hoverBariconDesc.fSizeX = 17; 
    hoverBariconDesc.fSizeY = 1100;//  282
    hoverBariconDesc.fX = g_iWinSizeX * 0.5f + 350;
    hoverBariconDesc.fY = g_iWinSizeY * 0.5f + 90;
    hoverBariconDesc.NumTexture = 10;
    hoverBariconDesc.name = TEXT("hoverbar");
    hoverBariconDesc.pParentActive = &m_bIsOpen;

    if (FAILED(m_pGameInstance->Add_GameObject(ETOI(LEVEL::STATIC), TEXT("Prototype_GameObject_InventoryIcon"),
        ETOI(LEVEL::STATIC), TEXT("Load_Layer"), &hoverBariconDesc)))
        return E_FAIL;


    CIcon::ICON_DESC ImageiconDesc{}; // 칼 이미지
    ImageiconDesc.fSizeX = 384;
    ImageiconDesc.fSizeY = 384;
    ImageiconDesc.fX = g_iWinSizeX - hoverBariconDesc.fSizeX - 280;
    ImageiconDesc.fY = g_iWinSizeY * 0.5f - 330;
    ImageiconDesc.NumTexture = 3;
    ImageiconDesc.name = TEXT("Image_Sword");
    ImageiconDesc.pParentActive = &m_bIsOpen;

    if (FAILED(m_pGameInstance->Add_GameObject(ETOI(LEVEL::STATIC), TEXT("Prototype_GameObject_InventoryIcon"),
        ETOI(LEVEL::STATIC), TEXT("Load_Layer"), &ImageiconDesc)))
        return E_FAIL;


    CIcon::ICON_DESC DamageImage_iconBackDesc{}; // 데미지 옆에 그림 뒷배경
    DamageImage_iconBackDesc.fSizeX = 60;
    DamageImage_iconBackDesc.fSizeY = 70;
    DamageImage_iconBackDesc.fX = g_iWinSizeX * 0.5f + 425;
    DamageImage_iconBackDesc.fY = g_iWinSizeY * 0.5f - 280;
    DamageImage_iconBackDesc.NumTexture = 12;
    DamageImage_iconBackDesc.name = TEXT("Image_DamageBack");
    DamageImage_iconBackDesc.pParentActive = &m_bIsOpen;

    if (FAILED(m_pGameInstance->Add_GameObject(ETOI(LEVEL::STATIC), TEXT("Prototype_GameObject_InventoryIcon"),
        ETOI(LEVEL::STATIC), TEXT("Load_Layer"), &DamageImage_iconBackDesc)))
        return E_FAIL;


    CIcon::ICON_DESC DamageImage_iconSwordDesc{}; // 데미지 옆에 그림 앞 칼 그림
    DamageImage_iconSwordDesc.fSizeX = 32;
    DamageImage_iconSwordDesc.fSizeY = 32;
    DamageImage_iconSwordDesc.fX = g_iWinSizeX * 0.5f + 425;
    DamageImage_iconSwordDesc.fY = g_iWinSizeY * 0.5f - 280;
    DamageImage_iconSwordDesc.NumTexture = 15;
    DamageImage_iconSwordDesc.name = TEXT("Image_DamageSword");
    DamageImage_iconSwordDesc.pParentActive = &m_bIsOpen;

    if (FAILED(m_pGameInstance->Add_GameObject(ETOI(LEVEL::STATIC), TEXT("Prototype_GameObject_InventoryIcon"),
        ETOI(LEVEL::STATIC), TEXT("Load_Layer"), &DamageImage_iconSwordDesc)))
        return E_FAIL;


    CIcon::ICON_DESC LockTextBack_Desc{}; // 칼 맨 밑에 잠김 밑에 글씨
    LockTextBack_Desc.fSizeX = 800;
    LockTextBack_Desc.fSizeY = 700;
    LockTextBack_Desc.fX = g_iWinSizeX  - 500;
    LockTextBack_Desc.fY = g_iWinSizeY - 155;
    LockTextBack_Desc.NumTexture = 7;
    LockTextBack_Desc.name = TEXT("Image_LockText");
    LockTextBack_Desc.pParentActive = &m_bIsOpen;

    if (FAILED(m_pGameInstance->Add_GameObject(ETOI(LEVEL::STATIC), TEXT("Prototype_GameObject_InventoryIcon"),
        ETOI(LEVEL::STATIC), TEXT("Load_Layer"), &LockTextBack_Desc)))
        return E_FAIL;
    

    for (size_t i = 0; i < 3; i++)
    {
        CIcon::ICON_DESC Lock_Desc{}; // 칼 맨 밑에 잠김 아이콘
        Lock_Desc.fSizeX = 200;
        Lock_Desc.fSizeY = 200;
        Lock_Desc.fX = g_iWinSizeX - 250 * (i +1);
        Lock_Desc.fY = g_iWinSizeY - 220;
        Lock_Desc.NumTexture = 11;
        Lock_Desc.name = TEXT("Image_Lock%d",i);
        Lock_Desc.pParentActive = &m_bIsOpen;

        if (FAILED(m_pGameInstance->Add_GameObject(ETOI(LEVEL::STATIC), TEXT("Prototype_GameObject_InventoryIcon"),
            ETOI(LEVEL::STATIC), TEXT("Load_Layer"), &Lock_Desc)))
            return E_FAIL;
    }
    
    for (size_t i = 0; i < 4; i++)
    {
        for (size_t j = 0; j < 3; j++)
        {
            CSlot::SLOT_DESC Slot_Desc{}; //  Deafult Slot
            Slot_Desc.fSizeX = 200;
            Slot_Desc.fSizeY = 200;
            Slot_Desc.fX = 1025 + (j * 225); //* (j + 0) * 0.5f + 10; //g_iWinSizeX * 0.5f - Slot_Desc.fSizeX - 150 * (j + 1);
            Slot_Desc.fY = 360 + (i * 250); //* ((i + 1) * 0.5f) + 100;
            Slot_Desc.NumTexture = 6;
            Slot_Desc.name = L"DeafultSlot" + to_wstring(j + 3 * i);
            Slot_Desc.pParentActive = &m_bIsOpen;
            Slot_Desc.pHover = &m_bHover;
            Slot_Desc.pClick = &m_bClick;

            if (FAILED(m_pGameInstance->Add_GameObject(ETOI(LEVEL::STATIC), TEXT("Prototype_GameObject_Slot"),
                ETOI(LEVEL::STATIC), TEXT("Load_Layer"), &Slot_Desc)))
                return E_FAIL;

        }
       
    }



    for (size_t j = 0; j < 3; j++)
    {
        CSlot::SLOT_DESC Slot_Desc{}; //  (장착)아래 Item Slot
        Slot_Desc.fSizeX = 175;
        Slot_Desc.fSizeY = 175;
        Slot_Desc.fX = 200 + (j * 250);
        Slot_Desc.fY = g_iWinSizeY - 160;
        Slot_Desc.NumTexture = 16;
        Slot_Desc.name = L"EquipITEMSlot" + to_wstring(j);
        Slot_Desc.pParentActive = &m_bIsOpen;
        Slot_Desc.pHover = &m_bHover;
        Slot_Desc.pClick = &m_bClick;

        if (FAILED(m_pGameInstance->Add_GameObject(ETOI(LEVEL::STATIC), TEXT("Prototype_GameObject_Slot"),
            ETOI(LEVEL::STATIC), TEXT("Load_Layer"), &Slot_Desc)))
            return E_FAIL;

    }


    for (size_t j = 0; j < 3; j++)
    {
        CSlot::SLOT_DESC Slot_Desc{}; // (장착)위 Item Slot
        Slot_Desc.fSizeX = 175;
        Slot_Desc.fSizeY = 175;
        Slot_Desc.fX = 150 + (j * 300);
        Slot_Desc.fY = j == 1 ? 330 : 400;
        Slot_Desc.NumTexture = 17;
        Slot_Desc.name = L"EquipSlot" + to_wstring(j);
        Slot_Desc.pParentActive = &m_bIsOpen;
        Slot_Desc.pHover = &m_bHover;
        Slot_Desc.pClick = &m_bClick;

        if (FAILED(m_pGameInstance->Add_GameObject(ETOI(LEVEL::STATIC), TEXT("Prototype_GameObject_Slot"),
            ETOI(LEVEL::STATIC), TEXT("Load_Layer"), &Slot_Desc)))
            return E_FAIL;

    }

    for (size_t i = 0; i < 7; i++)
    {
        CIcon::ICON_DESC AllIcon_Desc{}; // 슬롯 위 Icon
        AllIcon_Desc.fSizeX = 40;
        AllIcon_Desc.fSizeY = 40;
        AllIcon_Desc.fX = 950 + (i * 110);
        AllIcon_Desc.fY = 210;
        AllIcon_Desc.NumTexture = 19 + i;
        AllIcon_Desc.name = L"Allicon" + to_wstring(i);
        AllIcon_Desc.pParentActive = &m_bIsOpen;

        if (FAILED(m_pGameInstance->Add_GameObject(ETOI(LEVEL::STATIC), TEXT("Prototype_GameObject_InventoryIcon"),
            ETOI(LEVEL::STATIC), TEXT("Load_Layer"), &AllIcon_Desc)))
            return E_FAIL;
    }


    for (size_t i = 0; i < 12; i++) // deafult 슬롯 담기
    {
        CGameObject* slot = (m_pGameInstance->Get_GameObject(L"DeafultSlot" + to_wstring(i), TEXT("Load_Layer"), ETOI(LEVEL::STATIC)));
        m_vecSlot[ETOI(INVEN::INVENTORY)].push_back(dynamic_cast<CSlot*>(slot));
    }

    for (size_t i = 0; i < 3; i++) // 장착 슬롯 담기
    {
        CSlot* slot = dynamic_cast<CSlot*>(m_pGameInstance->Get_GameObject(L"EquipSlot" + to_wstring(i), TEXT("Load_Layer"), ETOI(LEVEL::STATIC)));
        m_vecSlot[ETOI(INVEN::EQUIP)].push_back(slot);
    }

    for (size_t i = 0; i < 3; i++) // 아이템장착 슬롯 담기
    {
        CSlot* slot = dynamic_cast<CSlot*>(m_pGameInstance->Get_GameObject(L"EquipITEMSlot" + to_wstring(i), TEXT("Load_Layer"), ETOI(LEVEL::STATIC)));
        m_vecSlot[ETOI(INVEN::EQUIPITEM)].push_back(slot);
    }

    return S_OK;
}

void CInventory::Priority_Update(_float fTimeDelta)
{
}

void CInventory::Update(_float fTimeDelta)
{
    if (!m_bIsOpen)
        return;

    __super::Update_Transform();

}

void CInventory::Late_Update(_float fTimeDelta)
{
    if (!m_bIsOpen)
        return;

    m_pGameInstance->Add_RenderGroup(RENDERGROUP::INVEN, this);
}

HRESULT CInventory::Render()
{
    if (!m_bIsOpen)
    {

    }
    else
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


        //슬롯을 클릭했는가? 슬롯에 아이템이 존재하는가? 만약 존재한다면 그 아이템의 정보를 띄워줘야한다 -> 어떤 아이템의 정보인가? 정보 이름과 설명 데미지인지 방어력인지 넣어주기
        m_pGameInstance->Draw_Font(TEXT("Font_BossUI1"), TEXT("검"), _float2(g_iWinSizeX * 0.5f + 400, 300.f));
        m_pGameInstance->Draw_Font(TEXT("Font_Damage"), TEXT("근접 피해 10-16"), _float2(g_iWinSizeX * 0.5f + 480, 420.f));
        m_pGameInstance->Draw_Font(TEXT("Font_Damage"), TEXT("튼튼하고 믿고 쓸 수 있는검 입니다."), _float2(g_iWinSizeX * 0.5f + 400, 570.f));

        m_pGameInstance->Draw_Font(TEXT("Font_Default"), TEXT("전체"), _float2(925 , 160));

    }


    return S_OK;
}

HRESULT CInventory::Ready_Components()
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

CInventory* CInventory::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CInventory* pInstance = new CInventory(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CInventory");
        Safe_Release(pInstance);
    }
    return pInstance;
}


CGameObject* CInventory::Clone(void* pArg)
{
    CInventory* pInstance = new CInventory(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CHotbar");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CInventory::Free()
{
    __super::Free();

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pVIBufferCom);
}
