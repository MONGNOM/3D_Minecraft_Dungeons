#pragma once

#include "Client_Defines.h"
#include "ContainerObject.h"

NS_BEGIN(Engine)
class CCollider;
class CModel;
class CNavigation;
class CShader;
NS_END

NS_BEGIN(Client)


enum BOSSSTATE {
	BOSS_AWAKE = 0,
	BOSS_ATTACK,
	BOSS_TP,
	BOSS_SPAWN,
	BOSS_CLONE,
	BOSS_DEATH,
	BOSS_END
};

class CNameLessKing final : public CContainerObject
{
public:
	typedef struct tagNameLessKing : public CContainerObject::CONTAINEROBJECT_DESC
	{
		_uint Shadow = 0;
	}NAMELESSKING_DESC;
private:
	CNameLessKing(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CNameLessKing(const CNameLessKing& Prototype);
	virtual ~CNameLessKing() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;


protected:
	HRESULT Ready_Components();
	HRESULT Ready_PartObjects();
	HRESULT Bind_ShaderResources();

private:
	_bool Intersect_ToPlayerSphere();

private:
	CCollider* m_pColliderCom[2] = { nullptr };
	CNavigation* m_pNavigationCom = { nullptr };
	CModel* m_pModelCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	_bool m_bAwake = true;
	_bool attacking = false;
	
	BOSSSTATE state = {};
	class CJusinBox* pBox = nullptr;

	_float m_iMaxHp{};
	_float m_iCurrentHp{};
	_uint m_iRandNum;	
	_float m_fStateTime = 0;
	_uint m_bShadow = 0;

public:
	static CNameLessKing* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END