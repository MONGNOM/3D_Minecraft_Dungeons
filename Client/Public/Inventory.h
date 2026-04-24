#pragma once

#include "Client_Defines.h"
#include "UIObject.h"

NS_BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
class CItemObject;
NS_END

NS_BEGIN(Client)

class CInventory : public CUIObject
{
public:
	typedef struct tagInventoryDesc final : public CUIObject::UI_DESC
	{
	}INVENTORY_DESC;

private:
	CInventory(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CInventory(const CInventory& Prototype);
	virtual ~CInventory() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	void InvenToggle() { m_bIsOpen = !m_bIsOpen; }
	_bool Get_Inven_SetActive() { return m_bIsOpen; }
	void Add_Item(wstring itemName, wstring itemDesc, _float itemDamage, _uint iNumTexture);
	void Equip_Item(wstring itemName, wstring itemDesc, _float itemDamage, _uint iNumTexture);


protected:
	HRESULT Ready_Components();
	void OnMouseEnter();
	_bool OnMouseClick(); // 이거 필요없을것 같은데??

	CShader* m_pShaderCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };

	// 마우스위치랑 슬롯 위치 비교 함수 --> 호버, 클릭
	// 
	// 슬롯에 아이템 데이터 추가

	_bool m_bIsOpen = false;
	_bool m_bHover = false;
	_bool m_bClick = false;
	_bool m_bAddItem = false;
	_bool bigImage = false;
	vector<class CSlot*> m_vecSlot[ETOI(INVEN::INVENEND)];

	wstring m_sItemDescription;
	wstring m_sItemName;
	wstring m_sItemDamage;
	_uint ItemTextuer = 999;

public:
	static CInventory* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

};

NS_END