#pragma once

#include "Client_Defines.h"
#include "ContainerObject.h"

NS_BEGIN(Engine)
class CCollider;
class CNavigation;
NS_END

NS_BEGIN(Client)

class CSkeleton final : public CContainerObject
{
public:
	enum SKELETONSTATE {
		IDLE = 0x00000001,
		WALK = 0x00000002,
		ATTACK = 0x00000004,
		DEATH = 0x00000008,
	};
private:
	CSkeleton(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSkeleton(const CSkeleton& Prototype);
	virtual ~CSkeleton() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	_bool* Get_FakeBool() { return &Fakebool; }
	_bool* Get_TakeHit() { return &m_bTakehit; }
	void Set_TakeHit(_bool value) {m_bTakehit = value; }
	_bool* Get_OwnerDead() { return &m_bOwner; }

protected:
	HRESULT Ready_Components();
	HRESULT Ready_PartObjects();
	_bool m_bOwner = true;

private:
	_uint			m_iState = {};

	CCollider* m_pColliderCom = { nullptr };
	CNavigation* m_pNavigationCom = { nullptr };
	class CBody_Skeleton* pBody = nullptr;
	_bool Fakebool = true;
	CGameObject* m_pHpBar = nullptr;
	void Intersect_ToPlayer();
	virtual void TakeHit(_uint damage) override;
	_bool m_bTakehit = false;


public:
	static CSkeleton* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END