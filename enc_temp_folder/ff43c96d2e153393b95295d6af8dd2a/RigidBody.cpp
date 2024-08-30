#include "RigidBody.h"

#include "GameObject.h"
#include "Transform.h"

CRigidBody::CRigidBody(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent{ pDevice,pContext}
{
}

CRigidBody::CRigidBody(const CRigidBody& Prototype)
	: CComponent{ Prototype }
{

}

void CRigidBody::Set_OwnerTransform(CTransform* pOwnerTransform, _float fOwnerOffsetY)
{
	if (pOwnerTransform)
	{
		m_pOwnerTransform = pOwnerTransform;
		m_fOwnerOffsetY = fOwnerOffsetY;
		//Safe_AddRef(m_pOwner);	�̰� ������Ÿ�Կ� ������ְ� �������� �ϸ� ���� �Ȼ���µ� ���纻���� �ٷ� �����ھƼ� ��ȣ ����������
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

void CRigidBody::Update(_float fTimeDelta)
{
	//�������� ���� ���� ���� ����
	//���� ���� �ӵ��� ����

	m_fTime += fTimeDelta;
	

	// ���ӵ��� �ְ� 1�ʵ��� �ӵ��� ������Ų��.
	if (m_fTime <= 0.5f && XMVectorGetX(XMLoadFloat3(&m_vAccel)) != 0.f)
	{
		_vector AddAccel = XMLoadFloat3(&m_vAccel) * m_fTime * 4;
		XMStoreFloat3(&m_vVelocity, XMLoadFloat3(&m_vVelocity) + AddAccel);
	}

	

	Calculate_Tranform(fTimeDelta);	
}

void CRigidBody::Calculate_Tranform(_float fTimeDelta)
{
	Calculate_Friction();

	//�߷� ���� && !m_isGround
	if (m_isGravity)
	{
		Calculate_Gravity();
	}
	
	
	if (nullptr == m_pOwnerTransform)
		return;

	_vector vCurrentPosition = m_pOwnerTransform->Get_State(CTransform::STATE_POSITION);

	//�ӵ� ����
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
}

void CRigidBody::Calculate_Gravity()
{
	_vector vGravity = { 0.f, m_fGravityAccel * m_fGravityScale, 0.f };

	if (fabsf(XMVectorGetY(vGravity) > m_fGravityLimit))
	{
		vGravity = XMVectorSetY(vGravity, m_fGravityLimit);
	}


	if (nullptr == m_pOwnerTransform)
		return;

	_vector vCurrentPosition = m_pOwnerTransform->Get_State(CTransform::STATE_POSITION);

	if(XMVectorGetY(vCurrentPosition) > m_fOwnerOffsetY)
		XMStoreFloat3(&m_vVelocity , XMLoadFloat3(&m_vVelocity) - vGravity * 4);
	else
	{
		vCurrentPosition = XMVectorSetY(vCurrentPosition, m_fOwnerOffsetY);
		m_pOwnerTransform->Set_State(CTransform::STATE_POSITION, vCurrentPosition);
	}
		

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
		//������ �߰����� ����(���Ը� ����)
	case Engine::CRigidBody::FORCE:
		//���ӵ��� = �� / ����
		XMStoreFloat3(&m_vAccel, XMLoadFloat3(&m_vAccel) + vForce / m_fMass);
		break;

		//������ �߰����� ����(���Ը� ���� �� ��)
	case Engine::CRigidBody::ACCELERATION:
		XMStoreFloat3(&m_vAccel, XMLoadFloat3(&m_vAccel) + vForce);
		break;

		//���������� �о��� ����(���Ը� ����)
	case Engine::CRigidBody::IMPLUSE:
		XMStoreFloat3(&m_vVelocity, XMLoadFloat3(&m_vVelocity) + vForce / m_fMass);
		break;

		//���������� �о��� ����(���Ը� ���� �� ��)
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

	
	Add_Force(eForceMode, vForce); // ���� Add_Force �Լ� �̿�
}

CRigidBody* CRigidBody::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CRigidBody* pInstance = new CRigidBody(pDevice , pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CRigidBody"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

/* ������ü�� ã�Ƽ� ������ü�� Clone�Լ��� ȣ���Ѵ�.  */
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
