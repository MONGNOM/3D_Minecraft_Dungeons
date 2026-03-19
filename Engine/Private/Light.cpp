#include "Light.h"

CLight::CLight()
{
}

HRESULT CLight::Initialize(const LIGHT_DESC& LightDesc)
{
	memcpy(&m_LightDesc, &LightDesc, sizeof(LIGHT_DESC));

    return S_OK;
}

CLight* CLight::Create(const LIGHT_DESC& LightDesc)
{
	CLight* pInstance = new CLight();

	if (FAILED(pInstance->Initialize(LightDesc)))
	{
		MSG_BOX("Failed to Created : CLight");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLight::Free()
{
    __super::Free();


}
