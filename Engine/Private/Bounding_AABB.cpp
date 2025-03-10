#include "..\Public\Bounding_AABB.h"

#include "Bounding_OBB.h"
#include "Bounding_Sphere.h"

CBounding_AABB::CBounding_AABB(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CBounding { pDevice, pContext }
{
}

HRESULT CBounding_AABB::Initialize(CBounding::BOUNDING_DESC * pBoundingDesc)	
{
	/* 기초 로컬에서의 상태. */
	BOUNDING_AABB_DESC*		pDesc = static_cast<BOUNDING_AABB_DESC*>(pBoundingDesc);

	m_pOriginalBoundingDesc = new BoundingBox(pDesc->vCenter, pDesc->vExtents);
	m_pBoundingDesc = new BoundingBox(*m_pOriginalBoundingDesc);

	return S_OK;
}

void CBounding_AABB::Update(_fmatrix WorldMatrix)
{
	_matrix			TransformMatrix = WorldMatrix;

	TransformMatrix.r[0] = XMVectorSet(1.f, 0.f, 0.f, 0.f) * XMVector3Length(WorldMatrix.r[0]);
	TransformMatrix.r[1] = XMVectorSet(0.f, 1.f, 0.f, 0.f) * XMVector3Length(WorldMatrix.r[1]);
	TransformMatrix.r[2] = XMVectorSet(0.f, 0.f, 1.f, 0.f) * XMVector3Length(WorldMatrix.r[2]);

	m_pOriginalBoundingDesc->Transform(*m_pBoundingDesc, TransformMatrix);


	m_isPreColl = m_isCurColl;
}

HRESULT CBounding_AABB::Render(PrimitiveBatch<VertexPositionColor>* pBatch)
{
#ifdef _DEBUG
	DX::Draw(pBatch, *m_pBoundingDesc, m_isCurColl == true ? XMVectorSet(1.f, 0.f, 0.f, 1.f) : XMVectorSet(0.f, 1.f, 0.f, 1.f));
#endif

	return S_OK;
}

_bool CBounding_AABB::Intersect(CCollider::TYPE eColliderType, CBounding * pBounding)
{
	m_isCurColl = false;

	
	if (isnan(m_pBoundingDesc->Center.x) ||
		isnan(m_pBoundingDesc->Center.y) ||
		isnan(m_pBoundingDesc->Center.z) )
	{
		return false;
	}


	switch (eColliderType)
	{
	case CCollider::TYPE_AABB:
	{
		const BoundingBox* pBoundingBox = static_cast<CBounding_AABB*>(pBounding)->Get_Desc();

		if (isnan(pBoundingBox->Center.x) ||
			isnan(pBoundingBox->Center.y) ||
			isnan(pBoundingBox->Center.z))
		{
			return false;
		}
		m_isCurColl = m_pBoundingDesc->Intersects(*pBoundingBox);
	}
		break;

	case CCollider::TYPE_OBB:
	{
		const BoundingOrientedBox* pBoundingBox = static_cast<CBounding_OBB*>(pBounding)->Get_Desc();

		if (isnan(pBoundingBox->Center.x) ||
			isnan(pBoundingBox->Center.y) ||
			isnan(pBoundingBox->Center.z))
		{
			return false;
		}
		m_isCurColl = m_pBoundingDesc->Intersects(*pBoundingBox);
	}
		break;

	case CCollider::TYPE_SPHERE:
	{
		const BoundingSphere* pBoundingBox = static_cast<CBounding_Sphere*>(pBounding)->Get_Desc();

		if (isnan(pBoundingBox->Center.x) ||
			isnan(pBoundingBox->Center.y) ||
			isnan(pBoundingBox->Center.z))
		{
			return false;
		}
		m_isCurColl = m_pBoundingDesc->Intersects(*pBoundingBox);
	}
		break;
	}

	return m_isCurColl;
}

CBounding_AABB * CBounding_AABB::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, CBounding::BOUNDING_DESC * pBoundingDesc)
{
	CBounding_AABB*		pInstance = new CBounding_AABB(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pBoundingDesc)))
	{
		MSG_BOX(TEXT("Failed to Created : CBounding_AABB"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBounding_AABB::Free()
{
	__super::Free();

	Safe_Delete(m_pBoundingDesc);
	Safe_Delete(m_pOriginalBoundingDesc);
}
