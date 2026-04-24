#include "ItemObject.h"

CItemObject::CItemObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CGameObject{ pDevice, pContext }
{
}

CItemObject::CItemObject(const CItemObject& Prototype)
	:CGameObject{ Prototype }
{
}



HRESULT CItemObject::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CItemObject::Initialize(void* pArg)
{
	ITEM_DESC* Desc = reinterpret_cast<ITEM_DESC*>(pArg);

	if (Desc != nullptr)
	{
		m_iIconTexture = Desc->iNumTexture;
		m_fItemDamage = Desc->itemDamage;
		m_sItemName = Desc->itemName;
		m_sItemDescription = Desc->itemDescription;
	}
	else
	{
		m_sItemName = L"";
		m_sItemDescription = L"";
		m_fItemDamage  = 0;
		m_iIconTexture  = 0;
	}

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}


void CItemObject::Set_ItemDate(string name, string description, _float damage, _uint iconTexture)
{
	/*m_sItemName = name;
	m_sItemDescription = description;
	m_fItemDamage = damage;
	m_iIconTexture = iconTexture;*/
}

void CItemObject::Priority_Update(_float fTimeDelta)
{
}

void CItemObject::Update(_float fTimeDelta)
{
}

void CItemObject::Late_Update(_float fTimeDelta)
{
}

HRESULT CItemObject::Render()
{
	return S_OK;
}

void CItemObject::Free()
{
	__super::Free();
}
