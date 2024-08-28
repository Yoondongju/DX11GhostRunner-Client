#include "stdafx.h"
#include "Player_Walk.h"

#include "Player.h"
#include "Body_Player.h"

#include "GameInstance.h"

CPlayer_Walk::CPlayer_Walk(class CGameObject* pOwner)
	: CState{ CPlayer::PLAYER_ANIMATIONID::WALK ,pOwner }
{

}

HRESULT CPlayer_Walk::Initialize()
{
	return S_OK;
}

HRESULT CPlayer_Walk::Start_State()
{

	return S_OK;
}

void CPlayer_Walk::Update(_float fTimeDelta)
{	
	if (m_pGameInstance->Get_KeyState(KEY::LSHIFT) == KEY_STATE::HOLD)
	{
		CFsm* pFsm = m_pOwner->Get_Fsm();
		CModel* pModel = static_cast<CContainerObject*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_BODY)->Get_Model();

		pFsm->Change_State(CPlayer::PLAYER_ANIMATIONID::RUN);
		pModel->SetUp_Animation(CPlayer::PLAYER_ANIMATIONID::RUN, true);
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

void CPlayer_Walk::End_State()
{

}


void CPlayer_Walk::Check_HookUp()
{
	if (m_pGameInstance->Get_KeyState(KEY::F) == KEY_STATE::TAP)
	{
		CFsm* pFsm = m_pOwner->Get_Fsm();
		CModel* pModel = static_cast<CContainerObject*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_BODY)->Get_Model();


		pModel->SetUp_Animation(CPlayer::PLAYER_ANIMATIONID::HOOK_UP, false, CPlayer::PLAYER_ANIMATIONID::IDLE);	// Change_State보다 먼저 세팅해줘야함 Hook이나 Attack같은 같은 State를 공유하는녀석일 경우
		pFsm->Change_State(CPlayer::PLAYER_ANIMATIONID::HOOK_UP);
	}
}

void CPlayer_Walk::Check_Jump()
{
	if (m_pGameInstance->Get_KeyState(KEY::SPACE) == KEY_STATE::TAP)
	{
		CFsm* pFsm = m_pOwner->Get_Fsm();
		CModel* pModel = static_cast<CContainerObject*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_BODY)->Get_Model();


		pModel->SetUp_Animation(CPlayer::PLAYER_ANIMATIONID::JUMP_START, false, CPlayer::PLAYER_ANIMATIONID::JUMP_LOOP);	// Change_State보다 먼저 세팅해줘야함 Hook이나 Attack같은 같은 State를 공유하는녀석일 경우
		pFsm->Change_State(CPlayer::PLAYER_ANIMATIONID::JUMP_START);
	}
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
