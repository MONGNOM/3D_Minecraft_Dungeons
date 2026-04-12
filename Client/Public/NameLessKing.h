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
	BOSSSAWAKE = 0,
	BOSSSWALK,
	BOSSSATTACK,
	BOSSSTP,
	BOSSSSPAWN,
	BOSSSDEATH,
	BOSSSEND
};

class CNameLessKing final : public CContainerObject
{
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
	_bool Intersect_ToPlayer();

private:
	CCollider* m_pColliderCom = { nullptr };
	CNavigation* m_pNavigationCom = { nullptr };
	CModel* m_pModelCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };


	BOSSSTATE state = {};
	class CJusinBox* pBox = nullptr;

	_float m_iMaxHp{};
	_float m_iCurrentHp{};

public:
	static CNameLessKing* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END