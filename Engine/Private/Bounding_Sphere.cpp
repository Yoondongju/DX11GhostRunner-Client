#include "..\Public\Bounding_Sphere.h"

#include "Bounding_AABB.h"
#include "Bounding_OBB.h"

CBounding_Sphere::CBounding_Sphere(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CBounding { pDevice, pContext }
{
}

HRESULT CBounding_Sphere::Initialize(CBounding::BOUNDING_DESC * pBoundingDesc)	
{
	/* 기초 로컬에서의 상태. */
	BOUNDING_SPHERE_DESC*		pDesc = static_cast<BOUNDING_SPHERE_DESC*>(pBoundingDesc);

	m_pBoundingDesc = new BoundingSphere(pDesc->vCenter, pDesc->fRadius);
	m_pOriginalBoundingDesc = new BoundingSphere(*m_pBoundingDesc);

	return S_OK;
}

void CBounding_Sphere::Update(_fmatrix WorldMatrix)
{
	m_pOriginalBoundingDesc->Transform(*m_pBoundingDesc, WorldMatrix);

	m_isPreColl = m_isCurColl;
}

HRESULT CBounding_Sphere::Render(PrimitiveBatch<VertexPositionColor>* pBatch)
{
#ifdef _DEBUG
	DX::Draw(pBatch, *m_pBoundingDesc, m_isCurColl == true ? XMVectorSet(1.f, 0.f, 0.f, 1.f) : XMVectorSet(0.f, 1.f, 0.f, 1.f));
#endif

	return S_OK;
}

_bool CBounding_Sphere::Intersect(CCollider::TYPE eColliderType, CBounding * pBounding)
{
	m_isCurColl = false;


	if (isnan(m_pBoundingDesc->Center.x) ||
		isnan(m_pBoundingDesc->Center.y) ||
		isnan(m_pBoundingDesc->Center.z))
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

CBounding_Sphere * CBounding_Sphere::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, CBounding::BOUNDING_DESC * pBoundingDesc)
{
	CBounding_Sphere*		pInstance = new CBounding_Sphere(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pBoundingDesc)))
	{
		MSG_BOX(TEXT("Failed to Created : CBounding_Sphere"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBounding_Sphere::Free()
{
	__super::Free();

	Safe_Delete(m_pBoundingDesc);
	Safe_Delete(m_pOriginalBoundingDesc);
}
