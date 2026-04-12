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


protected:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();
private:
	_bool Intersect_ToPlayer();

private:
	CCollider* m_pColliderCom = {nullptr};
	CNavigation* m_pNavigationCom = { nullptr };
	CModel* m_pModelCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	ZOMBIESTATE state = {};

	_float m_iMaxHp{};
	_float m_iCurrentHp{};

public:
	static CZombie* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END