#pragma once

#include "Engine_Define.h"

NS_BEGIN(Engine)

class ENGINE_DLL CBase abstract
{
protected:
	CBase();
	virtual ~CBase() = default;

public:
	/* 레퍼런스 카운트를 증가시켜준다. */
	/* 리턴값 : 증가한 레퍼런스 카운트 */
	unsigned int AddRef();

	/* 레퍼런스 카운트를 감소시켜준다 or 삭제한다. */
	/* 리턴값 : 감소하기 이전의 레퍼런스 카운트 */
	unsigned int Release();
	 
protected:
	unsigned int			m_iRefCnt = {};

public:
	virtual void Free() {}

};

NS_END