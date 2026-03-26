#pragma once

#include "Base.h"

NS_BEGIN(Engine)

class CCell final : public CBase
{
public:
	enum POINT { A, B, C, END };

private:
	CCell(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CCell() = default;

public:
	HRESULT Initialize(const _float3* pPoints);


#ifdef _DEBUG
public:
	HRESULT Render();
#endif
private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
	_float3					m_vPoints[POINT::END] = {};

#ifdef _DEBUG

private:
	class CVIBuffer_Cell* m_pVIBuffer = { nullptr };

#endif

public:
	static CCell* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _float3* pPoints);
	virtual void Free() override;
};

NS_END