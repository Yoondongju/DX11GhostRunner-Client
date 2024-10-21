#include "stdafx.h"
#include "Player_Climb.h"

#include "Player.h"
#include "GameInstance.h"


#include "Body_Player.h"
#include "Animation.h"


CPlayer_Climb::CPlayer_Climb(class CGameObject* pOwner)
	: CState{ CPlayer::PLAYER_ANIMATIONID::CLIMB , pOwner }
{

}

HRESULT CPlayer_Climb::Initialize()
{
	return S_OK;
}

HRESULT CPlayer_Climb::Start_State(void* pArg)
{
	// 방향에 대한건 고려안해도될것같고 
	// 거리도 고려하지말고 
	// 그냥 바로 포물선으로 그리면서 넘어가도록 하는걸 시도해보자


	CTransform* pTransform = m_pOwner->Get_Transform();
	CRigidBody* pRigidBody = m_pOwner->Get_RigidBody();

	pRigidBody->Set_Velocity(_float3(0.f, 0.f, 0.f));
	pRigidBody->Set_Accel(_float3(0.f, 0.f, 0.f));
	pRigidBody->Set_ZeroTimer();


	_vector vJumpDirection = _vector{ 0.f, 1.f, 0.f };
	_vector vLookNormal = pTransform->Get_State(CTransform::STATE_LOOK);

	vJumpDirection += vLookNormal;

	_float fXWeight = 1.3f;			
	_float fYWeight = 5.f;			
	_float fZWeight = 1.3f;			
	 
	vJumpDirection = XMVectorSetX(vJumpDirection, XMVectorGetX(vJumpDirection) * fXWeight);
	vJumpDirection = XMVectorSetY(vJumpDirection, XMVectorGetY(vJumpDirection) * fYWeight);
	vJumpDirection = XMVectorSetZ(vJumpDirection, XMVectorGetZ(vJumpDirection) * fZWeight);


	vJumpDirection = XMVector3Normalize(vJumpDirection);


	pRigidBody->Add_Force_Direction(vJumpDirection, 300, Engine::CRigidBody::ACCELERATION);
	pRigidBody->Add_Force_Direction(vJumpDirection, 200, Engine::CRigidBody::VELOCITYCHANGE);


	m_pGameInstance->Play_Sound(TEXT("ClimbVO.ogg"), SOUND_PLAYERVOICE, 2.f);

	return S_OK;
}

void CPlayer_Climb::Update(_float fTimeDelta)
{
	CModel* pModel = static_cast<CContainerObject*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_BODY)->Get_Model();
	
	if (CPlayer::PLAYER_ANIMATIONID::CLIMB == pModel->Get_CurAnimationIndex())
	{
		_double Duration = pModel->Get_CurAnimation()->Get_Duration();
		const _double& TrackPos = pModel->Get_Referene_CurrentTrackPosition();

		if (0.8f <= (TrackPos / Duration))
		{
			CFsm* pFsm = m_pOwner->Get_Fsm();

			_double& TrackPos = pModel->Get_Referene_CurrentTrackPosition();
			TrackPos = 0.0;

			pModel->SetUp_Animation(CPlayer::PLAYER_ANIMATIONID::IDLE, true);
			pFsm->Change_State(CPlayer::PLAYER_ANIMATIONID::IDLE);
		}
	}
}

void CPlayer_Climb::End_State()
{

}


CPlayer_Climb* CPlayer_Climb::Create(class CGameObject* pOwner)
{
	CPlayer_Climb* pInstance = new CPlayer_Climb(pOwner);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CPlayer_Climb"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer_Climb::Free()
{
	__super::Free();
}
