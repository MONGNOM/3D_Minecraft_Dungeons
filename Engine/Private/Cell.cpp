#include "Cell.h"


#include "Transform.h"


#ifdef _DEBUG
#include "VIBuffer_Cell.h"
#endif

CCell::CCell(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice{ pDevice }
	, m_pContext{ pContext }
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CCell::Initialize(const _float3* pPoints, _int iIndex)
{
	m_iIndex = iIndex;

	memcpy(m_vPoints, pPoints, sizeof(_float3) * ETOI(POINT::END));

	_vector			vLines[ETOI(LINE::END)];

	vLines[ETOI(LINE::AB)] = XMVector3Normalize(XMLoadFloat3(&m_vPoints[ETOI(POINT::B)]) - XMLoadFloat3(&m_vPoints[ETOI(POINT::A)]));
	m_vNormals[ETOI(LINE::AB)] = _float3(XMVectorGetZ(vLines[ETOI(LINE::AB)]) * -1.f, 0.f, XMVectorGetX(vLines[ETOI(LINE::AB)]));

	vLines[ETOI(LINE::BC)] = XMVector3Normalize(XMLoadFloat3(&m_vPoints[ETOI(POINT::C)]) - XMLoadFloat3(&m_vPoints[ETOI(POINT::B)]));
	m_vNormals[ETOI(LINE::BC)] = _float3(XMVectorGetZ(vLines[ETOI(LINE::BC)]) * -1.f, 0.f, XMVectorGetX(vLines[ETOI(LINE::BC)]));

	vLines[ETOI(LINE::CA)] = XMVector3Normalize(XMLoadFloat3(&m_vPoints[ETOI(POINT::A)]) - XMLoadFloat3(&m_vPoints[ETOI(POINT::C)]));
	m_vNormals[ETOI(LINE::CA)] = _float3(XMVectorGetZ(vLines[ETOI(LINE::CA)]) * -1.f, 0.f, XMVectorGetX(vLines[ETOI(LINE::CA)]));

	XMStoreFloat4(&m_vPlane, XMPlaneFromPoints(
		XMLoadFloat3(&m_vPoints[ETOI(POINT::A)]),
		XMLoadFloat3(&m_vPoints[ETOI(POINT::B)]),
		XMLoadFloat3(&m_vPoints[ETOI(POINT::C)])));


#ifdef _DEBUG
	m_pVIBuffer = CVIBuffer_Cell::Create(m_pDevice, m_pContext, pPoints);
	if (nullptr == m_pVIBuffer)
		return E_FAIL;
#endif


	return S_OK;
}

_bool CCell::isIn(_fvector vPoint, _int* pNeighborIndex)
{
	for (size_t i = 0; i < ETOI(LINE::END); i++)
	{
		_vector		vDir = XMVector3Normalize(XMVectorSetW(vPoint - XMLoadFloat3(&m_vPoints[i]), 0.f)); // 점 3개가 돌아가면서 현재 플레이어의 위치에 방향백터를 구한다

		if (0 < XMVectorGetX(XMVector3Dot(vDir, XMLoadFloat3(&m_vNormals[i])))) // 구한 방향백터와 법선백터의 외적값을 통해 0보다 크면 왜 밖에 존재하는가??
		{
			*pNeighborIndex = m_iNeighborIndices[i]; // ??  여기에 값을 넣어서 뭐해
			return false; // 안에 존재하지않는다
		}
	}

	return true;	// 안에 존재한다
}

_bool CCell::Compare_Points(_fvector vSour, _fvector vDest)
{
	if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[ETOI(POINT::A)]), vSour)) // a가 같아? 그럼 b c는? 같으면 넌 ab거나 ac선분이야 밑에도 동일함
	{
		if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[ETOI(POINT::B)]), vDest))
			return true;

		if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[ETOI(POINT::C)]), vDest))
			return true;
	}

	if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[ETOI(POINT::B)]), vSour))
	{
		if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[ETOI(POINT::C)]), vDest))
			return true;

		if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[ETOI(POINT::A)]), vDest))
			return true;
	}

	if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[ETOI(POINT::C)]), vSour))
	{
		if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[ETOI(POINT::A)]), vDest))
			return true;

		if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[ETOI(POINT::B)]), vDest))
			return true;
	}

	return false;
}

void CCell::Compute_Height(CTransform* pTransform)
{
	_vector			vPosition = pTransform->Get_State(STATE::POSITION);

	_float			fHeight = (-m_vPlane.x * XMVectorGetX(vPosition) - m_vPlane.z * XMVectorGetZ(vPosition) - m_vPlane.w) / m_vPlane.y;

	vPosition = XMVectorSetY(vPosition, fHeight);

	pTransform->Set_State(STATE::POSITION, vPosition);

}


#ifdef _DEBUG

HRESULT CCell::Render()
{
	if (FAILED(m_pVIBuffer->Bind_Resources()))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Render()))
		return E_FAIL;

	return S_OK;
}

#endif

CCell* CCell::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _float3* pPoints, _int iIndex)
{
	CCell* pInstance = new CCell(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pPoints, iIndex)))
	{
		MSG_BOX("Failed to Created : CCell");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCell::Free()
{
	__super::Free();

#ifdef _DEBUG
	Safe_Release(m_pVIBuffer);
#endif

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
