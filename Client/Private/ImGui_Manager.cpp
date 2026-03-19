#include "Client_Defines.h"
#include "ImGui_Panel_Inspector.h"
#include "ImGui_Panel_Hierarchy.h"
#include "GameInstance.h"
#include "ImGui_Manager.h"
#include "Monster.h"

IMPLEMENT_SINGLETON(CImGui_Manager)

CImGui_Manager::CImGui_Manager()
{
}

HRESULT CImGui_Manager::Initialize_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	m_pGameInstance = CGameInstance::GetInstance();
	m_pDevice = pDevice;
	m_pDeviceContext = pDeviceContext;

	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pDeviceContext);

	// ImGui Context 세팅
	IMGUI_CHECKVERSION();
	ImGuiContext* pContext = ImGui::CreateContext();
	if (!pContext)
		return E_FAIL;

	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

	// ImGui Style 세팅
	//ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();
	ImGui::StyleColorsLight();

	// Win32용 초기화
	if (!::ImGui_ImplWin32_Init(g_hWnd))
		return E_FAIL;

	// dx11용 초기화
	if (!::ImGui_ImplDX11_Init(m_pDevice, m_pDeviceContext))
		return E_FAIL;


	m_pPanels[ETOI(PanelType::INSPECTOR)] = CImGui_Panel_Inspector::Create();
	m_pPanels[ETOI(PanelType::HIERARCHY)] = CImGui_Panel_Hierarchy::Create();
	return S_OK;
}

void CImGui_Manager::Update_Engine()
{
	if (ImGui::GetIO().WantCaptureMouse)
		return;
	
	if (m_pGameInstance->Get_DIMouseDown(DIMB::LBUTTON))
	{
		
		CVIBuffer_Terrain* terrain = dynamic_cast<CVIBuffer_Terrain*>(m_pGameInstance->Get_Component(TEXT("Terrain"), TEXT("Layer_BackGround"), ETOI(LEVEL::GAMEPLAY), TEXT("Com_VIBuffer")));
		CTransform* transform = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(TEXT("Terrain"), TEXT("Layer_BackGround"), ETOI(LEVEL::GAMEPLAY), TEXT("Com_Transform")));
		
		_float3 hitpos{};
		
		if (!Picking_OnTerrain(g_hWnd, terrain, transform, m_iNumZ, m_iNumX, &hitpos))
			return;

		m_bClone = true;
		m_pickingPos = hitpos;
		//XMStoreFloat3( Picking_OnTerrain(g_hWnd, terrain, transform, m_iNumZ, m_iNumX, ));
	}
	
	/*if (m_pGameInstance->Get_DIMouseState(MOUSEKEYSTATE::LB))
	{
		_float3 pos = Set_PickTerrain();

		m_pGameInstance->A(ENUM_TO_UINT(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Player"),
			ENUM_TO_UINT(LEVEL::GAMEPLAY), TEXT("Layer_Player"), &pos);
	}*/
}

