#include "stdafx.h"
#include "Player_Sh_Attack1.h"

#include "Player.h"
#include "GameInstance.h"

#include "Body_Player.h"

#include "Animation.h"
#include "SubShuriken.h"

#include "Sniper.h"
#include "Pistol.h"
#include "Mira.h"
#include "Jetpack.h"

#include "ShurikenTrail.h"

CPlayer_Sh_Attack1::CPlayer_Sh_Attack1(class CGameObject* pOwner)
	: CState{ CPlayer::PLAYER_ANIMATIONID::SH_ATTACK , pOwner }
{

}

HRESULT CPlayer_Sh_Attack1::Initialize()
{
	return S_OK;
}

HRESULT CPlayer_Sh_Attack1::Start_State(void* pArg)
{
	return S_OK;
}

void CPlayer_Sh_Attack1::Update(_float fTimeDelta)
{
	m_fAccTime += fTimeDelta;

	CModel* pModel = static_cast<CContainerObject*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_BODY)->Get_Model();
	CFsm* pFsm = m_pOwner->Get_Fsm();

	
	_double Duration = pModel->Get_CurAnimation()->Get_Duration();
	const _double& TrackPos = pModel->Get_Referene_CurrentTrackPosition();


	CWeapon_Player* pMainShuriken = static_cast<CWeapon_Player*>(static_cast<CContainerObject*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_WEAPON));
	CSubShuriken** pSubShuriken = pMainShuriken->Get_SubShuriken();

	if (0.05f <= (TrackPos / Duration))
	{
		Check_Collision();
		pMainShuriken->Set_Attacking(true);
	}

	if (0.4f <= (TrackPos / Duration))
	{
		CShurikenTrail* pMainShurikenTrail = pMainShuriken->Get_ShurikenTrail();
		CShurikenTrail* pSub0ShurikenTrail = pSubShuriken[0]->Get_ShurikenTrail();
		CShurikenTrail* pSub1ShurikenTrail = pSubShuriken[1]->Get_ShurikenTrail();

		pMainShurikenTrail->Set_Active(true);
		pSub0ShurikenTrail->Set_Active(true);
		pSub1ShurikenTrail->Set_Active(true);
	}






	_bool bLBtnTap = m_pGameInstance->Get_KeyState(KEY::LBUTTON) == KEY_STATE::TAP;
	
	if (bLBtnTap)
	{
		if (m_fLastKeyPressTimeLBtn >= 0.f && (m_fAccTime - m_fLastKeyPressTimeLBtn <= 0.3f))
		{
			pModel->SetUp_Animation(CPlayer::PLAYER_ANIMATIONID::SH_ATTACK, true);
	
			m_fLastKeyPressTimeLBtn = -1.f;
		}
		m_fLastKeyPressTimeLBtn = m_fAccTime;
	}
	

	if (CPlayer::PLAYER_ANIMATIONID::SH_ATTACK == pModel->Get_CurAnimationIndex())
	{
		if (0.8f <= (TrackPos / Duration))
		{
			CFsm* pFsm = m_pOwner->Get_Fsm();

			_double& TrackPos = pModel->Get_Referene_CurrentTrackPosition();
			TrackPos = 0.0;

			pModel->SetUp_Animation(CPlayer::PLAYER_ANIMATIONID::SH_IDLE, true, 0.1f);
			pFsm->Change_State(CPlayer::PLAYER_ANIMATIONID::IDLE);
		}
	}

	if (Check_Jump())
		return;

 }


void CPlayer_Sh_Attack1::End_State()
{
	CWeapon_Player* pMainShuriken = static_cast<CWeapon_Player*>(static_cast<CContainerObject*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_WEAPON));

	CSubShuriken** pSubShuriken = pMainShuriken->Get_SubShuriken();


	CShurikenTrail* pMainShurikenTrail = pMainShuriken->Get_ShurikenTrail();

	CShurikenTrail* pSub0ShurikenTrail = pSubShuriken[0]->Get_ShurikenTrail();

	CShurikenTrail* pSub1ShurikenTrail = pSubShuriken[1]->Get_ShurikenTrail();


	pMainShuriken->Set_Attacking(false);


	pMainShurikenTrail->Set_Active(false);
	pSub0ShurikenTrail->Set_Active(false);
	pSub1ShurikenTrail->Set_Active(false);
}


_bool CPlayer_Sh_Attack1::Check_Jump()
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


		if (fLandY + fOffSetY == XMVectorGetY(pTransform->Get_State(CTransform::STATE_POSITION)))
		{
			CFsm* pFsm = m_pOwner->Get_Fsm();
			CModel* pModel = static_cast<CContainerObject*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_BODY)->Get_Model();


			pFsm->Change_State(CPlayer::PLAYER_ANIMATIONID::JUMP_START);
		}

		return true;
	}

	return false;
}

void CPlayer_Sh_Attack1::Check_Collision()
{
	list<CGameObject*>& Snipers = m_pGameInstance->Get_GameObjects(g_CurLevel, L"Layer_Sniper");
	for (auto& Sniper : Snipers)
	{
		_bool b = static_cast<CSniper*>(Sniper)->Check_Collision();
	}

	list<CGameObject*>& Pistols = m_pGameInstance->Get_GameObjects(g_CurLevel, L"Layer_Pistol");
	for (auto& Pistol : Pistols)
	{
		_bool b = static_cast<CPistol*>(Pistol)->Check_Collision();
	}

	list<CGameObject*>& Miras = m_pGameInstance->Get_GameObjects(g_CurLevel, L"Layer_Mira");
	for (auto& Mira : Miras)
	{
		_bool b = static_cast<CMira*>(Mira)->Check_Collision();
	}

	list<CGameObject*>& Jetpacks = m_pGameInstance->Get_GameObjects(g_CurLevel, L"Layer_Jetpack");
	for (auto& Jetpack : Jetpacks)
	{
		_bool b = static_cast<CJetpack*>(Jetpack)->Check_Collision();
	}
}

CPlayer_Sh_Attack1* CPlayer_Sh_Attack1::Create(class CGameObject* pOwner)
{
	CPlayer_Sh_Attack1* pInstance = new CPlayer_Sh_Attack1(pOwner);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CPlayer_Sh_Attack1"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer_Sh_Attack1::Free()
{
	__super::Free();
}
