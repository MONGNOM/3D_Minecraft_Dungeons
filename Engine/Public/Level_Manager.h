#pragma once

#include "Base.h"

/* 1. 현재 할당된 레벨의 주소를 들고 있는다. 게임 종료 시 삭제. */
/* 2. 할당된 레벨의 반복적인 업데이트를 호출해준다. */
/* 3. 할당된 레벨의 반복적인 렌더를 호출해준다. */
/* 4. 새로운 레벨로 교체해주는 기능(기존 레벨 파괴 + 기존 레벨용 자원들의 삭제) */

NS_BEGIN(Engine)

class CLevel_Manager final : public CBase
{
private:
	CLevel_Manager();
	virtual ~CLevel_Manager() = default;

public:
	HRESULT Change_Level(_uint iNewLevelIndex, class CLevel* pNewLevel);
	void Update(_float fTimeDelta);
	HRESULT Render();
	_uint* Get_NowScene() { return &m_iCurrentLevelIndex; }

private:
	class CLevel*			m_pCurrentLevel = { nullptr };
	class CGameInstance*	m_pGameInstance = { nullptr };
	_uint					m_iCurrentLevelIndex = { };

public:
	static CLevel_Manager* Create();
	virtual void Free() override;

};

NS_END