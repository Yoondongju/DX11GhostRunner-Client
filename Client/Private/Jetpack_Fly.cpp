#include "stdafx.h"
#include "Jetpack_Fly.h"


#include "Jetpack.h"
#include "GameInstance.h"
#include "Animation.h"


CJetpack_Fly::CJetpack_Fly(class CGameObject* pOwner)
	: CState{ CJetpack::JETPACK_ANIMATION::FLY_F , pOwner }
{

}

HRESULT CJetpack_Fly::Initialize()
{
	return S_OK;
}

HRESULT CJetpack_Fly::Start_State(void* pArg)
{
	CTransform* pJetpackTransform = m_pOwner->Get_Transform();

	XMStoreFloat3(&m_fOriginRight , pJetpackTransform->Get_State(CTransform::STATE_RIGHT));
	XMStoreFloat3(&m_fOriginUp, pJetpackTransform->Get_State(CTransform::STATE_UP));
	XMStoreFloat3(&m_fOriginLook, pJetpackTransform->Get_State(CTransform::STATE_LOOK));


	return S_OK;
}

void CJetpack_Fly::Update(_float fTimeDelta)
{
	CTransform* pPlayerTransform = m_pGameInstance->Find_Player(LEVEL_GAMEPLAY)->Get_Transform();
	CTransform* pJetpackTransform = m_pOwner->Get_Transform();

	_vector vPlayerPos = pPlayerTransform->Get_State(CTransform::STATE_POSITION);
	_vector vJetpackPos = pJetpackTransform->Get_State(CTransform::STATE_POSITION);

	

	m_fAccTime += fTimeDelta;
	m_fFlyTime -= fTimeDelta;

	CRigidBody* pRigidBody = m_pOwner->Get_RigidBody();
	CFsm* pFsm = m_pOwner->Get_Fsm();
	CModel* pModel = m_pOwner->Get_Model();

	if (true == m_isFindPlayer &&  5.f >= XMVector3Length(pRigidBody->Get_Velocity()).m128_f32[0] )
	{
		pModel->SetUp_Animation(CJetpack::JETPACK_ANIMATION::IDLE, true);

		_vector fDirY = XMVectorSet(0.f, 1.f, 0.f, 0.f);

		_float fPlayerY = XMVectorGetY(vPlayerPos);
		fPlayerY += 35.f;



		_float fJetpackY = XMVectorGetY(vJetpackPos);
		_float fDistanceY = fabs(fJetpackY - fPlayerY);

		

		if (m_fFlyTime <= 0.f || fDistanceY > 700.f)  // 5.f �̻��� ���̰� ���� ��ġ ����
		{
			m_fFlyTime = 2.f;      	// ó�� �ѹ��� ���Ѵ� .
			m_isForceVelocity = false;

			if (fPlayerY > fJetpackY)   
			{
				m_isFlyUp = true;		
			}

			else if (fJetpackY > fPlayerY)
			{
				m_isFlyUp = false;	
			}
		}

		if (false == m_isForceVelocity)
		{
			if (true == m_isFlyUp)
			{
				pRigidBody->Add_Force_Direction(fDirY, 90, Engine::CRigidBody::ACCELERATION);
				pRigidBody->Add_Force_Direction(fDirY, 150, Engine::CRigidBody::VELOCITYCHANGE);
			}
			else
			{
				pRigidBody->Add_Force_Direction(-fDirY, 70, Engine::CRigidBody::ACCELERATION);
				pRigidBody->Add_Force_Direction(-fDirY, 150, Engine::CRigidBody::VELOCITYCHANGE);
			}

			m_isForceVelocity = true;
		}
		
	}


	pJetpackTransform->LookAt(vPlayerPos, nullptr);

	if (Check_FollowDetect())
	{
		//pModel->SetUp_Animation(CJetpack::JETPACK_ANIMATION::FLY_F, true);
		pJetpackTransform->Go_Straight(fTimeDelta * 60.f);

		return;
	}


	if(Check_Detect())
	{
		if (m_fAccTime > 3.f)
		{
			_int iRandom = m_pGameInstance->Get_Random_Interger(0, 3);
			if (3 != iRandom)
			{
				pRigidBody->Set_Velocity(_float3(0.f, 0.f, 0.f));
				pRigidBody->Set_Accel(_float3(0.f, 0.f, 0.f));
				pRigidBody->Set_ZeroTimer();
			}

			switch (iRandom)
			{
			case 0:
			{
				_vector vLookNor = XMVector3Normalize(pJetpackTransform->Get_State(CTransform::STATE_LOOK));

				pModel->SetUp_Animation(CJetpack::JETPACK_ANIMATION::FLY_B, true);

				pRigidBody->Add_Force_Direction(-vLookNor, 90, Engine::CRigidBody::ACCELERATION);
				pRigidBody->Add_Force_Direction(-vLookNor, 150, Engine::CRigidBody::VELOCITYCHANGE);
			}
			break;

			case 1:
			{
				_vector vRightNor = XMVector3Normalize(pJetpackTransform->Get_State(CTransform::STATE_RIGHT));

				pModel->SetUp_Animation(CJetpack::JETPACK_ANIMATION::FLY_L, true);

				pRigidBody->Add_Force_Direction(vRightNor, 90, Engine::CRigidBody::ACCELERATION);
				pRigidBody->Add_Force_Direction(vRightNor, 150, Engine::CRigidBody::VELOCITYCHANGE);

			}
			break;

			case 2:
			{
				_vector vRightNor = XMVector3Normalize(pJetpackTransform->Get_State(CTransform::STATE_RIGHT));

				pModel->SetUp_Animation(CJetpack::JETPACK_ANIMATION::FLY_R, true);

				pRigidBody->Add_Force_Direction(-vRightNor, 90, Engine::CRigidBody::ACCELERATION);
				pRigidBody->Add_Force_Direction(-vRightNor, 150, Engine::CRigidBody::VELOCITYCHANGE);
			}
			break;

			case 3:
			{
				pModel->SetUp_Animation(CJetpack::JETPACK_ANIMATION::ATTACK_2, true);
				pFsm->Change_State(CJetpack::JETPACK_ANIMATION::ATTACK_1);
			}
				break;

			default:
				break;
			}

			m_fAccTime = 0.f;
		}
	}	
	else
	{
		pJetpackTransform->Go_Straight(fTimeDelta * 3.f);
	}



}

