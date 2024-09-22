#include "stdafx.h"
#include "Player_Run.h"

#include "Player.h"
#include "Body_Player.h"

#include "GameInstance.h"
#include "GrapplingPointUI.h"

#include "Animation.h"

CPlayer_Run::CPlayer_Run(class CGameObject* pOwner)
	: CState{ CPlayer::PLAYER_ANIMATIONID::RUN ,pOwner }
{

}

HRESULT CPlayer_Run::Initialize()
{
	return S_OK;
}

HRESULT CPlayer_Run::Start_State(void* pArg)
{
	_float fSpeedPerSec = m_pOwner->Get_Transform()->Get_SpeedPerSec();

	m_pOwner->Get_Transform()->Set_SpeedPerSec(fSpeedPerSec * 1.8);


	return S_OK;
}


void CPlayer_Run::Update(_float fTimeDelta)
{
	m_fSwordSpinTime -= fTimeDelta;

	CPlayer* pPlayer = static_cast<CPlayer*>(m_pOwner);
	CModel* pModel = static_cast<CContainerObject*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_BODY)->Get_Model();

	if (m_fSwordSpinTime < 0.f)
	{
		pModel->SetUp_Animation(CPlayer::PLAYER_ANIMATIONID::SPIN_A, true);	
		m_fSwordSpinTime = 5.f;
		m_fFristSpinCall = true;
	}
	else
	{
		if (CPlayer::PLAYER_ANIMATIONID::SPIN_A == pModel->Get_CurAnimationIndex())
		{
			_double Duration = pModel->Get_CurAnimation()->Get_Duration();
			const _double& TrackPos = pModel->Get_Referene_CurrentTrackPosition();

			if (true == m_fFristSpinCall && 0.8f <= (TrackPos / Duration))
			{
				CFsm* pFsm = m_pOwner->Get_Fsm();

				pModel->SetUp_Animation(CPlayer::PLAYER_ANIMATIONID::RUN, true);

				m_fFristSpinCall = false;
			}
		}
	}


	if (m_pGameInstance->Get_KeyState(KEY::LSHIFT) == KEY_STATE::NONE)
	{
		CFsm* pFsm = m_pOwner->Get_Fsm();
	
		pFsm->Change_State(CPlayer::PLAYER_ANIMATIONID::WALK);
		pModel->SetUp_Animation(CPlayer::PLAYER_ANIMATIONID::WALK, true);
	}


	if (m_pGameInstance->Get_KeyState(KEY::W) == KEY_STATE::NONE &&
		m_pGameInstance->Get_KeyState(KEY::S) == KEY_STATE::NONE &&
		m_pGameInstance->Get_KeyState(KEY::A) == KEY_STATE::NONE &&
		m_pGameInstance->Get_KeyState(KEY::D) == KEY_STATE::NONE)
	{
		CFsm* pFsm = m_pOwner->Get_Fsm();
		
		pFsm->Change_State(CPlayer::PLAYER_ANIMATIONID::IDLE);
		pModel->SetUp_Animation(CPlayer::PLAYER_ANIMATIONID::IDLE, true);
	}

	_uint iRandomValue = static_cast<_uint>(m_pGameInstance->Get_Random(0, 3));

	switch (iRandomValue)
	{
	case 0:
	{
		if (pPlayer->IsBlockActive() && Check_Block1())
			return;
	}
	break;
	case 1:
	{
		if (pPlayer->IsBlockActive() && Check_Block2())
			return;
	}
	break;
	case 2:
	{
		if (pPlayer->IsBlockActive() && Check_Block3())
			return;
	}
	break;
	default:
		break;
	}

	// SKILL
	if (pPlayer->IsCutAllActive() && Check_CutAll())
		return;


	if (Check_HookUp())
		return;
	
	if (Check_Jump())
		return;
}


void CPlayer_Run::End_State()
{
	_float fOriginSpeedPerSec = m_pOwner->Get_Transform()->Get_OriginSpeedPerSec();
	m_pOwner->Get_Transform()->Set_SpeedPerSec(fOriginSpeedPerSec);

	
}


_bool CPlayer_Run::Check_HookUp()
{
	if (false == static_cast<CPlayer*>(m_pOwner)->Get_GrapplingPoint()->Get_FindPlayer())
		return false;

	if (m_pGameInstance->Get_KeyState(KEY::F) == KEY_STATE::TAP)
	{
		CFsm* pFsm = m_pOwner->Get_Fsm();
		CModel* pModel = static_cast<CContainerObject*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_BODY)->Get_Model();


		pModel->SetUp_Animation(CPlayer::PLAYER_ANIMATIONID::HOOK_UP, false);	// Change_State보다 먼저 세팅해줘야함 Hook이나 Attack같은 같은 State를 공유하는녀석일 경우
		pFsm->Change_State(CPlayer::PLAYER_ANIMATIONID::HOOK_UP);

		return true;
	}

	return false;
}

