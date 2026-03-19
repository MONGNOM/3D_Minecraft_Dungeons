#pragma once

#include "Base.h"

NS_BEGIN(Engine)

class CTimer;
class CTimer_Manager : public CBase
{
private:
	CTimer_Manager();
	virtual ~CTimer_Manager() = default;

public:
	HRESULT			Add_Timer(const _wstring& strTimerTag);
	_float			Compute_TimeDelta(const _wstring& strTimerTag);

private:
	map<_wstring, CTimer*>		m_Timers;

private:
	CTimer* Find_Timer(const _wstring& strTimerTag);


public:
	static CTimer_Manager* Create();
	virtual void		Free();
};

NS_END