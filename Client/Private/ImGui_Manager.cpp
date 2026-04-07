#include "Client_Defines.h"
#include "ImGui_Panel_Inspector.h"
#include "ImGui_Panel_Hierarchy.h"

#include "GameInstance.h"
#include "ImGui_Manager.h"
#include "Monster.h"
#include "Engine_Macro.h"

IMPLEMENT_SINGLETON(CImGui_Manager)
static bool bIsCreateMode = false;
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

	m_pGalleryTexture = CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/SkyBox/mincraft_%d.dds"), 16);
	m_pThumnailsTexture = CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/SkyBox/Thum_%d.png"), 16);
	images = m_pGalleryTexture->Get_Texture();
	m_vThum = m_pThumnailsTexture->Get_Texture();

	m_pPanels[ETOI(PanelType::INSPECTOR)] = CImGui_Panel_Inspector::Create();
	m_pPanels[ETOI(PanelType::HIERARCHY)] = CImGui_Panel_Hierarchy::Create();
	return S_OK;
}

void CImGui_Manager::Update_Engine()
{
	if (ImGui::GetIO().WantCaptureMouse)
		return;

	if (m_pGameInstance->Get_DIKeyDown(DIK_T))
	{
		bIsCreateMode = !bIsCreateMode;
	}

	if (CGameInstance::GetInstance()->Get_DIKeyDown(DIK_F))
	{
		m_bNaviEditMode = !m_bNaviEditMode;
	}

	if (CGameInstance::GetInstance()->Get_DIKeyState(DIK_LCONTROL) && CGameInstance::GetInstance()->Get_DIKeyDown(DIK_Z))
	{
		m_vPoint.pop_back();
	}


	if (m_pGameInstance->Get_DIMouseDown(DIMB::LBUTTON))
	{	
		if (bIsCreateMode)
		{
			CVIBuffer_Terrain* terrain = dynamic_cast<CVIBuffer_Terrain*>(m_pGameInstance->Get_Component(TEXT("Terrain"), TEXT("Layer_BackGround"), ETOI(LEVEL::GAMEPLAY), TEXT("Com_VIBuffer")));
			CTransform* transform = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(TEXT("Terrain"), TEXT("Layer_BackGround"), ETOI(LEVEL::GAMEPLAY), TEXT("Com_Transform")));

			_float3 hitpos{};

			if (!Picking_OnTerrain(g_hWnd, terrain, transform, m_iNumZ, m_iNumX, &hitpos))
				return;

			m_bClone = true;
			m_pickingPos = hitpos;
		}


		if (m_bNaviEditMode)
		{
			// 네비게이션 점(vPoints) 추가 로직 실행!

			CVIBuffer_Terrain* terrain = dynamic_cast<CVIBuffer_Terrain*>(m_pGameInstance->Get_Component(TEXT("Terrain"), TEXT("Layer_BackGround"), ETOI(LEVEL::GAMEPLAY), TEXT("Com_VIBuffer")));
			CTransform* tertransform = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(TEXT("Terrain"), TEXT("Layer_BackGround"), ETOI(LEVEL::GAMEPLAY), TEXT("Com_Transform")));

			_float3 y;
			XMStoreFloat3(&y, tertransform->Get_State(STATE::POSITION));
			_float3 hitpos{};

			if (!Picking_OnTerrain(g_hWnd, terrain, tertransform, m_iNumZ, m_iNumX, &hitpos))
				return;

			m_bClone = true;
			m_pickingPos = hitpos;
			
			

			hitpos.y = y.y;
			

			m_vAllPoint.push_back(hitpos);
			if (m_vAllPoint.size() >= 3)
			{
				_int lastindex = m_vAllPoint.size() - 1;

				// [수정 완료] m_vPoint의 사이즈가 아니라 lastindex를 기준으로 가져옵니다.
				_float3 p1 = m_vAllPoint[lastindex - 2];
				_float3 p2 = m_vAllPoint[lastindex - 1];
				_float3 p3 = m_vAllPoint[lastindex]; // 방금 피킹한 점

				// 만들어질 삼각형의 순번 (1번째, 2번째, 3번째...)
				_int iTriangleIndex = m_vAllPoint.size() - 2;

				// 홀수 번째 삼각형 (1, 3, 5...)은 정상 순서대로 묶습니다.
				if (iTriangleIndex % 2 == 1)
				{
					m_vPoint.push_back(p1);
					m_vPoint.push_back(p2);
					m_vPoint.push_back(p3);
				}
				// 짝수 번째 삼각형 (2, 4, 6...)은 시계 방향 유지를 위해 p1, p2 순서를 뒤집습니다!
				else
				{
					m_vPoint.push_back(p2); // p2가 먼저 들어감
					m_vPoint.push_back(p1); // p1이 뒤에 들어감
					m_vPoint.push_back(p3);
				}
			}


			// 이걸 써서 마우스 로 클릭클릭 하는느낌?

			 /*vPoints[0]	= _float3(0.f, 0.f, 10.f);
			 vPoints[1]		= _float3(10.f, 0.f, 10.f);
			 vPoints[2] = _float3(10.f, 0.f, 0.f);
			 WriteFile(hFile, vPoints, sizeof(_float3) * 3, &dwByte, nullptr);

			 vPoints[0] = _float3(0.f, 0.f, 20.f);
			 vPoints[1] = _float3(10.f, 0.f, 10.f);
			 vPoints[2] = _float3(0.f, 0.f, 10.f);
			 WriteFile(hFile, vPoints, sizeof(_float3) * 3, &dwByte, nullptr);

			 vPoints[0] = _float3(10.f, 0.f, 10.f);
			 vPoints[1] = _float3(20.f, 0.f, 0.f);
			 vPoints[2] = _float3(10.f, 0.f, 0.f);
			 WriteFile(hFile, vPoints, sizeof(_float3) * 3, &dwByte, nullptr);*/

			
		}
		

		

		

	
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
	if (ImGui::Button("NaviGation_Save"))
	{
		_ulong			dwByte = { };

		HANDLE			hFile = CreateFile(TEXT("../Bin/DataFiles/Navigation.dat"), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
		if (0 == hFile)
			return;

		WriteFile(hFile, m_vPoint.data(), sizeof(_float3) * m_vPoint.size(), &dwByte, nullptr);

		MSG_BOX("네비게이션 저장 완료");
		 
		CloseHandle(hFile);
	}


	ImGui::Checkbox("Navigation_Cell Create", &m_bNaviEditMode);


	ImGui::Begin("Texture Gallery");
	
	

	ImGui::Text("Select a Texture:");
	ImGui::Separator();

	// 벡터에 담긴 텍스쳐 개수만큼 반복합니다. (예: m_vecTextures)
	for (int i = 0; i < images.size(); ++i)
	{
		ImGui::PushID(i); // 버튼 ID 꼬임 방지용

		// 1. 현재 그리는 버튼이 '내가 선택한 인덱스'라면 파란색 배경을 깔아줍니다.
		if (m_SelectedTextureIndex == i)
		{
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.6f, 1.0f, 1.0f)); // 파란색
		}
		else
		{
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.3f, 0.3f, 0.3f, 1.0f));
		}

		// 2. 텍스트 대신 '이미지 버튼'을 그립니다! (크기 64x64 예시)
		// m_vecTextures[i] 는 유저님의 텍스쳐 포인터 변수에 맞게 수정해주세요.
		if (ImGui::ImageButton("##Image",
			(void*)m_vThum[i],
			ImVec2(64.f, 64.f),
			ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f), // 기본 UV 좌표
			ImVec4(0.0f, 0.0f, 0.0f, 0.0f),         // 배경색 (투명)
			ImVec4(1.0f, 1.0f, 1.0f, 1.0f)))        // 틴트색 (R, G, B, Alpha) -> 알파를 1.0으로 강제 고정!
		{
			m_SelectedTextureIndex = i;
		}

		// 3. 색상 설정 원상복구
		ImGui::PopStyleColor();

		// 4. 세로로 한 줄로 나오는 걸 방지하고, 바둑판(Grid)처럼 가로로 나열합니다.
		// 예: 창 가로 길이에 맞춰서 알아서 줄바꿈 되게 하거나, 4개마다 줄바꿈
		if ((i + 1) % 4 != 0)
		{
			ImGui::SameLine();
		}

		ImGui::PopID();
	}

	// 줄바꿈이 깔끔하게 끝나도록 빈 줄 하나 추가
	ImGui::NewLine();
	ImGui::Separator();

	// 적용 버튼
	if (ImGui::Button("Apply Texture to Object", ImVec2(-FLT_MIN, 30)))
	{
		// 선택된 텍스쳐가 있고 (인덱스가 -1이 아님)
		if (m_SelectedTextureIndex != -1)
		{
			if (pSelectedObject != nullptr)
			{
				CShader* shader = dynamic_cast<CShader*>(pSelectedObject->Get_Component(TEXT("Com_Shader")));
				CTexture* texture = dynamic_cast<CTexture*>(pSelectedObject->Get_Component(TEXT("Com_Texture")));


				texture->Set_Texture(images);
				texture->Set_NumShaderResource(images.size());
				pSelectedObject->Set_m_iNumTexture(m_SelectedTextureIndex);
				
			}
		}
	}

	ImGui::NewLine();
	ImGui::Separator();

	ImGui::Text("Tool Mode:");
	ImGui::Checkbox("Enable Create Mode", &bIsCreateMode);
	ImGui::Separator();

	ImGui::End();
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
	
	Safe_Release(pSelectedObject);
	Safe_Release(m_pGalleryTexture);
	Safe_Release(m_pThumnailsTexture);
	Safe_Release(m_pGameInstance);
	Safe_Release(m_pDeviceContext);
	Safe_Release(m_pDevice);


}
