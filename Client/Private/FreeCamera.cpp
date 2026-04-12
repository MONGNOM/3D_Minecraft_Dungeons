#include "FreeCamera.h"
#include "GameInstance.h"

CFreeCamera::CFreeCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCamera { pDevice, pContext }
{
}

CFreeCamera::CFreeCamera(const CFreeCamera& Prototype)
	: CCamera { Prototype }
{
}

HRESULT CFreeCamera::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CFreeCamera::Initialize(void* pArg)
{
	auto		pDesc = static_cast<FREECAMERA_DESC*>(pArg);
	m_fMouseSensor = pDesc->fMouseSensor;
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_eObjectType = ENVIRONMENT;

	return S_OK;
}

void CFreeCamera::Priority_Update(_float fTimeDelta)
{
	switch (m_eSceneType)
	{
	case SCENETYPE::DUNGEON:
	{
		CTransform* pTerTransform = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(TEXT("Prototype_GameObject_Player0"), TEXT("Load_Layer"), ETOI(m_eSceneType), TEXT("Com_Transform")));
		if (nullptr == pTerTransform) break;
		_float3 playerPos;
		XMStoreFloat3(&playerPos, pTerTransform->Get_State(STATE::POSITION));
		m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(playerPos.x - 15.f, playerPos.y + 20.f, playerPos.z - 15.f, 1.f));

		_vector vTargetPos = XMVectorSet(playerPos.x, playerPos.y + 1, playerPos.z, 1.f);
		m_pTransformCom->LookAt(vTargetPos);
		break;
	}	
	case SCENETYPE::GAMEPLAY :
		if (m_pGameInstance->Get_DIKeyState(DIK_LSHIFT))
			m_fMoveSpeed = fTimeDelta * 2;
		else
			m_fMoveSpeed = fTimeDelta;

		if (m_pGameInstance->Get_DIKeyState(DIK_E) & 0x80)
		{
			m_pTransformCom->Go_Up(m_fMoveSpeed);
		}

		if (m_pGameInstance->Get_DIKeyState(DIK_Q) & 0x80)
		{
			m_pTransformCom->Go_Down(m_fMoveSpeed);
		}

		if (m_pGameInstance->Get_DIKeyState(DIK_W) & 0x80)
		{
			m_pTransformCom->Go_Straight(m_fMoveSpeed);
		}

		if (m_pGameInstance->Get_DIKeyState(DIK_S) & 0x80)
		{
			m_pTransformCom->Go_Backward(m_fMoveSpeed);
		}

		if (m_pGameInstance->Get_DIKeyState(DIK_A) & 0x80)
		{
			m_pTransformCom->Go_Left(m_fMoveSpeed);
		}

		if (m_pGameInstance->Get_DIKeyState(DIK_D) & 0x80)
		{
			m_pTransformCom->Go_Right(m_fMoveSpeed);
		}

		_long		MouseMove = {};

		if (m_pGameInstance->Get_DIMouseState(DIMB::RBUTTON))
		{
			if (MouseMove = m_pGameInstance->Get_DIMouseMove(DIMM::X))
			{
				m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * MouseMove * m_fMouseSensor);
			}

			if (MouseMove = m_pGameInstance->Get_DIMouseMove(DIMM::Y))
			{
				m_pTransformCom->Turn(m_pTransformCom->Get_State(STATE::RIGHT), fTimeDelta * MouseMove * m_fMouseSensor);
			}
		}
		break;
	}
	__super::Update_TransformMatrices();
	
}

void CFreeCamera::Update(_float fTimeDelta)
{
}

void CFreeCamera::Late_Update(_float fTimeDelta)
{
}

HRESULT CFreeCamera::Render()
{
	return S_OK;
}

CFreeCamera* CFreeCamera::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CFreeCamera* pInstance = new CFreeCamera(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CFreeCamera");
		Safe_Release(pInstance);
	}
	return pInstance;
}


CGameObject* CFreeCamera::Clone(void* pArg)
{
	CFreeCamera* pInstance = new CFreeCamera(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CFreeCamera");
		Safe_Release(pInstance);
	}
	return pInstance;
}


void CFreeCamera::Free()
{
	__super::Free();


}
