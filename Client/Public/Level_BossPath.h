#pragma once
#include "Client_Defines.h"
#include "Level.h"

NS_BEGIN(Client)
class CImGui_Manager;

class CLevel_BossPath : public CLevel
{
private:
	CLevel_BossPath(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_BossPath() = default;

public:
	virtual HRESULT Initialize() override;
	virtual void Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Ready_Lights();
	HRESULT Ready_Layer_Camera(const _wstring& strLayerTag);
	HRESULT Ready_Layer_BackGround(const _wstring& strLayerTag);
	HRESULT Ready_Layer_Player(const _wstring& strLayerTag);
	HRESULT Ready_Layer_Monster(const _wstring& strLayerTag);
	HRESULT Ready_Layer_UI(const _wstring& strLayerTag);
	HRESULT Ready_Layer_Load();
	
	HRESULT Add_HotBar(const _wstring& strLayerTag);

	CImGui_Manager* m_pImGui_Manager = { nullptr };
public:
	static CLevel_BossPath* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;


};

NS_END
