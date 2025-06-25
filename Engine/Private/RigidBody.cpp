#include "RigidBody.h"

#include "GameObject.h"
#include "Transform.h"

CRigidBody::CRigidBody(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent{ pDevice,pContext }
{
}

CRigidBody::CRigidBody(const CRigidBody& Prototype)
	: CComponent{ Prototype }
{

}

void CRigidBody::Set_OwnerTransform(CTransform* pOwnerTransform)
{
	if (pOwnerTransform)
	{
		m_pOwnerTransform = pOwnerTransform;
		//Safe_AddRef(m_pOwner);	이게 프로토타입에 만들어주고 얕은복사 하면 릭이 안생기는데 복사본에다 바로 때려박아서 상호 참조때문임
	}

}

HRESULT CRigidBody::Initialize_Prototype()
{

	return S_OK;
}

HRESULT CRigidBody::Initialize(void* pArg)
{
	return S_OK;
}

void CRigidBody::Update(_float fTimeDelta, _float fTargetY, _bool isColl)
{
	m_fTime += fTimeDelta;

	// 가속도가 있고 1초동안 속도를 증가시킨다.
	if (m_fTime <= 0.5f && 
		(XMVectorGetX(XMLoadFloat3(&m_vAccel)) != 0.f || 
		XMVectorGetY(XMLoadFloat3(&m_vAccel)) != 0.f || 
		XMVectorGetZ(XMLoadFloat3(&m_vAccel)) != 0.f))
	{
		_vector AddAccel = XMLoadFloat3(&m_vAccel) * m_fTime * 4;
		XMStoreFloat3(&m_vVelocity, XMLoadFloat3(&m_vVelocity) + AddAccel);
	}




	Calculate_Tranform(fTimeDelta, fTargetY);
}

void CRigidBody::Calculate_Tranform(_float fTimeDelta, _float fTargetY)
{
	Calculate_Friction();
	if (m_isGravity)
	{
		Calculate_Gravity(fTargetY);
	}

	if (nullptr == m_pOwnerTransform)
		MSG_BOX(TEXT("RigidBody no"));

	_vector vCurrentPosition = m_pOwnerTransform->Get_State(CTransform::STATE_POSITION);
	if (fabsf(m_vMaxVelocity.x) < fabsf(m_vVelocity.x))
	{
		m_vVelocity.x = (m_vVelocity.x / fabsf(m_vVelocity.x)) * fabsf(m_vMaxVelocity.x);
	}

	if (fabsf(m_vMaxVelocity.y) < fabsf(m_vVelocity.y))
	{
		m_vVelocity.y = (m_vVelocity.y / fabsf(m_vVelocity.y)) * fabsf(m_vMaxVelocity.y);
	}

	if (fabsf(m_vMaxVelocity.z) < fabsf(m_vVelocity.z))
	{
		m_vVelocity.z = (m_vVelocity.z / fabsf(m_vVelocity.z)) * fabsf(m_vMaxVelocity.z);
	}


	_vector vNewPosition = vCurrentPosition + (XMLoadFloat3(&m_vVelocity) * fTimeDelta);
	m_pOwnerTransform->Set_State(CTransform::STATE_POSITION, vNewPosition);

	if (XMVectorGetY(vNewPosition) < fTargetY)		
	{
		vNewPosition = XMVectorSetY(vNewPosition, fTargetY);
		m_pOwnerTransform->Set_State(CTransform::STATE_POSITION, vNewPosition);
	}

}

void CRigidBody::Calculate_Gravity(_float fTargetY)
{
	if (nullptr == m_pOwnerTransform)
		return;

	_vector vCurrentPosition = m_pOwnerTransform->Get_State(CTransform::STATE_POSITION);
	_float currentHeight = XMVectorGetY(vCurrentPosition);

	_float heightDifference = fabs(fTargetY - currentHeight);

	_float fHeightFactor = 0.0f;
	if (heightDifference > 0.0f)
	{
		fHeightFactor = heightDifference / 70.f + 0.1f;  

		fHeightFactor = max(fHeightFactor, 3.f); 
		fHeightFactor = min(fHeightFactor, 4.5f);  
	}

	_float fAdjustedGravityAccel = m_fGravityAccel * fHeightFactor;
	_vector vGravity = { 0.f, fAdjustedGravityAccel * 3.f, 0.f };

	if (fabsf(XMVectorGetY(vGravity)) > m_fGravityLimit)
	{
		vGravity = XMVectorSetY(vGravity, m_fGravityLimit);
	}
	XMStoreFloat3(&m_vVelocity, XMLoadFloat3(&m_vVelocity) - vGravity * m_fReducedGravity);
}



void CRigidBody::Calculate_Friction()
{
	_vector vVelocity = XMLoadFloat3(&m_vVelocity);
	_float velocityMagnitude = XMVectorGetX(XMVector3Length(vVelocity));

	if (velocityMagnitude > 0.f)
	{
		_vector vFrictionDir = vVelocity * -1.f;
		_vector vFriction = vFrictionDir * m_fFriction;
		_vector vNewVelocity = vVelocity + vFriction;

		_float newVelocityMagnitude = XMVectorGetX(XMVector3Length(vNewVelocity));
		if (newVelocityMagnitude < 0.01f)
		{
			m_vVelocity = { 0.f, 0.f, 0.f };
			m_vAccel = { 0.f, 0.f, 0.f };
		}
		else
		{
			XMStoreFloat3(&m_vVelocity, vNewVelocity);
		}
	}
}

void CRigidBody::Add_Force(FORCEMODE eForceMode, _fvector vForce)
{
	m_fTime = 0.f;

	switch (eForceMode)
	{
	case Engine::CRigidBody::FORCE:
		XMStoreFloat3(&m_vAccel, XMLoadFloat3(&m_vAccel) + vForce / m_fMass);
		break;

	case Engine::CRigidBody::ACCELERATION:
		XMStoreFloat3(&m_vAccel, XMLoadFloat3(&m_vAccel) + vForce);
		break;

	case Engine::CRigidBody::IMPLUSE:
		XMStoreFloat3(&m_vVelocity, XMLoadFloat3(&m_vVelocity) + vForce / m_fMass);
		break;

	case Engine::CRigidBody::VELOCITYCHANGE:
		XMStoreFloat3(&m_vVelocity, XMLoadFloat3(&m_vVelocity) + vForce);
		break;

	case Engine::CRigidBody::MODE_END:
		break;
	default:
		break;
	}
}

void CRigidBody::Add_Force_Direction(_fvector vDirection, _float fMagnitude, FORCEMODE eForceMode)
{
	_vector vForce = XMVector3Normalize(vDirection) * fMagnitude;


	Add_Force(eForceMode, vForce); // 기존 Add_Force 함수 이용
}

CRigidBody* CRigidBody::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CRigidBody* pInstance = new CRigidBody(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CRigidBody"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

/* 원형객체를 찾아서 원형객체의 Clone함수를 호출한다.  */
CComponent* CRigidBody::Clone(void* pArg)
{
	CRigidBody* pInstance = new CRigidBody(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CRigidBody"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRigidBody::Free()
{
	__super::Free();


}
