#pragma once

#include "GameObject.h"

NS_BEGIN(Engine)

class ENGINE_DLL CItemObject : public CGameObject
{
public:
	typedef struct tagItemDesc : CGameObject::GAMEOBJECT_DESC
	{
		wstring itemName;
		wstring itemDescription;
		_float itemDamage;
		_uint  iNumTexture;

	}ITEM_DESC;

public:
	CItemObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CItemObject(const CItemObject& Prototype);
	virtual ~CItemObject() = default;
	void Set_ItemDate(string name, string description, _float damage, _uint iconTexture);
	wstring Get_ItemName() { return m_sItemName; }

	wstring m_sItemName;
	wstring m_sItemDescription;
	_float m_fItemDamage;
	_uint  m_iIconTexture;

protected:
	

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

NS_END