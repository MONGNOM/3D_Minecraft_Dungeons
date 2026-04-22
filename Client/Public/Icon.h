#pragma once

#include "Client_Defines.h"
#include "UIObject.h"

NS_BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
NS_END

NS_BEGIN(Client)

class CIcon : public CUIObject
{
public:
	typedef struct tagIconDesc final : public CUIObject::UI_DESC
	{
		_bool* pParentActive = nullptr;
	}ICON_DESC;

private:
	CIcon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CIcon(const CIcon& Prototype);
	virtual ~CIcon() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	void IconToggle() { m_SetActive = !m_SetActive; }

protected:
	CShader* m_pShaderCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };

	HRESULT Ready_Components();
	_bool* m_pParentActive = nullptr;

public:
	static CIcon* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

};

NS_END