void CJetpack_Fly::End_State()
{
	CTransform* pJetpackTransform = m_pOwner->Get_Transform();

	pJetpackTransform->Set_State(CTransform::STATE_RIGHT, XMLoadFloat3(&m_fOriginRight));
	pJetpackTransform->Set_State(CTransform::STATE_UP, XMLoadFloat3(&m_fOriginUp));
	pJetpackTransform->Set_State(CTransform::STATE_LOOK, XMLoadFloat3(&m_fOriginLook));
}


_bool CJetpack_Fly::Check_Detect()
{
	_vector vPlayerPos = m_pGameInstance->Find_Player(LEVEL_GAMEPLAY)->Get_Transform()->Get_State(CTransform::STATE_POSITION);
	_vector vJetpackPos = m_pOwner->Get_Transform()->Get_State(CTransform::STATE_POSITION);

	_float  fPlayerX = XMVectorGetX(vPlayerPos);
	_float  fPlayerZ = XMVectorGetZ(vPlayerPos);

	_float  fvJetpackPosX = XMVectorGetX(vJetpackPos);
	_float  fvJetpackPosZ = XMVectorGetZ(vJetpackPos);


	_float fDistanceSquar = (fPlayerX - fvJetpackPosX) * (fPlayerX - fvJetpackPosX) + (fPlayerZ - fvJetpackPosZ) * (fPlayerZ - fvJetpackPosZ);


	return m_isFindPlayer = (fDistanceSquar <= (m_fCanFindPlayerDistance * m_fCanFindPlayerDistance));
}

_bool CJetpack_Fly::Check_FollowDetect()
{
	_vector vPlayerPos = m_pGameInstance->Find_Player(LEVEL_GAMEPLAY)->Get_Transform()->Get_State(CTransform::STATE_POSITION);
	_vector vJetpackPos = m_pOwner->Get_Transform()->Get_State(CTransform::STATE_POSITION);

	_float  fPlayerX = XMVectorGetX(vPlayerPos);
	_float  fPlayerZ = XMVectorGetZ(vPlayerPos);

	_float  fvJetpackPosX = XMVectorGetX(vJetpackPos);
	_float  fvJetpackPosZ = XMVectorGetZ(vJetpackPos);


	_float fDistanceSquar = (fPlayerX - fvJetpackPosX) * (fPlayerX - fvJetpackPosX) + (fPlayerZ - fvJetpackPosZ) * (fPlayerZ - fvJetpackPosZ);


	return m_isFindPlayer = (fDistanceSquar > (m_fCanFollowPlayerDistance * m_fCanFollowPlayerDistance));
}

CJetpack_Fly* CJetpack_Fly::Create(class CGameObject* pOwner)
{
	CJetpack_Fly* pInstance = new CJetpack_Fly(pOwner);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CJetpack_Fly"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CJetpack_Fly::Free()
{
	__super::Free();
}
