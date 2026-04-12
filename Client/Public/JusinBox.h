#pragma once

#include "Client_Defines.h"
#include "PartObject.h"

NS_BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Cube;
class CCollider;
NS_END

NS_BEGIN(Client)

class CJusinBox final : public CPartObject
{
public:
	typedef struct tagBowDesc final : public CPartObject::PARTOBJECT_DESC
	{
		const _float4x4* pSocketMatrix = { nullptr };
	}JUSINBOX_DESC;
private:
	CJusinBox(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CJusinBox(const CJusinBox& Prototype);
	virtual ~CJusinBox() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

protected:
	CShader* m_pShaderCom = { nullptr };
	CVIBuffer_Cube* m_pVIBufferCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };
	CCollider* m_pColliderCom = { nullptr };

private:
	const _float4x4* m_pSocketMatrix = { nullptr };
	_uint m_iBoXDamage = 0;
	_bool Intersect_ToPlayer();


protected:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();


public:
	static CJusinBox* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END