_bool CPlayer_Run::Check_Jump()
{
	if (true == static_cast<CBody_Player*>(static_cast<CContainerObject*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_BODY))->IsLandingWall())
		return false;

	if (m_pGameInstance->Get_KeyState(KEY::SPACE) == KEY_STATE::TAP)
	{
		CTransform* pTransform = m_pOwner->Get_Transform();
		CPlayer* pPlayer = static_cast<CPlayer*>(m_pOwner);

		_float fLandY = pPlayer->Get_LandPosY();
		_float fOffSetY = pPlayer->Get_OffsetY();
		CModel* pModel = static_cast<CContainerObject*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_BODY)->Get_Model();

		if (0.1f >= fabs((fLandY + fOffSetY) - XMVectorGetY(pTransform->Get_State(CTransform::STATE_POSITION))))
		{
			CFsm* pFsm = m_pOwner->Get_Fsm();
			CModel* pModel = static_cast<CContainerObject*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_BODY)->Get_Model();


			pFsm->Change_State(CPlayer::PLAYER_ANIMATIONID::JUMP_START);
			
		}

		return true;
	}

	return false;
}

_bool CPlayer_Run::Check_Block1()
{
	if (true == static_cast<CBody_Player*>(static_cast<CContainerObject*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_BODY))->IsLandingWall())
		return false;

	CModel* pModel = static_cast<CContainerObject*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_BODY)->Get_Model();

	if (CPlayer::PLAYER_ANIMATIONID::BLOCK_R1 != pModel->Get_CurAnimationIndex() &&
		m_pGameInstance->Get_KeyState(KEY::E) == KEY_STATE::TAP)
	{
		CFsm* pFsm = m_pOwner->Get_Fsm();


		pModel->SetUp_Animation(CPlayer::PLAYER_ANIMATIONID::BLOCK_R1, false);
		pFsm->Change_State(CPlayer::PLAYER_ANIMATIONID::BLOCK_R1);

		return true;
	}
	return false;
}

_bool CPlayer_Run::Check_Block2()
{
	if (true == static_cast<CBody_Player*>(static_cast<CContainerObject*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_BODY))->IsLandingWall())
		return false;

	CModel* pModel = static_cast<CContainerObject*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_BODY)->Get_Model();

	if (CPlayer::PLAYER_ANIMATIONID::BLOCK_R2 != pModel->Get_CurAnimationIndex() &&
		m_pGameInstance->Get_KeyState(KEY::E) == KEY_STATE::TAP)
	{
		CFsm* pFsm = m_pOwner->Get_Fsm();


		pModel->SetUp_Animation(CPlayer::PLAYER_ANIMATIONID::BLOCK_R2, false);
		pFsm->Change_State(CPlayer::PLAYER_ANIMATIONID::BLOCK_R2);

		return true;
	}
	return false;
}

_bool CPlayer_Run::Check_Block3()
{
	if (true == static_cast<CBody_Player*>(static_cast<CContainerObject*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_BODY))->IsLandingWall())
		return false;

	CModel* pModel = static_cast<CContainerObject*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_BODY)->Get_Model();

	if (CPlayer::PLAYER_ANIMATIONID::BLOCK_R3 != pModel->Get_CurAnimationIndex() &&
		m_pGameInstance->Get_KeyState(KEY::E) == KEY_STATE::TAP)
	{
		CFsm* pFsm = m_pOwner->Get_Fsm();


		pModel->SetUp_Animation(CPlayer::PLAYER_ANIMATIONID::BLOCK_R3, false);
		pFsm->Change_State(CPlayer::PLAYER_ANIMATIONID::BLOCK_R3);

		return true;
	}
	return false;
}

_bool CPlayer_Run::Check_CutAll()
{
	if (true == static_cast<CBody_Player*>(static_cast<CContainerObject*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_BODY))->IsLandingWall())
		return false;

	CModel* pModel = static_cast<CContainerObject*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_BODY)->Get_Model();

	if (CPlayer::PLAYER_ANIMATIONID::FURR_AIM_LOOP != pModel->Get_CurAnimationIndex() &&
		m_pGameInstance->Get_KeyState(KEY::Q) == KEY_STATE::TAP)
	{
		CFsm* pFsm = m_pOwner->Get_Fsm();


		pModel->SetUp_Animation(CPlayer::PLAYER_ANIMATIONID::FURR_AIM_LOOP, true);
		pFsm->Change_State(CPlayer::PLAYER_ANIMATIONID::FURR_AIM_LOOP);

		return true;
	}

	return false;
}



CPlayer_Run* CPlayer_Run::Create(class CGameObject* pOwner)
{
	CPlayer_Run* pInstance = new CPlayer_Run(pOwner);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CPlayer_Run"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer_Run::Free()
{
	__super::Free();
}
