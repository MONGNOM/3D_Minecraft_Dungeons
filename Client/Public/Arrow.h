#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class CShader;
class CModel;
class CCollider;
NS_END

NS_BEGIN(Client)

class CArrow final : public CGameObject
{
public:
	typedef struct tagArrowDesc : public CGameObject::GAMEOBJECT_DESC
	{
		_vector look;
	}ArrowDesc;

private:
	CArrow(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CArrow(const CArrow& Prototype);
	virtual ~CArrow() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	void Intersect_ToMonster();


private:
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };
	CCollider* m_pColliderCom = { nullptr };

private:
	_uint m_iArrowDamage = 0;
	const list<CGameObject*>* object = {nullptr};
	_float m_fDeleteTime = { 0 };
	_vector m_vLook;

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CArrow* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

};

NS_END