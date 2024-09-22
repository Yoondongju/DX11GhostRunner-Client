#include "stdafx.h"
#include "Player_Walk.h"

#include "Player.h"
#include "Body_Player.h"

#include "GameInstance.h"
#include "GrapplingPointUI.h"

#include "Animation.h"

CPlayer_Walk::CPlayer_Walk(class CGameObject* pOwner)
	: CState{ CPlayer::PLAYER_ANIMATIONID::WALK ,pOwner }
{

}

HRESULT CPlayer_Walk::Initialize()
{
	return S_OK;
}

HRESULT CPlayer_Walk::Start_State(void* pArg)
{

	return S_OK;
}

void CPlayer_Walk::Update(_float fTimeDelta)
{
	m_fSwordSpinTime -= fTimeDelta;

	CModel* pModel = static_cast<CContainerObject*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_BODY)->Get_Model();
	if (m_fSwordSpinTime < 0.f)
	{
		pModel->SetUp_Animation(CPlayer::PLAYER_ANIMATIONID::SPIN_A, true);
		m_fSwordSpinTime = 5.f;
		m_fFristSpinCall = true;
	}
	else
	{
		if (true == m_fFristSpinCall && CPlayer::PLAYER_ANIMATIONID::SPIN_A == pModel->Get_CurAnimationIndex())
		{
			_double Duration = pModel->Get_CurAnimation()->Get_Duration();
			const _double& TrackPos = pModel->Get_Referene_CurrentTrackPosition();

			if (0.8f <= (TrackPos / Duration))
			{
				CFsm* pFsm = m_pOwner->Get_Fsm();	

				pModel->SetUp_Animation(CPlayer::PLAYER_ANIMATIONID::WALK, true);

				m_fFristSpinCall = false;
			}
		}
	}


	if (m_pGameInstance->Get_KeyState(KEY::LSHIFT) == KEY_STATE::HOLD)
	{
		CFsm* pFsm = m_pOwner->Get_Fsm();
		
		pModel->SetUp_Animation(CPlayer::PLAYER_ANIMATIONID::RUN, true);
		pFsm->Change_State(CPlayer::PLAYER_ANIMATIONID::RUN);
	}


	if (m_pGameInstance->Get_KeyState(KEY::W) == KEY_STATE::NONE &&
		m_pGameInstance->Get_KeyState(KEY::S) == KEY_STATE::NONE &&
		m_pGameInstance->Get_KeyState(KEY::A) == KEY_STATE::NONE &&
		m_pGameInstance->Get_KeyState(KEY::D) == KEY_STATE::NONE)
	{
		CFsm* pFsm = m_pOwner->Get_Fsm();

		pModel->SetUp_Animation(CPlayer::PLAYER_ANIMATIONID::IDLE, true);
		pFsm->Change_State(CPlayer::PLAYER_ANIMATIONID::IDLE);
	}


	_uint iRandomValue = static_cast<_uint>(m_pGameInstance->Get_Random(0, 3));
	CPlayer* pPlayer = static_cast<CPlayer*>(m_pOwner);

	switch (iRandomValue)
	{
	case 0:
	{
		if (Check_Attack1())
			return;
		if (pPlayer->IsBlockActive() && Check_Block1())
			return;
	}
	break;
	case 1:
	{
		if (Check_Attack2())
			return;
		if (pPlayer->IsBlockActive() && Check_Block2())
			return;
	}
	break;
	case 2:
	{
		if (Check_Attack3())
			return;
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

void CPlayer_Walk::End_State()
{
	
}


_bool CPlayer_Walk::Check_HookUp()
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

_bool CPlayer_Walk::Check_Jump()
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


_bool CPlayer_Walk::Check_Block1()
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

_bool CPlayer_Walk::Check_Block2()
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

_bool CPlayer_Walk::Check_Block3()
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


_bool CPlayer_Walk::Check_Attack1()
{
	if (true == static_cast<CBody_Player*>(static_cast<CContainerObject*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_BODY))->IsLandingWall())
		return false;

	CModel* pModel = static_cast<CContainerObject*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_BODY)->Get_Model();

	if (CPlayer::PLAYER_ANIMATIONID::ATTACK_R1 != pModel->Get_CurAnimationIndex() &&
		m_pGameInstance->Get_KeyState(KEY::LBUTTON) == KEY_STATE::TAP)
	{
		CFsm* pFsm = m_pOwner->Get_Fsm();


		pModel->SetUp_Animation(CPlayer::PLAYER_ANIMATIONID::ATTACK_R1, false);	// Change_State보다 먼저 세팅해줘야함 Hook이나 Attack같은 같은 State를 공유하는녀석일 경우
		pFsm->Change_State(CPlayer::PLAYER_ANIMATIONID::ATTACK_R1);

		return true;
	}


	return false;
}

_bool CPlayer_Walk::Check_Attack2()
{
	if (true == static_cast<CBody_Player*>(static_cast<CContainerObject*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_BODY))->IsLandingWall())
		return false;

	CModel* pModel = static_cast<CContainerObject*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_BODY)->Get_Model();

	if (CPlayer::PLAYER_ANIMATIONID::ATTACK_R2 != pModel->Get_CurAnimationIndex() && 
		m_pGameInstance->Get_KeyState(KEY::LBUTTON) == KEY_STATE::TAP)
	{
		CFsm* pFsm = m_pOwner->Get_Fsm();
		

		pModel->SetUp_Animation(CPlayer::PLAYER_ANIMATIONID::ATTACK_R2, false);	// Change_State보다 먼저 세팅해줘야함 Hook이나 Attack같은 같은 State를 공유하는녀석일 경우
		pFsm->Change_State(CPlayer::PLAYER_ANIMATIONID::ATTACK_R2);

		return true;
	}

	return false;
}

_bool CPlayer_Walk::Check_Attack3()
{
	if (true == static_cast<CBody_Player*>(static_cast<CContainerObject*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_BODY))->IsLandingWall())
		return false;

	CModel* pModel = static_cast<CContainerObject*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_BODY)->Get_Model();

	if (CPlayer::PLAYER_ANIMATIONID::ATTACK_R3 != pModel->Get_CurAnimationIndex() &&
		m_pGameInstance->Get_KeyState(KEY::LBUTTON) == KEY_STATE::TAP)
	{
		CFsm* pFsm = m_pOwner->Get_Fsm();
		

		pModel->SetUp_Animation(CPlayer::PLAYER_ANIMATIONID::ATTACK_R3, false);	// Change_State보다 먼저 세팅해줘야함 Hook이나 Attack같은 같은 State를 공유하는녀석일 경우
		pFsm->Change_State(CPlayer::PLAYER_ANIMATIONID::ATTACK_R3);

		return true;
	}

	return false;
}


_bool CPlayer_Walk::Check_CutAll()
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



CPlayer_Walk* CPlayer_Walk::Create(class CGameObject* pOwner)
{
	CPlayer_Walk* pInstance = new CPlayer_Walk(pOwner);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CPlayer_Walk"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer_Walk::Free()
{
	__super::Free();
}
