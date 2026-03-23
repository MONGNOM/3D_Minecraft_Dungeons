#include "Client_Defines.h"
#include "GameInstance.h"
#include "ImGui_Panel_Hierarchy.h"
#include "Layer.h"
#include "GameObject.h"
#include "ImGui_Manager.h"
#include "ImGuizmo.h"


std::string WStringToString(const std::wstring& wstr)
{
    if (wstr.empty()) return std::string();

    // 1. 변환에 필요한 공간(바이트 수) 계산
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);

    // 2. 공간 확보 후 실제 변환 진행
    std::string strTo(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);

    return strTo;
}

CImGui_Panel_Hierarchy::CImGui_Panel_Hierarchy()
    : CImGui_Panel("HIERARCHY")
{
}

CImGui_Panel_Hierarchy::~CImGui_Panel_Hierarchy()
{
}

HRESULT CImGui_Panel_Hierarchy::Intiailzie()
{
    for (auto& pair : CGameInstance::GetInstance()->Get_Prototype(ETOI(LEVEL::GAMEPLAY)))
    {
        m_vecProtoName.push_back(pair.first);
        wcout << pair.first << endl;
    }

    return S_OK;
}

void CImGui_Panel_Hierarchy::Render()
{
    ImGui::Begin("Hierarchy");

    ImGuiTreeNodeFlags baseFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;

    const auto& player = CGameInstance::GetInstance()->Get_Layer(ETOI(LEVEL::GAMEPLAY));
    
  

    for (auto& pair : player)
    {
        int a = 0;
        wstring layerTag = pair.first;
        CLayer* layer = pair.second;
        std::string utf8LayerTag = WStringToString(layerTag);
        const auto& objects = layer->Get_GameObjects();

        if (ImGui::TreeNodeEx(utf8LayerTag.c_str(), baseFlags))
        {
            const auto& objects = layer->Get_GameObjects();
            for (CGameObject* pObj : objects)
            {
                ImGui::PushID(pObj);

                std::string utf8ObjName = WStringToString(pObj->Get_ObjectName());


                bool isSelected = (CImGui_Manager::GetInstance()->Get_SelectObject() == pObj);

                ImGuiSelectableFlags flags = ImGuiSelectableFlags_AllowDoubleClick;

                if (ImGui::Selectable(utf8ObjName.c_str(), isSelected, flags))
                {
                    CImGui_Manager::GetInstance()->Set_SelectObject(pObj);
                }

                if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
                {
                    // ---------------------------------------------------
                    //  여기에 카메라 이동 로직을 작성합니다!
                    // ---------------------------------------------------

                    // [예시] 오브젝트의 3D 월드 좌표를 가져옵니다. (가상의 함수)
                    // Vector3 objPos = pObj->Get_Transform()->Get_Position();

                    // [예시] 카메라를 오브젝트의 정면에서 살짝 떨어진 곳으로 이동시킵니다.
                    // pCamera->Set_Position(objPos + Vector3(0.0f, 2.0f, -5.0f)); 
                    // pCamera->LookAt(objPos);
                    CTransform* cameraTransform = dynamic_cast<CTransform*>(CGameInstance::GetInstance()->Get_Component(TEXT("Camera"), TEXT("Layer_Camera"), ETOI(LEVEL::GAMEPLAY), TEXT("Com_Transform")));
                    CTransform* pObjTransform = dynamic_cast<CTransform*>(CGameInstance::GetInstance()->Get_Component(CImGui_Manager::GetInstance()->Get_SelectObject()->Get_ObjectName(), TEXT("Layer_Clone"), ETOI(LEVEL::GAMEPLAY), TEXT("Com_Transform")));
                    
                    _float3 objPos;
                    XMStoreFloat3(&objPos, pObjTransform->Get_State(STATE::POSITION));

                    
                    cameraTransform->Set_State(STATE::POSITION, XMVectorSet(objPos.x + 0.4, objPos.y + 5.5f, objPos.z -4,  1.f));

                    cout << "현재 오브젝트 X값 : " << objPos.x << "\t Y값 : "<< objPos.y << "\t Z값 : " << objPos.z << endl;
                    cout << "카메라 날라가는중~" << endl;
                }
                
                ImGui::PopID();
            }
            ImGui::TreePop(); 
        }
    }

    ImGui::End();
    
    ImGui::Begin("Inspector");

    if (CImGui_Manager::GetInstance()->Get_SelectObject() != nullptr)
    {
        string utf8Name = WStringToString(CImGui_Manager::GetInstance()->Get_SelectObject()->Get_ObjectName());
        ImGui::Text("Object Name: %s", utf8Name.c_str());
        ImGui::Separator(); // 가로줄 긋기

      
        CTransform* pTransform = dynamic_cast<CTransform*>(CImGui_Manager::GetInstance()->Get_SelectObject()->Get_Component(TEXT("Com_Transform")));

        if (pTransform != nullptr)
        {
            _float3 vscale = pTransform->Get_Scaled();
            _float3 vRot = pTransform->Get_Rotation();
            _float3 vPos{};  
            XMStoreFloat3(&vPos , pTransform->Get_State(STATE::POSITION));

            if (ImGui::DragFloat("Scale", &vscale.x, 0.1f))
            {
                if (vscale.x > 0)
                    pTransform->SetUp_Scale(vscale.x, vscale.x, vscale.x);
            }

            if (ImGui::DragFloat3("Rotation", (float*)&vRot, 0.1f))
            {
                 pTransform->Set_Rotation(vRot);
            }
            

            if (ImGui::DragFloat3("Position", &vPos.x, 0.1f))
            {
                pTransform->Set_State(STATE::POSITION, XMVectorSet(vPos.x, vPos.y, vPos.z, 1));
            }

        }
    }
    else
    {
        // 선택된 게 없을 때 안내 문구
        ImGui::Text("Please select an object");
    }

    ImGui::End();



    // (참고용 변수) 현재 유저가 클릭해둔 프로토타입의 이름을 기억할 변수
   
    // 1. 창 시작
    ImGui::Begin("Object Creator");

    ImGui::Text("Prototype List");

    if (ImGui::BeginListBox("##ProtoTypes", ImVec2(-FLT_MIN, 5 * ImGui::GetTextLineHeightWithSpacing())))
    {
        int id_counter = 0;

        for (auto& pairName : m_vecProtoName)
        {
            ImGui::PushID(id_counter);
            string utf8Name = WStringToString(pairName);

            // 1. 현재 항목이 선택된 상태인지 확인
            bool isSelected = (selectedPrototypeName == utf8Name);

            // 2. 화면에 보여줄 이름 가공 (선택되었으면 화살표를 붙임!)
            string displayLabel = utf8Name;
            if (isSelected)
            {
                displayLabel = "Pick =========" + displayLabel + "========="; // 화살표 추가
            }

            // 3. 항목 그리기 및 클릭 체크
            //  주의: (char*) 강제 형변환을 완전히 제거했습니다!
            // isSelected 변수가 true로 들어가면 ImGui가 알아서 '파란색 하이라이트'를 깔아줍니다.
            if (ImGui::Selectable(displayLabel.c_str(), isSelected))
            {
                selectedPrototypeName = utf8Name; // 비교용 이름 기억
                cloneName = pairName;             // 클론(Clone)할 때 쓸 원본 wstring 기억
            }

            // 선택된 항목에 포커스 유지
            if (isSelected)
                ImGui::SetItemDefaultFocus();

            ImGui::PopID();
            id_counter++;
        }

        ImGui::EndListBox();
    }

    ImGui::Separator();
    isClone = CImGui_Manager::GetInstance()->Get_isClone();
    // 3. 생성 버튼
   // if (ImGui::Button("Create to Clone", ImVec2(-FLT_MIN, 30))) // 버튼 크기 지정
    {
        if (selectedPrototypeName != "" && isClone)
        {
            

            CGameObject::GAMEOBJECT_DESC desc;
            desc.name = cloneName;
            desc.pos = CImGui_Manager::GetInstance()->Get_PickingPos();
            if (FAILED(CGameInstance::GetInstance()->Add_GameObject(ETOI(LEVEL::GAMEPLAY), cloneName,
                ETOI(LEVEL::GAMEPLAY), TEXT("Layer_Clone"), &desc)))
            {
                MSG_BOX("Editor: Failed to Clone");
            };
            isClone = false;
            CImGui_Manager::GetInstance()->Set_isClone(isClone);
            // ==========================================================
            //  [여기에 코드를 작성해주세요!] 
            // 1. 프로토타입 매니저에서 selectedPrototypeName 으로 원본 찾기
            // 2. 원본->Clone() 호출하여 새 오브젝트 생성
            // 3. 생성된 새 오브젝트를 현재 Scene(하이어라키) 리스트에 추가
            // ==========================================================
        }
    }

    

    ImGui::End();


    // =====================================================================
//  1. 윗 빵: 기즈모 도화지 세팅 (매 프레임 호출되어야 함)
// =====================================================================
    ImGuizmo::SetOrthographic(false);
    ImGuizmo::BeginFrame();

    //  수정 필요: 유저님의 윈도우 창 가로, 세로 길이를 넣어주세요! (예: 1920, 1080)
    ImGuizmo::SetRect(0.f, 0.f, 1920.f, 1080.f);

    if (CImGui_Manager::GetInstance()->Get_SelectObject() != nullptr)
    {
        // =====================================================================
        //  1-2. 윗 빵: 행렬 가져오기 
        // =====================================================================
        //  수정 필요: 유저님의 엔진에서 카메라 View, Proj 행렬을 가져오는 코드로 바꾸세요!
        _float4x4 ViewMatrix = *CGameInstance::GetInstance()->Get_Transform(D3DTS::VIEW);
        _float4x4 ProjMatrix = *CGameInstance::GetInstance()->Get_Transform(D3DTS::PROJ);

        CTransform* pTransform = dynamic_cast<CTransform*>(CImGui_Manager::GetInstance()->Get_SelectObject()->Get_Component(TEXT("Com_Transform")));
        _float4x4 WorldMatrix = *pTransform->Get_WorldMatrixPtr();

        // =====================================================================
        //  2. 고기 패티: 조작 및 마법의 함수 (유저님이 보여주신 코드)
        // =====================================================================
        static ImGuizmo::OPERATION mCurrentGizmoOperation = ImGuizmo::TRANSLATE;

        if (ImGui::IsKeyPressed(ImGuiKey_W)) mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
        if (ImGui::IsKeyPressed(ImGuiKey_E)) mCurrentGizmoOperation = ImGuizmo::ROTATE;
        if (ImGui::IsKeyPressed(ImGuiKey_R)) mCurrentGizmoOperation = ImGuizmo::SCALE;

        ImGuizmo::Manipulate(
            &ViewMatrix.m[0][0],
            &ProjMatrix.m[0][0],
            mCurrentGizmoOperation,
            ImGuizmo::WORLD,
            &WorldMatrix.m[0][0]
        );

        // =====================================================================
        //  3. 밑 빵: 결과 적용하기 (가장 중요!)
        // =====================================================================
        // 만약 유저가 기즈모 화살표를 마우스로 잡고 드래그했다면?
        if (ImGuizmo::IsUsing())
        {
            // 바뀐 월드 행렬을 건물에게 다시 덮어씌워서 실제로 맵에서 움직이게 만듭니다!
            pTransform->Set_WorldMatrix(WorldMatrix);
        }
    }


}

CImGui_Panel_Hierarchy* CImGui_Panel_Hierarchy::Create()
{
    CImGui_Panel_Hierarchy* pInstance = new CImGui_Panel_Hierarchy();
    if (FAILED(pInstance->Intiailzie()))
    {
        MSG_BOX("CImGui_Panel_Hierarchy::Create, Failed");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CImGui_Panel_Hierarchy::Free()
{
    __super::Free();
}
