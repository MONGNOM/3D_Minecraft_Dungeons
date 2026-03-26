#pragma once

#include "Component.h"

NS_BEGIN(Engine)

class ENGINE_DLL CNavigation final : public CComponent
{
private:
	CNavigation(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CNavigation(const CNavigation& Prototype);
	virtual ~CNavigation() = default;

public:
	virtual HRESULT Initialize_Prototype(const _tchar* pNavigationDataFile);
	virtual HRESULT Initialize(void* pArg) override;

#ifdef _DEBUG
public:
	HRESULT Render();
#endif

private:
	_int								m_iCurrentCellIndex = { -1 };
	vector<class CCell*>				m_Cells;

#ifdef _DEBUG
private:
	class CShader* m_pShader = { nullptr };
#endif

public:
	static CNavigation* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pNavigationDataFile);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END