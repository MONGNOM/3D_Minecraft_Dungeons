#include "Picking_Manager.h"
#include "GameInstance.h"
#include "GameObject.h"

CPicking_Manager::CPicking_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice{ pDevice }, m_pDeviceContext{ pContext }, m_pGameInstance{ CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pDeviceContext);
	Safe_AddRef(m_pGameInstance);
}

HRESULT CPicking_Manager::Initialize()
{

	return S_OK;
}

_bool CPicking_Manager::Picking_Pos(HWND hWnd, CVIBuffer_Terrain* pTerrainBufferCom, CTransform* pTerrainTransformCom, _uint numZ, _uint numX, _float3* pos) // 맵툴에 쓸 함수
{
	::POINT	ptMouse{};

	GetCursorPos(&ptMouse);
	ScreenToClient(hWnd, &ptMouse);

	// 뷰포트 -> 투영
	_vector vMousePos = {};

	D3D11_VIEWPORT          ViewportDesc{};
	_uint           iNumViewports = { 1 };

	m_pDeviceContext->RSGetViewports(&iNumViewports, &ViewportDesc);

	//_float x = ptMouse.x / (ViewportDesc.Width * 0.5f) - 1.f;
	//_float y = ptMouse.y / -(ViewportDesc.Height * 0.5f) + 1.f;
	_float x = ((ptMouse.x - ViewportDesc.TopLeftX) / ViewportDesc.Width) * 2.f - 1.f;
	_float y = -(((ptMouse.y - ViewportDesc.TopLeftY) / ViewportDesc.Height) * 2.f - 1.f);
	_float z = 1.f;

	vMousePos = XMVectorSet(x, y, z, 1.f);


	// 투영 -> 뷰스페이스
	// 투영들고와야함

	_matrix	matProj;
	matProj = XMLoadFloat4x4(m_pGameInstance->Get_Transform(D3DTS::PROJ));
	matProj = XMMatrixInverse(nullptr, matProj);
	vMousePos = XMVector3TransformCoord(vMousePos, matProj);

	// 뷰 스페이스 -> 월드
	// 뷰스페이스 들고와야함
	_matrix	matView;
	matView = XMLoadFloat4x4(m_pGameInstance->Get_Transform(D3DTS::VIEW));
	matView = XMMatrixInverse(nullptr, matView);

	_vector rayPos{};
	_vector rayDir = vMousePos - rayPos;
	rayPos = XMVector3TransformCoord(rayPos, matView);
	rayDir = XMVector3TransformNormal(rayDir, matView);

	// 월드 -> 로컬
	_matrix	matWorld;
	matWorld = pTerrainTransformCom->Get_WorldInverse();

	rayPos = XMVector3TransformCoord(rayPos, matWorld);
	rayDir = XMVector3Normalize(XMVector3TransformNormal(rayDir, matWorld));

	_float	 fDist{}, fU{}, fV{};
	_ulong	dwVtxIdx[3]{};

	const _float3* pTerrainVtxPos = pTerrainBufferCom->Get_VtxPos();

	_vector v0{}, v1{}, v2{};
	_float3  p0, p1, p2;

	for (_ulong i = 0; i < numZ - 1; ++i)
	{
		for (_ulong j = 0; j < numX - 1; ++j)
		{
			_ulong	dwIndex = i * numX + j;

			// 오른쪽 위
			dwVtxIdx[0] = dwIndex + numX;
			dwVtxIdx[1] = dwIndex + numX + 1;
			dwVtxIdx[2] = dwIndex + 1;

			// V1 + U(V2 - V1) + V(V3 - V1)
			v0 = XMLoadFloat3(&pTerrainVtxPos[dwVtxIdx[0]]);
			v1 = XMLoadFloat3(&pTerrainVtxPos[dwVtxIdx[1]]);
			v2 = XMLoadFloat3(&pTerrainVtxPos[dwVtxIdx[2]]);


			XMStoreFloat3(&p0, v0);
			XMStoreFloat3(&p1, v1);
			XMStoreFloat3(&p2, v2);

			if (TriangleTests::Intersects(rayPos, rayDir, v1, v0, v2, fDist))
			{
				_float3 hitPos;
				hitPos.x = p1.x + fU * (p0.x - p1.x);
				hitPos.y = 0;
				hitPos.z = p1.z + fV * (p2.z - p0.z);

				cout << "X좌표:  " << hitPos.x << "\t" << "Y좌표:  " << hitPos.y << "\t" << "Z좌표:  " << hitPos.z << endl;
				*pos = hitPos;
				return true;
			}

			// 왼쪽 아래
			dwVtxIdx[0] = dwIndex + numX;
			dwVtxIdx[1] = dwIndex + 1;
			dwVtxIdx[2] = dwIndex;

			v0 = XMLoadFloat3(&pTerrainVtxPos[dwVtxIdx[0]]);
			v1 = XMLoadFloat3(&pTerrainVtxPos[dwVtxIdx[1]]);
			v2 = XMLoadFloat3(&pTerrainVtxPos[dwVtxIdx[2]]);

			XMStoreFloat3(&p0, v0);
			XMStoreFloat3(&p1, v1);
			XMStoreFloat3(&p2, v2);

			if (TriangleTests::Intersects(rayPos, rayDir, v2, v1, v0, fDist))
			{
				_float3 hitPos;
				hitPos.x = p2.x + fU * (p1.x - p2.x);
				hitPos.y = 0;
				hitPos.z = p2.z + fV * (p0.z - p2.z);

				cout << "X좌표:  " << hitPos.x << "\t" << "Y좌표:  " << hitPos.y << "\t" << "Z좌표:  " << hitPos.z << endl;
				*pos = hitPos;
				return true;
			}
		}
	}

	return false;
}



