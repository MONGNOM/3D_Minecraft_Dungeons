#include "Bounding_OBB.h"

#include "DebugDraw.h"

CBounding_OBB::CBounding_OBB(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CBounding(pDevice, pContext)
{
}

HRESULT CBounding_OBB::Initialize(CBounding::BOUNDING_DESC* pDesc)
{
	auto	pBoundingDesc = static_cast<CBounding_OBB::tagBoundingOBBDesc*>(pDesc);

	_float4	vRotation = {};

	XMStoreFloat4(&vRotation, XMQuaternionRotationRollPitchYaw(pBoundingDesc->vRadians.x, pBoundingDesc->vRadians.y, pBoundingDesc->vRadians.z));

	// pBoundingDesc->vRadians

	m_pOriginalDesc = new BoundingOrientedBox(pBoundingDesc->vCenter, pBoundingDesc->vExtents, vRotation);
	m_pDesc = new BoundingOrientedBox(*m_pOriginalDesc);

	return S_OK;
}

void CBounding_OBB::Update(_fmatrix WorldMatrix)
{

	m_pOriginalDesc->Transform(*m_pDesc, WorldMatrix);
}

_bool CBounding_OBB::Intersect(CBounding* pTarget)
{
	return _bool();
}


#ifdef _DEBUG

HRESULT CBounding_OBB::Render(PrimitiveBatch<VertexPositionColor>* pBatch, _fvector vColor)
{
	DX::Draw(pBatch, *m_pDesc, vColor);

	return S_OK;
}

#endif

CBounding_OBB* CBounding_OBB::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CBounding::BOUNDING_DESC* pDesc)
{
	CBounding_OBB* pInstance = new CBounding_OBB(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pDesc)))
	{
		MSG_BOX("Failed to Created : CBounding_OBB");
		Safe_Release(pInstance);
	}
	return pInstance;
}


void CBounding_OBB::Free()
{
	__super::Free();

	Safe_Delete(m_pOriginalDesc);
	Safe_Delete(m_pDesc);
	

}
