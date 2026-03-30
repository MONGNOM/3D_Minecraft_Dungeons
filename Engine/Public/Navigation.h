#pragma once

#include "Component.h"

NS_BEGIN(Engine)

class ENGINE_DLL CNavigation final : public CComponent
{
public:
	typedef struct tagNavigationDesc
	{
		_int			iCurrentCellIndex;
	}NAVIGATION_DESC;
private:
	CNavigation(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CNavigation(const CNavigation& Prototype);
	virtual ~CNavigation() = default;

public:
	virtual HRESULT Initialize_Prototype(const _tchar* pNavigationDataFile, const _tchar* pNeighbors);
	virtual HRESULT Initialize(void* pArg) override;

public:
	void SetUp_Neighbors();
	void SetUp_Neighbors(const _tchar* pNeighbors);
	_bool isMove(_vector vPoint);
	void Compute_Height(class CTransform* pTransform);

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
	static CNavigation* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pNavigationDataFile, const _tchar* pNeighbors);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END