#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class CShader;
class CModel;
NS_END

NS_BEGIN(Client)

class CWaterBucket final : public CGameObject
{
private:
	CWaterBucket(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CWaterBucket(const CWaterBucket& Prototype);
	virtual ~CWaterBucket() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

protected:
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };


protected:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();


public:
	static CWaterBucket* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

};


NS_END