CGameObject* CPicking_Manager::Picking_Object(HWND hWnd) // 게임 씬에서 쓸 피킹
{

	::POINT	ptMouse{};

	GetCursorPos(&ptMouse);
	ScreenToClient(hWnd, &ptMouse);

	// 뷰포트 -> 투영
	_vector vMousePos = {};

	D3D11_VIEWPORT          ViewportDesc{};
	_uint           iNumViewports = { 1 };

	m_pDeviceContext->RSGetViewports(&iNumViewports, &ViewportDesc);

	//_float x = ptMouse.x / (ViewportDesc.Width * 0.5f) - 1.f;
	//_float y = ptMouse.y / -(ViewportDesc.Height * 0.5f) + 1.f;
	_float x = ((ptMouse.x - ViewportDesc.TopLeftX) / ViewportDesc.Width) * 2.f - 1.f;
	_float y = -(((ptMouse.y - ViewportDesc.TopLeftY) / ViewportDesc.Height) * 2.f - 1.f);
	_float z = 1.f;

	vMousePos = XMVectorSet(x, y, z, 1.f);


	// 투영 -> 뷰스페이스
	// 투영들고와야함

	_matrix	matProj;
	matProj = XMLoadFloat4x4(m_pGameInstance->Get_Transform(D3DTS::PROJ));
	matProj = XMMatrixInverse(nullptr, matProj);
	vMousePos = XMVector3TransformCoord(vMousePos, matProj);

	// 뷰 스페이스 -> 월드
	// 뷰스페이스 들고와야함
	_matrix	matView;
	matView = XMLoadFloat4x4(m_pGameInstance->Get_Transform(D3DTS::VIEW));
	matView = XMMatrixInverse(nullptr, matView);

	_vector rayPos{};
	_vector rayDir = vMousePos - rayPos;
	rayPos = XMVector3TransformCoord(rayPos, matView);
	rayDir = XMVector3TransformNormal(rayDir, matView);
	

	return nullptr; //CGameObject;
}



CPicking_Manager* CPicking_Manager::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPicking_Manager* pInstance = new CPicking_Manager(pDevice, pContext);
	
	if(FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to CPicking_Manager");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPicking_Manager::Free()
{

	__super::Free();

	Safe_Release(m_pGameInstance);
	Safe_Release(m_pDevice);
	Safe_Release(m_pDeviceContext);

}
