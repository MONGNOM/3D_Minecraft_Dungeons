#pragma once

#include "Base.h"

NS_BEGIN(Engine)

class CCell final : public CBase
{
private:
	CCell(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CCell() = default;

public:
	_vector Get_Point(POINT ePoint) const {
		return XMLoadFloat3(&m_vPoints[ETOI(ePoint)]);
	}

	const _int* Get_NeighborIndices()
	{
		return m_iNeighborIndices;
	}
	

	void Set_Neighbor(LINE eLine, CCell* pNeighbor)
	{
		m_iNeighborIndices[ETOI(eLine)] = pNeighbor->m_iIndex;
	}
	
	void Set_Neighbors(const _int* pNeighbors)
	{
		memcpy(m_iNeighborIndices, pNeighbors, sizeof(_int) * 3);
	}

public:
	HRESULT Initialize(const _float3* pPoints, _int iIndex);
	_bool isIn(_fvector vPoint, _int* pNeighborIndex);
	_bool Compare_Points(_fvector vSour, _fvector vDest);

	void Compute_Height(class CTransform* pTransform);

#ifdef _DEBUG
public:
	HRESULT Render();
#endif
private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
	_float3					m_vPoints[ETOI(POINT::END)] = {};
	_float3					m_vNormals[ETOI(LINE::END)] = {};
	_int					m_iIndex = {};
	_int					m_iNeighborIndices[ETOI(LINE::END)] = { -1, -1, -1 };
	_float4					m_vPlane = {};

#ifdef _DEBUG

private:
	class CVIBuffer_Cell* m_pVIBuffer = { nullptr };

#endif

public:
	static CCell* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _float3* pPoints, _int iIndex);
	virtual void Free() override;
};

NS_END