#include "stdafx.h"
#include "Player_JumpFall.h"

#include "Player.h"
#include "GameInstance.h"

#include "Body_Player.h"
#include "Animation.h"

CPlayer_JumpFall::CPlayer_JumpFall(class CGameObject* pOwner)
	: CState{ CPlayer::PLAYER_ANIMATIONID::JUMP_LOOP , pOwner }
{

}

HRESULT CPlayer_JumpFall::Initialize()
{
	return S_OK;
}

HRESULT CPlayer_JumpFall::Start_State(void* pArg)
{


	return S_OK;
}

void CPlayer_JumpFall::Update(_float fTimeDelta)
{
	CPlayer* pPlayer = static_cast<CPlayer*>(m_pOwner);

	CTransform* pTransform = pPlayer->Get_Transform();
	pPlayer->Set_JumpProgressTime(m_fAccTime);	// 점프 진행시간이 누적된걸 던져


	_float fLandY = pPlayer->Get_LandPosY();
	_float fOffSetY = pPlayer->Get_OffsetY();


	CModel* pModel = pPlayer->Get_Part(CPlayer::PARTID::PART_BODY)->Get_Model();
	CFsm* pFsm = m_pOwner->Get_Fsm();

	if (true == m_isFirstCallLoop && (fLandY + fOffSetY) * 4 > XMVectorGetY(pTransform->Get_State(CTransform::STATE_POSITION)))
	{
		m_isFirstCallLoop = false;

		pModel->SetUp_Animation(CPlayer::PLAYER_ANIMATIONID::JUMP_LOOP, true);	
	}
	else if (true == m_isFirstCallEnd && 0.1f >= fabs((fLandY + fOffSetY) - XMVectorGetY(pTransform->Get_State(CTransform::STATE_POSITION))))
	{
		m_isFirstCallEnd = false;


		if(CWeapon_Player::WEAPON_TYPE::KATANA ==  pPlayer->Get_CurWeaponType())
			pModel->SetUp_Animation(CPlayer::PLAYER_ANIMATIONID::IDLE, true);	
		else
			pModel->SetUp_Animation(CPlayer::PLAYER_ANIMATIONID::SH_IDLE, true);

		pFsm->Change_State(CPlayer::PLAYER_ANIMATIONID::IDLE);		
	}

}

void CPlayer_JumpFall::End_State()
{
	m_isFirstCallLoop = true;
	m_isFirstCallEnd = true;
}


CPlayer_JumpFall* CPlayer_JumpFall::Create(class CGameObject* pOwner)
{
	CPlayer_JumpFall* pInstance = new CPlayer_JumpFall(pOwner);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CPlayer_JumpFall"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer_JumpFall::Free()
{
	__super::Free();
}
