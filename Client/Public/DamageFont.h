#pragma once

#include "Client_Defines.h"
#include "UIObject.h"

NS_BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
NS_END

NS_BEGIN(Client)

class CDamageFont : public CUIObject
{
public:
	typedef struct tagDamageFontDesc final : public CUIObject::UI_DESC
	{
		_uint damage;
	}DAMAGEFONT_DESC;

private:
	CDamageFont(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDamageFont(const CDamageFont& Prototype);
	virtual ~CDamageFont() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	_uint m_iNumTexture{};

protected:
	CShader* m_pShaderCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };
	_float m_fDeleteTime = 0;
	_uint m_iDamage = 0;
	HRESULT Ready_Components();

public:
	static CDamageFont* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

};

NS_END