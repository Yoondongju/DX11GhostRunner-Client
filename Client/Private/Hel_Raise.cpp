#include "stdafx.h"
#include "Hel_Raise.h"

#include "Hel.h"
#include "GameInstance.h"
#include "Animation.h"

#include "PartObject.h"
#include "Player.h"
#include "FreeCamera.h"

CHel_Raise::CHel_Raise(class CGameObject* pOwner)
	: CState{ CHel::HEL_ANIMATION::RAISE , pOwner }
{

}

HRESULT CHel_Raise::Initialize()
{
	return S_OK;
}

HRESULT CHel_Raise::Start_State(void* pArg)
{
	CModel* pModel = m_pOwner->Get_Model();
	_double& TrackPos = pModel->Get_Referene_CurrentTrackPosition();
	TrackPos = 0.0;

	CHel* pHel = static_cast<CHel*>(m_pOwner);
	pHel->Get_Part(CHel::PARTID::PART_PARTICLE_SWIRL)->SetActiveMyParticle(true);



	CFreeCamera* pCamera = static_cast<CFreeCamera*>(m_pGameInstance->Find_Camera(LEVEL_GAMEPLAY));
	CTransform* pCameraTransform = pCamera->Get_Transform();
	_vector vUpVector = pCameraTransform->Get_State(CTransform::STATE_UP);
	vUpVector = XMVector3Normalize(vUpVector);
	pCamera->Set_Shake(6, 0.4f, vUpVector);

	return S_OK;
}


void CHel_Raise::Update(_float fTimeDelta)
{
	CHel* pHel = static_cast<CHel*>(m_pOwner);
	pHel->Set_ActiveParticleBigElectric(false);

	CModel* pModel = m_pOwner->Get_Model();
	CFsm* pFsm = m_pOwner->Get_Fsm();
	_double Duration = pModel->Get_CurAnimation()->Get_Duration();
	_double TrackPos = pModel->Get_Referene_CurrentTrackPosition();

	if (0.9f < (_float)TrackPos / Duration)
	{
		CModel* pModel = m_pOwner->Get_Model();
		CFsm* pFsm = m_pOwner->Get_Fsm();

		pModel->SetUp_Animation(CHel::HEL_ANIMATION::IDLE, true);
		pFsm->Change_State(CHel::HEL_ANIMATION::IDLE);
	}
}

void CHel_Raise::End_State()
{
	CFreeCamera* pCamera = static_cast<CFreeCamera*>(m_pGameInstance->Find_Camera(LEVEL_GAMEPLAY));
	CTransform* pCamTransform = pCamera->Get_Transform();
	pCamera->Set_FollowPlayer(true);


	
}


CHel_Raise* CHel_Raise::Create(class CGameObject* pOwner)
{
	CHel_Raise* pInstance = new CHel_Raise(pOwner);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CHel_Raise"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHel_Raise::Free()
{
	__super::Free();
}
