#include "stdafx.h"
#include "Player_Attack2.h"

#include "Player.h"
#include "GameInstance.h"

#include "Body_Player.h"
#include "Animation.h"

#include "Sniper.h"
#include "Pistol.h"
#include "Mira.h"
#include "Jetpack.h"

CPlayer_Attack2::CPlayer_Attack2(class CGameObject* pOwner)
	: CState{ CPlayer::PLAYER_ANIMATIONID::ATTACK_R2 , pOwner }
{

}

HRESULT CPlayer_Attack2::Initialize()
{
	return S_OK;
}

HRESULT CPlayer_Attack2::Start_State()
{


	return S_OK;
}

void CPlayer_Attack2::Update(_float fTimeDelta)
{
	Check_Collision();

	CModel* pModel = static_cast<CContainerObject*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_BODY)->Get_Model();

	if (CPlayer::PLAYER_ANIMATIONID::ATTACK_R2 == pModel->Get_CurAnimationIndex())
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

void CPlayer_Attack2::End_State()
{

}


void CPlayer_Attack2::Check_Collision()
{
	CWeapon_Player* pWeapon = static_cast<CWeapon_Player*>(static_cast<CContainerObject*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_WEAPON));
	CCollider* pWeaponCollider = pWeapon->Get_Collider();

	list<CGameObject*>& Snipers = m_pGameInstance->Get_GameObjects(LEVEL_GAMEPLAY, L"Layer_Sniper");
	for (auto& Sniper : Snipers)
	{
		static_cast<CSniper*>(Sniper)->Check_Collision();
	}

	list<CGameObject*>& Pistols = m_pGameInstance->Get_GameObjects(LEVEL_GAMEPLAY, L"Layer_Pistol");
	for (auto& Pistol : Pistols)
	{
		static_cast<CPistol*>(Pistol)->Check_Collision();
	}

	list<CGameObject*>& Miras = m_pGameInstance->Get_GameObjects(LEVEL_GAMEPLAY, L"Layer_Mira");
	for (auto& Mira : Miras)
	{
		static_cast<CMira*>(Mira)->Check_Collision();
	}

}

CPlayer_Attack2* CPlayer_Attack2::Create(class CGameObject* pOwner)
{
	CPlayer_Attack2* pInstance = new CPlayer_Attack2(pOwner);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CPlayer_Attack2"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer_Attack2::Free()
{
	__super::Free();
}
