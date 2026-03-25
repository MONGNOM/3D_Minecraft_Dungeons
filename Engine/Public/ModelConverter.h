#pragma once
#include "Base.h"

NS_BEGIN(Engine)

class CModelConverter : public CBase
{
private:
	CModelConverter();
	virtual ~CModelConverter() = default;

public:
	HRESULT Ready_DynamicBinary(_uint numMeshs, const aiMesh* pAIMesh, const string& name);
	HRESULT Ready_StaticBinary(_uint numMeshs, const aiScene* m_pAIScene, const string& name, _fmatrix PreTransformMatrix);


public:
	static CModelConverter* Create();
	virtual void Free() override;
};

NS_END