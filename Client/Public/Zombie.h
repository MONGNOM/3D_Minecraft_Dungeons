#pragma once

#include "Client_Defines.h"
#include "ContainerObject.h"

NS_BEGIN(Engine)
class CShader;
class CCollider;
class CModel;
class CNavigation;
NS_END

NS_BEGIN(Client)


enum ZOMBIESTATE {
	ZOMBIEIDLE = 0,
	ZOMBIEWALK,
	ZOMBIEATTACK,
	ZOMBIEEND
};

class CZombie final : public CGameObject
{
private:
	CZombie(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CZombie(const CZombie& Prototype);
	virtual ~CZombie() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	_bool* Get_TakeHit() { return &m_bTakehit; }
	void Set_TakeHit(_bool value) { m_bTakehit = value; }


protected:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();
private:
	_bool Intersect_ToPlayer();
	_bool Intersect_ToPlayerAttack();
	_bool Intersect_ToPlayerSphere();
	CGameObject* m_pHpBar = nullptr;

private:
	CCollider* m_pColliderCom[ETOI(COLLIDER::END)] = {nullptr};
	CNavigation* m_pNavigationCom = { nullptr };
	CModel* m_pModelCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	ZOMBIESTATE state = {};
	_bool m_bTakehit = false;
	virtual void TakeHit(_uint damage) override;

	_uint damage;

public:
	static CZombie* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END