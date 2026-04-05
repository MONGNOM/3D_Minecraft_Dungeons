#pragma once

#include "Prototype_Manager.h"

/* 엔진과 클라이언트 개발자와의 소통의 창구가 된다. */
/* 엔진 개발자가 클라이언트 개발자에게 보여주기위한 객체 및 기능들을 게임인스턴스 안에 선언, 정의한다. */
/* 클라개발자는 엔진기능쓰고자한다라면!! 게임인스턴스로 접근하여 호출하념뇓다. */
/* 게임인슽너스는 == 싱글턴. */
/* 기타 엔진을 이용하기위한 주요 매니져들의 사전 준비작업을 수앻나다. IOnmitialize_Engine */
/* 엔진의 갱신이 필요한 경우 모아서 매프레임마다 갱신(Updqte )을 수행해준다 Update_Engine */


NS_BEGIN(Engine)

class ENGINE_DLL CGameInstance final : public CBase
{
	DECLARE_SINGLETON(CGameInstance)
private:
	CGameInstance();
	virtual ~CGameInstance() = default;

public:
	HRESULT Initialize_Engine(const ENGINE_DESC& EngineDesc, _Out_ ID3D11Device** ppDevice, _Out_ ID3D11DeviceContext** ppContext);
	void Update_Engine(_float fTimeDelta);
	HRESULT Draw();
	void Clear_Resources(_uint iLevelIndex);

	_float Random(_float fMin, _float fMax);


public: /* For.Graphic_Device */
	HRESULT Clear_Buffers(const _float4* pClearColor);
	HRESULT Present();

public: /* For.Timer_Manager */
	HRESULT	Add_Timer(const _wstring& strTimerTag);
	_float Compute_TimeDelta(const _wstring& strTimerTag);

public: /* For.Level_Manager */
	HRESULT Change_Level(_uint iNewLevelIndex, class CLevel* pNewLevel);

public: /* For.Prototype_Manager */
	HRESULT Add_Prototype(_uint iLevelIndex, const _wstring& strPrototypeTag, CBase* pPrototype);
	CBase* Clone_Prototype(PROTOTYPE ePrototype, _uint iLevelIndex, const _wstring& strPrototypeTag, void* pArg = nullptr);
	map<const _wstring, CBase*>& Get_Prototype(_uint iLevelIndex) { return m_pPrototype_Manager->Get_Prototype(iLevelIndex); }

public: /* For.Object_Manager */
	HRESULT Add_GameObject(_uint iPrototypeLevelIndex, const _wstring& strPrototypeTag, _uint iLayerLevelIndex, const _wstring& strLayerTag, void* pArg = nullptr);
	class CComponent* Get_Component(_wstring objName, _wstring layName, _uint iLayerIndex, _wstring componentName);
	vector<class CGameObject*> Get_GameObjects(_uint iLayerIndex);

	map<const _wstring, class CLayer*>& Get_Layer(_uint iLayerIndex);


public: /* For.Renderer */
	void Add_RenderGroup(RENDERGROUP eRenderGroup, class CGameObject* pGameObject);

public: /* For.PipeLine */
	HRESULT Bind_CamPosition(class CShader* pShader, const _char* pConstantName);
	HRESULT Bind_TransformMatrix(D3DTS eTransformState, class CShader* pShader, const _char* pConstantName);
	HRESULT Bind_TransformMatrix_Inverse(D3DTS eTransformState, class CShader* pShader, const _char* pConstantName);
	const _float4x4* Get_Transform(D3DTS eTransformState) const;
	const _float4* Get_CamPosition() const;
	void Set_Transform(D3DTS eTransformState, _fmatrix TransformStateMatrix);

public: /* For.Input_Device */
	_byte	Get_DIKeyState(_ubyte byKeyID);
	_byte	Get_DIMouseState(DIMB eMouse);
	_long	Get_DIMouseMove(DIMM eMouseState);

	_bool	Get_DIKeyPress(_ubyte byKeyID);
	_bool	Get_DIKeyDown(_ubyte byKeyID);
	_bool	Get_DIKeyUp(_ubyte byKeyID);
	_bool	Get_DIMousePress(DIMB eMouse);
	_bool	Get_DIMouseDown(DIMB eMouse);
	_bool	Get_DIMouseUp(DIMB eMouse);

public: /* For.Light_Manager */
	const LIGHT_DESC* Get_LightDesc(_uint iIndex);
	HRESULT Add_Light(const LIGHT_DESC& LightDesc);

public: /* For.Picking_Manager */
	_bool Picking_Pos(HWND hWnd, class CVIBuffer_Terrain* pTerrainBufferCom, class CTransform* pTerrainTransformCom, _uint numZ, _uint numX, _float3* pos);
	_bool Picking_Object(HWND hWnd, RayHit& hit);

public: /* For.Font_Manager */
	HRESULT Add_Font(const _wstring& strFontTag, const _tchar* pFontFilePath);
	void Draw_Font(const _wstring& strFontTag, const _tchar* pText, const _float2& vPosition, _fvector vColor = XMVectorSet(1.f, 1.f, 1.f, 1.f));


public: /* For.ModelConverter */
	HRESULT Ready_StaticBinary(_uint numMeshs, const aiScene* m_pAIScene, const string& name, _fmatrix PreTransformMatrix);
	HRESULT Ready_DynamicBinary(_uint numMeshs, const aiScene* m_pAIScene, const string& name, class CModel* pModel, vector<class CBone*>& bones);

public: /* For.CDataManaget */
	HRESULT Save_Date(const vector<tagObjectInfo>& objectinfo);
	HRESULT Load_Date(const _tchar* filePath, vector<tagObjectInfo>& objectinfo);

private:
	class CGraphic_Device* m_pGraphic_Device = { nullptr };
	class CTimer_Manager* m_pTimer_Manager = { nullptr };
	class CLevel_Manager* m_pLevel_Manager = { nullptr };
	class CPrototype_Manager* m_pPrototype_Manager = { nullptr };
	class CObject_Manager* m_pObject_Manager = { nullptr };
	class CRenderer* m_pRenderer = { nullptr };
	class CPipeLine* m_pPipeLine = { nullptr };
	class CInput_Device* m_pInput_Device = { nullptr };
	class CLight_Manager* m_pLight_Manager = { nullptr };
	class CPicking_Manager* m_pPicking_Manager = { nullptr };
	class CFont_Manager* m_pFont_Manager = { nullptr };
	class CModelConverter* m_pModelConverter = { nullptr };
	class CDataManager* m_pDataManager = { nullptr };

public:
	void Release_Engine();
	virtual void Free() override;
};

NS_END