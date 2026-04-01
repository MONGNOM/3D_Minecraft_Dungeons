#pragma once

#include "Base.h"

NS_BEGIN(Engine)

class CPicking_Manager : public CBase
{
private:
	CPicking_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CPicking_Manager() = default;

	HRESULT Initialize();

public:
	_bool Picking_Pos(HWND hWnd, class CVIBuffer_Terrain* pTerrainBufferCom, class CTransform* pTerrainTransformCom, _uint numZ, _uint numX, _float3* pos);
	_bool Picking_Object(HWND hWnd, RayHit& hit);

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pDeviceContext = { nullptr };
	class CGameInstance* m_pGameInstance = { nullptr };

public:
	static CPicking_Manager* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

NS_END
