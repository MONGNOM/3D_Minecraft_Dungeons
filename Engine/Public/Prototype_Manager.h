#pragma once

#include "Model.h"
#include "Shader.h"
#include "Texture.h"
#include "VIBuffer_Rect.h"
#include "VIBuffer_Cube.h"
#include "VIBuffer_Terrain.h"
#include "Navigation.h"
#include "Collider.h"


/* 생성한 원형객체를 보관한다. */
/* 요청에 따라 보관하고있던 원형객체를 복제하여 리턴해준다. */

NS_BEGIN(Engine)

class CPrototype_Manager final : public CBase
{
private:
	CPrototype_Manager();
	virtual ~CPrototype_Manager() = default;

public:
	HRESULT Initialize(_uint iNumLevels);
	HRESULT Add_Prototype(_uint iLevelIndex, const _wstring& strPrototypeTag, CBase* pPrototype);
	/* 내가 지정한 원형객체를 찾고 복제하여 리턴해준다. */
	CBase* Clone_Prototype(PROTOTYPE ePrototype, _uint iLevelIndex, const _wstring& strPrototypeTag, void* pArg);
	HRESULT Clear_Prototypes(_uint iLevelIndex);

	map<const _wstring, CBase*>& Get_Prototype(_uint iLevelIndex) { return m_pPrototypes[iLevelIndex]; }

private:
	map<const _wstring, CBase*>* m_pPrototypes = { nullptr };
	typedef map<const _wstring, CBase*>	PROTOTYPES;

	_uint			m_iNumLevels = {};

private:
	CBase* Find_Prototype(_uint iLevelIndex, const _wstring& strPrototypeTag);


public:
	static CPrototype_Manager* Create(_uint iNumLevels);
	virtual void Free() override;




};

NS_END