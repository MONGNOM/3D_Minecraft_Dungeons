#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Cube;
NS_END

NS_BEGIN(Client)

class CDefalutBlock final : public CGameObject
{
public:
	typedef struct tagTerrainDesc final : public CGameObject::GAMEOBJECT_DESC
	{

	}TERRAIN_DESC;
private:
	CDefalutBlock(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDefalutBlock(const CDefalutBlock& Prototype);
	virtual ~CDefalutBlock() = default;

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

protected:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();


public:
	static CDefalutBlock* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END
