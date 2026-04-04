#include "Bounding_Sphere.h"
#include "Bounding_OBB.h"
#include "Bounding_AABB.h"

#include "DebugDraw.h"

CBounding_Sphere::CBounding_Sphere(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CBounding(pDevice, pContext)
{
}

HRESULT CBounding_Sphere::Initialize(CBounding::BOUNDING_DESC* pDesc)
{
	auto	pBoundingDesc = static_cast<CBounding_Sphere::BOUNDING_SPHERE_DESC*>(pDesc);

	m_pOriginalDesc = new BoundingSphere(pBoundingDesc->vCenter, pBoundingDesc->fRadius);
	m_pDesc = new BoundingSphere(*m_pOriginalDesc);

	return S_OK;
}

void CBounding_Sphere::Update(_fmatrix WorldMatrix)
{

	m_pOriginalDesc->Transform(*m_pDesc, WorldMatrix);
}

_bool CBounding_Sphere::Intersect(CBounding* pTarget)
{
	_bool		isCollision = { false };

	const _char* pName = typeid(*pTarget).name();

	if (false == strcmp("class Engine::CBounding_AABB", pName))
		isCollision = m_pDesc->Intersects(*dynamic_cast<CBounding_AABB*>(pTarget)->Get_Desc());

	else if (false == strcmp("class Engine::CBounding_OBB", pName))
		isCollision = m_pDesc->Intersects(*dynamic_cast<CBounding_OBB*>(pTarget)->Get_Desc());
	else
		isCollision = m_pDesc->Intersects(*dynamic_cast<CBounding_Sphere*>(pTarget)->Get_Desc());


	return isCollision;
}


#ifdef _DEBUG

HRESULT CBounding_Sphere::Render(PrimitiveBatch<VertexPositionColor>* pBatch, _fvector vColor)
{
	DX::Draw(pBatch, *m_pDesc, vColor);

	return S_OK;
}

#endif

CBounding_Sphere* CBounding_Sphere::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CBounding::BOUNDING_DESC* pDesc)
{
	CBounding_Sphere* pInstance = new CBounding_Sphere(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pDesc)))
	{
		MSG_BOX("Failed to Created : CBounding_Sphere");
		Safe_Release(pInstance);
	}
	return pInstance;
}


void CBounding_Sphere::Free()
{
	__super::Free();

	Safe_Delete(m_pOriginalDesc);
	Safe_Delete(m_pDesc);

}
