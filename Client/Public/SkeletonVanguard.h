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


enum SkeletonVanguardSTATE {
	SkeletonVanguardIDLE = 0,
	SkeletonVanguardWALK,
	SkeletonVanguardATTACK,
	SkeletonVanguardEND
};

class CSkeletonVanguard final : public CGameObject
{
public:
	typedef struct tagVanguardDesc : public CGameObject::GAMEOBJECT_DESC
	{
		_vector look;
	}VANGUARD_Desc;
private:
	CSkeletonVanguard(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSkeletonVanguard(const CSkeletonVanguard& Prototype);
	virtual ~CSkeletonVanguard() = default;

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
	_bool Intersect_ToPlayerAttack();
	_bool Intersect_ToPlayerSphere();

private:
	CCollider* m_pColliderCom[ETOI(COLLIDER::END)] = { nullptr };
	CNavigation* m_pNavigationCom = { nullptr };
	CModel* m_pModelCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	SkeletonVanguardSTATE state = {};
	_float m_iMaxHp{};
	_float m_iCurrentHp{};
	_uint damage;
	_vector m_vLook;

public:
	static CSkeletonVanguard* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END