#include "stdafx.h"
#include "Hel_Attack.h"

#include "Hel.h"
#include "Weapon_Hel.h"

#include "GameInstance.h"
#include "Animation.h"

#include "Player.h"

#include "HelSwordTrail.h"

#include "HelMotionTrail.h"

CHel_Attack::CHel_Attack(class CGameObject* pOwner)
	: CState{ CHel::HEL_ANIMATION::ATTACK1 , pOwner }
{

}

HRESULT CHel_Attack::Initialize()
{
	m_fAttackRange = 40.f;

	return S_OK;
}

HRESULT CHel_Attack::Start_State(void* pArg)
{
	CHel* pHel = static_cast<CHel*>(m_pOwner);

	CModel* pModel = pHel->Get_Model();
	_double& TrackPos = pModel->Get_Referene_CurrentTrackPosition();
	TrackPos = 0.0;

	CHelSwordTrail* pSwordTrail = static_cast<CWeapon_Hel*>(pHel->Get_Part(CHel::PARTID::PART_WEAPON))->Get_SwordTrail();
	pSwordTrail->Set_Active(false);
	pSwordTrail->Set_Active(true);


	pHel->Set_ActiveParticleAttack(true);
	pHel->Get_MotionTrail()->Set_Active(true);

	return S_OK;
}


void CHel_Attack::Update(_float fTimeDelta)
{
	CHel* pHel = static_cast<CHel*>(m_pOwner);
	static_cast<CWeapon_Hel*>(pHel->Get_Part(CHel::PART_WEAPON))->Check_Collision();


	CTransform* pHelTransform = m_pOwner->Get_Transform();

	CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_Player(LEVEL_GAMEPLAY));
	_float fOffSetY = pPlayer->Get_OffsetY();
	_vector vPlayerPos = pPlayer->Get_Transform()->Get_State(CTransform::STATE_POSITION);

	pHelTransform->LookAt_XZ(vPlayerPos);



	_vector vHelPos = pHelTransform->Get_State(CTransform::STATE_POSITION);

	vPlayerPos.m128_f32[1] -= fOffSetY;
	_float fCurDistance = XMVectorGetX(XMVector3Length(vPlayerPos - vHelPos));


	CModel* pModel = m_pOwner->Get_Model();
	CFsm* pFsm = m_pOwner->Get_Fsm();

	_double Duration = pModel->Get_CurAnimation()->Get_Duration();
	_double TrackPos = pModel->Get_Referene_CurrentTrackPosition();


	if (0.9f < (_float)TrackPos / Duration)	// 거리차이가 너무많이 났어?
	{
		_int iRandom = m_pGameInstance->Get_Random_Interger(1, 1);   // 점프는 언제 할지 고민좀해보자
		switch (iRandom)
		{
		case 0:
		{
			pModel->SetUp_Animation(CHel::HEL_ANIMATION::IDLE_TO_JUMP, true);
			pFsm->Change_State(CHel::HEL_ANIMATION::IDLE_TO_JUMP);
		}
			break;
		case 1:
		{
			pModel->SetUp_Animation(CHel::HEL_ANIMATION::DASH_BACK, true);
			pFsm->Change_State(CHel::HEL_ANIMATION::DASH_BACK);
		}
			break;
		}
	}


	_float fSpeed = 3.5f;
	if (true == pHel->IsPage2())
	{
		fSpeed *= 2.f;
	}

	if(0.1f <= (_float)TrackPos / Duration &&
		m_fAttackRange < fCurDistance)
	{
		pHelTransform->Go_Straight(fTimeDelta * fSpeed);
	}


	if (m_fAttackSoundTime >= 1.f)
	{
		_int iRandom = m_pGameInstance->Get_Random_Interger(0, 3);
		switch (iRandom)
		{
		case 0:
			m_pGameInstance->Play_Sound(TEXT("HelAttack1.ogg"), SOUND_HEL_ATTACK, 1.f);
			m_pGameInstance->Play_Sound(TEXT("HelAttackVO1.ogg"), SOUND_HEL_VOICE, 10.f);
			break;
		case 1:
			m_pGameInstance->Play_Sound(TEXT("HelAttack2.ogg"), SOUND_HEL_ATTACK, 1.f);
			m_pGameInstance->Play_Sound(TEXT("HelAttackVO2.ogg"), SOUND_HEL_VOICE, 10.f);
			break;
		case 2:
			//m_pGameInstance->Play_Sound(TEXT("HelAttack3.ogg"), SOUND_HEL_ATTACK, 1.f);
			m_pGameInstance->Play_Sound(TEXT("HelAttackVO3.ogg"), SOUND_HEL_VOICE, 10.f);
			break;
		case 3:
			//m_pGameInstance->Play_Sound(TEXT("HelAttack4.ogg"), SOUND_HEL_ATTACK, 1.f);
			m_pGameInstance->Play_Sound(TEXT("HelAttackVO4.ogg"), SOUND_HEL_VOICE, 10.f);
			break;

		default:
			break;
		}	
		m_fAttackSoundTime = 0.f;
	}
	m_fAttackSoundTime += fTimeDelta;


}



void CHel_Attack::End_State()
{
	CHel* pHel = static_cast<CHel*>(m_pOwner);

	CHelSwordTrail* pSwordTrail = static_cast<CWeapon_Hel*>(pHel->Get_Part(CHel::PARTID::PART_WEAPON))->Get_SwordTrail();
	pSwordTrail->Set_Active(false);
	
	pHel->Set_ActiveParticleAttack(false);
	pHel->Get_MotionTrail()->Set_Active(false);
}


CHel_Attack* CHel_Attack::Create(class CGameObject* pOwner)
{
	CHel_Attack* pInstance = new CHel_Attack(pOwner);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CHel_Attack"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHel_Attack::Free()
{
	__super::Free();
}
