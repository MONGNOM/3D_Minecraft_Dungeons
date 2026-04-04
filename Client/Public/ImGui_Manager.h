#pragma once
#include "Base.h"
#include "Client_Defines.h"

NS_BEGIN(Engine)
class CGameInstance;
NS_END

NS_BEGIN(Client)

class CImGui_Manager final : public CBase
{
	DECLARE_SINGLETON(CImGui_Manager);
	enum class PanelType : unsigned int
	{
		INSPECTOR = 0,
		HIERARCHY,
		END
	};
private:
	CImGui_Manager();
	virtual ~CImGui_Manager() = default;
public:
	CGameObject* Get_SelectObject() { return pSelectedObject; }
	void Set_SelectObject(CGameObject* object) { pSelectedObject = object; }

	HRESULT Initialize_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	void Update_Engine();
	void Render();
	_float3 Get_PickingPos() { return m_pickingPos; }
	_bool Get_isClone() { return m_bClone;  }
	void Set_isClone(_bool isclone) { m_bClone = isclone; }
private:
	void Render_Panels();
private:
	CGameInstance* m_pGameInstance = { nullptr };
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pDeviceContext = { nullptr };
	class CImGui_Panel* m_pPanels[ETOI(PanelType::END)];
private:
	class CTexture* m_pGalleryTexture = nullptr; // 갤러리용 텍스쳐 포인터
	vector<ID3D11ShaderResourceView*> images = {};
	_float3 m_pickingPos{};
	_bool m_bClone  = false;
	int m_iNumX = 129;
	int m_iNumZ = 129;
	_bool m_bNaviEditMode = false;

	 CGameObject* pSelectedObject = nullptr;

	 vector<_float3> m_vPoint;

private:
	int m_SelectedTextureIndex = -1;
public:
	_bool			Picking_OnTerrain(HWND hWnd,
					class CVIBuffer_Terrain* pTerrainBufferCom,
					class CTransform* pTerrainTransformCom, 
					_uint numZ, _uint numX, _float3* pos);
public:
	_float			Compute_HeightOnTerrain(const _float3* pPos,
					const _float3* pTerrainVtxPos,
					const _ulong& dwCntX,
					const _ulong& dwCntZ,
					const _ulong& dwVtxItv = 1);

	_float3			Set_PickTerrain();

public:
	virtual void Free() override;
};

NS_END