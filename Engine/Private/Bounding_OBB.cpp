#include "Bounding_AABB.h"
#include "Bounding_OBB.h"
#include "Bounding_Sphere.h"
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
	m_pOwner = pBoundingDesc->owner;

	return S_OK;
}

void CBounding_OBB::Update(_fmatrix WorldMatrix)
{

	m_pOriginalDesc->Transform(*m_pDesc, WorldMatrix);
}

_bool CBounding_OBB::Intersect(CBounding* pTarget)
{
	_bool		isCollision = { false };

	const _char* pName = typeid(*pTarget).name();

	if (false == strcmp("class Engine::CBounding_AABB", pName))
		isCollision = m_pDesc->Intersects(*dynamic_cast<CBounding_AABB*>(pTarget)->Get_Desc());

	else if (false == strcmp("class Engine::CBounding_OBB", pName))
		// isCollision = m_pDesc->Intersects(*dynamic_cast<CBounding_OBB*>(pTarget)->Get_Desc());
		isCollision = Intersect_ToOBB(dynamic_cast<CBounding_OBB*>(pTarget));

	else
		//isCollision = m_pDesc->Intersects(*dynamic_cast<CBounding_Sphere*>(pTarget)->Get_Desc());


	return isCollision;
}

bool CBounding_OBB::Intersect_ToOBB(CBounding_OBB* pTarget)
{
	OBB_DESC			OBBDesc[2] = {
		Compute_OBBDesc(),
		pTarget->Compute_OBBDesc()
	};

	_float			fDistance[3] = {};

	for (size_t i = 0; i < 2; i++)
	{
		for (size_t j = 0; j < 3; j++)
		{
			fDistance[0] = fabs(XMVectorGetX(XMVector3Dot(
				XMLoadFloat3(&OBBDesc[1].vCenter) - XMLoadFloat3(&OBBDesc[0].vCenter),
				XMLoadFloat3(&OBBDesc[i].vAlignDir[j])
			)));

			fDistance[1] =
				fabs(XMVectorGetX(XMVector3Dot(
					XMLoadFloat3(&OBBDesc[0].vCenterDir[0]),
					XMLoadFloat3(&OBBDesc[i].vAlignDir[j])
				))) +
				fabs(XMVectorGetX(XMVector3Dot(
					XMLoadFloat3(&OBBDesc[0].vCenterDir[1]),
					XMLoadFloat3(&OBBDesc[i].vAlignDir[j])
				))) +
				fabs(XMVectorGetX(XMVector3Dot(
					XMLoadFloat3(&OBBDesc[0].vCenterDir[2]),
					XMLoadFloat3(&OBBDesc[i].vAlignDir[j])
				)));

			fDistance[2] =
				fabs(XMVectorGetX(XMVector3Dot(
					XMLoadFloat3(&OBBDesc[1].vCenterDir[0]),
					XMLoadFloat3(&OBBDesc[i].vAlignDir[j])
				))) +
				fabs(XMVectorGetX(XMVector3Dot(
					XMLoadFloat3(&OBBDesc[1].vCenterDir[1]),
					XMLoadFloat3(&OBBDesc[i].vAlignDir[j])
				))) +
				fabs(XMVectorGetX(XMVector3Dot(
					XMLoadFloat3(&OBBDesc[1].vCenterDir[2]),
					XMLoadFloat3(&OBBDesc[i].vAlignDir[j])
				)));


			if (fDistance[0] > fDistance[1] + fDistance[2])
				return false;

		}

	}





	return true;
}

CBounding_OBB::OBB_DESC CBounding_OBB::Compute_OBBDesc()
{
	OBB_DESC			OBBDesc{};

	_float3		vPoints[8] = {};
	m_pDesc->GetCorners(vPoints);

	OBBDesc.vCenter = m_pDesc->Center;

	_vector		vDir[3] = {
		XMLoadFloat3(&vPoints[5]) - XMLoadFloat3(&vPoints[4]),
		XMLoadFloat3(&vPoints[7]) - XMLoadFloat3(&vPoints[4]),
		XMLoadFloat3(&vPoints[0]) - XMLoadFloat3(&vPoints[4])
	};

	for (size_t i = 0; i < 3; i++)
	{
		XMStoreFloat3(&OBBDesc.vAlignDir[i], XMVector3Normalize(vDir[i]));
		XMStoreFloat3(&OBBDesc.vCenterDir[i], vDir[i] * 0.5f);
	}

	return OBBDesc;
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
