#pragma once

#include "Base.h"

/* 1.사본객체들을 보관한다. */
/* 1-1. 레벨별로 구분하여 보관한다. */
/* 1-2. 내가 구분하고 싶은 기준(CLayer)을 두고 나누어 보관한다. */
/* 2.보관하고 있는 객체들의 갱신(업데이트)을 담당한다.*/

NS_BEGIN(Engine)

class CObject_Manager final : public CBase
{
private:
	CObject_Manager();
	virtual ~CObject_Manager() = default;

public:
	HRESULT Initialize(_uint iNumLevels);
	HRESULT Add_GameObject(_uint iPrototypeLevelIndex, const _wstring& strPrototypeTag, _uint iLayerLevelIndex, const _wstring& strLayerTag, void* pArg);
	void Priority_Update(_float fTimeDelta);
	void Update(_float fTimeDelta);
	void Late_Update(_float fTimeDelta);
	HRESULT Clear_Layers(_uint iLevelIndex);
	class CComponent* Get_Component(_wstring objName, _wstring layName, _uint iLayerIndex, _wstring componentName);
	vector<class CGameObject*> Get_GameObjects(_uint iLayerIndex);

	map<const _wstring, class CLayer*>& Get_Layer(_uint iLayerIndex);

private:
	_uint											m_iNumLevels = {};
	map<const _wstring, class CLayer*>*				m_pLayers = { nullptr };
	typedef map<const _wstring, class CLayer*>		LAYERS;
	class CGameInstance*							m_pGameInstance = { nullptr };

private:
	class CLayer* Find_Layer(_uint iLayerLevelIndex, const _wstring& strLayerTag);


public:
	static CObject_Manager* Create(_uint iNumLevels);
	virtual void Free() override;
};

NS_END