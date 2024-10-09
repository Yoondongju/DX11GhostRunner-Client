#include "stdafx.h"
#include "Hel_Attack.h"

#include "Hel.h"
#include "Weapon_Hel.h"

#include "GameInstance.h"
#include "Animation.h"

#include "Player.h"

#include "HelSwordTrail.h"

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
	CModel* pModel = static_cast<CHel*>(m_pOwner)->Get_Model();
	_double& TrackPos = pModel->Get_Referene_CurrentTrackPosition();
	TrackPos = 0.0;

	CHelSwordTrail* pSwordTrail = static_cast<CWeapon_Hel*>(static_cast<CContainerObject*>(m_pOwner)->Get_Part(CHel::PARTID::PART_WEAPON))->Get_SwordTrail();
	pSwordTrail->Set_Active(false);
	pSwordTrail->Set_Active(true);

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
			pModel->SetUp_Animation(CHel::HEL_ANIMATION::IDLE, true);
			pFsm->Change_State(CHel::HEL_ANIMATION::IDLE);
		}
			break;
		}
	}


	if (m_fAttackRange < fCurDistance)
	{
		pHelTransform->Go_Straight(fTimeDelta * 1.5f);
	}
}



void CHel_Attack::End_State()
{
	CHelSwordTrail* pSwordTrail = static_cast<CWeapon_Hel*>(static_cast<CContainerObject*>(m_pOwner)->Get_Part(CHel::PARTID::PART_WEAPON))->Get_SwordTrail();
	pSwordTrail->Set_Active(false);
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
