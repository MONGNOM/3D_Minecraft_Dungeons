#pragma once

#include "Transform.h"

NS_BEGIN(Engine)

class ENGINE_DLL CGameObject abstract : public CBase
{
public:
	enum OBJECTTYPE { MONSTER, INTERACT, ENVIRONMENT, END};

public:
	typedef struct tagGameObjectDesc : public CTransform::TRANSFORM_DESC
	{
		_uint		iFlag = {};
		_wstring	name = {};
		_float3     pos = {};
		_float3     rot = {};
		string		m_sPrototype = {};
		_uint		NumTexture = {};

	}GAMEOBJECT_DESC;
protected:
	CGameObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject(const CGameObject& Prototype);
	virtual ~CGameObject() = default;

public:
	class CComponent* Get_Component(const _wstring& strComponentTag);
	OBJECTTYPE Get_ObjectType() { return m_eObjectType; }
	_wstring Get_ObjectName() { return m_Name; }
	string Get_PrototypeName() { return m_sPrototype; }
	void Set_m_iNumTexture(_uint numTex) { m_iNumTexture = numTex; }
	_uint Get_TextureNum() { return m_iNumTexture; }


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

protected:
	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };
	class CGameInstance*	m_pGameInstance = { nullptr };
	class CTransform*		m_pTransformCom = { nullptr };
	OBJECTTYPE m_eObjectType = {};
	_wstring m_Name = {};
	_float3 m_fPos = {};
	_float3 m_fRot = {};
	_uint			m_iNumTexture { 0 };
	string m_sPrototype{};
	_bool	m_SetActive;

protected:
	map<const _wstring, class CComponent*>		m_Components;

protected:
	/* 원형객체를 복제하여 맵에도 추가해주고 + 자식객체의 멤버변수에도 셋팅해준다. */
	HRESULT Add_Component(_uint iPrototypeLevelIndex, const _wstring& strPrototypeTag,
		const _wstring& strComponentTag, CComponent** ppOut, void* pArg = nullptr);

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

NS_END