void CImGui_Manager::Render()
{
	// win32, dx11, imguicontext 짝꿍
	::ImGui_ImplDX11_NewFrame();
	::ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	/////////////////////////////////////
	// 초반에는 켜놓고 기능 찾아보는 것 추천
	ImGui::ShowDemoWindow();

	Render_Panels();
	/////////////////////////////////////

	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void CImGui_Manager::Render_Panels()
{
	for (CImGui_Panel* pPanel : m_pPanels)
	{
		if (pPanel)
			pPanel->Render();
	}
}

_bool CImGui_Manager::Picking_OnTerrain(HWND hWnd, CVIBuffer_Terrain* pTerrainBufferCom, CTransform* pTerrainTransformCom, _uint numZ, _uint numX, _float3* pos)
{
	return m_pGameInstance->Picking_Pos(hWnd, pTerrainBufferCom, pTerrainTransformCom, numZ, numX, pos);

#pragma region 원본 코드

	/* 
	POINT	ptMouse{};

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
	matProj = m_pGameInstance->Get_Transform(D3DTS::PROJ);
	matProj = XMMatrixInverse(nullptr, matProj);
	vMousePos = XMVector3TransformCoord(vMousePos, matProj);

	// 뷰 스페이스 -> 월드
	// 뷰스페이스 들고와야함
	_matrix	matView;
	matView = m_pGameInstance->Get_Transform(D3DTS::VIEW);
	matView =  XMMatrixInverse(nullptr, matView);

	_vector rayPos{}; 
	_vector rayDir = vMousePos - rayPos;
	rayPos = XMVector3TransformCoord(rayPos, matView);
	rayDir = XMVector3TransformNormal(rayDir,matView);

	// 월드 -> 로컬
	_matrix	matWorld;
	matWorld = pTerrainTransformCom->Get_WorldInverse();
	 
	rayPos  = XMVector3TransformCoord(rayPos, matWorld);
	rayDir  = XMVector3Normalize(XMVector3TransformNormal(rayDir,  matWorld));

	_float	 fDist{}, fU{}, fV{};
	_ulong	dwVtxIdx[3]{};

	const _float3* pTerrainVtxPos = pTerrainBufferCom->Get_VtxPos();

	_vector v0{}, v1{}, v2{};
	_float3  p0, p1, p2;

	for (_ulong i = 0; i < m_iNumZ - 1; ++i)
	{
		for (_ulong j = 0; j < m_iNumX - 1; ++j)
		{
			_ulong	dwIndex = i * m_iNumX + j;

			// 오른쪽 위
			dwVtxIdx[0] = dwIndex + m_iNumX;
			dwVtxIdx[1] = dwIndex + m_iNumX + 1;
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

				return XMLoadFloat3(&hitPos);
			}

			// 왼쪽 아래
			dwVtxIdx[0] = dwIndex + m_iNumX;
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

				return XMLoadFloat3(&hitPos);
			}
		}
	}

	return _vector{ 0,0,0 };
	 */


#pragma endregion

}

_float CImGui_Manager::Compute_HeightOnTerrain(const _float3* pPos, const _float3* pTerrainVtxPos, const _ulong& dwCntX, const _ulong& dwCntZ, const _ulong& dwVtxItv)
{

	_ulong	dwIndex = _ulong(pPos->z / dwVtxItv) * dwCntX + _ulong(pPos->x / dwVtxItv);

	_float	fWidth = (pPos->x - pTerrainVtxPos[dwIndex + dwCntX].x) / dwVtxItv;
	_float	fHeight = (pTerrainVtxPos[dwIndex + dwCntX].z - pPos->z) / dwVtxItv;


	_vector v0{}, v1{}, v2{};

	_vector plane{};

	// 우 상단 
	if (fWidth > fHeight)
	{
		_vector v0 = XMLoadFloat3(&pTerrainVtxPos[dwIndex + dwCntX]);
		_vector v1 = XMLoadFloat3(&pTerrainVtxPos[dwIndex + dwCntX] + 1);
		_vector v2 = XMLoadFloat3(&pTerrainVtxPos[dwIndex + 1]);

		plane = XMPlaneFromPoints(v0 , v1, v2);
	}
	// 좌 하단
	else
	{
		_vector v0 = XMLoadFloat3(&pTerrainVtxPos[dwIndex + dwCntX]);
		_vector v1 = XMLoadFloat3(&pTerrainVtxPos[dwIndex + 1]);
		_vector v2 = XMLoadFloat3(&pTerrainVtxPos[dwIndex ]);

		plane = XMPlaneFromPoints(v0, v1, v2);
	}

	XMFLOAT4 fPlane{};
	XMStoreFloat4(&fPlane, plane);
	// ax + by + cz + d = 0

	// by = -ax - cz - d
	// 
	// y = (-ax - cz - d) / b

	return (-fPlane.x * pPos->x - fPlane.z * pPos->z - fPlane.w) / fPlane.y;
}



_float3 CImGui_Manager::Set_PickTerrain()
{
	/*CVIBuffer_Terrain* terrain = dynamic_cast<CVIBuffer_Terrain*>
		(m_pGameInstance->Get_Component(L"Terrain", TEXT("Com_VIBuffer"), ENUM_TO_UINT(LEVEL::GAMEPLAY), TEXT("Layer_BackGround")));

	CTransform* terrain1 = dynamic_cast<CTransform*>
		(m_pGameInstance->Get_Component(L"Terrain", TEXT("Com_Transform"), ENUM_TO_UINT(LEVEL::GAMEPLAY), TEXT("Layer_BackGround")));

	_vector pickkingPos = Picking_OnTerrain(g_hWnd, terrain, terrain1);
	
	_float3 fPos = {};

	XMStoreFloat3(&fPos, pickkingPos);

	fPos.y = Compute_HeightOnTerrain(&fPos, terrain->Get_VtxPos(),m_iNumX ,m_iNumZ, 1 ) + 1;
	
	return fPos;*/

	return _float3();
}

void CImGui_Manager::Free()
{
	__super::Free();
	for (CImGui_Panel* pPanel : m_pPanels)
	{
		Safe_Release(pPanel);
	}

	::ImGui_ImplDX11_Shutdown();
	::ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	Safe_Release(m_pGameInstance);
	Safe_Release(m_pDeviceContext);
	Safe_Release(m_pDevice);
}
