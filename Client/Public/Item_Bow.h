#pragma once

#include "Client_Defines.h"
#include "ItemObject.h"

NS_BEGIN(Engine)
class CShader;
class CModel;
class CCollider;
NS_END

NS_BEGIN(Client)

class CItem_Bow final : public CItemObject
{
private:
	CItem_Bow(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CItem_Bow(const CItem_Bow& Prototype);
	virtual ~CItem_Bow() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	_bool Get_SceneChange() { return m_bSceneChanger; }

protected:
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };
	CCollider* m_pColliderCom = nullptr;

protected:
	_bool Intersect_ToPlayer();
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();
	_bool m_bSceneChanger = false;

	_bool firstchanger = true;

public:
	static CItem_Bow* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

};


NS_END