#include "stdafx.h"
#include "Player_Sh_Walk.h"

#include "Player.h"
#include "Body_Player.h"

#include "GameInstance.h"
#include "GrapplingPointUI.h"

CPlayer_Sh_Walk::CPlayer_Sh_Walk(class CGameObject* pOwner)
	: CState{ CPlayer::PLAYER_ANIMATIONID::SH_WALK ,pOwner }
{

}

HRESULT CPlayer_Sh_Walk::Initialize()
{
	return S_OK;
}

HRESULT CPlayer_Sh_Walk::Start_State()
{
	_float fSpeedPerSec = m_pOwner->Get_Transform()->Get_SpeedPerSec();

	m_pOwner->Get_Transform()->Set_SpeedPerSec(fSpeedPerSec * 1.4);

	return S_OK;
}

void CPlayer_Sh_Walk::Update(_float fTimeDelta)
{
	if (m_pGameInstance->Get_KeyState(KEY::LSHIFT) == KEY_STATE::HOLD)
	{
		CFsm* pFsm = m_pOwner->Get_Fsm();
		CModel* pModel = static_cast<CContainerObject*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_BODY)->Get_Model();


		pModel->SetUp_Animation(CPlayer::PLAYER_ANIMATIONID::SH_RUN, true, 0.f);
		pFsm->Change_State(CPlayer::PLAYER_ANIMATIONID::SH_RUN);
	}


	if (m_pGameInstance->Get_KeyState(KEY::W) == KEY_STATE::NONE &&
		m_pGameInstance->Get_KeyState(KEY::S) == KEY_STATE::NONE &&
		m_pGameInstance->Get_KeyState(KEY::A) == KEY_STATE::NONE &&
		m_pGameInstance->Get_KeyState(KEY::D) == KEY_STATE::NONE)
	{
		CFsm* pFsm = m_pOwner->Get_Fsm();
		CModel* pModel = static_cast<CContainerObject*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_BODY)->Get_Model();

		pModel->SetUp_Animation(CPlayer::PLAYER_ANIMATIONID::SH_IDLE, true, 0.f);
		pFsm->Change_State(CPlayer::PLAYER_ANIMATIONID::IDLE);
	}


	if (Check_Sh_Attack1())
		return;

	if (Check_HookUp())
		return;

	if (Check_Jump())
		return;

}

void CPlayer_Sh_Walk::End_State()
{
	_float fOriginSpeedPerSec = m_pOwner->Get_Transform()->Get_OriginSpeedPerSec();

	m_pOwner->Get_Transform()->Set_SpeedPerSec(fOriginSpeedPerSec);
}


_bool CPlayer_Sh_Walk::Check_HookUp()
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

_bool CPlayer_Sh_Walk::Check_Jump()
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

		if ( 0.1f >= fabs((fLandY + fOffSetY) - XMVectorGetY(pTransform->Get_State(CTransform::STATE_POSITION))))
		{
			CFsm* pFsm = m_pOwner->Get_Fsm();
			CModel* pModel = static_cast<CContainerObject*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_BODY)->Get_Model();

			pFsm->Change_State(CPlayer::PLAYER_ANIMATIONID::JUMP_START);
		}

		return true;
	}

	return false;
}

_bool CPlayer_Sh_Walk::Check_Sh_Attack1()
{
	if (true == static_cast<CBody_Player*>(static_cast<CContainerObject*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_BODY))->IsLandingWall())
		return false;

	CModel* pModel = static_cast<CContainerObject*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_BODY)->Get_Model();

	if (CPlayer::PLAYER_ANIMATIONID::SH_ATTACK != pModel->Get_CurAnimationIndex() &&
		m_pGameInstance->Get_KeyState(KEY::LBUTTON) == KEY_STATE::TAP)
	{
		CFsm* pFsm = m_pOwner->Get_Fsm();
		_double& TrackPos = pModel->Get_Referene_CurrentTrackPosition();
		TrackPos = 0.0;

		pModel->SetUp_Animation(CPlayer::PLAYER_ANIMATIONID::SH_ATTACK, false);
		pFsm->Change_State(CPlayer::PLAYER_ANIMATIONID::SH_ATTACK);

		return true;
	}

	return false;
}



CPlayer_Sh_Walk* CPlayer_Sh_Walk::Create(class CGameObject* pOwner)
{
	CPlayer_Sh_Walk* pInstance = new CPlayer_Sh_Walk(pOwner);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CPlayer_Sh_Walk"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer_Sh_Walk::Free()
{
	__super::Free();
}
