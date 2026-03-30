#include "Navigation.h"
#include "Cell.h"

#include "GameInstance.h"

CNavigation::CNavigation(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent{ pDevice, pContext }
{
}

CNavigation::CNavigation(const CNavigation& Prototype)
	: CComponent{ Prototype }
	, m_Cells{ Prototype.m_Cells }
#ifdef _DEBUG
	, m_pShader{ Prototype.m_pShader }
#endif
{
	for (auto& pCell : m_Cells)
		Safe_AddRef(pCell);

#ifdef _DEBUG
	Safe_AddRef(m_pShader);
#endif
}

HRESULT CNavigation::Initialize_Prototype(const _tchar* pNavigationDataFile, const _tchar* pNeighbors)
{
	_ulong			dwByte = { };
	HANDLE			hFile = CreateFile(pNavigationDataFile, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return E_FAIL;

	_float3		vPoints[3] = {};

	while (true)
	{
		ReadFile(hFile, vPoints, sizeof(_float3) * 3, &dwByte, nullptr);
		if (0 == dwByte)
			break;

		CCell* pCell = CCell::Create(m_pDevice, m_pContext, vPoints, m_Cells.size());
		if (nullptr == pCell)
			return E_FAIL;

		m_Cells.push_back(pCell);
	}


	SetUp_Neighbors(pNeighbors);

	CloseHandle(hFile);


	 /*hFile = CreateFile(TEXT("../Bin/DataFiles/Neighbors.dat"), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return E_FAIL;

	for (size_t i = 0; i < m_Cells.size(); i++)
	{
		WriteFile(hFile, m_Cells[i]->Get_NeighborIndices(), sizeof(_int) * 3, &dwByte, nullptr);
	}

	CloseHandle(hFile);*/


#ifdef _DEBUG
	m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Cell.hlsl"), VTXPOS::Elements, VTXPOS::iNumElements);
	if (nullptr == m_pShader)
		return E_FAIL;
#endif
	return S_OK;
}

HRESULT CNavigation::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return S_OK;

	auto		pDesc = static_cast<NAVIGATION_DESC*>(pArg);

	m_iCurrentCellIndex = pDesc->iCurrentCellIndex;

	return S_OK;
}

void CNavigation::SetUp_Neighbors()
{
	for (auto& pSourCell : m_Cells) // 모든 셀을 돌면서 하나하나 체크 
	{
		for (auto& pDestCell : m_Cells)
		{
			if (pSourCell == pDestCell)
				continue;

			if (true == pDestCell->Compare_Points(pSourCell->Get_Point(POINT::A), pSourCell->Get_Point(POINT::B))) // 데스트셀의 점  A와 같다? 그럼 B의 점이 데스트의 B또는C랑 같은게 존재하면 AB와 이웃라인? 밑에도 동일한내용
				pSourCell->Set_Neighbor(LINE::AB, pDestCell);

			if (true == pDestCell->Compare_Points(pSourCell->Get_Point(POINT::B), pSourCell->Get_Point(POINT::C)))
				pSourCell->Set_Neighbor(LINE::BC, pDestCell);

			if (true == pDestCell->Compare_Points(pSourCell->Get_Point(POINT::C), pSourCell->Get_Point(POINT::A)))
				pSourCell->Set_Neighbor(LINE::CA, pDestCell);
		}
	}
}

void CNavigation::SetUp_Neighbors(const _tchar* pNeighbors)
{
	_ulong dwByte = {};
	HANDLE  hfile = CreateFile(pNeighbors, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (hfile == 0)
		return;

	for (auto& pCell : m_Cells)
	{
		_int iNeigbors[3] = {};

		ReadFile(hfile, iNeigbors, sizeof(_int) * 3, &dwByte, nullptr);

		pCell->Set_Neighbors(iNeigbors);

	}

	CloseHandle(hfile);
}

_bool CNavigation::isMove(_vector vPoint)
{
	if (-1 == m_iCurrentCellIndex)
		return false;

	_int		iNeighborIndex = { -1 };

	if (false == m_Cells[m_iCurrentCellIndex]->isIn(vPoint, &iNeighborIndex))
	{
		if (-1 != iNeighborIndex)
		{
			while (true)
			{
				if (true == m_Cells[iNeighborIndex]->isIn(vPoint, &iNeighborIndex))
					break;

				if (-1 == iNeighborIndex)	// 이웃이 없으면 이동을 못한다.
					return false;
			}

			m_iCurrentCellIndex = iNeighborIndex;
			return true;
		}
		else
			return false;
	}
	else
		return true;

}

void CNavigation::Compute_Height(CTransform* pTransform)
{
	if (-1 == m_iCurrentCellIndex)
		return;

	m_Cells[m_iCurrentCellIndex]->Compute_Height(pTransform);

}

#ifdef _DEBUG

HRESULT CNavigation::Render()
{
	_float4x4		WorldMatrix = {};
	XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());

	m_pShader->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform(D3DTS::VIEW));
	m_pShader->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform(D3DTS::PROJ));

	_float4 vColor = {};
	
	if (-1 == m_iCurrentCellIndex)
		vColor = _float4(0.f, 1.f, 0.f, 1.f);
	else
		vColor = _float4(1.f, 0.f, 0.f, 1.f);

	if (FAILED(m_pShader->Bind_RawValue("g_vColor", &vColor, sizeof vColor)))
		return E_FAIL;

	if (-1 == m_iCurrentCellIndex)
	{
		m_pShader->Bind_Matrix("g_WorldMatrix", &WorldMatrix);
		m_pShader->Begin(0);
		
		for (auto& pCell : m_Cells)
		{
			if (FAILED(pCell->Render()))
				return E_FAIL;
		}
	
	}
	else
	{
		WorldMatrix.m[3][1] = 0.1f;
		m_pShader->Bind_Matrix("g_WorldMatrix", &WorldMatrix);
		m_pShader->Begin(0);

		m_Cells[m_iCurrentCellIndex]->Render();
	}

	

	return S_OK;
}

#endif

CNavigation* CNavigation::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pNavigationDataFile, const _tchar* pNeighbors)
{
	CNavigation* pInstance = new CNavigation(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pNavigationDataFile, pNeighbors)))
	{
		MSG_BOX("Failed to Created : CNavigation");
		Safe_Release(pInstance);
	}
	return pInstance;
}


CComponent* CNavigation::Clone(void* pArg)
{
	CNavigation* pInstance = new CNavigation(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CNavigation");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CNavigation::Free()
{
	__super::Free();

#ifdef _DEBUG
	Safe_Release(m_pShader);
#endif

	for (auto& pCell : m_Cells)
		Safe_Release(pCell);

	m_Cells.clear();
}
