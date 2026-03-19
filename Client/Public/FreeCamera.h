#pragma once

#include "Client_Defines.h"
#include "Camera.h"

NS_BEGIN(Client)

class CFreeCamera final : public CCamera
{
public:
	typedef struct tagFreeCamera final : public CCamera::CAMERA_DESC
	{
		_float	fMouseSensor;
	}FREECAMERA_DESC;
private:
	CFreeCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CFreeCamera(const CFreeCamera& Prototype);
	virtual ~CFreeCamera() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	_float				m_fMouseSensor = {};
	_float				m_fMoveSpeed = {};

public:
	static CFreeCamera* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

};

NS_END