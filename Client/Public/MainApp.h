#pragma once

#include "Client_Defines.h"
#include "Base.h"


NS_BEGIN(Engine)
class CGameInstance;
NS_END

NS_BEGIN(Client)



class CMainApp final : public CBase
{
private:
	CMainApp();
	virtual ~CMainApp() = default;

public:
	HRESULT Initialize();
	void Update(_float fTimeDelta);
	HRESULT Render();
	
	HCURSOR g_hCustomCursor;

private:
	CGameInstance* m_pGameInstance = { nullptr };
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

private:
	HRESULT Ready_Fonts();
	HRESULT Ready_Prototype_For_Static_Level();
	HRESULT Ready_StartLevel(LEVEL eStartLevelID);
	HRESULT Ready_Layer_UI(const _tchar* pLayerTag);
private:
	

public:
	static CMainApp* Create();
	virtual void Free() override;
};

NS_END