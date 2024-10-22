#include "stdafx.h"
#include "Player_Hook.h"

#include "Player.h"
#include "Body_Player.h"
#include "Wire_Player.h"

#include "GameInstance.h"

#include "GrapplingPointUI.h"
#include "Static_Object.h"


CPlayer_Hook::CPlayer_Hook(class CGameObject* pOwner)
	: CState{ CPlayer::PLAYER_ANIMATIONID::HOOK_UP , pOwner }
{

}

HRESULT CPlayer_Hook::Initialize()
{
	

	return S_OK;
}

HRESULT CPlayer_Hook::Start_State(void* pArg)
{
	CPlayer* pPlayer = static_cast<CPlayer*>(m_pOwner);

	CModel* pModel = pPlayer->Get_Part(CPlayer::PARTID::PART_BODY)->Get_Model();
	_double& CurTrankPos = pModel->Get_Referene_CurrentTrackPosition();
	CurTrankPos = 0.f;

	CTransform* pTransform = pPlayer->Get_Transform();
	CRigidBody* pRigidBody = pPlayer->Get_RigidBody();

	pRigidBody->Set_Velocity(_float3(0.f, 0.f, 0.f));
	pRigidBody->Set_Accel(_float3(0.f, 0.f, 0.f));
	pRigidBody->Set_ZeroTimer();


	m_pGrapUI = static_cast<CGrapplingPointUI*>(pArg);
	Safe_AddRef(m_pGrapUI);


	
	static_cast<CWire_Player*>(pPlayer->Get_Part(CPlayer::PARTID::PART_WIRE))->Set_Active(true);
	m_pGameInstance->ActiveBlur(nullptr, CRenderer::BLUR_TYPE::MOTION_BLUR);


	return S_OK;
}

void CPlayer_Hook::Update(_float fTimeDelta)
{
	if (false == m_isHookSoundActive)
	{
		m_pGameInstance->Play_Sound(TEXT("Hook.ogg"), SOUND_PLAYER_HOOK, 3.f);
		m_isHookSoundActive = true;
	}


	

	CPlayer* pPlayer = static_cast<CPlayer*>(m_pOwner);
	CTransform* pTransform = pPlayer->Get_Transform();
	CRigidBody* pRigidBody = pPlayer->Get_RigidBody();

	_vector vGrapPointPos = m_pGrapUI->Get_Crane()->Get_Transform()->Get_State(CTransform::STATE_POSITION);
	_vector vPlayerPos = pTransform->Get_State(CTransform::STATE_POSITION);


	_float4x4* pPlayerRotationMatrix = static_cast<CPlayer*>(m_pOwner)->Get_RotationMatrixPtr();

	_float	fOutAngle = { 0.f };
	pTransform->LookAt_Smooth(vGrapPointPos , fTimeDelta * 2, pPlayerRotationMatrix , &fOutAngle);

	if (fOutAngle <= 45.f)
	{		
		m_isStartHook = true;
	}


	if (true == m_isStartHook)
	{
		_vector vDir = XMVector3Normalize(vGrapPointPos - vPlayerPos);

		pRigidBody->Add_Force_Direction(vDir, 700, Engine::CRigidBody::ACCELERATION);
		pRigidBody->Add_Force_Direction(vDir, 60, Engine::CRigidBody::VELOCITYCHANGE);
	}

	
	_vector vAbsDistance = XMVectorAbs(XMVectorSubtract(vGrapPointPos, vPlayerPos));

	if (300.f >= XMVectorGetX(XMVector3Length(vAbsDistance)))
	{
		_vector vDir = XMVector3Normalize(vGrapPointPos - vPlayerPos);

		_vector vCurVelocity = pRigidBody->Get_Velocity();

		vCurVelocity -= vCurVelocity * 0.2f;

		pRigidBody->Set_Velocity(_float3(vCurVelocity.m128_f32[0] , vCurVelocity.m128_f32[1], vCurVelocity.m128_f32[2]));
	}

	if (60.f >= XMVectorGetX(XMVector3Length(vAbsDistance)))
	{
		pRigidBody->Set_Velocity(_float3(0.f, 0.f, 0.f));
		pRigidBody->Set_Accel(_float3(0.f, 0.f, 0.f));
		pRigidBody->Set_ZeroTimer();

		CFsm* pFsm = m_pOwner->Get_Fsm();
		CModel* pModel = static_cast<CContainerObject*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_BODY)->Get_Model();

		pModel->SetUp_Animation(CPlayer::PLAYER_ANIMATIONID::IDLE, true);
		pFsm->Change_State(CPlayer::PLAYER_ANIMATIONID::IDLE);
	}

}

void CPlayer_Hook::End_State()
{
	static_cast<CWire_Player*>(static_cast<CPlayer*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_WIRE))->Set_Active(false);
	m_fAccTime = 0.f;
	m_isStartHook = false;

	m_isHookSoundActive = false;

	Safe_Release(m_pGrapUI);

	m_pGameInstance->UnActiveBlur();
}


CPlayer_Hook* CPlayer_Hook::Create(CGameObject* pOwner)
{
	CPlayer_Hook* pInstance = new CPlayer_Hook(pOwner);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CPlayer_Hook"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer_Hook::Free()
{
	__super::Free();

	Safe_Release(m_pGrapUI);
}
