#include "stdafx.h"
#include "Player_Run.h"

#include "Player.h"
#include "Body_Player.h"

#include "GameInstance.h"

CPlayer_Run::CPlayer_Run(class CGameObject* pOwner)
	: CState{ CPlayer::PLAYER_ANIMATIONID::RUN ,pOwner }
{

}

HRESULT CPlayer_Run::Initialize()
{
	return S_OK;
}

HRESULT CPlayer_Run::Start_State()
{
	_float fSpeedPerSec = m_pGameInstance->Find_Camera()->Get_Transform()->Get_SpeedPerSec();

	m_pGameInstance->Find_Player()->Get_Transform()->Set_SpeedPerSec(fSpeedPerSec * 1.6);


	return S_OK;
}


void CPlayer_Run::Update(_float fTimeDelta)
{
	if (m_pGameInstance->Get_KeyState(KEY::LSHIFT) == KEY_STATE::NONE)
	{
		CFsm* pFsm = m_pOwner->Get_Fsm();
		CModel* pModel = static_cast<CContainerObject*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_BODY)->Get_Model();

		pFsm->Change_State(CPlayer::PLAYER_ANIMATIONID::WALK);
		pModel->SetUp_Animation(CPlayer::PLAYER_ANIMATIONID::WALK, true);
	}



	if (m_pGameInstance->Get_KeyState(KEY::W) == KEY_STATE::NONE &&
		m_pGameInstance->Get_KeyState(KEY::S) == KEY_STATE::NONE &&
		m_pGameInstance->Get_KeyState(KEY::A) == KEY_STATE::NONE &&
		m_pGameInstance->Get_KeyState(KEY::D) == KEY_STATE::NONE)
	{
		CFsm* pFsm = m_pOwner->Get_Fsm();
		CModel* pModel = static_cast<CContainerObject*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_BODY)->Get_Model();

		pFsm->Change_State(CPlayer::PLAYER_ANIMATIONID::IDLE);
		pModel->SetUp_Animation(CPlayer::PLAYER_ANIMATIONID::IDLE, true);
	}


	Check_HookUp();
	Check_Jump();

}


void CPlayer_Run::End_State()
{
	_float fOriginSpeedPerSec = m_pGameInstance->Find_Camera()->Get_Transform()->Get_OriginSpeedPerSec();

	m_pGameInstance->Find_Player()->Get_Transform()->Set_SpeedPerSec(fOriginSpeedPerSec);
}


void CPlayer_Run::Check_HookUp()
{
	if (m_pGameInstance->Get_KeyState(KEY::F) == KEY_STATE::TAP)
	{
		CFsm* pFsm = m_pOwner->Get_Fsm();
		CModel* pModel = static_cast<CContainerObject*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_BODY)->Get_Model();


		pModel->SetUp_Animation(CPlayer::PLAYER_ANIMATIONID::HOOK_UP, false, CPlayer::PLAYER_ANIMATIONID::IDLE);	// Change_State보다 먼저 세팅해줘야함 Hook이나 Attack같은 같은 State를 공유하는녀석일 경우
		pFsm->Change_State(CPlayer::PLAYER_ANIMATIONID::HOOK_UP);
	}
}

void CPlayer_Run::Check_Jump()
{
	if (m_pGameInstance->Get_KeyState(KEY::SPACE) == KEY_STATE::TAP)
	{
		CFsm* pFsm = m_pOwner->Get_Fsm();
		CModel* pModel = static_cast<CContainerObject*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_BODY)->Get_Model();


		pModel->SetUp_Animation(CPlayer::PLAYER_ANIMATIONID::JUMP_START, false, CPlayer::PLAYER_ANIMATIONID::JUMP_LOOP);	// Change_State보다 먼저 세팅해줘야함 Hook이나 Attack같은 같은 State를 공유하는녀석일 경우
		pFsm->Change_State(CPlayer::PLAYER_ANIMATIONID::JUMP_START);
	}
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
