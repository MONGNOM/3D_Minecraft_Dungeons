#include "Bounding_AABB.h"
#include "Bounding_OBB.h"
#include "Bounding_Sphere.h"

#include "DebugDraw.h"

CBounding_AABB::CBounding_AABB(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CBounding(pDevice, pContext)
{
}

HRESULT CBounding_AABB::Initialize(CBounding::BOUNDING_DESC* pDesc)
{
	auto	pBoundingDesc = static_cast<CBounding_AABB::tagBoundingAABBDesc*>(pDesc);

	m_pOriginalDesc = new BoundingBox(pBoundingDesc->vCenter, pBoundingDesc->vExtents);
	m_pDesc = new BoundingBox(*m_pOriginalDesc);
	m_pOwner = pBoundingDesc->owner;

	return S_OK;
}

void CBounding_AABB::Update(_fmatrix WorldMatrix)
{
	_matrix		TransformMatrix = WorldMatrix;

	TransformMatrix.r[0] = XMVectorSet(1.f, 0.f, 0.f, 0.f) * XMVector3Length(WorldMatrix.r[0]);
	TransformMatrix.r[1] = XMVectorSet(0.f, 1.f, 0.f, 0.f) * XMVector3Length(WorldMatrix.r[1]);
	TransformMatrix.r[2] = XMVectorSet(0.f, 0.f, 1.f, 0.f) * XMVector3Length(WorldMatrix.r[2]);

	m_pOriginalDesc->Transform(*m_pDesc, TransformMatrix);
}

_bool CBounding_AABB::Intersect(CBounding* pTarget)
{
	_bool isCollision = { false };

	const _char* pName = typeid(*pTarget).name();

	if (false == strcmp("class Engine::CBounding_AABB", pName))
		isCollision = Intersect_ToAABB(dynamic_cast<CBounding_AABB*>(pTarget));
	else if (false == strcmp("class Engine::CBounding_OBB", pName))
		isCollision = m_pDesc->Intersects(*dynamic_cast<CBounding_OBB*>(pTarget)->Get_Desc());
	else
		isCollision = m_pDesc->Intersects(*dynamic_cast<CBounding_Sphere*>(pTarget)->Get_Desc());


	return isCollision;
}




#ifdef _DEBUG

HRESULT CBounding_AABB::Render(PrimitiveBatch<VertexPositionColor>* pBatch, _fvector vColor)
{
	DX::Draw(pBatch, *m_pDesc, vColor);

	return S_OK;
}

_bool CBounding_AABB::Intersect_ToAABB(CBounding_AABB* pTarget)
{
	_float3 srcMax, DestMax, srcMin, DestMin;
	
	srcMin  = Compute_Min();
	srcMax	= Compute_Max();

	DestMin = pTarget->Compute_Min();
	DestMax = pTarget->Compute_Max();

	if (max(srcMin.x, DestMin.x) > min(srcMax.x, DestMax.x))
		return false;

	if (max(srcMin.y, DestMin.y) > min(srcMax.y, DestMax.y))
		return false;

	if (max(srcMin.z, DestMin.z) > min(srcMax.z, DestMax.z))
		return false;

	return true;
}

_float3 CBounding_AABB::Compute_Max()
{
	return _float3(m_pDesc->Center.x  + m_pDesc->Extents.x,
					m_pDesc->Center.y + m_pDesc->Extents.y,
					m_pDesc->Center.z + m_pDesc->Extents.z);
}

_float3 CBounding_AABB::Compute_Min()
{
	return _float3( m_pDesc->Center.x - m_pDesc->Extents.x,
					m_pDesc->Center.y - m_pDesc->Extents.y,
					m_pDesc->Center.z - m_pDesc->Extents.z);
}

#endif

CBounding_AABB* CBounding_AABB::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CBounding::BOUNDING_DESC* pDesc)
{
	CBounding_AABB* pInstance = new CBounding_AABB(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pDesc)))
	{
		MSG_BOX("Failed to Created : CBounding_AABB");
		Safe_Release(pInstance);
	}
	return pInstance;
}


void CBounding_AABB::Free()
{
	__super::Free();

	Safe_Delete(m_pOriginalDesc);
	Safe_Delete(m_pDesc);

}

