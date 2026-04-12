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


enum CREEPERSTATE {
	CREEPERIDLE = 0,
	CREEPERWALK,
	CREEPERATTACK,
	CREEPEREND
};

class CCreeper final : public CGameObject
{
private:
	CCreeper(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCreeper(const CCreeper& Prototype);
	virtual ~CCreeper() = default;

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
	CREEPERSTATE state = {};
	_float m_iMaxHp{};
	_float m_iCurrentHp{};

public:
	static CCreeper* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END