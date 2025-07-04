#include "stdafx.h"
#include "Hel_BackBlock.h"

#include "Hel.h"
#include "GameInstance.h"
#include "Animation.h"

#include "PartObject.h"

#include "Player.h"
#include "FreeCamera.h"


#include "HelSwordTrail.h"
#include "Weapon_Hel.h"


CHel_BackBlock::CHel_BackBlock(class CGameObject* pOwner)
	: CState{ CHel::HEL_ANIMATION::BACK_BLOCK , pOwner }
{

}

HRESULT CHel_BackBlock::Initialize()
{
	return S_OK;
}

HRESULT CHel_BackBlock::Start_State(void* pArg)
{
	CHel* pHel = static_cast<CHel*>(m_pOwner);
	CHelSwordTrail* pSwordTrail = static_cast<CWeapon_Hel*>(pHel->Get_Part(CHel::PARTID::PART_WEAPON))->Get_SwordTrail();
	pSwordTrail->Set_Active(false);

	return S_OK;
}


void CHel_BackBlock::Update(_float fTimeDelta)
{
	CHel* pHel = static_cast<CHel*>(m_pOwner);
	pHel->Set_ActiveParticleBigElectric(false);

	CModel* pModel = m_pOwner->Get_Model();
	CFsm* pFsm = m_pOwner->Get_Fsm();

	_double Duration = pModel->Get_CurAnimation()->Get_Duration();
	_double TrackPos = pModel->Get_Referene_CurrentTrackPosition();

	//  11  / 104
	if (false == m_isStartBlock &&  0.13f < (_float)TrackPos / Duration)
	{
		m_pGameInstance->Set_TimeDelayActive(true);
		m_isStartBlock = true;


		pHel->Get_Part(CHel::PARTID::PART_PARTICLE_ATTACK)->SetActiveMyParticle(true);
		pHel->Set_ActiveParticleBigElectric(true);
		m_pGameInstance->Play_Sound(TEXT("Electric.ogg"), SOUND_HEL_ELECTRIC, 4.f);


		CFreeCamera* pCamera = static_cast<CFreeCamera*>(m_pGameInstance->Find_Camera(LEVEL_GAMEPLAY));
		CTransform* pCameraTransform = pCamera->Get_Transform();
		_vector vUpVector = pCameraTransform->Get_State(CTransform::STATE_UP);
		vUpVector = XMVector3Normalize(vUpVector);
		pCamera->Set_Shake(6, 1.5f, vUpVector);
	}	 
	else if (0.25f < (_float)TrackPos / Duration)
	{
		m_pGameInstance->Set_TimeDelayActive(false);
	}

	if (0.4f < (_float)TrackPos / Duration)
	{
		CTransform* pTransform = m_pOwner->Get_Transform();

		pTransform->Go_Straight(fTimeDelta * 15.f);

		_vector vPos = pTransform->Get_State(CTransform::STATE_POSITION);
		pTransform->Set_State(CTransform::STATE_POSITION, XMVectorSetY(vPos, vPos.m128_f32[1] + fTimeDelta * 70.f ));
	}
	if (0.9f < (_float)TrackPos / Duration)
	{
		pModel->SetUp_Animation(CHel::HEL_ANIMATION::IDLE, true);
		pFsm->Change_State(CHel::HEL_ANIMATION::IDLE);
	}

}



void CHel_BackBlock::End_State()
{
	
}


CHel_BackBlock* CHel_BackBlock::Create(class CGameObject* pOwner)
{
	CHel_BackBlock* pInstance = new CHel_BackBlock(pOwner);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CHel_BackBlock"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHel_BackBlock::Free()
{
	__super::Free();
}
