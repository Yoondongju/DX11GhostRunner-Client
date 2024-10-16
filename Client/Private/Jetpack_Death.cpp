#include "stdafx.h"
#include "Jetpack_Death.h"


#include "Jetpack.h"
#include "GameInstance.h"
#include "Animation.h"


CJetpack_Death::CJetpack_Death(class CGameObject* pOwner)
	: CState{ CJetpack::JETPACK_ANIMATION::FLY_FALL , pOwner }
{

}

HRESULT CJetpack_Death::Initialize()
{
	return S_OK;
}

HRESULT CJetpack_Death::Start_State(void* pArg)
{

	_uint iRandom = m_pGameInstance->Get_Random_Interger(0, 1);
	switch (iRandom)
	{
	case 0:
		m_pGameInstance->Play_Sound(TEXT("MiraDeath1.ogg"), SOUND_MIRA, 2.f);
		m_pGameInstance->Play_Sound(TEXT("HitBlood2.ogg"), SOUND_MIRABLOOD, 3.f);
		break;
	case 1:
		m_pGameInstance->Play_Sound(TEXT("MiraDeath1.ogg"), SOUND_MIRA, 2.f);
		m_pGameInstance->Play_Sound(TEXT("HitBlood3.ogg"), SOUND_MIRABLOOD, 3.f);
		break;
	default:
		break;
	}

	return S_OK;
}

void CJetpack_Death::Update(_float fTimeDelta)
{
	CModel* pModel = m_pOwner->Get_Model();

	_double Duration = pModel->Get_CurAnimation()->Get_Duration();
	const _double& TrackPos = pModel->Get_Referene_CurrentTrackPosition();

	CRigidBody* pRigidBody = m_pOwner->Get_RigidBody();

	_vector fDirY = XMVectorSet(0.f, 1.f, 0.f, 0.f);
	pRigidBody->Add_Force_Direction(fDirY, -20.f, Engine::CRigidBody::VELOCITYCHANGE);

	static_cast<CJetpack*>(m_pOwner)->SetDead();
}

void CJetpack_Death::End_State()
{

}


CJetpack_Death* CJetpack_Death::Create(class CGameObject* pOwner)
{
	CJetpack_Death* pInstance = new CJetpack_Death(pOwner);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CJetpack_Death"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CJetpack_Death::Free()
{
	__super::Free();
}
