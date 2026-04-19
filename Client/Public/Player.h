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
	void Set_TakeHit(_bool vaule) { m_bTakehit = vaule; }
	_bool* Get_TakeHit() { return &m_bTakehit; }
	_bool* Get_Roll() { return &m_bRoll; }
	_bool* Get_Potion() { return &m_bPotion; }
	_float* Get_PotionCoolTime() { return &PotioncoolTiem; }
	_float* Get_JumpCoolTime() { return &jumpCoolTime; }

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
	void TakeHit(_uint damage);
	_bool m_bRoll = false;
	_bool m_bTakehit = false;
	_bool m_bPotion = false;
	_float PotioncoolTiem = 0.f;
	_float maxcooltiem = 2;
	_float jumpCoolTime = 0;


public:
	static CPlayer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END