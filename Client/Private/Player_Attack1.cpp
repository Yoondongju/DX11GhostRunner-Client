#include "stdafx.h"
#include "Player_Attack1.h"

#include "Player.h"
#include "GameInstance.h"

#include "Body_Player.h"
#include "Animation.h"

#include "Collider.h"

#include "Sniper.h"
#include "Pistol.h"
#include "Mira.h"
#include "Jetpack.h"
#include "Elite.h"

#include "SwordTrail.h"

CPlayer_Attack1::CPlayer_Attack1(class CGameObject* pOwner)
	: CState{ CPlayer::PLAYER_ANIMATIONID::ATTACK_R1 , pOwner }
{

}

HRESULT CPlayer_Attack1::Initialize()
{
	return S_OK;
}

HRESULT CPlayer_Attack1::Start_State(void* pArg)
{
	CSwordTrail* pSwordTrail = static_cast<CWeapon_Player*>(static_cast<CContainerObject*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_WEAPON))->Get_SwordTrail();
	pSwordTrail->Set_Active(true);

	return S_OK;
}

void CPlayer_Attack1::Update(_float fTimeDelta)
{	
	Check_Collision();


	CModel* pModel = static_cast<CContainerObject*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_BODY)->Get_Model();
	CWeapon_Player* pKanata = static_cast<CWeapon_Player*>(static_cast<CContainerObject*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_WEAPON));
	pKanata->Set_Attacking(true);

	if (CPlayer::PLAYER_ANIMATIONID::ATTACK_R1 == pModel->Get_CurAnimationIndex() ||
		CPlayer::PLAYER_ANIMATIONID::ATTACK_L1 == pModel->Get_CurAnimationIndex())
	{
		_double Duration = pModel->Get_CurAnimation()->Get_Duration();
		const _double& TrackPos = pModel->Get_Referene_CurrentTrackPosition();
		
		if ( 0.8f <= (TrackPos / Duration))
		{
			CFsm* pFsm = m_pOwner->Get_Fsm();

			_double& TrackPos = pModel->Get_Referene_CurrentTrackPosition();
			TrackPos = 0.0;

			pModel->SetUp_Animation(CPlayer::PLAYER_ANIMATIONID::IDLE, true);
			pFsm->Change_State(CPlayer::PLAYER_ANIMATIONID::IDLE);
		}	
	}
}

void CPlayer_Attack1::End_State()
{
	CWeapon_Player* pKanata = static_cast<CWeapon_Player*>(static_cast<CContainerObject*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_WEAPON));
	pKanata->Set_Attacking(false);

CSwordTrail* pSwordTrail = static_cast<CWeapon_Player*>(static_cast<CContainerObject*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_WEAPON))->Get_SwordTrail();
	pSwordTrail->Set_Active(false);
}


void CPlayer_Attack1::Check_Collision()
{
	// 누구랑 비교할까
	// 일단 몬스터종류를 체크해야하고 , 그다음 그 종류의 몬스터중 (여러마리 있을때) 어떤놈인지 체크해야해
	// 몬스터가 내 레벨에 따라 나오는 몬스터 종류에 대한 이야기를 안해놔서 일단 고민되네

	list<CGameObject*>& Snipers = m_pGameInstance->Get_GameObjects(LEVEL_GAMEPLAY, L"Layer_Sniper");
	for (auto& Sniper : Snipers)
	{
		_bool b = static_cast<CSniper*>(Sniper)->Check_Collision();
	}

	list<CGameObject*>& Pistols = m_pGameInstance->Get_GameObjects(LEVEL_GAMEPLAY, L"Layer_Pistol");
	for (auto& Pistol : Pistols)
	{
		_bool b = static_cast<CPistol*>(Pistol)->Check_Collision();
	}

	list<CGameObject*>& Miras = m_pGameInstance->Get_GameObjects(LEVEL_GAMEPLAY, L"Layer_Mira");
	for (auto& Mira : Miras)
	{
		_bool b = static_cast<CMira*>(Mira)->Check_Collision();
	}

	list<CGameObject*>& Jetpacks = m_pGameInstance->Get_GameObjects(LEVEL_GAMEPLAY, L"Layer_Jetpack");
	for (auto& Jetpack : Jetpacks)
	{
		_bool b = static_cast<CJetpack*>(Jetpack)->Check_Collision();
	}

	list<CGameObject*>& Elites = m_pGameInstance->Get_GameObjects(LEVEL_GAMEPLAY, L"Layer_Elite");
	for (auto& Elite : Elites)
	{
		CElite* pElite = static_cast<CElite*>(Elite);
		_bool b = {};

		if (true == pElite->IsGroggy())
			b = pElite->Check_CollisionGroggy();
		else
			b = pElite->Check_Collision();		
	}
}

CPlayer_Attack1* CPlayer_Attack1::Create(class CGameObject* pOwner)
{
	CPlayer_Attack1* pInstance = new CPlayer_Attack1(pOwner);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CPlayer_Attack1"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer_Attack1::Free()
{
	__super::Free();
}
