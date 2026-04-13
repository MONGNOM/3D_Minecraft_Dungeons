#pragma once

#include "Client_Defines.h"
#include "ContainerObject.h"

NS_BEGIN(Engine)
class CCollider;
class CNavigation;
NS_END

NS_BEGIN(Client)


enum PLAYERSTATE {
	IDLE = 0,
	WALK,
	ATTACK,
	HEAL,
	FAILING,
	BOW,
	DEATH,
	END
};

class CPlayer final : public CContainerObject
{
private:
	CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayer(const CPlayer& Prototype);
	virtual ~CPlayer() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	void Set_Roll(_bool value) { m_bRoll = value; }

protected:
	HRESULT Ready_Components();
	HRESULT Ready_PartObjects();

private:
	void Intersect_ToMonster();

private:
	CCollider* m_pColliderCom = { nullptr };
	CNavigation* m_pNavigationCom = { nullptr };
	PLAYERSTATE state = {};
	class CBody_Player* pBody;
	const list<CGameObject*>* object = { nullptr };
	_float m_iMaxHp{};
	_float m_iCurrentHp{};
	_bool m_bRoll = false;

public:
	static CPlayer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END