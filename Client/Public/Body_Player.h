#pragma once

#include "Client_Defines.h"
#include "PartObject.h"
#include "Player.h"

NS_BEGIN(Engine)
class CShader;
class CModel;
class CCollider;
NS_END

NS_BEGIN(Client)

class CBody_Player final : public CPartObject
{
public:
	typedef struct tagBodyPlayerDesc final : public CPartObject::PARTOBJECT_DESC
	{
		 const PLAYERSTATE* pParentState = { nullptr };
		//const _uint* pParentState = { nullptr };
	}BODY_PLAYER_DESC;
private:
	CBody_Player(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBody_Player(const CBody_Player& Prototype);
	virtual ~CBody_Player() = default;

public:
	const _float4x4* Get_SocketBoneMatrixPtr(const _char* pBoneName);
public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	_bool IsAnimationFinished() const { return m_bIsAnimFinished; }

private:
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };
	CCollider* m_pColliderCom = { nullptr };

private:
	//const _uint* m_pParentState = { nullptr };
	const PLAYERSTATE* m_pParentPlayerState = { nullptr };
	PLAYERSTATE  m_PrevPlayerState = { PLAYERSTATE::END };
	_bool m_bIsAnimFinished = { false };



private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CBody_Player* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

};

NS_END