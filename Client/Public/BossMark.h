#pragma once

#include "Client_Defines.h"
#include "UIObject.h"

NS_BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
NS_END

NS_BEGIN(Client)

class CBossMark : public CUIObject
{
public:
	typedef struct tagBossMarkDesc final : public CUIObject::UI_DESC
	{
		_uint iNumTexture {};
		CGameObject* owner = nullptr;
		CGameObject** connet = nullptr;
	}BOSSMARK_DESC;

private:
	CBossMark(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBossMark(const CBossMark& Prototype);
	virtual ~CBossMark() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	void Set_OwnerDead() { m_bOwnerDead = true; }
	void isDead() { Set_Dead(); m_pOwner = nullptr; }
	
private:
	_uint m_iNumTexture{};

protected:
	CShader* m_pShaderCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };
	_float m_fOriginalSizeX;
	HRESULT Ready_Components();
	CGameObject* m_pOwner;
	_bool m_bOwnerDead = false;

	_float m_fOriginalX = 0.f;
public:
	static CBossMark* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

};

NS_END