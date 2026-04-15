#pragma once

#include "Client_Defines.h"
#include "UIObject.h"

NS_BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
NS_END

NS_BEGIN(Client)

class CHpBar : public CUIObject
{
public:
	typedef struct tagHpbarDesc final : public CUIObject::UI_DESC
	{
		_uint iNumTexture {};
		CGameObject* owner = nullptr;

	}HPBAR_DESC;

private:
	CHpBar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CHpBar(const CHpBar& Prototype);
	virtual ~CHpBar() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	void isDead() { Set_Dead(); m_pOwner = nullptr; }
private:
	_uint m_iNumTexture{};

protected:
	CShader* m_pShaderCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };
	CGameObject* m_pOwner = nullptr;
	HRESULT Ready_Components();

public:
	static CHpBar* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

};

NS_END