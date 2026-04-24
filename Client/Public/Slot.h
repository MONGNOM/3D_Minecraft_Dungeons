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

class CSlot : public CUIObject
{
public:
	typedef struct tagSlotDesc final : public CUIObject::UI_DESC
	{
		_bool* pParentActive = nullptr;
		_bool* pHover = nullptr;
		_bool* pClick = nullptr;
		_bool* pAddItem = nullptr;
	}SLOT_DESC;

private:
	CSlot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSlot(const CSlot& Prototype);
	virtual ~CSlot() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	void Set_ItemDate(wstring itemName, wstring itemDesc, _float itemDamage, _uint iNumTexture);

	void Set_Hover(_bool value) { m_bHover = value; }
	void Set_Click(_bool value) { m_bClick = value; }
	_bool  Get_Click() const { return m_bClick; }
	wstring Get_ItemName() { return m_sItemName; }
	wstring Get_ItemDesc() { return m_sItemDesc; }
	_float Get_ItemDamage() { return m_fItemDamage; }
	_uint Get_ItemNumTexture() { return m_uItemNumTexture; }
	void ResetItmeData() { m_sItemName = L"";  m_sItemDesc = L""; m_fItemDamage = 0;  m_uItemNumTexture = 0; m_bItmeImage = false; };

protected:
	CShader* m_pShaderCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };

	HRESULT Ready_Components();
	_bool* m_pParentActive = nullptr;
	_bool m_bHover = false;
	_bool m_bClick = false;
	_bool* m_bAddItem = nullptr;
	wstring m_sItemName;
	wstring m_sItemDesc;
	_float	m_fItemDamage;
	_uint  m_uItemNumTexture;
	_bool m_bItmeImage = false;

	CItemObject* m_pItemObject = nullptr; 

public:
	static CSlot* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

};

NS_END