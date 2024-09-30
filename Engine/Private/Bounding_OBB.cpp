#include "..\Public\Bounding_OBB.h"

#include "Bounding_AABB.h"
#include "Bounding_Sphere.h"

CBounding_OBB::CBounding_OBB(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CBounding { pDevice, pContext }
{
}

HRESULT CBounding_OBB::Initialize(CBounding::BOUNDING_DESC * pBoundingDesc)	
{
	/* 기초 로컬에서의 상태. */
	BOUNDING_OBB_DESC*		pDesc = static_cast<BOUNDING_OBB_DESC*>(pBoundingDesc);

	_float4			vQuaternion = {};
	XMStoreFloat4(&vQuaternion, XMQuaternionRotationRollPitchYaw(pDesc->vAngles.x, pDesc->vAngles.y, pDesc->vAngles.z));

	m_pBoundingDesc = new BoundingOrientedBox(pDesc->vCenter, pDesc->vExtents, vQuaternion);
	m_pOriginalBoundingDesc = new BoundingOrientedBox(*m_pBoundingDesc);

	return S_OK;
}

void CBounding_OBB::Update(_fmatrix WorldMatrix)
{
	m_pOriginalBoundingDesc->Transform(*m_pBoundingDesc, WorldMatrix);

	m_isPreColl = m_isCurColl;
}

HRESULT CBounding_OBB::Render(PrimitiveBatch<VertexPositionColor>* pBatch)
{
#ifdef _DEBUG
	DX::Draw(pBatch, *m_pBoundingDesc, m_isCurColl == true ? XMVectorSet(1.f, 0.f, 0.f, 1.f) : XMVectorSet(0.f, 1.f, 0.f, 1.f));
#endif

	return S_OK;
}

_bool CBounding_OBB::Intersect(CCollider::TYPE eColliderType, CBounding * pBounding)
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

CBounding_OBB * CBounding_OBB::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, CBounding::BOUNDING_DESC * pBoundingDesc)
{
	CBounding_OBB*		pInstance = new CBounding_OBB(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pBoundingDesc)))
	{
		MSG_BOX(TEXT("Failed to Created : CBounding_OBB"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBounding_OBB::Free()
{
	__super::Free();

	Safe_Delete(m_pBoundingDesc);
	Safe_Delete(m_pOriginalBoundingDesc);
}
