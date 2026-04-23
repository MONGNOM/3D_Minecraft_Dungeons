#pragma once

#include "Client_Defines.h"
#include "UIObject.h"
#include "ItemObject.h"

NS_BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
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
	void Set_ItemDate(CItemObject* ItemObject);

protected:
	CShader* m_pShaderCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };

	HRESULT Ready_Components();
	_bool* m_pParentActive = nullptr;
	_bool* m_bHover = nullptr;
	_bool* m_bClick = nullptr;

	CItemObject* m_pItemObject = nullptr; 

public:
	static CSlot* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

};

NS_END