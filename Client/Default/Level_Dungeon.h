#pragma once
#include "Client_Defines.h"
#include "Level.h"

NS_BEGIN(Client)
class CImGui_Manager;

class CLevel_Dungeon : public CLevel
{
private:
	CLevel_Dungeon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_Dungeon() = default;

public:
	virtual HRESULT Initialize() override;
	virtual void Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Ready_Lights();
	HRESULT Ready_Layer_Camera(const _wstring& strLayerTag);
	HRESULT Ready_Layer_BackGround(const _wstring& strLayerTag);
	HRESULT Ready_Layer_Player(const _wstring& strLayerTag);

	CImGui_Manager* m_pImGui_Manager = { nullptr };
public:
	static CLevel_Dungeon* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;


};

NS_END
